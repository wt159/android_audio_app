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
 *@file this header file define the flashsubcommand/property address
 *
 * for si479x headunit.
 *
 * @author Michael.Yi@silabs.com
 */

#ifndef _SI479X_HIFI_H_
#define _SI479X_HIFI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "si479x_audio.h"
#include "si479x_core.h"

/////////////////////////////////////////////////////////////////
// audio_hub
/////////////////////////////////////////////////////////////////
// #define NOP                     0x00

#define XBAR_CONNECT_NOTE 0x01
#define XBAR_DISCONNECT_NOTE 0x02
#define XBAR_QUERY 0x03
#define NODE_QUERY 0x04
#define XBAR_PORT_MUTE 0x06
#define XBAR_PORT_QUERY 0x07

#define PARAMETER_WRITE 0x0A
#define PARAMETER_READ 0x0B
#define DEBUG_ENABLE 0x0C
#define DEBUG_DISABLE 0x0D
#define TRACE_PORT 0x0E
#define TRACE_NODE 0x0F

#define XBAR_CONNECT_PORT 0x10
#define XBAR_DISCONNECT_PORT 0x11
#define CLOCK_DOMAIN_SYNC 0x12
#define CLOCK_DOMAIN_QUERY 0x13
#define ERROR_LOG_READ 0x14
#define ERROR_LOG_OPTIONS 0x15

#define FRAME_NUMBER_QUERY 0x18
#define CPU_OCCUPANCY_QUERY 0x19
#define SET_SLEEP_MODE 0x1a
#define SET_CONNECTION_GAIN 0x1b
#define QUERY_CONNECTION_GAINS 0x1c

#define COEFF_WRITE 0x21
#define DSP_MEMORY_QUERY 0x22

#define SDK_ENABLE 0x27
#define SET_DAC_OFFSET 0x28

#define WAVE_PLAY 0x29
#define WAVE_STOP 0x2a
#define WAVE_ADD 0x2b
#define WAVE_APPEND 0x2c
#define WAVE_GEN_QUERY 0x2d
#define WAVE_QUERY_LIST 0x2e

#define SLAB_TRACE_MASK_READ 0x30
#define SLAB_TRACE_MASK_WRITE 0x31
#define SLAB_TRACE_MASK_SET_BIT 0x32
#define SLAB_TRACE_MASK_CLR_BIT 0x33
#define TRACE_MASK_READ Read 0x34
#define TRACE_MASK_WRITE 0x35
#define TRACE_MASK_SET_BIT 0x36
#define TRACE_MASK_CLR_BIT 0x37
#define TONEGEN_REQUEST 0x38
#define TONEGEN_PLAY 0x39
#define TONEGEN_STOP 0x3a
#define TONEGEN_ADD 0x3b
#define TONEGEN_QUERY 0x3c
#define TONEGEN_QUERY_LIST 0x3d
#define TONEGEN_MODIFY 0x3e
#define TONEGEN_QUERY_SEQ_ID 0x3f

#define READ_TOPOLOGY_MEMORY 0x40
#define QUERY_TOPOLOGY_MEMORY 0x41
#define QUERY_DAC_OFFSET 0x42
#define SET_ADC_DC_NOTCH 0x43

/////////////////////////////////////////////////////////////////
// audio dsp
/////////////////////////////////////////////////////////////////
#define AUD_PROC_NOP 0x40
#define PROC_ENABLE 0x41
#define PROC_SUSPEND 0x42
#define PROC_RESUME 0x43
#define READ_METER 0x44
#define MODIFY_CONTROL 0x45
#define QUERY_CONTROL 0x46
#define MODULE_PARAMETER_WRITE 0x47
#define MODULE_PARAMETER_READ 0x48

typedef struct _hifi_resp_t {
    uint8_t pipeid;
    uint8_t count;
    uint16_t length;
    uint8_t rdata[252];
} hifi_resp_t;

typedef struct _hifi_port_node_connct_t {
    uint8_t port;
    uint8_t channel;
    uint8_t node;
    uint16_t gain;
} hifi_port_node_connct_t;

typedef struct _hifi_port_2_port_connct_t {
    uint8_t inport;
    uint8_t inchannel;
    uint8_t outport;
    uint16_t outchannel;
    uint16_t gain;
} hifi_port_2_port_connct_t;

