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

#ifndef _SI479X_TUNER_H_
#define _SI479X_TUNER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "si479x_core.h"

///////////////////////////////////////////////////////////
typedef uint8_t FM_SERVO_ID; enum
{
    FM_SERVO_DIGBLEND = 1,
    FM_SERVO_CHANBW,
    FM_SERVO_SOFTMUTE,
    FM_SERVO_HICUT,
    FM_SERVO_LOWCUT,
    FM_SERVO_STBLEND, //6
    FM_SERVO_HIBLEND,
    FM_SERVO_NBUSN1THR,
    FM_SERVO_NBUSN2THR,
    FM_SERVO_CUST1, //0xa
    FM_SERVO_CUST2,
    FM_SERVO_FADETHRESH,
};

typedef enum _SERVO_ACC
{
    SERVO_ACCMIN,
    SERVO_ACCMAX,
    SERVO_ACCSUM
}SERVO_ACC;

typedef uint8_t FM_METRIC_ID; enum
{
	FM_METRIC_DISABLE = 0x0,
	FM_METRIC_FREQOFF = 0x1,
	FM_METRIC_RSSI = 0x2,
	FM_METRIC_SNR = 0x3,
	FM_METRIC_LASSI = 0x5,
	FM_METRIC_HASSI = 0x6,
	FM_METRIC_ASSI100 = 0x7,
	FM_METRIC_LASSI200 = 0x8,
	FM_METRIC_HASSI200 = 0x9,
	FM_METRIC_ASSI200 = 0xa,
	FM_METRIC_MULTIPATH = 0xb,
	FM_METRIC_USN = 0xc,
	FM_METRIC_DEVIATION = 0xd,
	FM_METRIC_COCHANL = 0xe,
	FM_METRIC_PDEV = 0xf,
	FM_METRIC_RDSDEV = 0x10,
	FM_METRIC_SSDEV = 0x11,
	FM_METRIC_FREQOFFWB = 0x12,
	FM_METRIC_PILOTLOCK = 0x13,
	FM_METRIC_HDLEVEL = 0x14,
	FM_METRIC_RFPULSEPERIOD = 0x17,
	FM_METRIC_RSSI_DP = 0x18,
	FM_METRIC_SNR_DP = 0x19,
	FM_METRIC_MULTIPATH_DP = 0x1a,
	FM_METRIC_USN_DP = 0x1b,
	FM_METRIC_USN_WB = 0x1c,
	FM_METRIC_RSSI_ADJ = 0x1d,
	FM_METRIC_PHSDIV_COMB_WGHT = 0x1e,
	FM_METRIC_RDS_CHAN_BW = 0x1f,
	FM_METRIC_RSSI_NC = 0x20,
	FM_METRIC_RSSI_NC_ADJ = 0x21,
	FM_METRIC_FILTERED_HDLEVEL = 0x22,
	FM_METRIC_BLENDPIN = 0x38,
	FM_METRIC_HDSEARCHDONE = 0x39,
	FM_METRIC_DIGBLEND = 0x40,
	FM_METRIC_SOFTMUTE = 0x42,
	FM_METRIC_HICUT = 0x43,
	FM_METRIC_LOWCUT = 0x44,
	FM_METRIC_STBLEND = 0x45,
	FM_METRIC_HIBLEND = 0x46,
	FM_METRIC_NBUSN1THR = 0x47,
	FM_METRIC_NBUSN2THR = 0x48,
	FM_METRIC_FADETHRESH = 0x4b,
	FM_METRIC_FM_RF_GAIN = 0x70,
	FM_METRIC_FM_IF_GAIN = 0x71,
};

typedef struct _fm_servo_t
{
    FM_SERVO_ID servo_id;
    int16_t    min_limits;
    int16_t    max_limits;
    int16_t    init;
    SERVO_ACC    acc;
    int8_t     current_value;
}fm_servo_t;

typedef struct _fm_transform_t
{
    FM_METRIC_ID    metric_id;
    FM_SERVO_ID     servo_id;
    int16_t        x1;
    int16_t        y1;
    int16_t        x2;
    int16_t        y2;
    uint16_t        p1top2rate;
    uint16_t        p2top1rate;
    uint8_t        enable_other0;
    uint8_t        other_transform_at_ymax;
    uint8_t        enable_other1;
    uint8_t        other_transform_at_ymin;
    uint8_t        enable_trigger;
    uint8_t        invert_trigger;
    uint8_t        servo_trigger_id;
}fm_transform_t;


