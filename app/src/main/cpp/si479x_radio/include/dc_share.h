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
#ifndef _DC_SHARE_H_
#define _DC_SHARE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "si479x_core.h"
#include "si479x_tuner.h"

#include "platform_hal.h"

extern uint8_t bBootmode;
extern uint8_t bHDAlta;
extern uint8_t bHDBlend;
extern uint8_t bHDSeekMode;

extern part_info_t g_chip0_part_info;
extern part_info_t g_chip1_part_info;

extern func_info_t g_chip0_func_info;
extern func_info_t g_chip1_func_info;

extern part_info_t g_part_info; ///< global data for tuner part information
extern func_info_t g_func_info;

typedef uint8_t RADIO_CHIP_ID_T;
enum {
    RADIO_CHIP_0,
    RADIO_CHIP_1,
};

typedef uint8_t DC_TYPE;
enum {
    DC_TYPE_GE_2T,
    DC_TYPE_DE_1DT1F,
    DC_TYPE_UNKNOWN = 0xff,
};

typedef uint8_t RADIO_APP_T;
enum {
    APP_GE_FM_AM,
    APP_GE_FMPD_AM,
    APP_DE_FMPD_AM_DAB = 0x11,
    APP_DE_HD_10 = 0x21,
    APP_DE_FMPD_AM = 0x30,

    APP_UNKOWN,
};

typedef uint8_t PD_MODE;
enum {
    PD_MODE_COMBINED,
    PD_MODE_TUNER0,
    PD_MODE_TUNER1,
};

extern PD_MODE bWorkMode;

const power_up_args_t* get_power_up_args(CHIP_ID_T chipid);

void UIHDDemodSettingsChecking();

void dc_set_custom_setting(RADIO_CHIP_ID_T chip_id);

extern uint8_t tuner_fw_image_name[];
extern uint8_t flash_fw_image_name[];

extern uint8_t si469x_boot_patch_image_name[];

extern uint8_t si469x_dab_linker_image_name[];
extern uint8_t si469x_dab_demod_image_name[];

extern uint8_t si469x_hd_linker_image_name[];
extern uint8_t si469x_fmhd_demod_image_name[];
extern uint8_t si469x_amhd_demod_image_name[];

extern uint8_t part_key_image_name[];

extern uint8_t gChip0PropertyServoDefault;
extern uint8_t gChip1PropertyServoDefault;

extern tuner_metrics_t g_tuner_metrics;

void SetDivMode(uint8_t mode);

/**
 *   @breif get current tune/seek spacing both for AM and FM
 *
 *   @param : void.
 *
 *   @retval : return the space value
 */
uint8_t dc_get_fm_space();

uint8_t dc_get_am_space();

void ResetAbortSeeking();

void AbortSeeking();

uint8_t IsAbortSeeking();

void dc_Identify(void);

#ifdef __cplusplus
}
#endif

#endif
