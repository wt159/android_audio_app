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
#include <stdio.h>
#include <math.h>

#include "si479x_audio_types.h"
#include "si479x_audio_api.h"
#include "si479x_hifi.h"

// Unity gain biquad
#define BIQUAD_UNITY_GAIN {.a1=0, .a2=0, .b0=0x400000, .b1=0, .b2=0, .bShift=0, }

static const biquad_params_bin unity_gain = BIQUAD_UNITY_GAIN;

// If the total number of bits is less than 32,
//  then the remaining bits should be ignored.
uint32_t convert_to_binary_point(double value, bool_t is_signed, uint8_t total_bits, uint8_t fractional_bits)
{
	// The final binary point representation.
	uint64_t bin_rep = 0;
	uint8_t integral_bits = total_bits - fractional_bits;
	double remaining = value;

	// Decimal value of LSB
	double precision = 1.0 / (1 << fractional_bits);
	// Lower limit
	double lower_limit = 0;
	// Upper limit
	double upper_limit = 0;
	uint8_t i;
	uint32_t mask;

	//printf("ERROR: cannot get the binary representation of a format with >32 bits.\n");

	if (is_signed)
	{
		lower_limit = -1 * (1 << (integral_bits - 1));
		upper_limit = ((1 << (total_bits - 1)) - 1) * precision;
	}
	else
	{
		upper_limit = ((1 << total_bits) - 1) * precision;
	}

	// bounds checking
	if (value <= lower_limit)
	{
		// return most negative value, which is msb only set
		printf("WARNING: reached lower limit on binary point conversion.\n");
		return is_signed ? (1 << (total_bits - 1)) : 0;
	}

	if (value >= upper_limit)
	{
		printf("WARNING: reached positive limit on binary point conversion.\n");
		if (is_signed)
		{
			// For signed values, largest is all bits set except MSB
			// 2^(total_bits-1) - 1
			return (1 << (total_bits - 1)) - 1;
		}
		else
		{
			// Unsigned max is all bits set.
			// 2^(total_bits) - 1
			return (1 << total_bits) - 1;
		}
	}

	// Value within bounds, continue checking.
	if (remaining < 0)
	{
		// most negative number in 2's complement.
		bin_rep = (1 << (total_bits - 1));
		//printf("bin_rep: %d %x remaining: %f %x \n", bin_rep, bin_rep, remaining, remaining);
		remaining = (double)bin_rep + remaining;
		//printf("bin_rep: %d %x remaining: %f %x \n", bin_rep, bin_rep, remaining, remaining);
	}

	// remaining is now a positive number to convert
	//double cc = remaining / precision;
	bin_rep |= (uint64_t)round(remaining / precision);
	//printf("bin_rep: %d %x cc: %f %x \n", bin_rep, bin_rep, cc, cc);

	mask = 0;
	for (i = 0; i < total_bits; i++)
		mask |= (1 << i);

	// clear upper bits that are set.
	return bin_rep & mask;
}

uint32_t convert_to_binary_point_struct(double value, const atl_binary_format * num_format)
{
	//printf(num_format, "ERROR: invalid input convert_to_binary_point_struct()\n");
	return convert_to_binary_point(value, num_format->is_signed, 
		num_format->total_bits, num_format->fractional_bits);
}


void convert_biquad_fmt(const biquad_params* input, biquad_params_bin* output)
{
    if (!input || !output)
        return;

    output->a1 = convert_to_binary_point_struct(input->a1, &ATL_FMT_MODP_EQ_H);
    output->a2 = convert_to_binary_point_struct(input->a2, &ATL_FMT_MODP_EQ_H);
    output->b0 = convert_to_binary_point_struct(input->b0, &ATL_FMT_MODP_EQ_H);
    output->b1 = convert_to_binary_point_struct(input->b1, &ATL_FMT_MODP_EQ_H);
    output->b2 = convert_to_binary_point_struct(input->b2, &ATL_FMT_MODP_EQ_H);
    output->bShift = input->bShift;
	
	printf("a1: %f, a2: %f, b0: %f, b1: %f, b2: %f\n", input->a1, input->a2, input->b0, input->b1, input->b2);
	printf("a1: %x, a2: %x, b0: %x, b1: %x, b2: %x\n", output->a1, output->a2, output->b0, output->b1, output->b2);
}


