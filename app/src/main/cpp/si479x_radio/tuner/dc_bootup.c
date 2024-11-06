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
#include "dc_bootup.h"
#include "si479x_tuner.h" 
#include "string.h"
#include "si479x_hifi.h"
#include "si479x_audio.h"
#include "si479x_internel.h"
#include "si479x_flashload.h"

#include "flash_layout.h"

#include "dc_1T_implement.h"
#include "dc_2T_implement.h"
#include "dc_1DT_implement.h"

#include "dc_share.h"

#include <stdio.h>
#include <limits.h>

char firmware_image_name[NAME_MAX] = {0};
char share_folder[NAME_MAX] = {'.', '/'};


load_config_t si479x_load_cfg;

uint8_t bulk_buff[BULCK_BUFF_SIZE];


//dc_implement_t* g_p_dc_implement = NULL;

#ifdef SUPPORT_FMAMDAB	
#define DAB_FREQ_LIST_COUNT 41
uint32_t dab_default_freq_list[] = {
	174928000,
	176640000,
	178352000,
	180064000,
	181936000,
	183648000,
	185360000,
	187072000,
	188928000,
	190640000,
	192352000,
	194064000,
	195936000,
	197648000,
	199360000,
	201072000,
	202928000,
	204640000,
	206352000,
	208064000,
	209936000,
	210096000,
	211648000,
	213360000,
	215072000,
	216928000,
	217088000,
	218640000,
	220352000,
	222064000,
	223936000,
	224096000,
	225648000,
	227360000,
	229072000,
	230784000,
	232496000,
	234208000,
	235776000,
	237488000,
	239200000,
};

uint32_t IndexToDABFreq(uint8_t freq_index)
{
	if (freq_index >= DAB_FREQ_LIST_COUNT)
		freq_index = 0;

	return dab_default_freq_list[freq_index];
}

dab_tuner_metrics_t g_dab_tuner_metrics;
dab_demod_event_status_t g_dab_demod_event;
#endif

uint32_t guid_table[MAX_IMAGE_COUNT];

#ifdef SUPPORT_RDS
int8_t fm_afcheck_rssi_threshold;///<rssi threshold for fm do af check, once the signal drop down below it and af check begin

int8_t fm_afcheck_snr_threshold;///<snr threshold for fm do af check, once the signal drop down below it and af check begin
#endif

load_config_t * get_load_config_args(CHIP_ID_T chipid)
{
	si479x_load_cfg.numchips = 2;
	if (chipid == CHIP_ID_0)
	{
		si479x_load_cfg.chipid = 0;
		si479x_load_cfg.role = 0;
		si479x_load_cfg.icn_in = 0;
		si479x_load_cfg.nvmosi_out = 1;

		si479x_load_cfg.clksel8 = 0;
		si479x_load_cfg.spicfg3 = 0;
		si479x_load_cfg.drvdel = 0x00040004 ;
		si479x_load_cfg.capdel = 0x002800e0;
		si479x_load_cfg.spisctrl = 0x00000001;
		si479x_load_cfg.nvsclk = 0x000f0481;

		si479x_load_cfg.nvssb = 0x000f0481;
		si479x_load_cfg.nvmosi = 0x000f0482;
		si479x_load_cfg.nvmiso = 0x000f0481;

		si479x_load_cfg.diclk0 = 0x000f0480;
		si479x_load_cfg.difs0 = 0x000f0480;
		si479x_load_cfg.icn = 0x000f0480;
		si479x_load_cfg.icp = 0x000f0480;

	}
	else
	{
		si479x_load_cfg.chipid = 1;
		si479x_load_cfg.role = 1;
		si479x_load_cfg.icn_in = 1;
		si479x_load_cfg.nvmosi_out = 0;

		si479x_load_cfg.clksel8 = 0x00004000;
		si479x_load_cfg.spicfg3 = 0x00003000;
		si479x_load_cfg.drvdel = 0x00040004 ;
		si479x_load_cfg.capdel = 0x0028000e;
		si479x_load_cfg.spisctrl = 0x00000001;
		si479x_load_cfg.nvsclk = 0x000807f3;

		si479x_load_cfg.nvssb = 0x000807f3;
		si479x_load_cfg.nvmosi = 0x000807f3;
		si479x_load_cfg.nvmiso = 0x000807f1;

		si479x_load_cfg.diclk0 = 0x000d07f2;
		si479x_load_cfg.difs0 = 0x002d07f2;
		si479x_load_cfg.icn = 0x000f07f1;
		si479x_load_cfg.icp = 0x002d07f2;
	}

	si479x_load_cfg.debugp = 0x000f0480;
	si479x_load_cfg.debugn = 0x000f0480;
	si479x_load_cfg.isfsp = 0x000f0480;
	si479x_load_cfg.isfsn = 0x000f0480;
	si479x_load_cfg.zifiout = 0x000f0480;
	si479x_load_cfg.zifclk = 0x000f0480;
	si479x_load_cfg.zifqout = 0x000f0480;
	si479x_load_cfg.ziffs = 0x000f0480;

	return &si479x_load_cfg;
}

