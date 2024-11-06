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

/**
 *@file this header file for internal API. Not API for customer directly using.
 *
 * @author Michael.Yi@silabs.com
 */
#ifndef _DC_DATA_H_
#define _DC_DATA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "dc_config.h"

extern const uint8_t DAC_filter3_on_cmd[64];

extern const uint8_t DAC_filter2_off_cmd[208];

extern const uint8_t DAC_filter3_off_cmd[64];

extern const uint8_t EQ_1band_no_filter_coefficients[16];

extern const uint8_t EQ_9band_filter_coefficients[][9][16];

#ifdef SUPPORT_WAV_PLAY
extern const uint8_t wave0_sec0[];
extern const uint8_t wave0_sec1[];
extern const uint8_t wave0_sec2[];

extern const uint8_t wave1_sec0[];
extern const uint8_t wave1_sec1[];
extern const uint8_t wave1_sec2[];
#endif

// #ifdef SUPPORT_FMAM
#define RADIO_PORT2NODE_NUM 2
extern hifi_port_node_connct_t radio_port2node_connections[RADIO_PORT2NODE_NUM];
// #endif

#ifdef SUPPORT_HUB_OUTPUT
extern hifi_port_node_connct_t output_node2port_hub_connections[HIFI_HUB_OUTPUT_CHANNELS];
#endif

#ifdef SUPPORT_4CHAN_OUTPUT
extern hifi_port_node_connct_t output_node2port_4ch_connections[HIFI_4CHAN_OUTPUT_CHANNELS];
#endif

#ifdef SUPPORT_6CHAN_OUTPUT
extern hifi_port_node_connct_t output_node2port_6ch_connections[HIFI_6CHAN_OUTPUT_CHANNELS];
#endif

#define AUDIO_6CH_RADIO_GAIN 0x0600 //-6.0 db
#define AUDIO_6CH_AUX_GAIN 0xfa00 //-6.0 db
#define AUDIO_6CH_I2S_GAIN 0xfa00 //-6.0 db
#define AUDIO_6CH_DAC_GAIN 0x0600 // 6.0 db

#define AUDIO_ADC_GAIN ADC_0DB

#define RADIO_PORT2PORT_NUM 2
extern hifi_port_2_port_connct_t radio_port2port_connections[RADIO_PORT2PORT_NUM];

#define AUXIN_PORT2NODE_NUM 2
extern hifi_port_node_connct_t auxin_port2node_connections[AUXIN_PORT2NODE_NUM];

#define I2S_PORT2NODE_NUM 2
extern hifi_port_node_connct_t i2s_port2node_connections[I2S_PORT2NODE_NUM];

#ifdef SUPPORT_AUDIO_EVENT_TONE
#define TONE_REQUEST_NUM 2
#define WAVE_REQUEST_NUM 1
extern hifi_port_node_connct_t tone_port2node_connections[TONE_REQUEST_NUM];

extern hifi_port_node_connct_t tone_port2node_multichan_connections[TONE_REQUEST_NUM];

extern hifi_port_node_connct_t wav_port2node_multichan_connections[WAVE_REQUEST_NUM];

extern hifi_port_node_connct_t tone_port2node_hub_connections[TONE_REQUEST_NUM];

extern hifi_port_node_connct_t wav_port2node_hub_connections[WAVE_REQUEST_NUM];

extern hifi_tone_descriptor_t g_tone_descriptor_t[2 * TONE_REQUEST_NUM];
#endif

#ifdef SUPPORT_WAV_PLAY
extern hifi_wave_type_t wave_list[4];
#endif

#if SUPPORT_WRITE_ADSP_MODULE_PARAM
extern const uint8_t g_mixer_gain[0x10];
#endif

#ifdef __cplusplus
}
#endif

#endif