RET_CODE calc_biquad_coeffs(FILTER_TYPE filter, double gain_db,
    FILTER_PARAM_TYPE param_type, double param_value, double freq_hz, double sample_rate,
    biquad_params* coeffs_out)
{
    // Biquad coefficients
    double a0 = 1.0, a1 = 0.0, a2 = 0.0, b0 = 1.0, b1 = 0.0, b2 = 0.0;
    // Normalized biquad coefficients
    double a1N, a2N, b0N, b1N, b2N;

    double bnExtrema;
    int32_t shift;

    double a = pow(10, (gain_db / 40.0));
    double w0 = 2 * M_PI * freq_hz / sample_rate;
    double cs = cos(w0);
    double sn = sin(w0);

    double alpha = 0;
    double beta = 0;

    if (!coeffs_out)
        return RET_INVALIDINPUT;

    if ((filter == FILTER_HIGH_SHELF || filter == FILTER_LOW_SHELF)
        && param_type != FILTER_PARAM_S)
    {
        return RET_INVALIDINPUT;
    }

    switch (param_type)
    {
    case FILTER_PARAM_Q:
        alpha = sn / (2.0 * param_value);
        break;
    case FILTER_PARAM_BW:
    {
        double q = freq_hz / param_value;
        alpha = sn / (2.0 * q);
        //alpha = sn*Math.Sinh(NaturalLog(2)/2*param_value*w0/sn);
        break;
    }
    case FILTER_PARAM_S:
        alpha = sn / (2.0 * sqrt((a + 1.0 / a) * (1.0 / param_value - 1) + 2));
        beta = sqrt((a*a + 1) / param_value - (a - 1)*(a - 1));
        break;
    }

    switch (filter)
    {
    case FILTER_LOW_PASS:
        b0 = (1 - cs) / 2.0;
        b1 = 1 - cs;
        b2 = (1 - cs) / 2.0;
        a0 = 1 + alpha;
        a1 = -2.0 * cs;
        a2 = 1 - alpha;
        break;
    case FILTER_HIGH_PASS:
        b0 = (1 + cs) / 2.0;
        b1 = -1 - cs;
        b2 = (1 + cs) / 2.0;
        a0 = 1 + alpha;
        a1 = -2 * cs;
        a2 = 1 - alpha;
        break;
    case FILTER_BAND_PASS:
        b0 = alpha;
        b1 = 0;
        b2 = -alpha;
        a0 = 1 + alpha;
        a1 = -2 * cs;
        a2 = 1 - alpha;
        break;
    case FILTER_NOTCH:
        b0 = 1;
        b1 = -2 * cs;
        b2 = 1;
        a0 = 1 + alpha;
        a1 = -2 * cs;
        a2 = 1 - alpha;
        break;
    case FILTER_ALL_PASS:
        b0 = 1 - alpha;
        b1 = -2 * cs;
        b2 = 1 + alpha;
        a0 = 1 + alpha;
        a1 = -2 * cs;
        a2 = 1 - alpha;
        break;
    case FILTER_PEAK_EQ:
        b0 = 1 + alpha*a;
        b1 = -2 * cs;
        b2 = 1 - alpha*a;
        a0 = 1 + alpha / a;
        a1 = -2 * cs;
        a2 = 1 - alpha / a;
        break;
    case FILTER_HIGH_SHELF:
        b0 = a * ((a + 1) + (a - 1) * cs + beta * sn);
        b1 = -2 * a * ((a - 1) + (a + 1) * cs);
        b2 = a * ((a + 1) + (a - 1) * cs - beta * sn);
        a0 = (a + 1) - (a - 1) * cs + beta * sn;
        a1 = 2 * ((a - 1) - (a + 1) * cs);
        a2 = (a + 1) - (a - 1) * cs - beta * sn;
        break;
    case FILTER_LOW_SHELF:
        b0 = a*((a + 1) - (a - 1)*cs + beta*sn);
        b1 = 2 * a*((a - 1) - (a + 1)*cs);
        b2 = a*((a + 1) - (a - 1)*cs - beta*sn);
        a0 = (a + 1) + (a - 1)*cs + beta*sn;
        a1 = -2 * ((a - 1) + (a + 1)*cs);
        a2 = (a + 1) + (a - 1)*cs - beta*sn;
        break;
    }

    // Normalize to a0
    a1N = -a1 / a0;
    a2N = -a2 / a0;
    b0N = b0 / a0;
    b1N = b1 / a0;
    b2N = b2 / a0;

    // Deal with values that are too large.
    bnExtrema = MAX(MAX(ABS(b0N), ABS(b1N)), ABS(b2N));

    shift = 0;
    if (bnExtrema > 2)
        shift = (int32_t)floor(log(bnExtrema) / log(2));
    else if (b0N >= 2)
        b0N = 1.9999999999999;
    else if (b1N >= 2)
        b1N = 1.9999999999999;
    else if (b2N >= 2)
        b2N = 1.9999999999999;

    //D_ASSERT_MSG(shift < 31, "ERROR: should use pow() instead to avoid errors!\n");
    b0N = b0N / (1 << shift);
    b1N = b1N / (1 << shift);
    b2N = b2N / (1 << shift);

    coeffs_out->a1 = a1N;
    coeffs_out->a2 = a2N;
    coeffs_out->b0 = b0N;
    coeffs_out->b1 = b1N;
    coeffs_out->b2 = b2N;
    coeffs_out->bShift = shift;

    return RET_SUCCESS;
}


