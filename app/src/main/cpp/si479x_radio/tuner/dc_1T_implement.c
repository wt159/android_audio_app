/***************************************************************************************
                  Silicon Laboratories Broadcast Si475x/6x/9x module

   EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND CONDITIONS OF
     THE SOFTWARE LICENSE AGREEMENT IN THE DOCUMENTATION FILE CORRESPONDING
     TO THIS SOURCE FILE.
   IF YOU DO NOT AGREE TO THE LIMITED LICENSE AND CONDITIONS OF SUCH AGREEMENT,
     PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.

   Date: June 06 2014
  (C) Copyright 2014, Silicon Laboratories, Inc. All rights reserved.
****************************************************************************************/
#include "dc_1T_implement.h"
#include "dc_bootup.h"
#include "dc_share.h"

#include "si479x_internel.h"
#include "si479x_core.h"
#include "si479x_flashload.h"
#include "si479x_tuner.h"
#include "si479x_hifi.h"

#include <stdio.h>

#include "flash_layout.h"

#ifdef SUPPORT_1T

#define KEY_SIZE	448
RET_CODE check_park_key(BOOT_MODE_T bootmode)
{
	RET_CODE ret = RET_SUCCESS;

	uint16_t read_length = 0;
	uint16_t length = 0;
	FILE *file = NULL;

	SetFirmwareImage(PART_KEY);

	file = fopen(GetFirmwareImage(), "r");
	if (NULL == file) 
	{
		printf("Open file %s failed!\n", GetFirmwareImage());
		return RET_INVALIDINPUT;
	}
	
	read_length = fread(bulk_buff+4, 1, KEY_SIZE, file);
	fclose(file);

	bulk_buff[0] = KEY_EXCHANGE;
	bulk_buff[1] = 0;
	bulk_buff[2] = 0;
	bulk_buff[3] = 0;

	return	ERR_CHECKING(si479x_command(read_length+4, bulk_buff, 4, rsp_buff));
		
	#if 0
	if (bootmode == BOOT_MODE_USB)//usb
	{
		SetFirmwareImage(PART_KEY);

		file = fopen(GetFirmwareImage(), "r");
		if (NULL == file) 
		{
			printf("Open file %s failed!\n", GetFirmwareImage());
			return RET_INVALIDINPUT;
		}
		
		read_length = fread(bulk_buff+4, 1, 4096, file);
		fclose(file);

		bulk_buff[0] = KEY_EXCHANGE;
		bulk_buff[1] = 0;
		bulk_buff[2] = 0;
		bulk_buff[3] = 0;

		return	ERR_CHECKING(si479x_command(read_length+4, bulk_buff, 4, rsp_buff));
	}
	else //flash
	{
		//in fact, the length of key is fixed as 448 bytes.
		length = 448;

		//Since si479x 2T head unit has Chip1 connected with flash, so need read the data from ic1
		
		ERR_CHECKING(si479x_read_block(SI479X_PART_KEY_START, 512, bulk_buff));
		//ERR_CHECKING(si479x_read_block(0x400, 512, bulk_buff));
		
		/**
		  for (i=0; i<length; i++)
		  {
		  bulk_buff[4+i] = I2cReadByte(PART_KEY_ADDR + i);
		  }
		 */
		bulk_buff[0] = KEY_EXCHANGE;
		bulk_buff[1] = 0;
		bulk_buff[2] = 0;
		bulk_buff[3] = 0;
		return	ERR_CHECKING(si479x_command(length+4, bulk_buff, 4, rsp_buff));
	}
	#endif

	return ret;	
}

RET_CODE dc_1T_flashload_bootup()
{
	RET_CODE ret = RET_SUCCESS;
	const power_up_args_t * p_powerup_args;
	load_config_t * p_load_config;
	uint8_t guid;
	//POWER UP
	
	p_powerup_args = get_power_up_args(CHIP_ID_0);
	ERR_CHECKING(si479x_powerup(p_powerup_args));
	ERR_CHECKING(si479x_get_part_info(&g_chip0_part_info));
		
	//Note: Only custom part need this action, eg. si47925
	check_park_key(BOOT_MODE_FLASH);
	
	guid = get_guid_table(BOOT_MODE_FLASH, CHIP_ID_0, TUNER_FW);
	ERR_CHECKING(si479x_loader_init(guid, guid_table));
		
	//FLASH LOAD  nFlashLoad SizenFlashLoadSize+4
	ERR_CHECKING(si479x_flash_load_image(SI479X_FIRMWARE_START, SI479X_FIRMWARE_MAX_SIZE));
	//BOOT
	
	ERR_CHECKING(si479x_boot());
	ERR_CHECKING(si479x_get_func_info(&g_chip0_func_info));
			
	return ret;
}
RET_CODE dc_1T_hostload_bootup()
{
	RET_CODE ret = RET_SUCCESS;
	const power_up_args_t * p_powerup_args = NULL;
	uint8_t guid;
	//power up command
	
	p_powerup_args = get_power_up_args(CHIP_ID_0);
	printf("%s:%d\n", __func__, __LINE__);
	ERR_CHECKING(si479x_powerup(p_powerup_args));
	printf("%s:%d\n", __func__, __LINE__);
	ERR_CHECKING(si479x_get_part_info(&g_chip0_part_info));
	printf("%s:%d\n", __func__, __LINE__);
	// check_park_key(BOOT_MODE_USB);
	//load init
	guid = get_guid_table(BOOT_MODE_USB, CHIP_ID_0, TUNER_FW);
	ERR_CHECKING(si479x_loader_init(guid, guid_table));
	//host load
	ERR_CHECKING(si479x_host_load(APP_GE_FM_AM, TUNER_FW));
	//boot
	ERR_CHECKING(si479x_boot());
	ERR_CHECKING(si479x_get_func_info(&g_chip0_func_info));
	printf("%s:%d\n", __func__, __LINE__);
	return ret;
}

