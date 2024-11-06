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

#ifndef _SI479X_AUDIO_H_
#define _SI479X_AUDIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "dc_config.h"
#include "si479x_core.h"

////////////////////////////////////////////////////////
typedef uint8_t PIN_GROUP;
enum {
    PIN_FLASH = 1,
    PIN_ZIF0,
    PIN_ZIF1,
    PIN_DIN0,
    PIN_GP = 9,
    PIN_DOUT0,
    PIN_IC = 0xc,
};

typedef struct _pin_des_t {
    uint8_t digdrive;
    uint8_t lvdsload;
    uint8_t lvdsdrive;
} pin_des_t;

////////////////////////////////////////////////////////
#define ADC_CHANNEL_COUNT 4

typedef uint8_t ADC_SOURCE_PINS;
enum {
    AIN0,
    AIN1,
    AIN2,
    AIN3,
    AIN4,
    AIN5,
    AIN6,
    ADC_OFF
};

typedef uint8_t ADC_GAINS;
enum {
    ADC_0DB,
    ADC_3DB,
    ADC_6DB,
    ADC_9DB,
    ADC_12DB,
    ADC_15DB,
    ADC_18DB,
};

typedef struct _adc_status_t {
    ADC_SOURCE_PINS source_pins;
    uint8_t gain;
} adc_status_t;

////////////////////////////////////////////////////////
typedef uint8_t SAMPLE_RATE;
enum {
    FS8KS = 0x01,
    FS12KS,
    FS16KS,
    FS24KS,
    FS32KS,
    FS48KS,
    FS96KS,
    FS192KS,
    FS384KS,
    FS11KS = 0x10,
    FS22KS,
    FS44KS,
    FS88KS,
    FS176KS,
    FS352KS = 0x16
};

typedef uint8_t WHITENCONTROL;
enum {
    DEFAULT,
    WHITEN10,
    WHITEN20,
    WHITEN30,
    WHITENCONTROL_DISABLE = 0x07,
};

typedef uint8_t PURPOSE;
enum {
    PURPOSE_AUDIO,
    PURPOSE_BLEND,
    PURPOSE_TUNER0AUDIO = 0xc,
};

typedef uint8_t LOOPBACK;
enum {
    LOOPBACK_NORMAL,
    LOOPBACK_LOOPBACK
};

typedef uint8_t I2S_MODE;
enum {
    I2S_SLAVE,
    I2S_MASTER
};

typedef uint8_t CLKFS;
enum {
    CLKFS_NONE,
    IOUT = 0x2,
    IQ,
    IN0,
    IN1,
    IN2,
    IN3,
    IN4,
    GP,
    OUT0,
    OUT1,
};

typedef uint8_t DADJ;
enum {
    DADJ_NORMAL,
    DADJ_LOSYNC
};

typedef uint8_t HIZ;
enum {
    HIZ_ZERO,
    HIZ_TRI,
    HIZ_DROP,
    HIZ_CLOCK,
    HIZ_HALF,
};

typedef uint8_t DATA1;
enum {
    DATA1_NONE,
    ZIFQOUT = 0x02,
    ISFSN,
};

typedef uint8_t DATA0;
enum {
    DATA0_NONE,
    IOUTP = 0x02,
    IQCLKN,
    DIN0,
    DIN1,
    DIN2,
    DIN3,
    DIN4,
    GP2,
    DOUT0,
    DOUT1,
};

typedef uint8_t FILL;
enum {
    ZEROS,
    ONES,
    SIGN,
    RANDOM,
};

typedef uint8_t SLOT_SIZE, SAMPLE_SIZE;
enum {
    EIGHT,
    SIXTEEN,
    TWENTY,
    TWENTYFOUR,
    THIRTYTWO,
    EIGHTEEN
};

typedef uint8_t BITORDER;
enum {
    MSBFIRST,
    LSBFIRST
};

typedef uint8_t SWAP;
enum {
    LEFTRIGHT,
    RIGHTLEFT
};

typedef uint8_t CLKINV;
enum {
    CLK_NORMAL,
    CLK_INVERTED
};

typedef uint8_t FRAMING_MODE;
enum {
    I2S,
    DSP = 0x6,
};

typedef uint8_t CLOCK_DOMAIN;
enum {
    AUDIOSYNTH
};

typedef struct _i2s_config {
    SAMPLE_RATE sample_rate;
    WHITENCONTROL whitencontrol;
    PURPOSE purpose;
    I2S_MODE master;
    CLKFS clkfs;
    DATA0 data0;
    FILL fill;
    SLOT_SIZE slot_size;
    SAMPLE_SIZE sample_size;
    BITORDER bitorder;
    SWAP swap;
    CLKINV clkinv;
    FRAMING_MODE frame_mode;
    CLOCK_DOMAIN clock_domain;
    HIZ hiz;
    uint8_t length;
    DATA1 data1;
} i2s_config;

typedef uint8_t I2S_PORT;
enum {
    TX0,
    TX1,
    TX2,
    TX3,
    TX4,