RET_CODE calc_biquad_coeffs_binary(FILTER_TYPE filter, double gain_db,
    FILTER_PARAM_TYPE param_type, double param_value, double freq_hz, double sample_rate,
    biquad_params_bin* coeffs_out)
{
    RET_CODE ret = RET_SUCCESS;
    biquad_params reg_coeffs;
    if (!coeffs_out)
        return RET_INVALIDINPUT;

    ret = calc_biquad_coeffs(filter, gain_db, param_type, param_value, freq_hz, sample_rate, &reg_coeffs);
    convert_biquad_fmt(&reg_coeffs, coeffs_out);
    return ret;
}

#ifdef SUPPORT_3X3BANDS_TONE_CONTROL	
#define TONE_BIQUAD_CNT  3
#define BIQUAD_PARAM_H_SZ 3
#define BIQUAD_PARAM_H_CNT 5
#define BIQUAD_PARAM_H_ARR_SZ (BIQUAD_PARAM_H_SZ * BIQUAD_PARAM_H_CNT)
#define BIQUAD_PARAM_BSHFT_SZ 1
#define BIQUAD_PARAMS_SZ (BIQUAD_PARAM_H_ARR_SZ + BIQUAD_PARAM_BSHFT_SZ)
#define PARAMS_SZ (TONE_BIQUAD_CNT * BIQUAD_PARAMS_SZ)
// Byte array big enough to update Tone params H[45] and bShift[9]  (9 biquads)

static biquad_params_bin s_bass[TONE_BIQUAD_CNT] = {
	BIQUAD_UNITY_GAIN,
	BIQUAD_UNITY_GAIN,
	BIQUAD_UNITY_GAIN
};

static biquad_params_bin s_mid[TONE_BIQUAD_CNT] = {
	BIQUAD_UNITY_GAIN,
	BIQUAD_UNITY_GAIN,
	BIQUAD_UNITY_GAIN
};

static biquad_params_bin s_treble[TONE_BIQUAD_CNT] = {
	BIQUAD_UNITY_GAIN,
	BIQUAD_UNITY_GAIN,
	BIQUAD_UNITY_GAIN
};
#endif

