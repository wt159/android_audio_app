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

//-----------------------------------------------------------------------------
//
// audio_mgr.h
//
// Contains the function prototypes for the functions contained in AMRXtest.c
//
//-----------------------------------------------------------------------------
#ifndef _SI479X_AUDIO_API_H_
#define _SI479X_AUDIO_API_H_

#include "type.h"

RET_CODE si479x_set_tone_treble(uint8_t index, double level, double s_value, uint16_t freq_hz);
RET_CODE si479x_set_tone_mid(uint8_t index, double level, double q_value, uint16_t freq_hz);
RET_CODE si479x_set_tone_bass(uint8_t index, double level, double s_value, uint16_t freq_hz);

RET_CODE si479x_set_fade(double front, double rear);
RET_CODE si479x_set_balance(double front, double rear);

RET_CODE si479x_get_mixer(uint8_t module_id, uint16_t gains[]);
RET_CODE si479x_set_mixer(uint8_t module_id, uint16_t gains[]);

RET_CODE si479x_set_output_gain(double gain);

#endif
