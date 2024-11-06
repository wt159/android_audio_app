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
#include "dc_config.h"
#include "dc_bootup.h"


uint8_t si479x_chip_id;


#include <stdlib.h>  
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>   //define O_WRONLY and O_RDONLY  
#include <stdint.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

void sleep_us(uint16_t us)
{
    usleep(us);
}


void sleep_ms(uint16_t ms)
{
    usleep(1000*ms);
}


#define SYSFS_GPIO_EXPORT           "/sys/class/gpio/export"  
#define SYSFS_GPIO_RST_DIR          "/sys/class/gpio/gpio%s/direction"
#define SYSFS_GPIO_RST_DIR_VAL      "out"  
#define SYSFS_GPIO_RST_VAL          "/sys/class/gpio/gpio%s/value"
#define SYSFS_GPIO_RST_VAL_H        "1"
#define SYSFS_GPIO_RST_VAL_L        "0"
#define SYSFS_GPIO_UNEXPORT          "/sys/class/gpio/unexport"  

int gpio_reset(const char* gpio_id)
{
    	int fd; 
		int ret;
		char path[256];

		sprintf(path, "/sys/class/gpio/gpio%s/direction", gpio_id);
        fd = open(path, O_WRONLY);
        if(fd == -1)
        {
			fd = open(SYSFS_GPIO_EXPORT, O_WRONLY);
			if(fd == -1)
			{
					   printf("ERR: Open %s failed.\n", SYSFS_GPIO_EXPORT);
					   return -1;
			}
			ret = write(fd, gpio_id ,strlen(gpio_id)+1); 
			if (ret == -1)
			{
				printf("INFO: Write %s to %s return %d.\n", gpio_id, SYSFS_GPIO_EXPORT, ret);
				goto clean;
			}
		
			usleep(100000);
			fd = open(path, O_WRONLY);
			if(fd == -1)
			{
				printf("ERR: Open %s failed.\n", path);
				return-1;
			}
        }
		
        ret = write(fd, SYSFS_GPIO_RST_DIR_VAL, sizeof(SYSFS_GPIO_RST_DIR_VAL)); 
		if (ret == -1)
		{
			printf("INFO: Write %s to %s return %d.\n", SYSFS_GPIO_RST_DIR_VAL, path, ret);
			goto clean;
		}

		sprintf(path, "/sys/class/gpio/gpio%s/value", gpio_id);
        fd = open(path, O_RDWR);
        if(fd == -1)
        {
                  printf("ERR: SOpen %s failed.\n.", path);
                  return -1;
        }       
                  
		ret = write(fd, SYSFS_GPIO_RST_VAL_L, sizeof(SYSFS_GPIO_RST_VAL_L));
		if (ret == -1)
		{
			printf("INFO: Write %s to %s return %d.\n", SYSFS_GPIO_RST_VAL_L, path, ret);
			goto clean;
		}
        usleep(2000);
		
        ret = write(fd, SYSFS_GPIO_RST_VAL_H, sizeof(SYSFS_GPIO_RST_VAL_H));
		if (ret == -1)
		{
			printf("INFO: Write %s to %s return %d.\n", SYSFS_GPIO_RST_VAL_H, path, ret);
			goto clean;
		}
        usleep(5000);

clean:
        close(fd);
		
		/**
        fd = open(SYSFS_GPIO_UNEXPORT, O_WRONLY);
        if(fd == -1)
        {
                   printf("ERR: Si479xx hard reset pin close error.\n");
                   return -1;
        }
        ret = write(fd, gpio_id ,sizeof(gpio_id)); 
		if (ret == -1)
			printf("INFO: Write %s to %s return %d.\n", gpio_id, SYSFS_GPIO_UNEXPORT, ret);
        close(fd); 
		*/
}

int chips_reset()
{
	//chip 0
	gpio_reset("436");
	
	//chip 1
	// gpio_reset("27");
        
	printf("INFO: Si479xx hard reset pin.\n");
    return 0;
}  

//spi bus related parameters
static const char *chip0_device = "/dev/spidev0.0";
static const char *chip1_device = "/dev/spidev0.1";
static uint8_t mode;
static uint8_t bits = 8;
static uint32_t speed = 12000000;
static uint16_t delay;
static char * current_spi_device = NULL;


void set_chip_id(CHIP_ID_T chip_id)
{
    si479x_chip_id = chip_id;
	
	if (chip_id == CHIP_ID_0)
		current_spi_device = (char*)chip0_device;
	// else
	// 	current_spi_device = (char*)chip1_device;
	
	#ifdef SUPPORTS_LOGS
	printf("Set chip device to %s chip(%d)\n", current_spi_device, chip_id);
	#endif
}

int spi_init(const char *device)
{
	int fd;
	int ret;

	fd = open(device, O_RDWR);
	if (fd < 0)
	{
		printf("can't open %s", device);
		return ret;
	}

	/*
	 * spi mode
	 */
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
	{
		printf("can't set spi mode");
		return ret;
	}
		

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
	{
		printf("can't get spi mode");
		return ret;
	}

	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
	{
		printf("can't set bits per word");
		return ret;
	}

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
	{
		printf("can't get bits per word");
		return ret;
	}

	/*
	 * max speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
	{
		printf("can't set max speed hz");
		return ret;
	}

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
	{
		printf("can't get max speed hz");
		return ret;
	}

	printf("spi mode: %d\n", mode);
	printf("bits per word: %d\n", bits);
	printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);

	close(fd);
}

int platform_hal_init()
{
	int ret = 0;
	
	//init chip0 and chip1 device
	spi_init(chip0_device);
	
	// spi_init(chip1_device);
	
    set_chip_id(CHIP_ID_0);
	
	return ret;
}

void chip_writeCommand(uint16_t len, uint8_t *buffer)
{	
	int ret;
	int fd;
	
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)buffer,
		.rx_buf = 0,
		.len = len,
		.delay_usecs = delay,
		.speed_hz = speed,
		.bits_per_word = bits,
	};
	
	fd = open(current_spi_device, O_RDWR);
	if (fd < 0)
		printf("can't open %s\n", current_spi_device);

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1) {
		printf("can't send spi message. %d\n", ret);
		return;
	}
	
	#ifdef SUPPORTS_LOGS
	printf("%s SPI send: ====>", current_spi_device);
	for (ret = 0; ret < len; ret++) {
		if (!(ret % 16))
			puts("");
		printf("0x%.2x ", buffer[ret]);
	}
	puts("");
	#endif
	
	close(fd);
}

void si479x_readReply(uint16_t len, uint8_t *buffer)
{
	int ret;
	uint8_t tx[] = {
		0x00
	};
	int fd;
	static uint8_t rx[1028];
	
	int act_size = len <= 1024 ? len : 1024;
	
	struct spi_ioc_transfer tr = {
		.tx_buf = 0,
		.rx_buf = (unsigned long)rx,
		.len = act_size+1,
		.delay_usecs = delay,
		.speed_hz = speed,
		.bits_per_word = bits,
	};
	
	//write 0x00 to chip
	chip_writeCommand(1, tx);
	
	fd = open(current_spi_device, O_RDWR);

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1) {
		printf("can't send spi message. %d\n", ret);
		return;
	}
	
	for (ret=0; ret<act_size; ret++)
	{
		buffer[ret] = rx[ret+1];
	}

	#ifdef SUPPORTS_LOGS
	printf("%s SPI read: <====", current_spi_device);
	for (ret = 0; ret < act_size; ret++) {
		if (!(ret % 16))
			puts("");
		printf("0x%.2x ", buffer[ret]);
	}
	puts("");
	#endif
	
	close(fd);
}