#ifdef SUPPORT_9BANDS_TONE_CONTROL	
#define TONE_BIQUAD_CNT  9
#define BIQUAD_PARAM_H_SZ 3
#define BIQUAD_PARAM_H_CNT 5
#define BIQUAD_PARAM_H_ARR_SZ (BIQUAD_PARAM_H_SZ * BIQUAD_PARAM_H_CNT)
#define BIQUAD_PARAM_BSHFT_SZ 1
#define BIQUAD_PARAMS_SZ (BIQUAD_PARAM_H_ARR_SZ + BIQUAD_PARAM_BSHFT_SZ)
#define PARAMS_SZ (TONE_BIQUAD_CNT * BIQUAD_PARAMS_SZ)
// Byte array big enough to update Tone params H[45] and bShift[9]  (9 biquads)

#define BASS_BQ_OFFSET	0
#define MIDS_BQ_OFFSET	3
#define TREB_BQ_OFFSET	6

static biquad_params_bin s_tone_control[TONE_BIQUAD_CNT] = {
	BIQUAD_UNITY_GAIN,
	BIQUAD_UNITY_GAIN,
	BIQUAD_UNITY_GAIN,
	BIQUAD_UNITY_GAIN,
	BIQUAD_UNITY_GAIN,
	BIQUAD_UNITY_GAIN,
	BIQUAD_UNITY_GAIN,
	BIQUAD_UNITY_GAIN,
	BIQUAD_UNITY_GAIN,
};
#endif


// Copies biquad coefficients into a buffer (of multiple biquads).
static RET_CODE copy_biquad_params(uint8_t* buf, uint8_t biquad, uint8_t biquad_cnt, const biquad_params_bin* filter)
{
    const uint16_t h_offset = biquad * BIQUAD_PARAM_H_ARR_SZ ;
    const uint16_t bshift_offset = (biquad_cnt * BIQUAD_PARAM_H_ARR_SZ) + biquad *  BIQUAD_PARAM_BSHFT_SZ;
    uint8_t* pbuf = buf;

    // copy the H[] values
    pbuf += h_offset;

    memcpy(pbuf, &filter->a1, BIQUAD_PARAM_H_SZ);
    pbuf += BIQUAD_PARAM_H_SZ;
    memcpy(pbuf, &filter->a2, BIQUAD_PARAM_H_SZ);
    pbuf += BIQUAD_PARAM_H_SZ;
    memcpy(pbuf, &filter->b0, BIQUAD_PARAM_H_SZ);
    pbuf += BIQUAD_PARAM_H_SZ;
    memcpy(pbuf, &filter->b1, BIQUAD_PARAM_H_SZ);
    pbuf += BIQUAD_PARAM_H_SZ;
    memcpy(pbuf, &filter->b2, BIQUAD_PARAM_H_SZ);
    pbuf += BIQUAD_PARAM_H_SZ;

    // copy the bShift[] value
    pbuf = buf + bshift_offset;
    memcpy(pbuf, &filter->bShift, BIQUAD_PARAM_BSHFT_SZ);

    return RET_SUCCESS;
}

#define EQ_DPV_PAPA_ID	1
RET_CODE apply_tone_filter(biquad_params_bin* filter, uint8_t module_id)
{
    RET_CODE ret = RET_SUCCESS;

    uint8_t params_buf[PARAMS_SZ] = { 0 };
	
	for (uint8_t i=0; i<TONE_BIQUAD_CNT; i++) {
		ret |= copy_biquad_params(params_buf, i, TONE_BIQUAD_CNT, filter+i);
	}
	
	return si479x_hifi_module_parameter_write(module_id, EQ_DPV_PAPA_ID, PARAMS_SZ, params_buf);
}


