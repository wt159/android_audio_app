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
#ifndef _PLATFORM_HAL_H_
#define _PLATFORM_HAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "dc_config.h"

#include "si479x_core.h"
#include "type.h"

// #define unused(x) {x = x;}

typedef uint8_t CHIP_ID_T;
enum {
    CHIP_ID_0,
    CHIP_ID_1,
    CHIP_ID_01,
};

void chip_writeCommand(uint16_t len, uint8_t* buffer);
void si479x_readReply(uint16_t len, uint8_t* buffer);

/**
 *   @breif sleep us function
 *
 *   Customer need add the implement according their platform
 *
 *   @param us: the time to delay in us
 *
 *   @retval void
 */
extern void sleep_us(uint16_t us);

/**
 *   @breif sleep us function
 *
 *   Customer need add the implement according their platform
 *
 *   @param us: the time to delay in ms
 *
 *   @retval void
 */
extern void sleep_ms(uint16_t ms);

void set_chip_id(CHIP_ID_T chip_id);

int platform_hal_init();

int chips_reset();

#ifdef __cplusplus
}
#endif

#endif
