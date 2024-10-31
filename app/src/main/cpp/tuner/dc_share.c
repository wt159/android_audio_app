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
#include "dc_share.h"
#include "dc_bootup.h"

#include "si479x_tuner.h"

uint8_t bBootmode; //0, usb 1, flash

uint8_t bHDAlta;
uint8_t bHDBlend;
uint8_t bHDSeekMode;
PD_MODE bWorkMode;    //0, combined, 1, primary 2, secondary

uint8_t tuner_fw_image_name[] = "bootimage.bin";
uint8_t flash_fw_image_name[] = "flash_bl.bin";

uint8_t si469x_boot_patch_image_name[] = "ROMPATCH.BIN";

uint8_t si469x_dab_linker_image_name[] = "dblinker.bin";
uint8_t si469x_dab_demod_image_name[] = "dab_demd.bin";

uint8_t si469x_hd_linker_image_name[] = "hd_link.bin";
uint8_t si469x_fmhd_demod_image_name[] = "fhd_demd.bin";
uint8_t si469x_amhd_demod_image_name[] = "ahd_demd.bin";

uint8_t part_key_image_name[] = "key_exch.kyx";

uint8_t gChip0PropertyServoDefault = 0;
uint8_t gChip1PropertyServoDefault = 0;

#if 0
power_up_args_t si479x_powerup_args;

typedef struct _power_up_args_t
{
	uint8_t 	CTSIEN;
	uint8_t     CLKO_CURRENT;
	uint8_t     VIO;
	uint8_t 	CLKOUT;
	CLK_MODE_T 	CLK_MODE;
	uint8_t 	TR_SIZE;
	uint8_t		CTUN;
	uint32_t	XTAL_FREQ; //crystal clock
	uint8_t		IBIAS;	//only for crytal oscillator
	uint8_t		AFS; //0: 48kS/s 1:44.1kS/s
	uint8_t		DIGDAT; //0: HD 1: DAB
	uint8_t		CHIPID;
	uint8_t     ISFSMODE;

	uint8_t     ICLINK;

	uint8_t     IQCHANNELS;
	uint8_t     IQFMT;
	uint8_t     IQMUX;
	uint8_t     IQRATE;

	uint8_t     IQSWAP1;
	uint8_t     IQSWAP0;
	uint8_t     IQSLOT;
	uint8_t     IQEARLY;
	uint8_t     IQOUT;
}power_up_args_t;
#endif

#ifdef SUPPORT_1T
const power_up_args_t ge_powerup_args_single = 
{
	0, 7, 0, 1, XTAL, 8, 21, 36864000, 67, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
#endif

#ifdef SUPPORT_2T
const power_up_args_t ge_powerup_args_pd_primary = 
{
	0, 7, 0, 1, XTAL, 8, 21, 36864000, 67, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const power_up_args_t ge_powerup_args_pd_secondary = 
{
	0, 0, 0, 0, SINGLE_DC, 0, 0, 36864000, 0, 0, 0, 1, 0, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
#endif

#ifdef SUPPORT_1DT
#ifdef SUPPORT_FMAMDAB
const power_up_args_t de_powerup_args_eu = 
{
	0, 7, 0, 1, XTAL, 8, 15, 36864000, 67, 0, 1, 0, 0, 0, 
	2, 1, 0, 0, 0, 0, 5, 0, 1
};
#endif

#ifdef SUPPORT_FMAMHD
const power_up_args_t de_powerup_args_na = 
{
	0, 7, 0, 1, XTAL, 8, 15, 36864000, 67, 0, 0, 0, 0, 0, 
	2, 1, 0, 1, 0, 0, 1, 0, 1
};
#endif
#endif

#ifdef SUPPORT_1DT
const power_up_args_t de_powerup_args_na = 
{
	0, 7, 0, 1, XTAL, 8, 15, 36864000, 67, 0, 0, 0, 0, 0, 
	2, 1, 0, 1, 0, 0, 1, 0, 1
};
#endif

part_info_t g_chip0_part_info;
part_info_t g_chip1_part_info;

func_info_t g_chip0_func_info;  
func_info_t g_chip1_func_info;  

part_info_t g_part_info;
func_info_t g_func_info;

tuner_metrics_t g_tuner_metrics;

const power_up_args_t * get_power_up_args(CHIP_ID_T chipid)
{    
	if (chipid == CHIP_ID_0)
	{
	#ifdef SUPPORT_1DT1F
		
		#ifdef SUPPORT_FMAMDAB
			return &de_powerup_args_eu;
		#endif
		
		#ifdef SUPPORT_FMAMHD
			return &de_powerup_args_na;
		#endif
		
	#endif
		
	#ifdef SUPPORT_1DT
		return &de_powerup_args_na;
	#endif
		
	#ifdef SUPPORT_1T
		return &ge_powerup_args_single;
	#endif
	
	#ifdef SUPPORT_2T
		return &ge_powerup_args_pd_primary;
	#endif

	}
	else
	{
		#ifdef SUPPORT_2T
		return &ge_powerup_args_pd_secondary;
		#else
		return NULL; //fix compile warning
		#endif
	}
}

void UIHDDemodSettingsChecking()
{
	//HD demod option
	#ifdef SUPPORT_FMAMHD
		set_chip_id(CHIP_ID_1);
		si469x_satelite_set_property(1, HD_ALTA_CONTROL, bHDAlta);
		si469x_satelite_set_property(1, HD_BLEND_OPTIONS, (((uint16_t)bHDBlend&0x3)<<14) | 0x0E);
		set_chip_id(CHIP_ID_0);
	#endif
}


void dc_set_custom_setting(RADIO_CHIP_ID_T chip_id)
{
		if (chip_id == RADIO_CHIP_0)
		{
			gChip0PropertyServoDefault = 0;   
		}
		else
		{
			gChip1PropertyServoDefault = 0;       
		}     

#ifdef SUPPORT_RDS
	si479x_tuner_set_property(TUNER0, FM_RDS_CONFIG, 0x01);
#endif
}

uint8_t dc_get_fm_space()
{
	return FM_SPACE;
}

uint8_t dc_get_am_space()
{
	return AM_SPACE;
}

static uint8_t _abort_seeking = 0;
void ResetAbortSeeking()
{
	_abort_seeking = 0;
}

void AbortSeeking()
{	
	_abort_seeking = 1;
}

uint8_t IsAbortSeeking()
{
	return _abort_seeking;
}

void dc_Identify(void)
{
	//read the power up data from eeprom?
	platform_hal_init();
}


