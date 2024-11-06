/********************************************
                  Silicon Laboratories Broadcast Si475x/6x/9x module

   EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND CONDITIONS OF
     THE SOFTWARE LICENSE AGREEMENT IN THE DOCUMENTATION FILE CORRESPONDING
     TO THIS SOURCE FILE.
   IF YOU DO NOT AGREE TO THE LIMITED LICENSE AND CONDITIONS OF SUCH AGREEMENT,
     PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.

   Date: June 06 2014
  (C) Copyright 2014, Silicon Laboratories, Inc. All rights reserved.
********************************************/
#include "dc_2T_implement.h"
#include "dc_bootup.h"
#include "dc_share.h"

#include "si479x_internel.h"
#include "si479x_core.h"
#include "si479x_flashload.h"
#include "si479x_tuner.h"

#include "flash_layout.h"
#include "si479x_hifi.h"

#ifdef SUPPORT_2T

RET_CODE dc_2T_flashload_bootup()
{
	RET_CODE ret = RET_SUCCESS;
	const power_up_args_t * p_powerup_args;
	load_config_t * p_load_config;
	uint8_t guid;

	//POWER UP
	set_chip_id(CHIP_ID_0);

	p_powerup_args = get_power_up_args(CHIP_ID_0);

	ERR_CHECKING(si479x_powerup(p_powerup_args));

	ERR_CHECKING(si479x_get_part_info(&g_chip0_part_info));

	set_chip_id(CHIP_ID_1);

	p_powerup_args = get_power_up_args(CHIP_ID_1);
	ERR_CHECKING(si479x_powerup(p_powerup_args));

	ERR_CHECKING(si479x_get_part_info(&g_chip1_part_info));

	//LOAD INIT
	set_chip_id(CHIP_ID_0);
	guid = get_guid_table(BOOT_MODE_FLASH, CHIP_ID_0, TUNER_FW);
	ERR_CHECKING(si479x_loader_init(guid, guid_table));

	set_chip_id(CHIP_ID_1);
	guid = get_guid_table(BOOT_MODE_FLASH, CHIP_ID_1, TUNER_FW);
	ERR_CHECKING(si479x_loader_init(guid, guid_table));

	//LOAD CONFIG
	set_chip_id(CHIP_ID_0);
	p_load_config = get_load_config_args(CHIP_ID_0);
	//do NOT wait CTS here
	si479x_load_config(p_load_config);

	set_chip_id(CHIP_ID_1);
	p_load_config = get_load_config_args(CHIP_ID_1);
	//do NOT wait CTS here
	si479x_load_config(p_load_config);

	//FLASH LOAD  nFlashLoad SizenFlashLoadSize+4
	ERR_CHECKING(si479x_flash_load_image(SI479X_FIRMWARE_START, SI479X_FIRMWARE_MAX_SIZE));

	//BOOT
	set_chip_id(CHIP_ID_0);
	ERR_CHECKING(si479x_boot());
	ERR_CHECKING(si479x_get_func_info(&g_chip0_func_info));

	set_chip_id(CHIP_ID_1);
	ERR_CHECKING(si479x_boot());
	ERR_CHECKING(si479x_get_func_info(&g_chip1_func_info));

	set_chip_id(CHIP_ID_0);	
		
		return ret;
}

RET_CODE dc_2T_hostload_bootup()
{
	RET_CODE ret = RET_SUCCESS;
	const power_up_args_t * p_powerup_args = NULL;
	uint8_t guid;

	//power up command
	set_chip_id(CHIP_ID_0);
	p_powerup_args = get_power_up_args(CHIP_ID_0);

	ERR_CHECKING(si479x_powerup(p_powerup_args));

	ERR_CHECKING(si479x_get_part_info(&g_chip0_part_info));

	set_chip_id(CHIP_ID_1);
	p_powerup_args = get_power_up_args(CHIP_ID_1);
	ERR_CHECKING(si479x_powerup(p_powerup_args));

	ERR_CHECKING(si479x_get_part_info(&g_chip1_part_info));

	//load init
	set_chip_id(CHIP_ID_0);
	guid = get_guid_table(BOOT_MODE_USB, CHIP_ID_0, TUNER_FW);
	ERR_CHECKING(si479x_loader_init(guid, guid_table));

	set_chip_id(CHIP_ID_1);
	guid = get_guid_table(BOOT_MODE_USB, CHIP_ID_1, TUNER_FW);
	ERR_CHECKING(si479x_loader_init(guid, guid_table));

	//host load
	set_chip_id(CHIP_ID_0);
	ERR_CHECKING(si479x_host_load(APP_GE_FMPD_AM, TUNER_FW));

	//boot
	set_chip_id(CHIP_ID_0);
	ERR_CHECKING(si479x_boot());
	ERR_CHECKING(si479x_get_func_info(&g_chip0_func_info));

	set_chip_id(CHIP_ID_1);
	ERR_CHECKING(si479x_boot());
	ERR_CHECKING(si479x_get_func_info(&g_chip1_func_info));

	set_chip_id(CHIP_ID_0);

	return ret;
}

RET_CODE dc_2T_post_bootup(uint8_t band)
{
	RET_CODE ret = RET_SUCCESS;

	set_chip_id(CHIP_ID_1);

	if (band == BAND_FM)
		si479x_tuner_set_mode(TUNER0, band);

	dc_set_custom_setting(RADIO_CHIP_1);                  

	set_chip_id(CHIP_ID_0);

	//enable hifi proc
	si479x_hifi_proc1_enable(); 

	si479x_tuner_set_mode(TUNER0, band); 
	dc_set_custom_setting(RADIO_CHIP_0);
	
	#ifdef SUPPORT_HIFI
	si479x_set_sleep_mode(3);
	si479x_set_dac_offset(0);
	#endif
	
	return ret;
}

