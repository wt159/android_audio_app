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
#include "si479x_flashload.h"
#include "si479x_hifi.h"
#include "si479x_audio.h"
#include "dc_bootup.h"

#include <string.h>

#if 0
RET_CODE si479x_pin_drive(PIN_GROUP group, pin_des_t pinList[4])
{
    RET_CODE ret = RET_SUCCESS;
    uint8_t i;
    
    clear_cmd_memory();
    
    cmd_buff[0] = PIN_DRIVE;
    cmd_buff[1] = (((uint8_t)group) << 4) & 0xf0;

    for (i=0; i<4; i++)
    {
        cmd_buff[2+i*2] = pinList[i].digdrive & 0x0f;
        cmd_buff[3+i*2] = ((pinList[i].lvdsload & 0x07) << 5) |
            (pinList[i].lvdsdrive & 0x1f);
    }
    
    ERR_CHECKING(si479x_command(10, cmd_buff, 4, rsp_buff));

    return ret;
}

RET_CODE si479x_pin_status(PIN_GROUP group, uint8_t pins[][8])
{
    RET_CODE ret = RET_SUCCESS;
    uint8_t i;
    uint8_t numpins;
    
    clear_cmd_memory();
    
    cmd_buff[0] = PIN_STATUS;
    cmd_buff[1] = (((uint8_t)group) << 4) & 0xf0;

    ERR_CHECKING(si479x_command(2, cmd_buff, 8, rsp_buff));
    
    return ret;
}

RET_CODE si479x_pin_config_hdblend(uint8_t pin_id, uint8_t options)
{
    RET_CODE ret = RET_SUCCESS;
    
    clear_cmd_memory();
    
    cmd_buff[0] = PIN_CONFIG_HDBLEND;
    cmd_buff[2] = pin_id;
    cmd_buff[3] = (options << 7) & 0x80;

    ERR_CHECKING(si479x_command(4, cmd_buff, 4, rsp_buff));

    return ret;
}

#endif


//audio command
RET_CODE si479x_DAC_config(uint8_t DAC01_MODE, 
    uint8_t DAC23_MODE, uint8_t DAC45_MODE, uint8_t slow)
{
    RET_CODE ret = RET_SUCCESS;
    
    clear_cmd_memory();
    
    cmd_buff[0] = DAC_CONFIG;
    cmd_buff[1] = DAC01_MODE & 0x7;
    cmd_buff[2] = DAC23_MODE & 0x7;
    cmd_buff[3] = DAC45_MODE & 0x7;
    
    if (slow != 0)
    {
        cmd_buff[1] |= 0x10; 
        cmd_buff[2] |= 0x10; 
        cmd_buff[3] |= 0x10; 
    }

    ERR_CHECKING(si479x_command(4, cmd_buff, 4, rsp_buff));
    
    return ret;
}

RET_CODE si479x_ADC_connect(uint8_t ADC_CHANNEL, 
    uint8_t SOURCE_PINS, ADC_GAINS GAIN)
{
    RET_CODE ret = RET_SUCCESS;
    
    clear_cmd_memory();
    
    cmd_buff[0] = ADC_CONNECT;
    cmd_buff[1] = ADC_CHANNEL & 0x0f;
    cmd_buff[2] = SOURCE_PINS & 0x0f;
    cmd_buff[3] = GAIN & 0x0f;

    ERR_CHECKING(si479x_command(4, cmd_buff, 4, rsp_buff));
        
    return ret;
}


RET_CODE si479x_ADC_disconnect(uint8_t ADC_CHANNEL)
{
    RET_CODE ret = RET_SUCCESS;
    
    clear_cmd_memory();
    
    cmd_buff[0] = ADC_DISCONNECT;
    cmd_buff[1] = ADC_CHANNEL & 0x0f;

    ERR_CHECKING(si479x_command(2, cmd_buff, 4, rsp_buff));
        
    return ret;
}

#if 0
RET_CODE si479x_ADC_status(adc_status_t adc_set[ADC_CHANNEL_COUNT])
{
    RET_CODE ret = RET_SUCCESS;
    uint8_t i;
    
    clear_cmd_memory();
    
    cmd_buff[0] = ADC_STATUS;

    ERR_CHECKING(si479x_command(2, cmd_buff, 12, rsp_buff));

    for (i=0; i<ADC_CHANNEL_COUNT; i++)
    {
        adc_set[i].source_pins = cmd_buff[4 + i*2] & 0x0f;
        adc_set[i].gain = cmd_buff[5 + i*2] & 0x0f;
    }
        
    return ret;
}
#endif

static RET_CODE si479x_I2STX_config(uint8_t cmd, i2s_config *pCfg)
{
    RET_CODE ret = RET_SUCCESS;

	if (pCfg == NULL)
	{
		return RET_INVALIDINPUT;
	}
    
    clear_cmd_memory();
    
    cmd_buff[0] = cmd;
	cmd_buff[1] = pCfg->sample_rate;
	cmd_buff[2] = pCfg->purpose & 0x0f;
	cmd_buff[3] = ((pCfg->master & 0x1) << 4) | (pCfg->clkfs & 0xf);
	cmd_buff[4] = (pCfg->data0 & 0x0f);
	cmd_buff[5] = ((pCfg->fill & 0x03) << 6) | ((pCfg->slot_size & 0x07) << 3 ) |(pCfg->sample_size & 0x07);
    cmd_buff[6] = ((pCfg->bitorder & 0x01) << 7) | 
        ((pCfg->swap & 0x01) << 6) |
        ((pCfg->clkinv & 0x01) << 5) |
        (pCfg->frame_mode &0x0f);
    cmd_buff[7] = (pCfg->clock_domain & 0x07) << 5;
    cmd_buff[8] = ((pCfg->hiz & 0x07) << 5) | (pCfg->length & 0x1f);
    cmd_buff[10] = pCfg->data1;

    ERR_CHECKING(si479x_command(11, cmd_buff, 4, rsp_buff));
        
    return ret;
}

