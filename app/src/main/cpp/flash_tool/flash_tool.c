/***************************************************************************************
                  Silicon Laboratories Broadcast Si475x/6x/9x module code

   EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND CONDITIONS OF
     THE SOFTWARE LICENSE AGREEMENT IN THE DOCUMENTATION FILE CORRESPONDING
     TO THIS SOURCE FILE.
   IF YOU DO NOT AGREE TO THE LIMITED LICENSE AND CONDITIONS OF SUCH AGREEMENT,
     PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.

   Date: June 06 2014
  (C) Copyright 2014, Silicon Laboratories, Inc. All rights reserved.
****************************************************************************************/
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>

#include "dc_config.h"
#include "dc_bootup.h"

#include "si479x_flashload.h"
#include "flash_layout.h"

typedef uint8_t FLASH_OPERATION; enum {
	UPDATE = 0x0,
	DUMP,
};

#define MAX_LENGTH	0x100000
static const char *folder = "./";//si479x.bin
static uint32_t start = 0x0;
static uint32_t length = MAX_LENGTH; //Default length
static FLASH_OPERATION operation = UPDATE;

#define MAX_FLASH_SIZE	4*1024*1024

static void print_usage(const char *prog)
{
	printf("Usage: %s [-dufsl]\n", prog);
	puts("  -d --dump     Dump the data from flash.\n"
	     "  -u --update   Update data to the flash. Default operation.\n"
	     "  -f --folder   Folder to locate the si479x.bin. (Default folder is ./) \n"
	     "  -s --start    Start address on the flash. Default value is 0.\n"
	     "  -l --length   The length to dump from flash. (Only for dump operation. Dump the firmware bin only by default, the max size is 0x1000000.)\n");
		 
	exit(1);
}

static void parse_opts(int argc, char *argv[])
{
	while (1) {
		static const struct option lopts[] = {
			{ "dump",    0, 0, 'd' },
			{ "update",  0, 0, 'u' },
			{ "folder",    1, 0, 'f' },
			{ "start",   1, 0, 's' },
			{ "length",  1, 0, 'l' },
			{ NULL, 0, 0, 0 },
		};
		int c;

		c = getopt_long(argc, argv, "f:s:l:du", lopts, NULL);

		if (c == -1)
			break;

		switch (c) {
		case 'f':
			folder = optarg;
			break;
		case 's':
			start = atoi(optarg);
			break;
		case 'l':
			length = atoi(optarg);
			if (length > MAX_LENGTH)
			{
				length = MAX_LENGTH;
			}
			break;
		case 'u':
			operation = UPDATE;
			break;
		case 'd':
			operation = DUMP;
			break;
		default:
			print_usage(argv[0]);
			break;
		}
	}
}


#define SECTOR_SIZE 0x1000

RET_CODE EraseFlashSectors(uint32_t from, uint32_t end) //unit is 4k
{
	RET_CODE ret = RET_SUCCESS;
	uint32_t add;
	
	from &= 0xfffff000;
	end &= 0xfffff000;

	#ifdef SUPPORT_2T
	set_chip_id(CHIP_ID_1);
	#endif
	
    //erase SPI flash
	printf("Erase SPI flash from 0x%x to 0x%08x...\n", from, end);
	
	for ( add=from; add<end; add+=SECTOR_SIZE)
	{		
		printf(".");
		ret = si479x_erase_sector(add);
		
		while (RET_TIMEOUT == ret)
		{
			sleep_ms(100);
			ret = si479x_waitForCTS();
		}
	}
	printf("\nDone.\n");
	
	return ret;
}

#define WRITE_BLOCK_COMMAND_PAYLOAD_OFFSET	16
RET_CODE WriteFlash(FW_IMG_T file, uint32_t add)
{
    RET_CODE ret = RET_SUCCESS;
    uint16_t read_length = 0;
	uint32_t start = 0;
    FILE *fp = NULL;

	SetFirmwareImage(file);
	
	fp = fopen(GetFirmwareImage(), "r");
	if (NULL == fp) 
	{
		printf("Open file %s failed!\n", GetFirmwareImage());
		return RET_INVALIDINPUT;
	}
	    
	printf("Start to write %s to SPI flash 0x%08x...\n", GetFirmwareImage(), add);
    //get firmware/bootloader segment
	read_length = fread(bulk_buff+WRITE_BLOCK_COMMAND_PAYLOAD_OFFSET, 1, BULCK_BUFF_SIZE-WRITE_BLOCK_COMMAND_PAYLOAD_OFFSET, fp);

	#ifdef SUPPORT_2T
	set_chip_id(CHIP_ID_1);
	#endif
	
	start = add;
    while (read_length > 0)
    {
		printf(".");
		ret = si479x_write_block(start, read_length, bulk_buff);
        while (RET_TIMEOUT == ret)
		{
			sleep_ms(10);
            ret = si479x_waitForCTS();
		}
		
		start += read_length;
        
		read_length = fread(bulk_buff+WRITE_BLOCK_COMMAND_PAYLOAD_OFFSET, 1, BULCK_BUFF_SIZE-WRITE_BLOCK_COMMAND_PAYLOAD_OFFSET, fp);
    }
	
	fclose(fp);
	printf("\nDone.\n");
	
	return ret;
}