RET_CODE si479x_set_tone_treble(uint8_t index, double level, double s_value, uint16_t freq_hz)
{
	RET_CODE ret = RET_SUCCESS;

	level = LIMIT(level, AUDIO_TREBLE_MIN, AUDIO_TREBLE_MAX);
	s_value = LIMIT(s_value, AUDIO_BASS_S_VALUE_MIN, AUDIO_BASS_S_VALUE_MAX);
	freq_hz = LIMIT(freq_hz, AUDIO_FREQ_MIN, AUDIO_FREQ_MAX);
	index = LIMIT(index, 0, TONE_BIQUAD_CNT-1);

	#ifdef SUPPORT_3X3BANDS_TONE_CONTROL
	// Get the biquad filter gains for the treble level
	calc_biquad_coeffs_binary(FILTER_HIGH_SHELF, level, FILTER_PARAM_S, s_value, freq_hz, AUDIO_SAMPLE_RATE, &s_bass[index]);
	
	ret |= apply_tone_filter(s_bass, HIFI_TREB_ID);
	#endif
	
	#ifdef SUPPORT_9BANDS_TONE_CONTROL
	// Get the biquad filter gains for the treble level
	calc_biquad_coeffs_binary(FILTER_HIGH_SHELF, level, FILTER_PARAM_S, s_value, freq_hz, AUDIO_SAMPLE_RATE, &s_tone_control[index+TREB_BQ_OFFSET]);
	
	ret |= apply_tone_filter(s_tone_control, HIFI_6CHAN_LEFT_EQ_ID);
	ret |= apply_tone_filter(s_tone_control, HIFI_6CHAN_RIGHT_EQ_ID);
	#endif

	return ret;
}

RET_CODE si479x_set_tone_mid(uint8_t index, double level, double q_value, uint16_t freq_hz)
{
	RET_CODE ret = RET_SUCCESS;

	level = LIMIT(level, AUDIO_MID_MIN, AUDIO_MID_MAX);
	q_value = LIMIT(q_value, AUDIO_MID_Q_VALUE_MIN, AUDIO_MID_Q_VALUE_MAX);
	freq_hz = LIMIT(freq_hz, AUDIO_FREQ_MIN, AUDIO_FREQ_MAX);
	index = LIMIT(index, 0, TONE_BIQUAD_CNT-1);

	#ifdef SUPPORT_3X3BANDS_TONE_CONTROL
	// Get the biquad filter gains for the mid level
	calc_biquad_coeffs_binary(FILTER_PEAK_EQ, level, FILTER_PARAM_Q, q_value, freq_hz, AUDIO_SAMPLE_RATE, &s_mid[index]);
	ret |= apply_tone_filter(s_mid, HIFI_MIDS_ID);
	#endif

	#ifdef SUPPORT_9BANDS_TONE_CONTROL
	// Get the biquad filter gains for the mid level
	calc_biquad_coeffs_binary(FILTER_PEAK_EQ, level, FILTER_PARAM_Q, q_value, freq_hz, AUDIO_SAMPLE_RATE, &s_tone_control[index+MIDS_BQ_OFFSET]);

	ret |= apply_tone_filter(s_tone_control, HIFI_6CHAN_LEFT_EQ_ID);
	ret |= apply_tone_filter(s_tone_control, HIFI_6CHAN_RIGHT_EQ_ID);
	#endif
	
	return ret;
}

RET_CODE si479x_set_tone_bass(uint8_t index, double level, double s_value, uint16_t freq_hz)
{
	RET_CODE ret = RET_SUCCESS;

	level = LIMIT(level, AUDIO_BASS_MIN, AUDIO_BASS_MAX);
	s_value = LIMIT(s_value, AUDIO_BASS_S_VALUE_MIN, AUDIO_BASS_S_VALUE_MAX);
	freq_hz = LIMIT(freq_hz, AUDIO_FREQ_MIN, AUDIO_FREQ_MAX);
	index = LIMIT(index, 0, TONE_BIQUAD_CNT-1);

	#ifdef SUPPORT_3X3BANDS_TONE_CONTROL
	// Get the biquad filter gains for the bass level
	calc_biquad_coeffs_binary(FILTER_LOW_SHELF, level, FILTER_PARAM_S, s_value, freq_hz, AUDIO_SAMPLE_RATE, &s_treble[index]);
	ret |= apply_tone_filter(s_treble, HIFI_BASS_ID);
	#endif

	#ifdef SUPPORT_9BANDS_TONE_CONTROL
	calc_biquad_coeffs_binary(FILTER_LOW_SHELF, level, FILTER_PARAM_S, s_value, freq_hz, AUDIO_SAMPLE_RATE, &s_tone_control[index+BASS_BQ_OFFSET]);

	ret |= apply_tone_filter(s_tone_control, HIFI_6CHAN_LEFT_EQ_ID);
	ret |= apply_tone_filter(s_tone_control, HIFI_6CHAN_RIGHT_EQ_ID);
	#endif
	
	return ret;
}


