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
#ifndef _AUDIOMGR_H_
#define _AUDIOMGR_H_

#include "dc_config.h"
#include "si479x_core.h"
#include "si479x_hifi.h"
#include "type.h"

typedef uint8_t AUDIO_SOURCE;
enum {
    RADIO_AUDIO,
    AUXIN_AUDIO,
    I2S_AUDIO,
    OFF_AUDIO = 0xff,
};

#define EQ_TYPE_COUNT 5

typedef uint8_t EQ_TYPE_ID;
enum {
    JAZZ,
    POP,
    ROCK,
    CLASSIC,
    INDOOR,
    EQ_OFF,
};

void Audio_mgr_init();
void Audio_switch(uint8_t mode);
void Audio_mute(uint8_t mute);
void Audio_volume(uint8_t volume);

#ifdef SUPPORT_AUDIO_EVENT_TONE
void Audio_tonegen(uint8_t tone_id);
#endif

#ifdef SUPPORT_PRESET_EQ
void Audio_EQ(uint8_t id);
#endif

#ifdef SUPPORT_WAV_PLAY
RET_CODE Audio_load_wav();
#endif

void Audio_Treble(double gain);

void Audio_Middle(double gain);

void Audio_Bass(double gain);

void Audio_FadeFront(double front);

void Audio_FadeRear(double rear);

void Audio_BalanceLeft(double left);

void Audio_BalanceRight(double right);

void Audio_Bypass_Delay(int8_t onoff);

void Audio_Bypass_Cabin_EQ(int8_t onoff);

#endif