RET_CODE dc_2T_set_mode(uint8_t band)
{
	RET_CODE ret = RET_SUCCESS;

	if (BAND_FM == band)
	{
		set_chip_id(CHIP_ID_1);

		si479x_tuner_set_mode(TUNER0, band);

		set_chip_id(CHIP_ID_0);
	}

	return si479x_tuner_set_mode(TUNER0, band);
}

RET_CODE dc_2T_fm_tune(uint16_t Freq)
{
	RET_CODE ret = RET_SUCCESS;
	set_chip_id(CHIP_ID_1);
	si479x_tuner_fm_tune(TUNER0, 0, 0, Freq);

	set_chip_id(CHIP_ID_0);
	si479x_tuner_fm_tune(TUNER0, 0, 0, Freq);

	SetDivMode(bWorkMode);

	return ret;
}

RET_CODE dc_2T_am_tune(uint16_t Freq)
{
	return si479x_tuner_am_tune(TUNER0, 0, 0, Freq);
}

uint16_t dc_2T_fm_get_frequency()
{
	RET_CODE ret = RET_SUCCESS;

	#ifdef SUPPORT_2T
	if (bWorkMode == PD_MODE_TUNER1)
		set_chip_id(CHIP_ID_1);
	#endif

	ret = si479x_tuner_fm_rsq_status(TUNER0, 0, &g_tuner_metrics);

	#ifdef SUPPORT_2T
	if (bWorkMode == PD_MODE_TUNER1)
		set_chip_id(CHIP_ID_0);
	#endif

	if (ret != RET_SUCCESS)
		return FM_BOT_FREQ;

	return g_tuner_metrics.Freq;
}

uint16_t dc_2T_am_get_frequency()
{
	RET_CODE ret = RET_SUCCESS;

	ret = si479x_tuner_am_rsq_status(TUNER0, 0, &g_tuner_metrics);

	if (ret != RET_SUCCESS)
		return AM_BOT_FREQ;

	return g_tuner_metrics.Freq;	
}

RET_CODE dc_2T_fm_get_tuner_metrics(tuner_metrics_t *p_tuner_metrics_t)
{
	RET_CODE ret = RET_SUCCESS;

	if (bWorkMode == PD_MODE_TUNER1)
		set_chip_id(CHIP_ID_1);

	ret = si479x_tuner_fm_rsq_status(TUNER0, 0, p_tuner_metrics_t);

	if (bWorkMode == PD_MODE_TUNER1)
		set_chip_id(CHIP_ID_0);

	return ret;
}

RET_CODE dc_2T_am_get_tuner_metrics(tuner_metrics_t *p_tuner_metrics_t)
{
	RET_CODE ret = RET_SUCCESS;

	ERR_CHECKING(si479x_tuner_am_rsq_status(TUNER0, 0, p_tuner_metrics_t));

	return ret;
}



RET_CODE dc_2T_fm_validate(uint16_t freq)
{
	RET_CODE ret = RET_NOT_FOUND;

	set_chip_id(CHIP_ID_0);
	si479x_fm_phase_div(0, 0, 0);

	switch (bWorkMode)
	{
		case PD_MODE_COMBINED:
			si479x_tuner_fm_tune(TUNER0, 0, 0, freq);
			si479x_tuner_fm_rsq_status(TUNER0, 5, &g_tuner_metrics);
			ret = (g_tuner_metrics.Valid != 0) ? RET_SUCCESS : RET_NOT_FOUND;

			if (ret == RET_NOT_FOUND) 
			{
				set_chip_id(CHIP_ID_1);
				si479x_tuner_fm_tune(TUNER0, 0, 0, freq);
				si479x_tuner_fm_rsq_status(TUNER0, 5, &g_tuner_metrics);
				ret = (g_tuner_metrics.Valid != 0) ? RET_SUCCESS : RET_NOT_FOUND;			
				set_chip_id(CHIP_ID_0);
			}
			break;

		case PD_MODE_TUNER0:
			si479x_tuner_fm_tune(TUNER0, 0, 0, freq);
			si479x_tuner_fm_rsq_status(TUNER0, 5, &g_tuner_metrics);
			ret =  (g_tuner_metrics.Valid != 0) ? RET_SUCCESS : RET_NOT_FOUND;
			break;

		case PD_MODE_TUNER1:
			set_chip_id(CHIP_ID_1);
			si479x_tuner_fm_tune(TUNER0, 0, 0, freq);
			si479x_tuner_fm_rsq_status(TUNER0, 5, &g_tuner_metrics);
			ret =  (g_tuner_metrics.Valid != 0) ? RET_SUCCESS : RET_NOT_FOUND;			
			set_chip_id(CHIP_ID_0);
			break;

		default:
			return ret;
	}   

	return ret;

}

RET_CODE dc_2T_am_validate(uint16_t freq)
{
	si479x_tuner_am_tune(TUNER0, 0, 0, freq);

	si479x_tuner_am_rsq_status(TUNER0, 5, &g_tuner_metrics);

	return g_tuner_metrics.Valid!=0 ? RET_SUCCESS : RET_NOT_FOUND;	
}

#endif
