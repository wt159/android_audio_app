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
#include "si479x_internel.h"
#include "si479x_core.h"
#include "si479x_commanddefs.h"
#include "si479x_propertydefs.h"
#include "si479x_flashload.h"
#include "si479x_hifi.h"
#include "si479x_audio.h"
#ifdef SUPPORT_RDS
#include "si479x_rds.h"
#endif
#include "dc_bootup.h"

#include <string.h>

#include <stdio.h>

uint8_t cmd_buff[CMD_BUFF_SIZE];
uint8_t rsp_buff[RSQ_BUFF_SIZE];

#if SUPPORT_HOST_BITENDIAN
void _swap_16 (uint16_t *pValue)
{
    uint16_t temp = *pValue;

    ((uint8_t *)pValue)[0] = ((uint8_t *)&temp)[1];
    ((uint8_t *)pValue)[1] = ((uint8_t *)&temp)[0];
}

void _swap_32 (uint32_t *pValue)
{
    uint16_t byte12 = *(uint16_t *)pValue;
    uint16_t byte34 = *((uint16_t *)pValue+1);

    ((uint8_t *)pValue)[0] = ((uint8_t *)&byte34)[1];
    ((uint8_t *)pValue)[1] = ((uint8_t *)&byte34)[0];
    
    ((uint8_t *)pValue)[2] = ((uint8_t *)&byte12)[1];
    ((uint8_t *)pValue)[3] = ((uint8_t *)&byte12)[0];
}
#endif

//just clear the top 16 bytes memory
void clear_cmd_memory()
{
    memset(cmd_buff, 0, CMD_BUFF_SIZE);
}

#if 0
uint8_t _si479x_getCommandRequestStatus(uint8_t cmd)
{
    POWERUP_STATE state = POWERUP_APPLICATION;
    
    switch (cmd) 
    {
        case POWER_UP:
            state = POWERUP_RESET;
            break;
        case HOST_LOAD:
        case LOAD_INIT:
        case LOAD_CONFIG:
            state = POWERUP_BOOTLOADER;
            break;
        case BOOT:
            state = POWERUP_BOOTLOADER | POWERUP_BOOTREADY;
            break;
        case FLASH_LOAD:
        case GET_PART_INFO:
            state = POWERUP_BOOTLOADER | POWERUP_BOOTREADY | POWERUP_APPLICATION;
            break;
        default:
            state = POWERUP_APPLICATION;
            break;
    }
    
    return state;
}

RET_CODE _si479x_checkPowerupStatus(uint8_t state)
{
    uint8_t ts;
    
    si479x_readReply(4, rsp_buff);

    ts = 1 << ((rsp_buff[3] >> 6) & 0x03);
    if ((ts & state) == ts)
    {
        return RET_SUCCESS;
    }
    else
    {
        return RET_STATEERROR;
    }
}
#endif

RET_CODE si479x_waitForSTC(uint8_t tunerid)
{
    uint16_t i = 1000;

	do
	{
		si479x_readReply(4, rsp_buff);
        
        if ((rsp_buff[tunerid] & STCINT) == STCINT)
        {
            return RET_SUCCESS;
        }

        if ((rsp_buff[0] & 0x60) != 0 ||
            (rsp_buff[3] & 0x3f) != 0)
        {
            return RET_ERROR;
        }
            
        sleep_ms(1);
	}while (--i);

    return RET_TIMEOUT;
}


RET_CODE si479x_waitForCTS()
{
    uint16_t i = 1000;

	do
	{
		si479x_readReply(4, rsp_buff);
        
        if ((rsp_buff[0] & CTS) == CTS)
        {
            return RET_SUCCESS;
        }

        if ((rsp_buff[0] & 0x60) != 0 ||
            (rsp_buff[3] & 0x3f) != 0)
        {
            return RET_ERROR;
        }
            
        sleep_ms(1);
	}while (--i);

	printf("CTS Timeout!\n");

    return RET_TIMEOUT;
}

RET_CODE si479x_command(uint16_t cmd_size, uint8_t *cmd, uint16_t reply_size, uint8_t *reply)
{
    RET_CODE ret = RET_SUCCESS;
    uint8_t state = 0;

    if (cmd_size == 0)
        return RET_INVALIDINPUT;
    
//    state = _si479x_getCommandRequestStatus(cmd[0]);
//    ERR_CHECKING(_si479x_checkPowerupStatus(state));

    ERR_CHECKING(si479x_waitForCTS());

    chip_writeCommand(cmd_size, cmd);

    if (reply_size != 0)
    {
        ERR_CHECKING(si479x_waitForCTS());
        si479x_readReply(reply_size, reply);
    }

    return ret;
}


