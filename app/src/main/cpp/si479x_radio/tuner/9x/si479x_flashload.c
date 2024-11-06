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

#include "type.h"

#include "si479x_internel.h"
#include "si479x_core.h"
#include "dc_bootup.h"
#include "si479x_flashload.h"
#include "si479x_commanddefs.h"

void _si479x_fill_cmd_uint16_t(uint8_t *buffer, uint8_t cmd_start, uint16_t u16data)
{
    buffer[cmd_start] = (uint8_t)u16data;
    buffer[cmd_start+1] = (uint8_t)(u16data >> 8);
}

void _si479x_fill_cmd_uint32_t(uint8_t *buffer, uint8_t cmd_start, uint32_t u32data)
{
    buffer[cmd_start] = (uint8_t)u32data;
    buffer[cmd_start+1] = (uint8_t)(u32data >> 8);
    buffer[cmd_start+2] = (uint8_t)(u32data >> 16);
    buffer[cmd_start+3] = (uint8_t)(u32data >> 24);
}

RET_CODE si479x_flash_load_image(uint32_t add, uint32_t size)
{
    RET_CODE ret = RET_SUCCESS;
    
    clear_cmd_memory();

    cmd_buff[0] = FLASH_LOAD;
    cmd_buff[1] = LOAD_IMG;

    _si479x_fill_cmd_uint32_t(cmd_buff, 4, add);
    _si479x_fill_cmd_uint32_t(cmd_buff, 8, size);

    ERR_CHECKING(si479x_command(12, cmd_buff, 4, rsp_buff));

    return ret;
}
#if 0
RET_CODE si479x_flash_load_image_check_crc32(uint32_t crc32, uint32_t add, uint32_t size)
{    
    RET_CODE ret = RET_SUCCESS;
    
    clear_cmd_memory();

    cmd_buff[0] = FLASH_LOAD;
    cmd_buff[1] = LOAD_IMG_CHECK_CRC32;

    _si479x_fill_cmd_uint32_t(cmd_buff, 4, crc32);
    _si479x_fill_cmd_uint32_t(cmd_buff, 8, add);
    _si479x_fill_cmd_uint32_t(cmd_buff, 12, size);

    ERR_CHECKING(si479x_command(16, cmd_buff, 8, rsp_buff));

    if (crc32 != *((uint32_t*)rsp_buff))
        return RET_ERROR;
    else
        return RET_SUCCESS;
}


RET_CODE si479x_flash_check_crc32(uint32_t crc32, uint32_t add, uint32_t size)
{
    RET_CODE ret = RET_SUCCESS;
    
    clear_cmd_memory();

    cmd_buff[0] = FLASH_LOAD;
    cmd_buff[1] = CHECK_CRC32;
    
    _si479x_fill_cmd_uint32_t(cmd_buff, 4, crc32);
    _si479x_fill_cmd_uint32_t(cmd_buff, 8, add);
    _si479x_fill_cmd_uint32_t(cmd_buff, 12, size);

    ERR_CHECKING(si479x_command(16, cmd_buff, 4, rsp_buff));
    
    if (crc32 != *((uint32_t*)rsp_buff))
        return RET_ERROR;
    else
        return RET_SUCCESS;
}



RET_CODE si479x_set_flash_prop_list(uint16_t prop_list[][2], uint16_t prop_cnt)
{
    RET_CODE ret = RET_SUCCESS;
    uint16_t i;
    
    clear_cmd_memory();

    cmd_buff[0] = FLASH_LOAD;
    cmd_buff[1] = SET_PROP_LIST;

    for (i=0; i<prop_cnt; i++)
    {
        _si479x_fill_cmd_uint16_t(cmd_buff, 4*(i+1), prop_list[i][0]);
        _si479x_fill_cmd_uint16_t(cmd_buff, 4*(i+1)+2, prop_list[i][1]);
    }

    ERR_CHECKING(si479x_command(4*(prop_cnt+1), cmd_buff, 6, rsp_buff));

    return ret;
}

uint16_t si479x_get_flash_prop(uint16_t prop_id)
{
    RET_CODE ret = RET_SUCCESS;
    
    clear_cmd_memory();

    cmd_buff[0] = FLASH_LOAD;
    cmd_buff[1] = SET_PROP_LIST;

    _si479x_fill_cmd_uint16_t(cmd_buff, 2, prop_id);

    ret = si479x_command(4, cmd_buff, 6, rsp_buff);
    
    if (ret != RET_SUCCESS)
        return 0xffff;

    return (*(uint16_t*)(&rsp_buff[4]));
}
#endif

#define HEX_0C 0x0c
#define HEX_ED 0xed

#define HEX_C0 0xc0
#define HEX_DE 0xde

RET_CODE si479x_read_block(uint32_t add, uint32_t size, uint8_t datalist[])
{
    RET_CODE ret = RET_SUCCESS;
    
    clear_cmd_memory();
	
	if (size > 512)
		return RET_INVALIDINPUT;
    
    cmd_buff[0] = FLASH_LOAD;
    cmd_buff[1] = READ_BLOCK;
    cmd_buff[2] = 0;
    cmd_buff[3] = 0;
    
    _si479x_fill_cmd_uint32_t(cmd_buff, 4, add);
    _si479x_fill_cmd_uint32_t(cmd_buff, 8, size);

    ERR_CHECKING(si479x_command(12, cmd_buff, 4+size, datalist));

    return ret;
}