void SetFirmwareImage(FW_IMG_T type)
{
	strcpy(firmware_image_name, share_folder);
	
	

	switch (type) {
		case TUNER_FW:
			strcat(firmware_image_name, tuner_fw_image_name);
			break;
		case FLASH_UTIL:
			strcat(firmware_image_name, flash_fw_image_name);
			break;
		case BOOT_PATCH:
			strcat(firmware_image_name, si469x_boot_patch_image_name);
			break;
			//case FLASH_READER:
			//	strcat(firmware_image_name, si469x_flash_reader_image_name);
			//	break;
		case DAB_LINKER:
			strcat(firmware_image_name, si469x_dab_linker_image_name);
			break;
		case DAB_DEMOD:
			strcat(firmware_image_name, si469x_dab_demod_image_name);
			break;
		case HD_LINKER:
			strcat(firmware_image_name, si469x_hd_linker_image_name);
			break;
		case FMHD_DEMOD:
			strcat(firmware_image_name, si469x_fmhd_demod_image_name);
			break;
		case AMHD_DEMOD:
			strcat(firmware_image_name, si469x_amhd_demod_image_name);
			break;
		case PART_KEY:
			strcat(firmware_image_name, part_key_image_name);
			break;
		default:
			break;
	}
	
}

void SetFirmwareImageFolder(const char *path)
{
	int len = strlen(path);
	if (len + 12 > NAME_MAX)
	{
		printf("%s is not an invalid path!", path);
		return;
	}
	
	
	strcpy(share_folder, path);	
	if (path[len-1] != '/')
	{
		strcat(share_folder, "/");
	}
	printf("share_folder  = %s\n", share_folder);
}
const char* GetFirmwareImage()
{
	
	return firmware_image_name;
}

uint8_t parse_guid_table(CHIP_ID_T chipid, uint8_t * buff)
{
	uint16_t cnt = 0;
	uint16_t i = 0;
	uint16_t tb = 0;
	uint8_t id = 0x1 << chipid;
	uint8_t * p = NULL;

	cnt = ((uint16_t)buff[1] << 8) | ((uint16_t)buff[0] & 0x00ff);

	if (cnt > MAX_IMAGE_COUNT)
		return 0;

	for (i=0; i<cnt; i++)
	{
		if ((buff[(1 + i) * IMAGE_INFO_SIZE + IC_BITFIELD_IN_TABLE_OFFSET] & id) != 0)
		{
			//guid_table[tb] = *(uint32_t*)();
			p = buff+(i + 1)*IMAGE_INFO_SIZE;
			
			guid_table[tb] = (((uint32_t)p[3] << 24)&0xff000000) | (((uint32_t)p[2] << 16)&0xff0000) | (((uint32_t)p[1] << 8)&0xff00) | ((uint32_t)p[0] & 0xff);

			tb++;
		}
	}

	return tb;
}

