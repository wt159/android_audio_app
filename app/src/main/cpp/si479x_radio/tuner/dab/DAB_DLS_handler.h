/***************************************************************************************
                  Skyworks MSS Automotive Tuner + Demod SDK

   EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND CONDITIONS OF
     THE SOFTWARE LICENSE AGREEMENT IN THE DOCUMENTATION FILE CORRESPONDING
     TO THIS SOURCE FILE.
   IF YOU DO NOT AGREE TO THE LIMITED LICENSE AND CONDITIONS OF SUCH AGREEMENT,
     PLEASE RETURN ALL SOURCE FILES TO SKYWORKS.

   SDK DAB DLS decoder header
   FILE: DAB_DLS_Handler.h
   Supported IC : Si479xx + Si461x
   Date: September 21, 2015
  (C) Copyright 2015, Skyworks, Inc. All rights reserved.
****************************************************************************************/

#ifndef DAB_DLS_HANDLER_H_
#define DAB_DLS_HANDLER_H_

#include "feature_types.h"
#include "si461x_core.h"
#include "type.h"

#ifdef SUPPORT_DAB_DLS
//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------
typedef enum DAB_DLS_PLUS_TAG_TYPE {
    DL_PLUS_TITLE,
    DL_PLUS_NOT_IN_USE = 0xFF
} DAB_DLS_PLUS_TAG_TYPE;

//-----------------------------------------------------------------------------
// Function prototypes
//-----------------------------------------------------------------------------
void dls_init(dab_dls_string*);
void dls_update(dab_dls_string*, demod_dab_digital_service_t dlsData);

#endif

#endif