typedef struct _fm_fast_transform_t
{
    uint8_t  		fast_transform_id;
    uint8_t    		enable;
    int16_t        	x1;
    int16_t        	y1;
    int16_t        	x2;
    int16_t        	y2;
    uint16_t        p1top2rate;
    uint16_t        p2top1rate;
}fm_fast_transform_t;


typedef struct _fm_servo_status_t
{
    uint8_t digblend;
    uint8_t chanbw;
    uint8_t softmute;
    uint8_t highcut;
    uint8_t lowcut;
    uint8_t stereoblend;
    uint8_t highblend;
    uint8_t nbusn1thr;
    uint8_t nbusn2thr;
    uint8_t cust1;
    uint8_t cust2;
    uint8_t fadethresh;
}fm_servo_status_t;

///////////////////////////////////////////////////////////
typedef enum _AM_SERVO_ID
{
    AM_SERVO_DIGBLEND = 0x1,
    AM_SERVO_CHANBW,
    AM_SERVO_SOFTMUTE,
    AM_SERVO_HICUT,
    AM_SERVO_LOWCUT,
    AM_SERVO_NBAUDTH,
    AM_SERVO_NBIQ1TH,
    AM_SERVO_NBIQ2TH = 0x8,
    AM_SERVO_NBIQ3TH,
    AM_SERVO_AVC,
    AM_SERVO_MAXAVC //0xbs
}AM_SERVO_ID;

typedef enum _AM_METRIC_ID
{
	AM_METRIC_DISABLE = 0x0,
	AM_METRIC_FREQOFF = 0x1,
	AM_METRIC_RSSI = 0x2,
	AM_METRIC_SNR = 0x3,
	AM_METRIC_LASSI = 0x5,
	AM_METRIC_HASSI = 0x6,
	AM_METRIC_LASSI2 = 0x7,
	AM_METRIC_HASSI2 = 0x8,
	AM_METRIC_LASSI3 = 0x9,
	AM_METRIC_HASSI3 = 0xa,
	AM_METRIC_LASSI4 = 0xb,
	AM_METRIC_HASSI4 = 0xc,
	AM_METRIC_ASSI = 0xd,
	AM_METRIC_ASSI2 = 0xe,
	AM_METRIC_ASSI3 = 0xf,
	AM_METRIC_ASSI4 = 0x10,
	AM_METRIC_IQSNR = 0x11,
	AM_METRIC_MODINDEX = 0x12,
	AM_METRIC_RMS_USB = 0x14,
	AM_METRIC_RMS_LSB = 0x15,
	AM_METRIC_HDLEVEL = 0x16,
	AM_METRIC_RSSI_ADJ = 0x17,
	AM_METRIC_RSSI_NC = 0x18,
	AM_METRIC_RSSI_NC_ADJ = 0x19,
	AM_METRIC_FILTERED_HDLEVEL = 0x1a,
	AM_METRIC_BLENDPIN = 0x38,
	AM_METRIC_HDSEARCHDONE = 0x39,
	AM_METRIC_DIGBLEND = 0x40,
	AM_METRIC_CHANBW = 0x41,
	AM_METRIC_SOFTMUTE = 0x42,
	AM_METRIC_HICUT = 0x43,
	AM_METRIC_LOWCUT = 0x44,
	AM_METRIC_NBAUDTH = 0x45,
	AM_METRIC_NBIQ1TH = 0x46,
	AM_METRIC_NBIQ2TH = 0x47,
	AM_METRIC_NBIQ3TH = 0x48,
	AM_METRIC_AVC = 0x49,
	AM_METRIC_MAXAVC = 0x4a,
	AM_METRIC_AM_RF_GAIN = 0x70,
	AM_METRIC_AM_IF_GAIN = 0x71,
	AM_METRIC_AM_MS_GAIN = 0x72,
}AM_METRIC_ID;

typedef struct _am_transform_t
{
    AM_METRIC_ID    metric_id;
    AM_SERVO_ID     servo_id;
    int16_t        x1;
    int16_t        y1;
    int16_t        x2;
    int16_t        y2;
    uint16_t        p1top2rate;
    uint16_t        p2top1rate;
    
    uint8_t        enable_other0;
    uint8_t        other_transform_at_ymax;
    uint8_t        enable_other1;
    uint8_t        other_transform_at_ymin;
    uint8_t        enable_trigger;
    uint8_t        invert_trigger;
    uint8_t        servo_trigger_id;
}am_transform_t;