uint8_t get_guid_table(BOOT_MODE_T bootmode, CHIP_ID_T chipid, FW_IMG_T image)
{
	uint16_t read_length = 0;
	uint16_t i = 0;

	if (bootmode == BOOT_MODE_USB)//usb
	{
		SetFirmwareImage(image);

		FILE *file = fopen(firmware_image_name, "r");
		if (NULL == file) 
		{
			printf("Open file %s failed!\n", firmware_image_name);
			return 0;
		}
		
		read_length = fread(bulk_buff, 1, 1024, file);
		fclose(file);
		
		if (read_length < 1024)
		{
			printf("The firmware image %s is corroputed!", firmware_image_name);
			
			return 0;
		}

		for (i=0; i<(MAX_IMAGE_COUNT+1)*IMAGE_INFO_SIZE; i++)
		{
			bulk_buff[i+4] = bulk_buff[IMAGE_CNT_OFFSET+i];
		}
	}
	else //flash
	{ 
		//primary and secondary share the same space
		if (CHIP_ID_0 == chipid)
		{
			//Since si479x 2T head unit has Chip1 connected with flash, so need read the data from ic1
			#ifdef SUPPORT_2T
				set_chip_id(CHIP_ID_1);
			#endif

			si479x_read_block(IMAGE_CNT_OFFSET, (MAX_IMAGE_COUNT+1)*IMAGE_INFO_SIZE, bulk_buff);
			set_chip_id(CHIP_ID_0);
		}
	}

	return parse_guid_table(chipid, bulk_buff+4);
}


RET_CODE dc_hostload_bootup()
{	
		#ifdef SUPPORT_1T
			return dc_1T_hostload_bootup();
		#endif	
		#ifdef SUPPORT_2T
			return dc_2T_hostload_bootup();
		#endif	
		#ifdef SUPPORT_1DT
			return dc_1DT_hostload_bootup();
		#endif	
		#ifdef SUPPORT_1DT1F
			return dc_1DT1F_HD_hostload_bootup();
		#endif	
}


RET_CODE dc_flashload_bootup()
{
		#ifdef SUPPORT_1T
			return dc_1T_flashload_bootup();
		#endif	
		#ifdef SUPPORT_2T
			return dc_2T_flashload_bootup();
		#endif	
		#ifdef SUPPORT_1DT
			return dc_1DT_flashload_bootup();
		#endif	
		#ifdef SUPPORT_1DT1F
			return dc_1DT1F_HD_flashload_bootup();
		#endif	
}


RET_CODE dc_post_bootup(uint8_t band)
{
		#ifdef SUPPORT_1T
			return dc_1T_post_bootup(band);
		#endif	
		#ifdef SUPPORT_2T
			return dc_2T_post_bootup(band);
		#endif	
		#ifdef SUPPORT_1DT
			return dc_1DT_post_bootup(band);
		#endif	
		#ifdef SUPPORT_1DT1F
			return dc_1DT1F_HD_post_bootup(band);
		#endif	
}

RET_CODE dc_set_mode(uint8_t band)
{
		#ifdef SUPPORT_1T
			return dc_1T_set_mode(band);
		#endif	
		#ifdef SUPPORT_2T
			return dc_2T_set_mode(band);
		#endif	
		#ifdef SUPPORT_1DT
			return dc_1DT_set_mode(band);
		#endif	
		#ifdef SUPPORT_1DT1F
			return dc_1DT1F_HD_set_mode(band);
		#endif	
}

RET_CODE dc_fm_tune(uint16_t Freq)
{
		#ifdef SUPPORT_1T
			return dc_1T_fm_tune(Freq);
		#endif	
		#ifdef SUPPORT_2T
			return dc_2T_fm_tune(Freq);
		#endif	
		#ifdef SUPPORT_1DT
			return dc_1DT_fm_tune(Freq);
		#endif	
		#ifdef SUPPORT_1DT1F
			return dc_1DT1F_HD_fm_tune(Freq);
		#endif	
}

RET_CODE dc_am_tune(uint16_t Freq)
{
		#ifdef SUPPORT_1T
			return dc_1T_am_tune(Freq);
		#endif	
		#ifdef SUPPORT_2T
			return dc_2T_am_tune(Freq);
		#endif	
		#ifdef SUPPORT_1DT
			return dc_1DT_am_tune(Freq);
		#endif	
		#ifdef SUPPORT_1DT1F
			return dc_1DT1F_HD_am_tune(Freq);
		#endif	
}

uint16_t dc_am_get_frequency()
{
		#ifdef SUPPORT_1T
			return dc_1T_am_get_frequency();
		#endif	
		#ifdef SUPPORT_2T
			return dc_2T_am_get_frequency();
		#endif	
		#ifdef SUPPORT_1DT
			return dc_1DT_am_get_frequency();
		#endif	
		#ifdef SUPPORT_1DT1F
			return dc_1DT1F_HD_am_get_frequency();
		#endif	
}

