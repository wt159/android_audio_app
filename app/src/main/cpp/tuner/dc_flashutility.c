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
#include "dc_flashutility.h"
#include "dc_bootup.h"
#include "dc_share.h"

RET_CODE dc_1T_hostload_flashutility_bootup()
{
	uint16_t guid;
	RET_CODE ret = RET_SUCCESS;
	const power_up_args_t * p_powerup_args = NULL;

	//reset two tuners.
	chips_reset();

	//power up chip0 to provide clock to chip1.
	p_powerup_args = get_power_up_args(CHIP_ID_0);
	ERR_CHECKING(si479x_powerup(p_powerup_args));

	//load init
	guid = get_guid_table(BOOT_MODE_USB, CHIP_ID_1, FLASH_UTIL);
	ERR_CHECKING(si479x_loader_init(guid, guid_table));

	//host load
	ERR_CHECKING(si479x_host_load(APP_GE_FM_AM, FLASH_UTIL));

	//boot
	ERR_CHECKING(si479x_boot());

	return ret;
}

RET_CODE dc_2T_hostload_flashutility_bootup()
{
	uint16_t guid;
	RET_CODE ret = RET_SUCCESS;
	const power_up_args_t * p_powerup_args = NULL;

	//reset two tuners.
	chips_reset();

	//power up chip0 to provide clock to chip1.
	set_chip_id(CHIP_ID_0);
	p_powerup_args = get_power_up_args(CHIP_ID_0);
	ERR_CHECKING(si479x_powerup(p_powerup_args));

	//power up chip1
	set_chip_id(CHIP_ID_1);
	p_powerup_args = get_power_up_args(CHIP_ID_1);
	ERR_CHECKING(si479x_powerup(p_powerup_args));

	//load init
	guid = get_guid_table(BOOT_MODE_USB, CHIP_ID_1, FLASH_UTIL);
	ERR_CHECKING(si479x_loader_init(guid, guid_table));

	//host load
	ERR_CHECKING(si479x_host_load(APP_GE_FM_AM, FLASH_UTIL));

	//boot
	ERR_CHECKING(si479x_boot());

	//
	set_chip_id(CHIP_ID_0);

	return ret;
}


RET_CODE dc_1DT1F_HD_hostload_flashutility_bootup()
{    
	RET_CODE ret = RET_SUCCESS;
	const power_up_args_t * p_powerup_args = NULL;
	uint16_t guid;

	//power up chip0.
	set_chip_id(CHIP_ID_0);
	p_powerup_args = get_power_up_args(CHIP_ID_0);
	ERR_CHECKING(si479x_powerup(p_powerup_args));

	//load init
	guid = get_guid_table(BOOT_MODE_USB, CHIP_ID_0, FLASH_UTIL);
	ERR_CHECKING(si479x_loader_init(guid, guid_table));

	//host load
	ERR_CHECKING(si479x_host_load(APP_DE_FMPD_AM, FLASH_UTIL));

	//boot
	ERR_CHECKING(si479x_boot());

	//
	set_chip_id(CHIP_ID_0);

	return ret;
}


RET_CODE dc_hostload_flashutility_bootup()
{
	//reset two tuners.
	chips_reset();
	
	#ifdef SUPPORT_1T
	return dc_1T_hostload_flashutility_bootup();
	#endif
	
	#ifdef SUPPORT_2T
	return dc_2T_hostload_flashutility_bootup();
	#endif
	
	#if (defined SUPPORT_1DT1F) || (defined SUPPORT_1DT) 
	return dc_1DT1F_HD_hostload_flashutility_bootup();
	#endif
}


