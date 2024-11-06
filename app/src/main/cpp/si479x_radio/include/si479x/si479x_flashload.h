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

/**
 *@file this header file define the flashsubcommand/property address
 *
 * for si479x headunit.
 *
 * @author Michael.Yi@silabs.com
 */

#ifndef _SI479X_FLASH_LOAD_H_
#define _SI479X_FLASH_LOAD_H_

#define LOAD_IMG 0x00 // Load a boot_img from flash with no CRC32 check
#define LOAD_IMG_CHECK_CRC32 0x01 // Load a boot_img from flash with CRC32 check
#define CHECK_CRC32 0x02 // CRC32 check only, no load of boot_img into memories

#define SET_PROP_LIST 0x10 // Process the property list.
#define GET_PROP 0x11 // Returns a property value.

#define READ_BLOCK 0xe0

#define WRITE_BLOCK 0xf0 // Write a block of bytes to the flash
#define WRITE_BLOCK_READBACK_VERIFY 0xf1 // Write a block of bytes to the flash with read back verification
#define WRITE_BLOCK_PACKET_VERIFY 0xf2 // Write a block of bytes to the flash with packet verification
#define WRITE_BLOCK_VERIFY 0xf3 // Write a block of bytes to the flash with full verification
#define WRITE_SECTOR_4096 0xf4 // Write a block of data to the flash

#define ERASE_SECTOR 0xfe // Erase a sector from the flash.
#define ERASE_CHIP 0xff // Erase the entire flash.

#define FLASH_LOAD_SPI_CLOCK_FREQ_MHZ 0x0001 // SPI Clock frequency
#define FLASH_LOAD_SPI_SPI_MODE 0x0002 // SPI Mode
#define FLASH_LOAD_SPI_SPI_DUAL_READ 0x0003 // SPI dual read

#define FLASH_LOAD_READ_READ_CMD 0x0101 // Flash read command
#define FLASH_LOAD_READ_HIGH_SPEED_READ_CMD 0x0102 // Flash fast or high speed read command

#define FLASH_LOAD_WRITE_WRITE_CMD 0x0201 // Flash write command
#define FLASH_LOAD_WRITE_ERASE_SECTOR_CMD 0x0202 // Flash erase sector command
#define FLASH_LOAD_WRITE_ERASE_SECTOR_SIZE 0x0203 // Flash erase sector size
#define FLASH_LOAD_WRITE_ERASE_CHIP 0x0204 // Flash erase chip command

RET_CODE si479x_flash_load_image(uint32_t add, uint32_t size);
RET_CODE si479x_flash_load_image_check_crc32(uint32_t crc32, uint32_t add, uint32_t size);
RET_CODE si479x_flash_check_crc32(uint32_t crc32, uint32_t add, uint32_t size);

RET_CODE si479x_set_flash_prop_list(uint16_t prop_list[][2], uint16_t prop_cnt);
uint16_t si479x_get_flash_prop(uint16_t prop_id);

RET_CODE si479x_read_block(uint32_t add, uint32_t size, uint8_t datalist[]);

RET_CODE si479x_write_block(uint32_t add, uint32_t size, uint8_t datalist[]);
RET_CODE si479x_write_block_readback_verify(uint32_t crc32, uint32_t add, uint32_t size, uint32_t datalist[]);
RET_CODE si479x_write_block_packet_verify(uint32_t crc32, uint32_t add, uint32_t size, uint32_t datalist[]);
RET_CODE si479x_write_block_verify(uint32_t crc32, uint32_t add, uint32_t size, uint32_t datalist[]);
RET_CODE si479x_write_sector_4096(uint16_t sector, uint8_t cmd_n_data[]);
RET_CODE si479x_erase_sector(uint32_t add);
RET_CODE si479x_erase_chip();

#endif