uint16_t dc_fm_get_frequency()
{	
		#ifdef SUPPORT_1T
			return dc_1T_fm_get_frequency();
		#endif	
		#ifdef SUPPORT_2T
			return dc_2T_fm_get_frequency();
		#endif	
		#ifdef SUPPORT_1DT
			return dc_1DT_fm_get_frequency();
		#endif	
		#ifdef SUPPORT_1DT1F
			return dc_1DT1F_HD_fm_get_frequency();
		#endif	
}

RET_CODE dc_fm_get_tuner_metrics(tuner_metrics_t *p_tuner_metrics_t)
{	
		#ifdef SUPPORT_1T
			return dc_1T_fm_get_tuner_metrics(p_tuner_metrics_t);
		#endif	
		#ifdef SUPPORT_2T
			return dc_2T_fm_get_tuner_metrics(p_tuner_metrics_t);
		#endif	
		#ifdef SUPPORT_1DT
			return dc_1DT_fm_get_tuner_metrics(p_tuner_metrics_t);
		#endif	
		#ifdef SUPPORT_1DT1F
			return dc_1DT1F_HD_fm_get_tuner_metrics(p_tuner_metrics_t);
		#endif	
}

RET_CODE dc_am_get_tuner_metrics(tuner_metrics_t *p_tuner_metrics_t)
{	
		#ifdef SUPPORT_1T
			return dc_1T_am_get_tuner_metrics(p_tuner_metrics_t);
		#endif	
		#ifdef SUPPORT_2T
			return dc_2T_am_get_tuner_metrics(p_tuner_metrics_t);
		#endif	
		#ifdef SUPPORT_1DT
			return dc_1DT_am_get_tuner_metrics(p_tuner_metrics_t);
		#endif	
		#ifdef SUPPORT_1DT1F
			return dc_1DT1F_HD_am_get_tuner_metrics(p_tuner_metrics_t);
		#endif	
}

RET_CODE dc_fm_validate(uint16_t freq)
{
		#ifdef SUPPORT_1T
			return dc_1T_fm_validate(freq);
		#endif	
		#ifdef SUPPORT_2T
			return dc_2T_fm_validate(freq);
		#endif	
		#ifdef SUPPORT_1DT
			return dc_1DT_fm_validate(freq);
		#endif	
		#ifdef SUPPORT_1DT1F
			return dc_1DT1F_HD_fm_validate(freq);
		#endif	
}


RET_CODE dc_am_validate(uint16_t freq)
{	
		#ifdef SUPPORT_1T
			return dc_1T_am_validate(freq);
		#endif	
		#ifdef SUPPORT_2T
			return dc_2T_am_validate(freq);
		#endif	
		#ifdef SUPPORT_1DT
			return dc_1DT_am_validate(freq);
		#endif	
		#ifdef SUPPORT_1DT1F
			return dc_1DT1F_HD_am_validate(freq);
		#endif	
}


RET_CODE dc_fm_seek(uint8_t seekup, uint8_t seekmode, _SEEK_PROCESS seek_callback)
{
	uint8_t space = 0;
	uint16_t tuneFreq = 0;
	uint16_t startFreq = 0;
	RET_CODE found = RET_NOT_FOUND;

	startFreq = dc_fm_get_frequency();
	space = dc_get_fm_space();
	

	if (seekup == 1)
	{
		tuneFreq = startFreq + space;
	}
	else
	{
		tuneFreq = startFreq - space;
	}
	//printf("startFrque = %d, space = %d tuneFreq = %d \n", startFreq, space, tuneFreq);

	while (found == RET_NOT_FOUND)
	{
		//user abort seeking
		if (IsAbortSeeking())
		{
			break;
		}

		if (seekmode == 0) 
		{
			if (seekup == 1 && tuneFreq > FM_TOP_FREQ)
			{
				tuneFreq = FM_BOT_FREQ;
			}
			else if (seekup == 0 && tuneFreq < FM_BOT_FREQ)
			{
				tuneFreq = FM_TOP_FREQ;
			}
		}
		else
		{
			if (seekup == 1 && tuneFreq > FM_TOP_FREQ ||
					seekup == 0 && tuneFreq < FM_BOT_FREQ)
			{
				found = RET_NOT_FOUND;
				break;
			}
		}

		//begin to process current frequncy, notify user the seeking progress
		if (NULL != seek_callback)
			(*seek_callback)(tuneFreq);
		
		found = dc_fm_validate(tuneFreq);

		//if it's the current frequency, stop here
		if (tuneFreq == startFreq)
		{
			break;
		}

		//update next tune freq
		if (seekup == 1)
		{
			tuneFreq += space;
		}
		else
		{
			tuneFreq -= space;
		}

		//sleep_ms(40);
	}

	return found;
}


