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
#ifndef _AUDIODTS_H_
#define _AUDIODTS_H_

#include "type.h"

void DTS_Init();

void DTS_Bypass(uint16_t onoff);

void DTS_Set_PROCESS_MODE(uint16_t value);
void DTS_Set_PHANTOM_CENTER(uint16_t value);

void DTS_Set_FOCUS_CENTER(uint16_t value);
void DTS_Set_FOCUS_FRONT(uint16_t value);
void DTS_Set_FOCUS_REAR(uint16_t value);

void DTS_Set_TB_FRONT(uint16_t value);
void DTS_Set_TB_SUB(uint16_t value);
void DTS_Set_TB_REAR(uint16_t value);

void DTS_Set_TB_FRONT_SPKSZ(uint16_t value);
void DTS_Set_TB_SUB_SPKSZ(uint16_t value);
void DTS_Set_TB_REAR_SPKSZ(uint16_t value);

#endif