uint8_t* add_param_to_buf(uint8_t* buf, double val, atl_binary_format format)
{
    //D_ASSERT(buf);

    uint32_t tmp = convert_to_binary_point_struct(val, &format);
    memcpy(buf, &tmp, format.size_bytes);
    buf += format.size_bytes;

    return buf;
}

RET_CODE si479x_set_fade_module(uint8_t module_id, uint8_t index, double level)
{
    // non-binary struct must be larger than the binary representation
	uint8_t parambuf[MIXER_GAIN_CNT*2] = {0};
    uint8_t *pbuf = parambuf;

    // actual len
    for (uint8_t i = 0; i < MIXER_GAIN_CNT; i++)
	{
		if (i == index) {
			pbuf = add_param_to_buf(pbuf, level, ATL_FMT_MODP_MIXER_GINS);
		} else {
			pbuf = add_param_to_buf(pbuf, 0.0f, ATL_FMT_MODP_MIXER_GINS);
		}
	}
	
	return si479x_hifi_module_parameter_write(module_id, MIXER_PARA_ID, sizeof(parambuf), parambuf);
}

RET_CODE si479x_set_fade(double front, double rear)
{
    RET_CODE ret = RET_SUCCESS;
    uint16_t offset = 0;

	#ifdef HIFI_6CHAN_OUTPUT_CHANNELS
	ret |= si479x_set_fade_module(HIFI_6CHAN_LEFT_FRONT_MIXER_ID, 0, front);
	ret |= si479x_set_fade_module(HIFI_6CHAN_RIGHT_FRONT_MIXER_ID, 0, front);
	
	ret |= si479x_set_fade_module(HIFI_6CHAN_LEFT_REAR_MIXER_ID, 0, rear);
	ret |= si479x_set_fade_module(HIFI_6CHAN_RIGHT_REAR_MIXER_ID, 0, rear);
	#endif
	
	#ifdef HIFI_4CHAN_OUTPUT_CHANNELS
	ret |= si479x_set_fade_module(HIFI_4CHAN_LEFT_FRONT_MIXER_ID, 0, front);
	ret |= si479x_set_fade_module(HIFI_4CHAN_RIGHT_FRONT_MIXER_ID, 0, front);
	
	ret |= si479x_set_fade_module(HIFI_4CHAN_LEFT_REAR_MIXER_ID, 0, rear);
	ret |= si479x_set_fade_module(HIFI_4CHAN_RIGHT_REAR_MIXER_ID, 0, rear);
	#endif
	
	return ret;
}

 
#define BALANCE_GAIN_CNT	1
#define BALANCE_PARA_ID	5

RET_CODE si479x_set_banlance_module(uint8_t module_id, double level)
{
    // non-binary struct must be larger than the binary representation
	uint8_t parambuf[BALANCE_GAIN_CNT*2] = {0};
    uint8_t *pbuf = parambuf;

    // actual len
	add_param_to_buf(pbuf, level, ATL_FMT_MODP_VOLUME_GAIN);
	
	return si479x_hifi_module_parameter_write(module_id, BALANCE_PARA_ID, sizeof(parambuf), parambuf);
}