typedef struct _transform_t
{
    uint8_t    metric_id;
    uint8_t     servo_id;
    int16_t        x1;
    int16_t        y1;
    int16_t        x2;
    int16_t        y2;
    uint16_t       p1top2rate;
    uint16_t       p2top1rate;

    uint8_t        enable_other0;
    uint8_t        other_transform_at_ymax;
    uint8_t        enable_other1;
    uint8_t        other_transform_at_ymin;
    uint8_t        enable_trigger;
    uint8_t        invert_trigger;
    uint8_t        servo_trigger_id;
}transform_t;

typedef struct _servo_trigger_t
{
    uint8_t     inactive_int;
    uint8_t     active_int;
    uint8_t     metric_id;
    int16_t     lower;
    int16_t     upper;
}servo_trigger_t;

typedef struct _am_servo_t
{
    AM_SERVO_ID servo_id;
    int16_t    min_limits;
    int16_t    max_limits;
    int16_t    init;
    SERVO_ACC    acc;
    int8_t     current_value;
}am_servo_t;

typedef struct _am_servo_status_t
{
    uint8_t digblend;
    uint8_t chanbw;
    uint8_t softmute;
    uint8_t highcut;
    uint8_t lowcut;
    uint8_t nbaudth;
    uint8_t nbiq1th;
    uint8_t nbiq2th;
    uint8_t nbiq3th;
    uint8_t avc;
    uint8_t maxavc;
}am_servo_status_t;


typedef struct _am_nb_status_t
{
    uint16_t nb_aud_cnt;
    uint16_t nb_iq_cnt1;
    uint16_t nb_iq_cnt2;
    uint16_t nb_iq_cnt3;
}am_nb_status_t;

typedef struct _tuner_metrics_t
{
    uint8_t Valid;
    int8_t RSSI;
    int8_t SNR;
    int8_t FREQOFF;
    int8_t ASSI100;
    int8_t ASSI200;
    int8_t MULTPATH;
    int8_t FREQOFFWB;
    uint16_t Freq;
}tuner_metrics_t;

typedef struct _dab_tuner_metrics
{
	uint8_t valid;
	// not set = 0xFF
	uint8_t freq_index;
	uint32_t freq_hz;
	uint8_t dab_detect;
	int8_t sqi;
	int8_t rssi;
	int8_t rssi_adj;
} dab_tuner_metrics_t;

typedef struct _dab_demo_event_status
{
    uint8_t EVENT_INT;
    uint8_t EVENT_STATUS;
    uint16_t SVLISTVER;
    uint8_t AUDIO_STATUS;
}dab_demod_event_status_t;

#define FM_RSQ_STATUS_RET_DATA_LENGTH	42
#define FM_SERVO_STATUS_RET_DATA_LENGTH 18
#define FM_SIGNAL_EVENT_COUNT_RET_DATA_LENGTH 6
#define AM_RSQ_STATUS_RET_DATA_LENGTH 34
#define AM_SERVO_STATUS_RET_DATA_LENGTH 19
#define AM_SIGNAL_EVENT_COUNT_RET_DATA_LENGTH 12
#define AGC_STATUS_RET_DATA_LENGTH 25

extern dab_tuner_metrics_t g_dab_tuner_metrics;

RET_CODE si479x_tuner_cmd(uint8_t tuner_id, uint16_t cmd_length, uint16_t ret_length);
/**
*   @breif set property to tuner
*
*   Be sure the tuner is working when call this routine.
*
*   @param propNumber: property address.
*   @param propValue: property value.
*
*   @retval void
*/
RET_CODE si479x_tuner_set_property(uint8_t tuner_id, uint16_t propNumber, uint16_t propValue);

RET_CODE si479x_tuner_fm_tune(uint8_t tuner_id, uint8_t tune_mode, uint8_t injection, uint16_t freq);


RET_CODE si479x_tuner_fm_rsq_status(uint8_t tuner_id, uint8_t mode, tuner_metrics_t * p_tuner_metrics);

RET_CODE si479x_tuner_fm_rsq_datalist(uint8_t tuner_id, uint8_t length, uint8_t datalist[]);

uint8_t si479x_tuner_fm_current_metrics(uint8_t tuner_id, uint8_t metricsId);

uint8_t si479x_tuner_fm_current_servo(uint8_t tuner_id, uint8_t servoId);