//boot command

RET_CODE si479x_powerup(const power_up_args_t *pPowerup_args)
{
    RET_CODE ret = RET_SUCCESS;

    if (pPowerup_args == NULL)
    {
        return RET_INVALIDINPUT;
    }
        
    clear_cmd_memory();
    
    cmd_buff[0] = POWER_UP;

    if (pPowerup_args->CTSIEN)
        cmd_buff[1] |= (pPowerup_args->CTSIEN & 0x01) << 7;

    if (pPowerup_args->CLKO_CURRENT)
        cmd_buff[1] |= (pPowerup_args->CLKO_CURRENT & 0x07) << 4;
    
    if (pPowerup_args->VIO)
        cmd_buff[1] |= (pPowerup_args->VIO & 0x01) << 2;
    
    if (pPowerup_args->CLKOUT)
        cmd_buff[1] |= (pPowerup_args->CLKOUT & 0x01) << 1;

    if (pPowerup_args->CLK_MODE)
        cmd_buff[2] |= (pPowerup_args->CLK_MODE & 0x0f) << 4;

    if (pPowerup_args->TR_SIZE)
        cmd_buff[2] |= (pPowerup_args->TR_SIZE & 0x0f);

    if (pPowerup_args->CTUN)
        cmd_buff[3] |= (pPowerup_args->CTUN & 0x3f);

    cmd_buff[4] = (uint8_t) (pPowerup_args->XTAL_FREQ);
    cmd_buff[5] = (uint8_t) (pPowerup_args->XTAL_FREQ >> 8);
    cmd_buff[6] = (uint8_t) (pPowerup_args->XTAL_FREQ >> 16);
    cmd_buff[7] = (uint8_t) (pPowerup_args->XTAL_FREQ >> 24);

    cmd_buff[8] = pPowerup_args->IBIAS;

    if (pPowerup_args->AFS)
        cmd_buff[9] |= ((pPowerup_args->AFS & 0x01) << 7);

    if (pPowerup_args->DIGDAT)
        cmd_buff[9] |= ((pPowerup_args->DIGDAT & 0x01) << 4);
    
    if (pPowerup_args->CHIPID)
        cmd_buff[10] |= (pPowerup_args->CHIPID & 0x07);
        
    if (pPowerup_args->ISFSMODE)
        cmd_buff[20] |= (pPowerup_args->ISFSMODE & 0x03);

    if (pPowerup_args->ICLINK)
        cmd_buff[21] |= (pPowerup_args->ICLINK & 0x7F);

    if (pPowerup_args->IQCHANNELS)
        cmd_buff[22] |= (pPowerup_args->IQCHANNELS & 0x03) << 6;

    if (pPowerup_args->IQFMT)
        cmd_buff[22] |= (pPowerup_args->IQFMT & 0x03) << 4;

    if (pPowerup_args->IQMUX)
        cmd_buff[22] |= (pPowerup_args->IQMUX & 0x01) << 3;

    if (pPowerup_args->IQRATE)
        cmd_buff[22] |= (pPowerup_args->IQRATE & 0x07);    

    if (pPowerup_args->IQSWAP1)
        cmd_buff[23] |= (pPowerup_args->IQSWAP1 & 0x01) << 7;

    if (pPowerup_args->IQSWAP0)
        cmd_buff[23] |= (pPowerup_args->IQSWAP0 & 0x01) << 6;

    if (pPowerup_args->IQSLOT)
        cmd_buff[23] |= (pPowerup_args->IQSLOT & 0x07) << 3;

    if (pPowerup_args->IQEARLY)
        cmd_buff[23] |= (pPowerup_args->IQEARLY & 0x01) << 2;

    if (pPowerup_args->IQOUT)
        cmd_buff[23] |= (pPowerup_args->IQOUT & 0x03);

    cmd_buff[0x18] = 0x11;

    ERR_CHECKING(si479x_command(0x18+1, cmd_buff, 6, rsp_buff));
    
    sleep_ms(5);

    return ret;
}