RET_CODE si479x_set_balance(double left, double right)
{
    RET_CODE ret = RET_SUCCESS;
    uint16_t offset = 0;

	#ifdef HIFI_6CHAN_OUTPUT_CHANNELS
	ret |= si479x_set_banlance_module(HIFI_6CHAN_LEFT_VOLUME_ID, left);
	ret |= si479x_set_banlance_module(HIFI_6CHAN_RIGHT_VOLUME_ID, right);
	#endif
	
	#ifdef HIFI_4CHAN_OUTPUT_CHANNELS
	ret |= si479x_set_banlance_module(HIFI_4CHAN_LEFT_VOLUME_ID, left);
	ret |= si479x_set_banlance_module(HIFI_4CHAN_RIGHT_VOLUME_ID, right);
	#endif
	
	return ret;
}


RET_CODE si479x_get_mixer(uint8_t module_id, uint16_t gains[])
{
    RET_CODE ret = RET_SUCCESS;
	uint8_t buffer[32];
	uint8_t length;
	
	//convert_to_binary_point_struct
	ret |= si479x_hifi_module_parameter_read(module_id, MIXER_PARA_ID, &length, buffer);
	
    // actual len
    for (uint8_t i = 0; i < MIXER_GAIN_CNT; i++)
	{
		gains[i] = ((uint16_t)buffer[2*i+8]&0xff) | ((uint16_t)buffer[2*i+9]<<8);
	}
	
	return ret;
}


RET_CODE si479x_set_mixer(uint8_t module_id, uint16_t gains[])
{
    RET_CODE ret = RET_SUCCESS;
	uint8_t parambuf[MIXER_GAIN_CNT*2] = {0};
	uint8_t length;
		
    // actual len
    for (uint8_t i = 0; i < MIXER_GAIN_CNT; i++)
	{
		parambuf[2*i] = (uint8_t)gains[i];
		parambuf[2*i+1] = (uint8_t)(gains[i]>>8);
	}
	
	return si479x_hifi_module_parameter_write(module_id, MIXER_PARA_ID, sizeof(parambuf), parambuf);
}


#define MIXER_OUTPUT_PARA_ID	3
static RET_CODE si479x_set_mixer_output(uint8_t module_id, double level)
{
    // non-binary struct must be larger than the binary representation
	uint8_t parambuf[2] = {0};

    // actual len
	add_param_to_buf(parambuf, level, ATL_FMT_MODP_MIXER_GINS);
	
	return si479x_hifi_module_parameter_write(module_id, MIXER_OUTPUT_PARA_ID, sizeof(parambuf), parambuf);
}


RET_CODE si479x_set_output_gain(double gain)
{
    RET_CODE ret = RET_SUCCESS;
    uint16_t offset = 0;

	#ifdef HIFI_6CHAN_OUTPUT_CHANNELS
	ret |= si479x_set_mixer_output(HIFI_6CHAN_LEFT_FRONT_MIXER_ID, gain);
	ret |= si479x_set_mixer_output(HIFI_6CHAN_RIGHT_FRONT_MIXER_ID, gain);

	ret |= si479x_set_mixer_output(HIFI_6CHAN_LEFT_REAR_MIXER_ID, gain);
	ret |= si479x_set_mixer_output(HIFI_6CHAN_RIGHT_REAR_MIXER_ID, gain);
	
	ret |= si479x_set_mixer_output(HIFI_6CHAN_CENTER_MIXER_ID, gain);
	ret |= si479x_set_mixer_output(HIFI_6CHAN_SUBVOF_MIXER_ID, gain);
	#endif
	
	#ifdef HIFI_4CHAN_OUTPUT_CHANNELS
	ret |= si479x_set_mixer_output(HIFI_4CHAN_LEFT_FRONT_MIXER_ID, gain);
	ret |= si479x_set_mixer_output(HIFI_4CHAN_RIGHT_FRONT_MIXER_ID, gain);

	ret |= si479x_set_mixer_output(HIFI_4CHAN_LEFT_REAR_MIXER_ID, gain);
	ret |= si479x_set_mixer_output(HIFI_4CHAN_RIGHT_REAR_MIXER_ID, gain);
	#endif
	
	return ret;
}
