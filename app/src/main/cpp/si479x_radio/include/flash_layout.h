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

#ifndef _FLASH_LAYOUT_H_
#define _FLASH_LAYOUT_H_

#include "type.h"

#define SI479X_FIRMWARE_START 0x400 // 1k
// 1K ~ 634K
#define SI479X_FIRMWARE_MAX_SIZE (uint32_t)633 * 0x400 // 633k

// 636K ~ 637K
#define SI479X_PART_KEY_START (uint32_t)636 * 0x400 // 636k
#define SI479X_PART_KEY_MAX_SIZE (uint32_t)1 * 0x400 // 1k

// 640K ~ 648K
#define SI469X_BOOT_PATCH_START (uint32_t)640 * 0x400 // 640k bytes
#define SI469X_BOOT_PATCH_MAX_SIZE 16 * 0x400 // 8k bytes

// 652K ~ 676K
// #define SI469X_FLASH_PATCH_START (uint32_t)652*0x400 //652k bytes
// #define SI469X_FLASH_PATCH_MAX_SIZE 24*0x400 //24k bytes

// 680K ~ 1240K
#define SI469X_DAB_LINKER_START (uint32_t)680 * 0x400 // 680k bytes
#define SI469X_DAB_LINKER_MAX_SIZE (uint32_t)560 * 0x400 // 560k bytes

// 1240K ~ 1800K
#define SI469X_DAB_DEMOD_START (uint32_t)1240 * 0x400 // 1240k bytes
#define SI469X_DAB_DEMOD_MAX_SIZE (uint32_t)560 * 0x400 // 560k bytes

// 1800K ~ 2360K
#define SI469X_HD_LINKER_START (uint32_t)1800 * 0x400 // 1800k bytes
#define SI469X_HD_LINKER_MAX_SIZE (uint32_t)560 * 0x400 // 560k bytes

// 2360K ~ 2920K
#define SI469X_FMHD_DEMOD_START (uint32_t)2360 * 0x400 // 2360k bytes
#define SI469X_FMHD_DEMOD_MAX_SIZE (uint32_t)560 * 0x400 // 560k bytes

// 2920K ~ 3480K
#define SI469X_AMHD_DEMOD_START (uint32_t)2920 * 0x400 // 2920k bytes
#define SI469X_AMHD_DEMOD_MAX_SIZE (uint32_t)560 * 0x400 // 560k bytes

#endif