typedef struct _hifi_tone_descriptor_t {
    uint8_t sum_or_mpy;
    uint8_t fallramp;
    uint8_t decayramp;
    uint8_t attackramp;
    uint8_t wavetype;
    uint8_t oscid;
    uint8_t mod_index;
    uint8_t amp_peak;
    uint8_t amp_decay;
    uint16_t frequency;
    uint16_t start_time;
    uint16_t attack_time;
    uint8_t peak_duration;
    uint16_t peak_time;
    uint16_t decay_time;
    uint16_t fall_time;
} hifi_tone_descriptor_t;

typedef struct _hifi_wave_type_t {
    uint8_t wave_id;
    uint8_t wave_gain;
    uint16_t wave_duration;
} hifi_wave_type_t;

extern hifi_resp_t _hifi_resp;

RET_CODE si479x_hifi_cmd0(uint8_t count, uint16_t length);

RET_CODE si479x_hifi_resp0(hifi_resp_t* presp);

RET_CODE si479x_hifi_cmd1(uint8_t count, uint16_t length);

RET_CODE si479x_hifi_resp1(hifi_resp_t* presp);

RET_CODE si479x_hifi_xbar_connect_node(hifi_port_node_connct_t connections[], uint8_t con_count);

RET_CODE si479x_hifi_xbar_disconnect_node(hifi_port_node_connct_t connections[], uint8_t con_count);

RET_CODE si479x_hifi_xbar_connect_port(hifi_port_2_port_connct_t connections[], uint8_t con_count);

RET_CODE si479x_hifi_xbar_disconnect_port(hifi_port_2_port_connct_t connections[], uint8_t con_count);

RET_CODE si479x_hifi_set_connection_gain_port_2_node(uint8_t i_portid, uint8_t i_chid,
    uint8_t i_nodeid, uint16_t gain);

RET_CODE si479x_hifi_set_connection_gain_node_2_port(uint8_t o_portid, uint8_t o_chid,
    uint8_t o_nodeid, uint16_t gain);

RET_CODE si479x_hifi_set_connection_gain_port_2_port(uint8_t i_portid, uint8_t i_chid,
    uint8_t o_portid, uint8_t o_chid, uint16_t gain);

RET_CODE si479x_hifi_port_mute(uint8_t portid, uint8_t chid, uint8_t mutectl);

RET_CODE si479x_hifi_proc1_enable();

// RET_CODE si479x_hifi_proc2_enable();

RET_CODE si479x_hifi_module_parameter_write(uint16_t module_id, uint16_t para_id,
    uint16_t length, const uint8_t* pData);

RET_CODE si479x_hifi_module_parameter_read(uint16_t module_id, uint16_t para_id,
    uint8_t* pLength, uint8_t* pData);

RET_CODE si479x_hifi_port_query(PORT_ID port_id, PORT_STATE* pStatus);

RET_CODE si479x_hifi_coeff_write(uint8_t filterid, uint8_t ntaps, uint8_t datalen, const uint8_t* datalist);

RET_CODE si479x_set_sleep_mode(uint16_t mode);

RET_CODE si479x_set_dac_offset(uint32_t dac_offset);

#ifdef SUPPORT_AUDIO_EVENT_TONE
RET_CODE si479x_hifi_tonegen_request(uint8_t tone_id, uint16_t amp_abs,
    uint16_t repeat_count, uint8_t tone_count, hifi_tone_descriptor_t* tone);
#endif

#ifdef SUPPORT_WAV_PLAY
RET_CODE si479x_hifi_wave_play(uint8_t wave_gen_id, uint8_t wave_count, uint8_t loop_control,
    uint16_t loop_count, hifi_wave_type_t* wave_list);

RET_CODE si479x_hifi_wave_stop(uint8_t wave_gen_id);
#endif

RET_CODE si479x_hifi_cmd_exe(uint8_t cmd_size, uint8_t* cmd);

RET_CODE si479x_hifi_wave_add(uint8_t wave_id, uint16_t length, uint8_t* wavdata);

RET_CODE si479x_hifi_wave_append(uint8_t wave_id, uint16_t length, uint8_t* wavdata);

RET_CODE si479x_hifi_read_meter(uint16_t module_id, uint8_t meter_id);

RET_CODE si479x_hifi_modify_control(uint16_t module_id, uint8_t control_id, uint8_t op_oode, uint32_t control, uint32_t mask);

#ifdef __cplusplus
}
#endif

#endif
