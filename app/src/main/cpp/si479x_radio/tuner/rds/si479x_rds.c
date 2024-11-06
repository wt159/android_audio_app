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
#include <stdio.h>
#include <string.h>

#include "si479x_internel.h"
#include "si479x_rds.h"
#include "dc_bootup.h"
#include "si479x_tuner.h"

#ifdef SUPPORT_RDS

RET_CODE si479x_rds_status(uint8_t intack, uint8_t mtfifo, uint8_t statusonly)
{
    RET_CODE ret = RET_SUCCESS;
    
    clear_cmd_memory();
    
    cmd_buff[COMMAND_OFFSET+0] = FM_RDS_STATUS;
    if (statusonly != 0)
    {
        cmd_buff[COMMAND_OFFSET+1] |= RDS_STATUSONLY;
    }

    if (mtfifo != 0)
    {
        cmd_buff[COMMAND_OFFSET+1] |= RDS_MTFIFO;
    }

    if (intack != 0)
    {
        cmd_buff[COMMAND_OFFSET+1] |= RDS_INTACK;
    }

    ERR_CHECKING(si479x_tuner_cmd(TUNER0, 2, 20));

    return ret;
}


extern RET_CODE si479x_stauts_datalist(uint8_t cmd, uint8_t para, uint8_t maxlength, uint8_t buflength, uint8_t datalist[]);

RET_CODE si479x_rds_datalist(uint8_t length, uint8_t datalist[])
{
    return si479x_stauts_datalist(FM_RDS_STATUS, 0x04, 16, length, datalist);
}


#if 0

extern void _swap_16 (uint16_t *pValue);


RET_CODE si479x_rds_blockcount(uint8_t clear, rds_blockcount_t* pBlockCount)
{
    RET_CODE ret = RET_SUCCESS;
    
    clear_cmd_memory();
    
    cmd_buff[COMMAND_OFFSET+0] = FM_RDS_BLOCKCOUNT;
    if (clear != 0)
    {
        cmd_buff[COMMAND_OFFSET+1] = 0x01;
    }

    ERR_CHECKING(si479x_tuner_cmd(TUNER0, 2, 10));

    pBlockCount->expeted = *((uint16_t*)&rsp_buff[4]);
    pBlockCount->received = *((uint16_t*)&rsp_buff[6]);
    pBlockCount->uncorrectable = *((uint16_t*)&rsp_buff[8]);
#if SUPPORT_HOST_BITENDIAN
    _swap_16(&pBlockCount->expeted);
    _swap_16(&pBlockCount->received);
    _swap_16(&pBlockCount->uncorrectable);
#endif

    return ret;
}

RET_CODE si479x_af_rsq(uint16_t af, tuner_metrics_t * p_tuner_metrics)
{
    RET_CODE ret = RET_SUCCESS;
    
    //af check & smooth
    ERR_CHECKING(si479x_tuner_fm_tune(TUNER0, 3, 0, af));

    ERR_CHECKING(si479x_tuner_fm_rsq_status(TUNER0, 5, p_tuner_metrics));

    return ret;
}
#endif
#endif