    RX0 = 0x8,
    RX1,
    RX2,
    RX3,
    RX4
};

typedef uint8_t PORT_MODE;
enum {
    PORT_QUERY,
    PORT_RELEASE,
    PORT_DISABLE,
    PORT_DISREL,
    PORT_ENABLE
};

typedef uint8_t PORT_CURRENT_MODE;
enum {
    QUERY,
    DISABLE,
    ENABLE
};

typedef struct _i2s_control {
    i2s_config config;
    PORT_CURRENT_MODE current_mode;
} i2s_control;

typedef uint8_t PORT_ID;
enum {
    PORT_ADC0 = 0x0,
    PORT_ADC1,
    PORT_ADC2,
    PORT_ADC3,
    PORT_ADC4,

    PORT_DIN0 = 0x6,
    PORT_DIN1,
    PORT_DIN2,
    PORT_DIN3,
    PORT_DIN4,
    PORT_DIN5,

    PORT_Q1TOH_0 = 0xd,
    PORT_Q1TOH_1,
    PORT_Q1TOH_2,
    PORT_Q2TOH_0,
    PORT_Q2TOH_1,
    PORT_Q2TOH_2,

    PORT_TONE1 = 0x13,
    PORT_TONE2,

    PORT_ICIN0 = 0x17,
    PORT_ICIN1,
    PORT_ICIN2,
    PORT_ICIN3,

    PORT_WAVE1 = 0x1b,
    PORT_WAVE2,

    PORT_NULLIN = 0x1f,
    PORT_ALLIN = 0x8f,

    PORT_DAC0 = 0x20,
    PORT_DAC1,
    PORT_DAC2,
    PORT_DAC3,
    PORT_DAC4,
    PORT_DAC5,

    PORT_DOUT0 = 0x26,
    PORT_DOUT1,
    PORT_DOUT2,
    PORT_DOUT3,
    PORT_DOUT4,
    PORT_DOUT5,
    PORT_DOUT6,

    PORT_HTOQ1_0 = 0x2d,
    PORT_HTOQ1_1,
    PORT_HTOQ1_2,
    PORT_HTOQ2_0,
    PORT_HTOQ2_1,
    PORT_HTOQ2_2,

    PORT_ICOUT0 = 0x33,
    PORT_ICOUT1,
    PORT_ICOUT2,
    PORT_ICOUT3,

    PORT_NULLOUT = 0x3f,
    PORT_ALLOUT = 0xaf,
    PORT_ALL = 0xff
};

typedef uint8_t NODE_ID;
enum {
    NODE_IN0 = 0x0,
    NODE_IN1,
    NODE_IN2,
    NODE_IN3,
    NODE_IN4,
    NODE_IN5,
    NODE_IN6,
    NODE_IN7,
    NODE_IN8,
    NODE_IN9,
    NODE_IN10,
    NODE_IN11,

    ALLIN = 0x4f,

    NODE_OUT0 = 0x0,
    NODE_OUT1,
    NODE_OUT2,
    NODE_OUT3,
    NODE_OUT4,
    NODE_OUT5,
    NODE_OUT6,
    NODE_OUT7,

    ALLOUT = 0xcf,
    ALL = 0Xff
};

typedef struct _PORT_STATE {
    uint8_t port_state;
    uint8_t chan_state;
} PORT_STATE;

// RET_CODE si479x_pin_drive(PIN_GROUP group, pin_des_t pinList[4]);

RET_CODE si479x_pin_status(PIN_GROUP group, uint8_t pins[][8]);

RET_CODE si479x_pin_config_hdblend(uint8_t pin_id, uint8_t options);

RET_CODE si479x_DAC_config(uint8_t DAC01_MODE,
    uint8_t DAC23_MODE, uint8_t DAC45_MODE, uint8_t slow);

RET_CODE si479x_ADC_connect(uint8_t ADC_CHANNEL,
    uint8_t SOURCE_PINS, ADC_GAINS GAIN);

RET_CODE si479x_ADC_disconnect(uint8_t ADC_CHANNEL);

RET_CODE si479x_ADC_status(adc_status_t adc_set[ADC_CHANNEL_COUNT]);

RET_CODE si479x_I2STX0_config(i2s_config* pCfg);

RET_CODE si479x_I2STX1_config(i2s_config* pCfg);

// RET_CODE si479x_I2STX2_config(i2s_config *pCfg);

// RET_CODE si479x_I2STX3_config(i2s_config *pCfg);

// RET_CODE si479x_I2STX4_config(i2s_config *pCfg);

RET_CODE si479x_I2SRX0_config(i2s_config* pCfg);

RET_CODE si479x_I2SRX1_config(i2s_config* pCfg);

RET_CODE si479x_I2SRX2_config(i2s_config* pCfg);

// RET_CODE si479x_I2SRX3_config(i2s_config *pCfg);

// RET_CODE si479x_I2SRX4_config(i2s_config *pCfg);

RET_CODE si479x_I2S_control(I2S_PORT port, PORT_MODE mode, i2s_control* pControl);

#ifdef __cplusplus
}
#endif

#endif