enum FIXED_SlotSize {
    FIXED_SlotSize_16Bit = 0x1,
    FIXED_SlotSize_20Bit = 0x2,
    FIXED_SlotSize_18Bit = 0x5
};
enum FIXED_Rate {
    FIXED_Rate_IQ192KS = 0x8,
    FIXED_Rate_IQ650KS = 0x40,
    FIXED_Rate_IQ744KS = 0x42,
    FIXED_Rate_IQ2048KS = 0x80
};

RET_CODE dc_1T_post_bootup(uint8_t band)
{
	RET_CODE ret = RET_SUCCESS;

	dc_set_custom_setting(RADIO_CHIP_0);

	if (band == BAND_DAB) {
        si479x_set_property(EZIQ_FIXED_SETTINGS, FIXED_SlotSize_16Bit<<8|FIXED_Rate_IQ2048KS);
        si479x_set_property(EZIQ_OUTPUT_SOURCE, 0x0780); //b 0 0111 1000 0000
    }

    si479x_tuner_set_mode(TUNER0, band);

    if (band == BAND_DAB) {
        si479x_set_property(EZIQ_DAB_CONFIG0, 0x0001);
    }
#ifdef SUPPORT_HIFI
	si479x_set_sleep_mode(3);
	si479x_set_dac_offset(0);
#endif
	return ret;
}
RET_CODE dc_1T_set_mode(uint8_t band)
{
	return si479x_tuner_set_mode(TUNER0, band);
}
RET_CODE dc_1T_fm_tune(uint16_t Freq)
{
	return si479x_tuner_fm_tune(TUNER0, 0, 0, Freq);
}
RET_CODE dc_1T_am_tune(uint16_t Freq)
{
	return si479x_tuner_am_tune(TUNER0, 0, 0, Freq);
}
uint16_t dc_1T_fm_get_frequency()
{
	RET_CODE ret = RET_SUCCESS;
	ret = si479x_tuner_fm_rsq_status(TUNER0, 0, &g_tuner_metrics);
	if (ret != RET_SUCCESS)
		return FM_BOT_FREQ;
	return g_tuner_metrics.Freq;	
}
uint16_t dc_1T_am_get_frequency()
{
	RET_CODE ret = RET_SUCCESS;
	ret = si479x_tuner_am_rsq_status(TUNER0, 0, &g_tuner_metrics);
	if (ret != RET_SUCCESS)
		return AM_BOT_FREQ;
	return g_tuner_metrics.Freq;	
}

RET_CODE dc_1T_fm_get_tuner_metrics(tuner_metrics_t *p_tuner_metrics_t)
{
	RET_CODE ret = RET_SUCCESS;
	ret = si479x_tuner_fm_rsq_status(TUNER0, 0, p_tuner_metrics_t);
	return ret;
}
RET_CODE dc_1T_am_get_tuner_metrics(tuner_metrics_t *p_tuner_metrics_t)
{
	RET_CODE ret = RET_SUCCESS;
	ERR_CHECKING(si479x_tuner_am_rsq_status(TUNER0, 0, p_tuner_metrics_t));
	return ret;
}

RET_CODE dc_1T_fm_validate(uint16_t freq)
{
	si479x_tuner_fm_tune(TUNER0, 0, 0, freq);
	si479x_tuner_fm_rsq_status(TUNER0, 5, &g_tuner_metrics);
	return (g_tuner_metrics.Valid != 0) ? RET_SUCCESS : RET_NOT_FOUND;
}
RET_CODE dc_1T_am_validate(uint16_t freq)
{
	si479x_tuner_am_tune(TUNER0, 0, 0, freq);
	si479x_tuner_am_rsq_status(TUNER0, 5, &g_tuner_metrics);
	return g_tuner_metrics.Valid!=0 ? RET_SUCCESS : RET_NOT_FOUND;
}
#endif