RET_CODE si479x_tuner_fm_set_servo(uint8_t tuner_id, fm_servo_t *pServo);

RET_CODE si479x_tuner_fm_get_servo(uint8_t tuner_id, FM_SERVO_ID servo_id, fm_servo_t *pServo);

RET_CODE si479x_tuner_fm_servo_status(uint8_t tuner_id, fm_servo_status_t *status);

RET_CODE si479x_tuner_fm_servo_datalist(uint8_t tuner_id, uint8_t length, uint8_t datalist[]);

RET_CODE si479x_tuner_fm_set_servo_transform(uint8_t tuner_id, uint8_t transform_id, fm_transform_t *pTransform);

RET_CODE si479x_tuner_fm_set_fast_servo_transform(uint8_t tuner_id, fm_fast_transform_t *pFastTransform);

RET_CODE si479x_tuner_fm_get_servo_transform(uint8_t tuner_id, uint8_t transform_id, fm_transform_t *pTransform);

RET_CODE si479x_tuner_fm_set_servo_trigger(uint8_t tuner_id, uint8_t trigger_id, servo_trigger_t *ptrigger);

uint8_t si479x_tuner_dab_current_metrics(uint8_t tuner_id, uint8_t metricsId);

/**
*   @breif AM tune function.
*
*   @param frequency: The frenquency unit is in 100 Hz.
*
*   @retval void
*/
RET_CODE si479x_tuner_am_tune(uint8_t tuner_id, uint8_t tune_mode, uint8_t smoothmetrics, uint16_t freq);

RET_CODE si479x_tuner_am_rsq_status(uint8_t tuner_id, uint8_t mode, tuner_metrics_t * p_tuner_metrics);

RET_CODE si479x_tuner_am_rsq_datalist(uint8_t tuner_id, uint8_t length, uint8_t datalist[]);

uint8_t si479x_tuner_am_current_metrics(uint8_t tuner_id, uint8_t metricsId);

uint8_t si479x_tuner_am_current_servo(uint8_t tuner_id, uint8_t servoId);

RET_CODE si479x_tuner_am_set_servo(uint8_t tuner_id, am_servo_t *pservo);

RET_CODE si479x_tuner_am_servo_status(uint8_t tuner_id, am_servo_status_t *status);

RET_CODE si479x_tuner_am_servo_datalist(uint8_t tuner_id, uint8_t length, uint8_t datalist[]);

uint16_t si479x_tuner_am_signal_event_count(uint8_t tuner_id, uint8_t eventIndex);

RET_CODE si479x_tuner_am_signal_event_datalist(uint8_t tuner_id, uint8_t length, uint8_t datalist[]);

RET_CODE si479x_tuner_am_set_servo_transform(uint8_t tuner_id, uint8_t transform_id, am_transform_t *pTransform);

RET_CODE si479x_tuner_am_get_servo_transform(uint8_t tuner_id, uint8_t transform_id, am_transform_t *pTransform);

RET_CODE si479x_tuner_am_set_servo_trigger(uint8_t tuner_id, uint8_t trigger_id, servo_trigger_t *ptrigger);

RET_CODE si479x_tuner_fm_signal_event_datalist(uint8_t tuner_id, uint8_t length, uint8_t datalist[]);

uint16_t si479x_tuner_fm_signal_event_count(uint8_t tuner_id, uint8_t eventIndex);

RET_CODE si479x_tuner_set_mode(uint8_t tuner_id, uint8_t mode);

//uint8_t si479x_tuner_get_mode(uint8_t tuner_id);

RET_CODE si479x_tuner_setvolume(uint8_t tuner_id, uint8_t value);

RET_CODE si479x_tuner_mute(uint8_t tuner_id, uint8_t value);

#ifdef SUPPORT_FMAMDAB
RET_CODE si479x_tuner_dab_tune(uint8_t tuner_id, uint8_t detectmode, uint8_t freq_index);

RET_CODE si479x_tuner_dab_rsq_status(uint8_t tuner_id, uint8_t attune, uint8_t cancel, uint16_t stcack, dab_tuner_metrics_t * p_tuner_metrics);
#endif

RET_CODE si479x_fmpd_tuner_fm_tune(uint8_t tune_mode, uint8_t injection, uint16_t freq);

RET_CODE si479x_fmpd_tuner_set_mode(uint8_t mode);

#ifdef __cplusplus
}
#endif

#endif
