/***************************************************************************************
                  Silicon Laboratories Broadcast Si475x/6x/9x module

   EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND CONDITIONS OF
     THE SOFTWARE LICENSE AGREEMENT IN THE DOCUMENTATION FILE CORRESPONDING
     TO THIS SOURCE FILE.
   IF YOU DO NOT AGREE TO THE LIMITED LICENSE AND CONDITIONS OF SUCH AGREEMENT,
     PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.

   Date: June 06 2014
  (C) Copyright 2014, Silicon Laboratories, Inc. All rights reserved.
****************************************************************************************/


//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <stddef.h>
#include <string.h>

#include "si479x_audio_api.h"
#include "si479x_hifi.h"

#include "audio_dts.h"

#ifdef SUPPORT_DTS_CS

typedef uint16_t DTS_PARA_ID; enum {
	DTS_PROCESS_MODE_PARA_ID = 2,
	DTS_PHANTOM_CENTER_PARA_ID = 4,

	DTS_FOCUS_CENTER_PARA_ID = 9,
	DTS_FOCUS_FRONT_PARA_ID = 10,
	DTS_FOCUS_REAR_PARA_ID = 11,

	DTS_TB_FRONT_PARA_ID = 12,
	DTS_TB_SUB_PARA_ID = 13,
	DTS_TB_REAR_PARA_ID = 14,

	DTS_TB_FRONT_SPKSZ_PARA_ID = 15,
	DTS_TB_SUB_SPKSZ_PARA_ID = 16,
	DTS_TB_REAR_SPKSZ_PARA_ID = 17,
	
	DTS_BYPASS_PARA_ID = 23,
};

typedef struct _dts_para_t {
	DTS_PARA_ID para_id;
	uint16_t min;
	uint16_t max;
}dts_para_t;

#define DTS_PARA(X, MIN, MAX) {.para_id = X, .min = MIN, .max = MAX,}

const dts_para_t dts_process_mode = DTS_PARA(DTS_PROCESS_MODE_PARA_ID, 0, 9);

const dts_para_t dts_phantom_center = DTS_PARA(DTS_PHANTOM_CENTER_PARA_ID, 0, 1);

const dts_para_t dts_focus_center = DTS_PARA(DTS_FOCUS_CENTER_PARA_ID, 0, 32767);
const dts_para_t dts_focus_front = DTS_PARA(DTS_FOCUS_FRONT_PARA_ID, 0, 32767);
const dts_para_t dts_focus_rear = DTS_PARA(DTS_FOCUS_REAR_PARA_ID, 0, 32767);

const dts_para_t dts_tb_front = DTS_PARA(DTS_TB_FRONT_PARA_ID, 0, 32767);
const dts_para_t dts_tb_sub = DTS_PARA(DTS_TB_SUB_PARA_ID, 0, 32767);
const dts_para_t dts_tb_rear = DTS_PARA(DTS_TB_REAR_PARA_ID, 0, 32767);

const dts_para_t dts_tb_front_spksz = DTS_PARA(DTS_TB_FRONT_SPKSZ_PARA_ID, 0, 8);
const dts_para_t dts_tb_sub_spksz = DTS_PARA(DTS_TB_SUB_SPKSZ_PARA_ID, 0, 8);
const dts_para_t dts_tb_rear_spksz = DTS_PARA(DTS_TB_REAR_SPKSZ_PARA_ID, 0, 8);

const dts_para_t dts_bypass = DTS_PARA(DTS_BYPASS_PARA_ID, 0, 1);

const uint8_t dts_data[46] = {
	0xFF, 0x7F, 0x4, 0x0, 0x1, 0x0, 0x0, 0x0, 0x1, 0x0, 0x1, 0x0, 
	0x1, 0x0, 0x1, 0x0, 0x1, 0x0, 0x1, 0x0, 0x1, 0x0, 0x0, 0x40, 
	0x0, 0x40, 0x0, 0x40, 0x2, 0x0, 0x2, 0x0, 0x2, 0x0, 0x0, 0x40,
	0x0, 0x40, 0x0, 0x40, 0x0, 0x40, 0x0, 0x40, 0x0, 0x0};

void DTS_Init()
{	
	si479x_hifi_module_parameter_write(DTS_CS_MODULE_ID, 0, 46, dts_data);
}

#define DTS_BYPASS_PARA_ID			23

void DTS_Write_Short(uint16_t para_id, uint16_t ws)
{
	uint8_t bytes[2] = {0};
	
	bytes[0] = (uint8_t)ws;
	bytes[1] = (uint8_t)(ws>>8);
	
	si479x_hifi_module_parameter_write(DTS_CS_MODULE_ID, para_id, 2, bytes);
}

void DTS_Set(dts_para_t dts_para, uint16_t value)
{
	value = LIMIT(value, dts_para.min, dts_para.max);
	
	DTS_Write_Short(dts_para.para_id, value);
}

void DTS_Bypass(uint16_t onoff)
{
	DTS_Set(dts_bypass, onoff);
}

void DTS_Set_PROCESS_MODE(uint16_t value)
{
	DTS_Set(dts_process_mode, value);
}

void DTS_Set_PHANTOM_CENTER(uint16_t value)
{
	DTS_Set(dts_phantom_center, value);
}

void DTS_Set_FOCUS_CENTER(uint16_t value)
{
	DTS_Set(dts_focus_center, value);
}

void DTS_Set_FOCUS_FRONT(uint16_t value)
{
	DTS_Set(dts_focus_front, value);
}

void DTS_Set_FOCUS_REAR(uint16_t value)
{
	DTS_Set(dts_focus_rear, value);
}

void DTS_Set_TB_FRONT(uint16_t value)
{
	DTS_Set(dts_tb_front, value);
}

void DTS_Set_TB_SUB(uint16_t value)
{
	DTS_Set(dts_tb_sub, value);
}

void DTS_Set_TB_REAR(uint16_t value)
{
	DTS_Set(dts_tb_rear, value);
}

void DTS_Set_TB_FRONT_SPKSZ(uint16_t value)
{
	DTS_Set(dts_tb_front_spksz, value);
}

void DTS_Set_TB_SUB_SPKSZ(uint16_t value)
{
	DTS_Set(dts_tb_sub_spksz, value);
}

void DTS_Set_TB_REAR_SPKSZ(uint16_t value)
{
	DTS_Set(dts_tb_rear_spksz, value);
}
#endif
