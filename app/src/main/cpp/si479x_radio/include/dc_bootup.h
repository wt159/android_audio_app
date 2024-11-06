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
#ifndef _DC_BOOTUP_H_
#define _DC_BOOTUP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "dc_config.h"
#include "dc_share.h"
#include "platform_hal.h"

#include "si479x_core.h"
#include "type.h"

#ifdef SUPPORT_RDS
#define AFCHECK_FM_RSSI_THRESHOLD 18
#define AFCHECK_FM_SNR_THRESHOLD 12
#endif

typedef uint8_t GUID_TYPE;
enum {
    TUNER_HUB,
    TUNER_ONLY,
    FLASH_UTILITY,
};

typedef uint8_t BOOT_MODE_T;
enum {
    BOOT_MODE_USB,
    BOOT_MODE_FLASH,
};

#define BULCK_BUFF_SIZE 4096

extern uint8_t bulk_buff[BULCK_BUFF_SIZE];
extern uint32_t fw_size;

typedef uint8_t FW_IMG_T;
enum {
    TUNER_FW,
    FLASH_UTIL,
    BOOT_PATCH,
    FLASH_READER,
    DAB_LINKER,
    DAB_DEMOD,
    HD_LINKER,
    FMHD_DEMOD,
    AMHD_DEMOD,
    PART_KEY
};

load_config_t* get_load_config_args(CHIP_ID_T chipid);

void SetFirmwareImage(FW_IMG_T type);
void SetFirmwareImageFolder(const char* path);
const char* GetFirmwareImage();

#define MAX_IMAGE_COUNT 0x0f // 0x0f
#define IMAGE_INFO_SIZE 0x10

#define IMAGE_CNT_OFFSET 0x0100
#define GUID_IN_TABLE_OFFSET 0x0
#define IC_BITFIELD_IN_TABLE_OFFSET 0x8

extern uint32_t guid_table[MAX_IMAGE_COUNT];

extern uint8_t get_guid_table(BOOT_MODE_T bootmode, CHIP_ID_T chipid, FW_IMG_T image);

typedef void (*_SEEK_PROCESS)(uint16_t freq);

typedef void (*_SEEK_FOUND)(uint16_t freq);

void dc_init(uint8_t band);

RET_CODE dc_hostload_bootup();

RET_CODE dc_flashload_bootup();

RET_CODE dc_post_bootup(uint8_t band);

RET_CODE dc_set_mode(uint8_t band);

RET_CODE dc_fm_tune(uint16_t Freq);

RET_CODE dc_am_tune(uint16_t Freq);

uint16_t dc_fm_get_frequency();

uint16_t dc_am_get_frequency();

RET_CODE dc_fm_get_tuner_metrics(tuner_metrics_t* p_tuner_metrics_t);

RET_CODE dc_am_get_tuner_metrics(tuner_metrics_t* p_tuner_metrics_t);

RET_CODE dc_fm_seek(uint8_t seekUp, uint8_t seekMode, _SEEK_PROCESS seek_callback);

RET_CODE dc_am_seek(uint8_t seekUp, uint8_t seekMode, _SEEK_PROCESS seek_callback);

uint8_t dc_fm_autoseek(_SEEK_PROCESS seek_callback, _SEEK_FOUND found_callback);

uint8_t dc_am_autoseek(_SEEK_PROCESS seek_callback, _SEEK_FOUND found_callback);

void BootFailedHandler();

RET_CODE si479x_host_load(RADIO_APP_T tuner_work_app, FW_IMG_T image);

uint8_t GetNextBand(uint8_t band);

RET_CODE dc_hostload_flashutility_bootup();

RET_CODE chip_host_load(RADIO_APP_T tuner_work_app, FW_IMG_T image, uint8_t skip_header, uint8_t demod);

RET_CODE si479x_host_load(RADIO_APP_T tuner_work_app, FW_IMG_T image);

#ifdef __cplusplus
}
#endif

#endif