RET_CODE si479x_loader_init(uint8_t guid, uint32_t _guid_table[])
{
    RET_CODE ret = RET_SUCCESS;
    uint8_t cmd_leng = 0;
    uint8_t i;
    
    clear_cmd_memory();
    
    cmd_buff[0] = LOAD_INIT;
    cmd_buff[1] = guid;

    for (i=0; i<guid; i++)
    {
        _si479x_fill_cmd_uint32_t(cmd_buff, 4+i*4, _guid_table[i]);
    }

    cmd_leng = 4 + guid*4;

    ERR_CHECKING(si479x_command(cmd_leng, cmd_buff, 4, rsp_buff));

    return ret;
}

RET_CODE si479x_boot()
{
    RET_CODE ret = RET_SUCCESS;
    
    clear_cmd_memory();
    
    cmd_buff[0] = BOOT;

    ERR_CHECKING(si479x_command(2, cmd_buff, 4, rsp_buff));

    return ret;
}


RET_CODE si479x_get_part_info(part_info_t * p_part_info)
{
    RET_CODE ret = RET_SUCCESS;

    if (NULL == p_part_info)
        return RET_INVALIDINPUT;
    
    clear_cmd_memory();
    
    cmd_buff[0] = GET_PART_INFO;

    ERR_CHECKING(si479x_command(2, cmd_buff, 20, rsp_buff));

    p_part_info->chipRW = rsp_buff[4];
    p_part_info->ROMId = rsp_buff[5];
    p_part_info->PartNum = (((uint16_t)rsp_buff[8] << 8) & 0xff00) | (((uint16_t)rsp_buff[9]) & 0xff);
    p_part_info->Major = rsp_buff[10];

    return ret;
}