uint8_t dc_fm_autoseek(_SEEK_PROCESS seek_callback, _SEEK_FOUND found_callback)
{
	RET_CODE found = RET_NOT_FOUND;
	uint8_t found_channel = 0;
	uint16_t current_frequency = FM_BOT_FREQ;

	si479x_tuner_mute(TUNER0, 1);

	if (dc_fm_validate(FM_BOT_FREQ) != RET_NOT_FOUND)
	{
		if (NULL != found_callback)
		{
			(*found_callback)(current_frequency);
		}
	}

	found = dc_fm_seek(1, 1, seek_callback);
	while (found != RET_NOT_FOUND)
	{
		dc_fm_get_tuner_metrics(&g_tuner_metrics);
		si479x_tuner_fm_rsq_status(TUNER0, 0, &g_tuner_metrics);

		current_frequency = g_tuner_metrics.Freq;

		if (NULL != found_callback)
		{
			(*found_callback)(current_frequency);
		}

		found_channel ++;

		found = dc_fm_seek(1, 1, seek_callback);
	}

	si479x_tuner_mute(TUNER0, 0);

	return found_channel;
}

RET_CODE dc_am_seek(uint8_t seekUp, uint8_t seekMode, _SEEK_PROCESS seek_callback)
{
	RET_CODE found = RET_NOT_FOUND;
	uint8_t space;
	uint16_t tuneFreq = 0;
	uint16_t startFreq = 0;

	startFreq = dc_am_get_frequency();
	space = dc_get_am_space();

	startFreq = startFreq/space * space;

	if (seekUp == 1)
	{
		tuneFreq = startFreq + space;
	}
	else
	{
		tuneFreq = startFreq - space;
	}
	
	while (found == RET_NOT_FOUND)
	{
		//user abort seeking
		if (IsAbortSeeking())
		{
			return found;
		}

		if (seekMode == 0) 
		{
			if (seekUp == 1 && tuneFreq > AM_TOP_FREQ)
			{
				tuneFreq = AM_BOT_FREQ;
			}
			else if (seekUp == 0 && tuneFreq < AM_BOT_FREQ)
			{
				tuneFreq = AM_TOP_FREQ;
			}
		}
		else
		{
			if (seekUp == 1 && tuneFreq > AM_TOP_FREQ ||
					seekUp == 0 && tuneFreq < AM_BOT_FREQ)
			{
				return found;
			}
		}

		//begin to process current frequncy, notify user the seeking progress
		if (NULL != seek_callback)
			(*seek_callback)(tuneFreq);
		
		found = dc_am_validate(tuneFreq);

		//if it's the current freqence, stop here
		if (tuneFreq == startFreq)
		{
			return found;
		}

		//update next tune freq
		if (seekUp == 1)
		{
			tuneFreq += space;
		}
		else
		{
			tuneFreq -= space;
		}
	}

	return found;
}

uint8_t dc_am_autoseek(_SEEK_PROCESS seek_callback, _SEEK_FOUND found_callback)
{
	RET_CODE found = RET_NOT_FOUND;
	//	uint8_t found = NONE_VALID;
	uint8_t space;
	uint8_t found_channel = 0;
	uint16_t current_frequency = AM_BOT_FREQ;

	si479x_tuner_mute(TUNER0, 1);

	space = dc_get_am_space();

	if (dc_am_validate(current_frequency) != RET_NOT_FOUND)
	{
		if (NULL != found_callback)
		{
			(*found_callback)(current_frequency);
		}
	}

	found = dc_am_seek(1, 1, seek_callback);
	while (found != RET_NOT_FOUND)
	{
		dc_am_get_tuner_metrics(&g_tuner_metrics);

		current_frequency = g_tuner_metrics.Freq;

		if (NULL != found_callback)
		{
			(*found_callback)(current_frequency);
		}

		found_channel ++;

		found = dc_am_seek(1, 1, seek_callback);
	}

	si479x_tuner_mute(TUNER0, 0);

	return found_channel;
}

