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

/**
 *@file this header file declear the core function for si479x headunit.
 *
 * @author Michael.Yi@silabs.com
*/

#ifndef _SI479X_RDS_H_
#define _SI479X_RDS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "si479x_commanddefs.h"
#include "si479x_propertydefs.h"
#include "si479x_tuner.h"
#include "si479x_core.h"


typedef uint8_t RDS_OPTION; enum
{
    RDS_STATUSONLY = 0x04,
    RDS_MTFIFO = 0x02,
    RDS_INTACK = 0x01
};

typedef struct _rds_blockcount_t
{
    uint16_t expeted;
    uint16_t received;
    uint16_t uncorrectable;
}rds_blockcount_t;

RET_CODE si479x_rds_status(uint8_t intack, uint8_t mtfifo, uint8_t statusonly);
RET_CODE si479x_rds_datalist(uint8_t length, uint8_t datalist[]);
RET_CODE si479x_rds_blockcount(uint8_t clear, rds_blockcount_t* pBlockCount);

RET_CODE si479x_af_rsq(uint16_t af, tuner_metrics_t * p_tuner_metrics);
#ifdef __cplusplus
}
#endif

#endif