#define BLOCK_SIZE	512

#define GUID_COUNT_OFFSET 256

uint32_t ReadFlash(FW_IMG_T file, uint32_t add)
{
    RET_CODE ret = RET_SUCCESS;
    uint32_t read_length = 0;
	uint32_t start = 0;
    FILE *fp = NULL;
	uint8_t eoc = 0;
	uint8_t guids = 0;

	SetFirmwareImage(file);
	
	fp = fopen(GetFirmwareImage(), "w");
	if (NULL == fp) 
	{
		printf("Open file %s failed!\n", GetFirmwareImage());
		return 0;
	}
	    
	printf("Start to dump SPI flash from 0x%08x to %s ...\n", add, GetFirmwareImage());
    //get firmware/bootloader segment

	#ifdef SUPPORT_2T
	set_chip_id(CHIP_ID_1);
	#endif
	
	ret = si479x_read_block(start, BLOCK_SIZE, bulk_buff);
	while (RET_TIMEOUT == ret)
	{
		sleep_ms(10);
		ret = si479x_waitForCTS();
	}
	
	guids = bulk_buff[4+GUID_COUNT_OFFSET];
	
	//search 2 sereals '0x1a' '0x1a' '0x1a' '0x1a'
	int lac = 0;
	int cnt = 0;
	
	start = add;
	while (start < MAX_FLASH_SIZE && read_length < length && eoc == 0)
	{
		printf(".");
		ret = si479x_read_block(start, BLOCK_SIZE, bulk_buff);
        while (RET_TIMEOUT == ret)
		{
			sleep_ms(10);
            ret = si479x_waitForCTS();
		}
		
		for (int i=4; i<BLOCK_SIZE+4; i++)
		{
			if (bulk_buff[i] == 0x1a)
			{
				lac ++;
			} else {
				lac = 0;
			}
			
			if (lac == 4) 
			{
				cnt ++;
				lac = 0;
			}
			
			if (cnt == guids+3)
			{
				eoc = 1;
				//printf("eoc = 1\n");
			}
		}
				
		fwrite(bulk_buff+4, 1, BLOCK_SIZE, fp);
		
		start += BLOCK_SIZE;
		read_length += BLOCK_SIZE;
		//printf("start = 0x%08x, read_length = 0x%08x\n", start, read_length);
	}
	
	fclose(fp);
	printf("\nDone.\n");
	
	return read_length;
}


////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
	int ret = 0;
	int fd;
	uint32_t actual_length = 0;

	parse_opts(argc, argv);
	
	if (MAX_FLASH_SIZE < (start+length))
	{
		printf("Max flash size is %d.\n", MAX_FLASH_SIZE);
		return -1;
	}
	
	//set the folder to locate the si479x.bin
	SetFirmwareImageFolder(folder);
		
	//Step a. Platform hal init, GPIO, spi/i2c init, etc.
	platform_hal_init();
	
	//Step b. Host load flash_bl.bin
	if (RET_SUCCESS != dc_hostload_flashutility_bootup()) {
		return -1;
	}
	
	if (operation == UPDATE) {
		//Step c. Erase SPI flash
		if (RET_SUCCESS != EraseFlashSectors(0, SI479X_FIRMWARE_MAX_SIZE+SI479X_FIRMWARE_START))
			return -1;
        //Step d. Write SPI flash
        if (RET_SUCCESS != WriteFlash(TUNER_FW, 0x0))
			return -1;
		
		printf("Update flash %s to 0x%08x successful.\n", GetFirmwareImage(), start);
	} else {
		//Step c. Read SPI flash
		actual_length = ReadFlash(TUNER_FW, 0x0);
		if (actual_length == 0)
			return -1;
		
		printf("Damp flash from 0x%08x with %d bytes data to %s successful.\n", start, actual_length,  GetFirmwareImage());
	}
	
	return 0;
}