static RET_CODE si479x_I2SRX_config(uint8_t cmd, i2s_config *pCfg)
{
    RET_CODE ret = RET_SUCCESS;

	if (pCfg == NULL)
	{
		return RET_INVALIDINPUT;
	}
    
    clear_cmd_memory();
    
    cmd_buff[0] = cmd;
	cmd_buff[1] = pCfg->sample_rate;
	cmd_buff[2] = pCfg->purpose & 0x0f;
	cmd_buff[3] = (pCfg->master & 0x1) << 4 | (pCfg->clkfs & 0xf);
	cmd_buff[4] = pCfg->data0 & 0x3f;
	cmd_buff[5] = ((pCfg->fill & 0x03) << 6) | ((pCfg->slot_size & 0x07) << 3 ) |(pCfg->sample_size & 0x07);
    cmd_buff[6] = ((pCfg->bitorder & 0x01) << 7) | 
        ((pCfg->swap & 0x01) << 6) |
        ((pCfg->clkinv & 0x01) << 5) |
        (pCfg->frame_mode &0x0f);
    cmd_buff[7] = (pCfg->clock_domain & 0x07) << 5;

    ERR_CHECKING(si479x_command(11, cmd_buff, 4, rsp_buff));
        
    return ret;
}

RET_CODE si479x_I2STX0_config(i2s_config *pCfg)
{
    return si479x_I2STX_config(I2STX0_CONFIG, pCfg);
}

#if 0

RET_CODE si479x_I2STX1_config(i2s_config *pCfg)
{
    return si479x_I2STX_config(I2STX1_CONFIG, pCfg);
}

RET_CODE si479x_I2STX2_config(i2s_config *pCfg)
{
    return si479x_I2STX_config(I2STX2_CONFIG, pCfg);
}

RET_CODE si479x_I2STX3_config(i2s_config *pCfg)
{
    return si479x_I2STX_config(I2STX3_CONFIG, pCfg);
}

RET_CODE si479x_I2STX4_config(i2s_config *pCfg)
{
    return si479x_I2STX_config(I2STX4_CONFIG, pCfg);
}

RET_CODE si479x_I2SRX0_config(i2s_config *pCfg)
{
    return si479x_I2STX_config(I2SRX0_CONFIG, pCfg);
}


RET_CODE si479x_I2SRX2_config(i2s_config *pCfg)
{
    return si479x_I2STX_config(I2SRX2_CONFIG, pCfg);
}

RET_CODE si479x_I2SRX3_config(i2s_config *pCfg)
{
    return si479x_I2STX_config(I2SRX3_CONFIG, pCfg);
}

RET_CODE si479x_I2SRX4_config(i2s_config *pCfg)
{
    return si479x_I2STX_config(I2SRX4_CONFIG, pCfg);
}
#endif

RET_CODE si479x_I2SRX1_config(i2s_config *pCfg)
{
    return si479x_I2SRX_config(I2SRX1_CONFIG, pCfg);
}

RET_CODE si479x_I2S_control(I2S_PORT port, PORT_MODE mode, i2s_control *pControl)
{
    RET_CODE ret = RET_SUCCESS;

    clear_cmd_memory();
    
    cmd_buff[0] = I2S_CONTROL;
	cmd_buff[1] = ((port & 0xf) << 4) | (mode & 0x7);
    
    ERR_CHECKING(si479x_command(2, cmd_buff, 14, rsp_buff));
    
	if (pControl != NULL)
	{ 
        pControl->config.sample_rate = rsp_buff[4];
        
        pControl->config.whitencontrol = (rsp_buff[5] >> 5) & 0x7;
        pControl->config.purpose = rsp_buff[5] & 0x0f;
        pControl->config.master = (rsp_buff[6] & 0x10) >> 4;
        pControl->config.clkfs = (rsp_buff[6] & 0x0f);
        
        //pControl->config.lvdsdata = (rsp_buff[7] & 0x80) >> 7;

        pControl->config.data0 = (rsp_buff[7] & 0x3f);
        
        pControl->config.fill = (rsp_buff[8] & 0xc0) >> 6;
        pControl->config.slot_size = (rsp_buff[8] & 0x38) >> 3;
        pControl->config.sample_size = (rsp_buff[8] & 0x7);

        pControl->config.bitorder = (rsp_buff[9] & 0x80) >> 7;
        pControl->config.swap = (rsp_buff[9] & 0x40) >> 6;
        pControl->config.clkinv = (rsp_buff[9] & 0x20) >> 5;
        pControl->config.frame_mode = (rsp_buff[9] & 0xf);

        pControl->config.clock_domain = (rsp_buff[10] & 0xe0) >> 5;
        pControl->config.hiz = (rsp_buff[11] & 0xe0) >> 5;
        pControl->config.length = (rsp_buff[11] & 0x1f);
		
        pControl->config.data1 = rsp_buff[13];
        //pControl->current_mode = (rsp_buff[10] & 0x7);
	}
        
    return ret;
}

