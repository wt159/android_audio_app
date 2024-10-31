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
#ifndef _DC_1T_IMPLEMENT_H_
#define _DC_1T_IMPLEMENT_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "dc_bootup.h"
    
RET_CODE dc_1T_flashload_bootup();
RET_CODE dc_1T_hostload_bootup();

RET_CODE dc_1T_post_bootup(uint8_t band);

RET_CODE dc_1T_hostload_flashutility_bootup();

RET_CODE dc_1T_set_mode(uint8_t band);

RET_CODE dc_1T_fm_tune(uint16_t Freq);

RET_CODE dc_1T_am_tune(uint16_t Freq);

uint16_t dc_1T_fm_get_frequency();

RET_CODE dc_1T_fm_get_tuner_metrics(tuner_metrics_t *p_tuner_metrics_t);

uint16_t dc_1T_am_get_frequency();

RET_CODE dc_1T_am_get_tuner_metrics(tuner_metrics_t *p_tuner_metrics_t);

RET_CODE dc_1T_fm_validate(uint16_t freq);

RET_CODE dc_1T_am_validate(uint16_t freq);

//extern dc_implement_t const dc_1T_implment;

#ifdef __cplusplus
}
#endif

#endif