RET_CODE si479x_write_block(uint32_t add, uint32_t size, uint8_t datalist[])
{
    RET_CODE ret = RET_SUCCESS;
    uint32_t i;
    	
    clear_cmd_memory();

    datalist[0] = FLASH_LOAD;
    datalist[1] = WRITE_BLOCK;
    datalist[2] = HEX_0C;
    datalist[3] = HEX_ED;
	
	_si479x_fill_cmd_uint32_t(datalist, 4, 0);
	_si479x_fill_cmd_uint32_t(datalist, 8, add);
	_si479x_fill_cmd_uint32_t(datalist, 12, size);
	
    ERR_CHECKING(si479x_command(size+16, datalist, 4, rsp_buff));

    return ret;
}


#if 0
RET_CODE si479x_write_block_readback_verify(uint32_t crc32, uint32_t add, uint32_t size, uint32_t datalist[])
{
    RET_CODE ret = RET_SUCCESS;
    uint32_t i;
    
    clear_cmd_memory();

    cmd_buff[0] = FLASH_LOAD;
    cmd_buff[1] = WRITE_BLOCK_READBACK_VERIFY;
    cmd_buff[2] = HEX_0C;
    cmd_buff[3] = HEX_ED;
    
    _si479x_fill_cmd_uint32_t(cmd_buff, 4, crc32);
    _si479x_fill_cmd_uint32_t(cmd_buff, 8, add);
    _si479x_fill_cmd_uint32_t(cmd_buff, 12, size);

    for (i=0; i<size; i++)
    {
        _si479x_fill_cmd_uint32_t(cmd_buff, 4*(i+4), datalist[i]);
    }

    ERR_CHECKING(si479x_command((size+4)*4, cmd_buff, 4, rsp_buff));

    return ret;
}


RET_CODE si479x_write_block_packet_verify(uint32_t crc32, uint32_t add, uint32_t size, uint32_t datalist[])
{
    RET_CODE ret = RET_SUCCESS;
    uint32_t i;
    
    clear_cmd_memory();

    cmd_buff[0] = FLASH_LOAD;
    cmd_buff[1] = WRITE_BLOCK_PACKET_VERIFY;
    cmd_buff[2] = HEX_0C;
    cmd_buff[3] = HEX_ED;
    
    _si479x_fill_cmd_uint32_t(cmd_buff, 4, crc32);
    _si479x_fill_cmd_uint32_t(cmd_buff, 8, add);
    _si479x_fill_cmd_uint32_t(cmd_buff, 12, size);

    for (i=0; i<size; i++)
    {
        _si479x_fill_cmd_uint32_t(cmd_buff, 4*(i+4), datalist[i]);
    }

    ERR_CHECKING(si479x_command((size+4)*4, cmd_buff, 4, rsp_buff));

    return ret;
}



RET_CODE si479x_write_block_verify(uint32_t crc32, uint32_t add, uint32_t size, uint32_t datalist[])
{
    RET_CODE ret = RET_SUCCESS;
    uint32_t i;
    
    clear_cmd_memory();

    cmd_buff[0] = FLASH_LOAD;
    cmd_buff[1] = WRITE_BLOCK_VERIFY;
    cmd_buff[2] = HEX_0C;
    cmd_buff[3] = HEX_ED;
    
    _si479x_fill_cmd_uint32_t(cmd_buff, 4, crc32);
    _si479x_fill_cmd_uint32_t(cmd_buff, 8, add);
    _si479x_fill_cmd_uint32_t(cmd_buff, 12, size);

    for (i=0; i<size; i++)
    {
        _si479x_fill_cmd_uint32_t(cmd_buff, 4*(i+4), datalist[i]);
    }

    ERR_CHECKING(si479x_command((size+4)*4, cmd_buff, 4, rsp_buff));

    return ret;
}
#endif


RET_CODE si479x_write_sector_4096(uint16_t sector, uint8_t cmd_n_data[])
{
    RET_CODE ret = RET_SUCCESS;

    cmd_n_data[0] = FLASH_LOAD;
    cmd_n_data[1] = WRITE_SECTOR_4096;
    cmd_n_data[2] = (uint8_t)sector;
    cmd_n_data[3] = (uint8_t)(sector>>8);

    ERR_CHECKING(si479x_command(4100, cmd_n_data, 4, rsp_buff));

    return ret;
}

RET_CODE si479x_erase_sector(uint32_t add)
{
    RET_CODE ret = RET_SUCCESS;
    
    clear_cmd_memory();

    cmd_buff[0] = FLASH_LOAD;
    cmd_buff[1] = ERASE_SECTOR;
    cmd_buff[2] = HEX_C0;
    cmd_buff[3] = HEX_DE;
    
    _si479x_fill_cmd_uint32_t(cmd_buff, 4, add);

    ERR_CHECKING(si479x_command(8, cmd_buff, 4, rsp_buff));

    return ret;
}

#if 0
RET_CODE si479x_erase_chip()
{
    RET_CODE ret = RET_SUCCESS;
    
    clear_cmd_memory();

    cmd_buff[0] = FLASH_LOAD;
    cmd_buff[1] = ERASE_CHIP;
    cmd_buff[2] = HEX_DE;
    cmd_buff[3] = HEX_C0;
    
    ERR_CHECKING(si479x_command(4, cmd_buff, 4, rsp_buff));

    return ret;
}

#endif

/**
RET_CODE si479x_xxxx()
{
    RET_CODE ret = RET_SUCCESS;
    uint16_t i;
    
    clear_cmd_memory();

    cmd_buff[0] = XXXX;

    ERR_CHECKING(si479x_command(x, cmd_buff, x, rsp_buff));

    return ret;
}
*/


