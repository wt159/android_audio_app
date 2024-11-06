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

#ifndef _SI479X_INTERNEL_H_
#define _SI479X_INTERNEL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "type.h"

#define CMD_BUFF_SIZE 256
#define RSQ_BUFF_SIZE 260

extern uint8_t cmd_buff[CMD_BUFF_SIZE];
extern uint8_t rsp_buff[RSQ_BUFF_SIZE];

extern void clear_cmd_memory();
extern void _si479x_fill_cmd_uint16_t(uint8_t* buffer, uint8_t cmd_start, uint16_t u16data);
extern void _si479x_fill_cmd_uint32_t(uint8_t* buffer, uint8_t cmd_start, uint32_t u32data);

#ifdef __cplusplus
}
#endif

#endif