#define LOAD_CONFIG_LENGTH_MAX 0x68
RET_CODE si479x_load_config(load_config_t *p_load_config_t)
{
    RET_CODE ret = RET_SUCCESS;
    int bufOffset = 0;
    
    clear_cmd_memory();

    //Error check for bit values higher than 1
    if((p_load_config_t->icn_in > 1) || (p_load_config_t->nvmosi_out > 1))
    {
        return RET_INVALIDINPUT;
    }

	// Load_Config
	//************************************************
	cmd_buff[0] = LOAD_CONFIG;
	cmd_buff[1] = p_load_config_t->numchips;
    cmd_buff[2] = p_load_config_t->chipid;
    cmd_buff[3] = p_load_config_t->role;

	// FLOWCTRLIN
	cmd_buff[0x04] = 0;
	cmd_buff[0x05] = 0;
	cmd_buff[0x06] = 0;
	cmd_buff[0x07] = 0;
	cmd_buff[0x08] = 0;
	cmd_buff[0x09] = ((p_load_config_t->icn_in & 0x01) << 2);
	cmd_buff[0x0a] = 0;
	cmd_buff[0x0b] = 0;

	// FLOWCTRLOUT
	cmd_buff[0x0c] = ((p_load_config_t->nvmosi_out & 0x01)  << 2);
	cmd_buff[0x0d] = 0;
	cmd_buff[0x0e] = 0;
	cmd_buff[0x0f] = 0;
	cmd_buff[0x10] = 0;
	cmd_buff[0x11] = 0;
	cmd_buff[0x12] = 0;
	cmd_buff[0x13] = 0;

	bufOffset = 0x14;
	// clksel8
	cmd_buff[bufOffset++] = (uint8_t)(p_load_config_t->clksel8 & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->clksel8 >> 8) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->clksel8 >> 16) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->clksel8 >> 24) & 0xFF);

	// spicfg3
	cmd_buff[bufOffset++] = (uint8_t)(p_load_config_t->spicfg3 & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->spicfg3 >> 8) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->spicfg3 >> 16) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->spicfg3 >> 24) & 0xFF);

	// drvdel
	cmd_buff[bufOffset++] = (uint8_t)(p_load_config_t->drvdel & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->drvdel >> 8) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->drvdel >> 16) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->drvdel >> 24) & 0xFF);

	// capdel
	cmd_buff[bufOffset++] = (uint8_t)(p_load_config_t->capdel & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->capdel >> 8) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->capdel >> 16) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->capdel >> 24) & 0xFF);

	// spisctrl
	cmd_buff[bufOffset++] = (uint8_t)(p_load_config_t->spisctrl & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->spisctrl >> 8) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->spisctrl >> 16) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->spisctrl >> 24) & 0xFF);

	// nvsclk
	cmd_buff[bufOffset++] = (uint8_t)(p_load_config_t->nvsclk & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->nvsclk >> 8) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->nvsclk >> 16) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->nvsclk >> 24) & 0xFF);

	// nvssb
	cmd_buff[bufOffset++] = (uint8_t)(p_load_config_t->nvssb & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->nvssb >> 8) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->nvssb >> 16) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->nvssb >> 24) & 0xFF);

	// nvmosi
	cmd_buff[bufOffset++] = (uint8_t)(p_load_config_t->nvmosi & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->nvmosi >> 8) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->nvmosi >> 16) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->nvmosi >> 24) & 0xFF);

	// nvmiso
	cmd_buff[bufOffset++] = (uint8_t)(p_load_config_t->nvmiso & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->nvmiso >> 8) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->nvmiso >> 16) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->nvmiso >> 24) & 0xFF);

	// diclk0
	cmd_buff[bufOffset++] = (uint8_t)(p_load_config_t->diclk0 & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->diclk0 >> 8) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->diclk0 >> 16) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->diclk0 >> 24) & 0xFF);

	// difs0
	cmd_buff[bufOffset++] = (uint8_t)(p_load_config_t->difs0 & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->difs0 >> 8) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->difs0 >> 16) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->difs0 >> 24) & 0xFF);

	// icn
	cmd_buff[bufOffset++] = (uint8_t)(p_load_config_t->icn & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->icn >> 8) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->icn >> 16) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->icn >> 24) & 0xFF);

	// icp
	cmd_buff[bufOffset++] = (uint8_t)(p_load_config_t->icp & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->icp >> 8) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->icp >> 16) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->icp >> 24) & 0xFF);

	// debugp
	cmd_buff[bufOffset++] = (uint8_t)(p_load_config_t->debugp & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->debugp >> 8) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->debugp >> 16) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->debugp >> 24) & 0xFF);

	// debugn
	cmd_buff[bufOffset++] = (uint8_t)(p_load_config_t->debugn & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->debugn >> 8) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->debugn >> 16) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->debugn >> 24) & 0xFF);

	// isfsp
	cmd_buff[bufOffset++] = (uint8_t)(p_load_config_t->isfsp & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->isfsp >> 8) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->isfsp >> 16) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->isfsp >> 24) & 0xFF);

	// isfsn
	cmd_buff[bufOffset++] = (uint8_t)(p_load_config_t->isfsn & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->isfsn >> 8) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->isfsn >> 16) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->isfsn >> 24) & 0xFF);

	// zifiout
	cmd_buff[bufOffset++] = (uint8_t)(p_load_config_t->zifiout & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->zifiout >> 8) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->zifiout >> 16) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->zifiout >> 24) & 0xFF);

	// zifclk
	cmd_buff[bufOffset++] = (uint8_t)(p_load_config_t->zifclk & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->zifclk >> 8) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->zifclk >> 16) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->zifclk >> 24) & 0xFF);

	// zifqout
	cmd_buff[bufOffset++] = (uint8_t)(p_load_config_t->zifqout & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->zifqout >> 8) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->zifqout >> 16) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->zifqout >> 24) & 0xFF);

	// ziffs
	cmd_buff[bufOffset++] = (uint8_t)(p_load_config_t->ziffs & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->ziffs >> 8) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->ziffs >> 16) & 0xFF);
	cmd_buff[bufOffset++] = (uint8_t)((p_load_config_t->ziffs >> 24) & 0xFF);

    ERR_CHECKING(si479x_command(LOAD_CONFIG_LENGTH_MAX, cmd_buff, 0, rsp_buff));

	return ret;
}


RET_CODE si479x_get_func_info(func_info_t *p_si479x_func_info)
{
    RET_CODE ret = RET_SUCCESS;
    uint8_t i;
    
	if (p_si479x_func_info == NULL)
		return RET_INVALIDINPUT;
	
    clear_cmd_memory();
    
    cmd_buff[0] = GET_FUNC_INFO;

    ERR_CHECKING(si479x_command(2, cmd_buff, 22, rsp_buff));

    for (i=0; i<5; i++)
    {
        p_si479x_func_info->VERINFO[i] = rsp_buff[4+i];
    }

    for (i=0; i<4; i++)
    {
        p_si479x_func_info->HIFIINFO[i] = rsp_buff[9+i];
    }
    
    for (i=0; i<4; i++)
    {
        p_si479x_func_info->CUSTINFO[i] = rsp_buff[13+i];
    }

    p_si479x_func_info->SVNINFO = rsp_buff[17];

    p_si479x_func_info->SVNID = (((uint32_t)rsp_buff[18])&0xff) |
                               ((((uint32_t)rsp_buff[19])<<8)&0xff00) |
                               ((((uint32_t)rsp_buff[20]<<16))&0xff0000) |
                               ((((uint32_t)rsp_buff[21]<<24))&0xff000000);
    
    return ret;
}