uint8_t GetNextBand(uint8_t band)
{
	uint8_t temp = ++band;

		#ifdef SUPPORT_FMAMDAB
		temp %= 3;
		#else
		temp %= 2;
		#endif

	return temp;
}

void SetDivMode(uint8_t mode)
{
	#if (defined SUPPORT_2T) || (defined SUPPORT_1DT) || (defined SUPPORT_1DT1F)	
	if (mode == 1)
	{
		si479x_fm_phase_div(0, 0, 0);
	}
	else if (mode == 2)
	{
		si479x_fm_phase_div(1, 0, 0);
	}
	else
	{
		si479x_fm_phase_div(1, 1, 0);
		si479x_fm_phase_div(0, 1, 0);
	}
	#endif
}


RET_CODE si479x_command_dualtuner(uint16_t cmd_size, uint8_t *cmd)
{
	RET_CODE ret = RET_SUCCESS;

	if (cmd_size == 0)
		return RET_INVALIDINPUT;

	
	
	set_chip_id(CHIP_ID_0);
	//ERR_CHECKING(_si479x_checkPowerupStatus(state));
	ERR_CHECKING(si479x_waitForCTS());
	chip_writeCommand(cmd_size, cmd);
	ERR_CHECKING(si479x_waitForCTS());

	
	set_chip_id(CHIP_ID_1);
	//ERR_CHECKING(_si479x_checkPowerupStatus(state));
	ERR_CHECKING(si479x_waitForCTS());
	chip_writeCommand(cmd_size, cmd);
	ERR_CHECKING(si479x_waitForCTS());

	
	set_chip_id(CHIP_ID_0);

	return ret;
}

#define MAX_BYTES_PER_COMMAND	4096
RET_CODE chip_host_load(RADIO_APP_T tuner_work_app, FW_IMG_T image, uint8_t skip_header, uint8_t demod)
{
	RET_CODE ret = RET_SUCCESS;
	uint16_t read_length;

	FILE *file = NULL;

	SetFirmwareImage(image);
	
	if (skip_header != 0) 
	{
	
		file = fopen(GetFirmwareImage(), "r");
		if (NULL == file) 
		{
			printf("Open file %s failed!\n", GetFirmwareImage());
			return RET_INVALIDINPUT;
		}
		
		read_length = fread(bulk_buff+4, 1, 1024, file);
		
		if (read_length != 1024)
			return RET_ERROR;
	}

	
	bulk_buff[0] = HOST_LOAD;
	bulk_buff[1] = demod&0x03;

	//get firmware/bootloader segment
	read_length = fread(bulk_buff+4, 1, MAX_BYTES_PER_COMMAND-4, file);
	
	
	while (read_length > 0)
	{        
		_si479x_fill_cmd_uint16_t(cmd_buff, 2, read_length);
		bulk_buff[2] = 0;//(uint8_t)read_length;
		bulk_buff[3] = 0;//(uint8_t)(read_length>>8);

		//in fact, the read back length may not equal si479x_fw_buff_size-4
		if (APP_GE_FM_AM == tuner_work_app || APP_DE_FMPD_AM == tuner_work_app)
		{
			ERR_CHECKING(si479x_command(read_length+4, bulk_buff, 0, NULL));
		}
		else if(tuner_work_app == APP_GE_FMPD_AM)
		{
			ERR_CHECKING(si479x_command_dualtuner(read_length+4, bulk_buff));
		}

		read_length = fread(bulk_buff+4, 1, MAX_BYTES_PER_COMMAND-4, file);
	}
	
	fclose(file);

	//boot command
	return ret;
}

RET_CODE si479x_host_load(RADIO_APP_T tuner_work_app, FW_IMG_T image)
{
	return chip_host_load(tuner_work_app, image, 1, 0);
}

