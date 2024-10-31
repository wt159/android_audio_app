/***************************************************************************************
                  Skyworks MSS Automotive Tuner + Demod SDK

   EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND CONDITIONS OF
     THE SOFTWARE LICENSE AGREEMENT IN THE DOCUMENTATION FILE CORRESPONDING
     TO THIS SOURCE FILE.
   IF YOU DO NOT AGREE TO THE LIMITED LICENSE AND CONDITIONS OF SUCH AGREEMENT,
     PLEASE RETURN ALL SOURCE FILES TO SKYWORKS.

  DAB implmentation helper functions.
   FILE: DAB_common.h
   Date: June 28, 2016
  (C) Copyright 2015, Skyworks, Inc. All rights reserved.
****************************************************************************************/
#ifndef DAB_NVM_STORAGE_H_
#define DAB_NVM_STORAGE_H_

#include "type.h"
#include "i2c.h"
#include "feature_types.h"

size_t f_write(uint8_t* buf, size_t buflen);

size_t f_read(uint8_t* buf, size_t buflen);

void f_seek(uint8_t nIndex);

#endif // !DAB_NVM_STORAGE_H_
