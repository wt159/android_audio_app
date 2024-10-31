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
#ifndef _SI479X_AUDIO_TYPES_H_
#define _SI479X_AUDIO_TYPES_H_

#include "type.h"
#include "dc_config.h"

typedef enum FILTER_TYPE
{
    FILTER_LOW_PASS,
    FILTER_HIGH_PASS,
    FILTER_BAND_PASS,
    FILTER_ALL_PASS,
    FILTER_NOTCH,
    FILTER_PEAK_EQ,
    FILTER_HIGH_SHELF,
    FILTER_LOW_SHELF,
} FILTER_TYPE;

typedef enum FILTER_PARAM_TYPE
{
    FILTER_PARAM_Q,
    FILTER_PARAM_S,
    FILTER_PARAM_BW,
} FILTER_PARAM_TYPE;

typedef struct biquad_params_bin {
	// S.W24.F22 
	uint32_t a1, a2, b0, b1, b2;
	int8_t bShift;
} biquad_params_bin;

typedef struct biquad_params {
	// gain dB
	double a1, a2, b0, b1, b2;
	int8_t bShift;
} biquad_params;

// IF THIS CHANGES, ref_audio_atl.c probably must change
typedef struct atl_binary_format  
{
	bool_t is_signed;
	uint8_t total_bits;
	uint8_t fractional_bits;

	// should be CEIL(total_bits / 8.0)
	uint8_t size_bytes;
} atl_binary_format ;

// Creates atl_binary_format struct. Convenience when defining other types.
#define ATL_NUM_FORMAT(sign, total_bits, fraction_bits, bytes) { (sign), (total_bits), (fraction_bits), (bytes) }
static atl_binary_format create_atl_num_format(bool_t is_signed, uint8_t total_bits, uint8_t fractional_bits, uint8_t size_bytes)
{
    atl_binary_format fmt = { 0 };
    fmt.is_signed = is_signed;
    fmt.total_bits = total_bits;
    fmt.fractional_bits = fractional_bits;
    fmt.size_bytes = size_bytes;
    return fmt;
}

const atl_binary_format atl_fmt_s8f0   = ATL_NUM_FORMAT(TRUE,  8, 0, 1);
const atl_binary_format atl_fmt_s16f8  = ATL_NUM_FORMAT(TRUE,  16, 8, 2);
const atl_binary_format atl_fmt_s16f0  = ATL_NUM_FORMAT(TRUE,  16, 0, 2);
const atl_binary_format atl_fmt_s24f22 = ATL_NUM_FORMAT(TRUE,  24, 22, 3);
const atl_binary_format atl_fmt_s32f24 = ATL_NUM_FORMAT(TRUE,  32, 24, 4);
const atl_binary_format atl_fmt_s32f31 = ATL_NUM_FORMAT(TRUE,  32, 31, 4);

const atl_binary_format atl_fmt_u16f8  = ATL_NUM_FORMAT(FALSE, 16, 8, 2);
const atl_binary_format atl_fmt_u32f24 = ATL_NUM_FORMAT(FALSE, 32, 24, 4);
const atl_binary_format atl_fmt_u32f30 = ATL_NUM_FORMAT(FALSE, 32, 30, 4);
const atl_binary_format atl_fmt_u32f31 = ATL_NUM_FORMAT(FALSE, 32, 31, 4);

// XBAR connect port/node gain format
#define ATL_FMT_XBAR_GAIN atl_fmt_s16f8

// EQ (siEqDpV) parameters
#define ATL_FMT_MODP_EQ_TAUSHIFT atl_fmt_u32f30
#define ATL_FMT_MODP_EQ_GAMMAH   atl_fmt_u32f31
#define ATL_FMT_MODP_EQ_THLD     atl_fmt_u32f30
#define ATL_FMT_MODP_EQ_H        atl_fmt_s24f22
#define ATL_FMT_MODP_EQ_BSHIFT   atl_fmt_s8f0

// Mixer - Fade parameter (gOut)
#define ATL_FMT_MODP_MIXER_GDUCK     atl_fmt_s16f8
#define ATL_FMT_MODP_MIXER_GOUT      atl_fmt_s16f8
#define ATL_FMT_MODP_MIXER_GINS      atl_fmt_s16f8
#define ATL_FMT_MODP_MIXER_ALPHAQPD  atl_fmt_s32f31
#define ATL_FMT_MODP_MIXER_THLDNOISE atl_fmt_s16f8
#define ATL_FMT_MODP_MIXER_TAUATTACK atl_fmt_s16f0
#define ATL_FMT_MODP_MIXER_TAUDECAY  atl_fmt_s16f0
#define ATL_FMT_MODP_MIXER_TDELAY    atl_fmt_u16f8

// Volume - gains format
#define ATL_FMT_MODP_VOLUME_GAIN atl_fmt_s16f8
#define ATL_FMT_MODP_VOLUME_RATE atl_fmt_s16f0

// QPD - meter format
#define ATL_FMT_MODP_QPD_METER_PCT  atl_fmt_u32f24
#define ATL_FMT_MODP_QPD_METER_DBFS atl_fmt_s32f24

#endif