RET_CODE si479x_set_property(uint16_t prop_id, uint16_t prop_value)
{
    RET_CODE ret = RET_SUCCESS;
    
    clear_cmd_memory();
    
    cmd_buff[0] = SET_PROPERTY;

    cmd_buff[2] = (uint8_t)prop_id;
    cmd_buff[3] = (uint8_t)(prop_id >> 8);
    
    cmd_buff[4] = (uint8_t)prop_value;
    cmd_buff[5] = (uint8_t)(prop_value >> 8);
    
    ERR_CHECKING(si479x_command(6, cmd_buff, 4, rsp_buff));

    return ret;
}

RET_CODE si479x_agc_debug_set_property(uint16_t prop_id, uint16_t prop_value)
{
    RET_CODE ret = RET_SUCCESS;
    
    clear_cmd_memory();
    
    cmd_buff[0] = AGC_DEBUG;
    
    cmd_buff[1] = 0X02; //SET PROPERTY

    cmd_buff[2] = (uint8_t)prop_id;
    cmd_buff[3] = (uint8_t)(prop_id >> 8);
    
    cmd_buff[4] = (uint8_t)prop_value;
    cmd_buff[5] = (uint8_t)(prop_value >> 8);
    
    ERR_CHECKING(si479x_command(6, cmd_buff, 4, rsp_buff));

    return ret;
}

RET_CODE si479x_stauts_datalist(uint8_t cmd, uint8_t para, uint8_t maxlength, uint8_t buflength, uint8_t datalist[])
{
    RET_CODE ret = RET_SUCCESS;
    uint8_t totallength = buflength <= maxlength ? buflength : maxlength;
    uint8_t i;
   
    clear_cmd_memory();
    
    cmd_buff[0] = cmd;
    cmd_buff[1] = para;
    
    ERR_CHECKING(si479x_command(2, cmd_buff, maxlength+4, rsp_buff));

    for (i=0; i<totallength; i++)
    {
        datalist[i] = rsp_buff[4+i];
    }
       
    return ret;
}


RET_CODE si479x_fm_phase_div(uint8_t audio_output, uint8_t combile_mode, int8_t weight)
{
    RET_CODE ret = RET_SUCCESS;
    
    clear_cmd_memory();
    
    cmd_buff[0] = FM_PHASE_DIVERSITY;
    cmd_buff[1] = (audio_output & 0x03) << 4 | (combile_mode & 0x07);
	cmd_buff[2] = weight;

    ERR_CHECKING(si479x_command(3, cmd_buff, 4, rsp_buff));

	return ret;
}

RET_CODE si479x_fm_phase_div_datalist(uint8_t length, uint8_t datalist[])
{
    return si479x_stauts_datalist(FM_PHASE_DIV_STATUS, 0, 2, length, datalist);
}

uint8_t si479x_agc(uint8_t agcIndex)
{
    if (agcIndex >= 18)
        return 0xff;
   
    clear_cmd_memory();
    
    cmd_buff[0] = AGC_STATUS;
    cmd_buff[1] = 0;
    
    si479x_command(2, cmd_buff, AGC_STATUS_RET_DATA_LENGTH, rsp_buff);

    return rsp_buff[7+agcIndex];
}


RET_CODE si479x_agc_stauts_datalist(uint8_t length, uint8_t datalist[])
{
    return si479x_stauts_datalist(AGC_STATUS, 0, AGC_STATUS_RET_DATA_LENGTH-4, length, datalist);
}

#ifdef SUPPORT_FMAMDAB
RET_CODE si479x_dab_set_freq_list(uint8_t count, uint32_t* freqlist)
{
    RET_CODE ret = RET_SUCCESS;
	uint8_t i;
	
    clear_cmd_memory();
		
    cmd_buff[0] = DAB_SET_FREQ_LIST;
    cmd_buff[1] = count;
	
	for (i=0; i<count; i++)
	{
		_si479x_fill_cmd_uint32_t(cmd_buff, 4+4*i, freqlist[i]);
	}

    ERR_CHECKING(si479x_command(4+4*count, cmd_buff, 4, rsp_buff));
    
    return ret;	
}
#endif



