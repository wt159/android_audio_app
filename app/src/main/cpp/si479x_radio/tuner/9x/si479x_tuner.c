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
#include "si479x_tuner.h"
#include "si479x_core.h"
#include "si479x_internel.h"
#include "si479x_propertydefs.h"
#include <stdio.h>

RET_CODE si479x_tuner_cmd(uint8_t tuner_id, uint16_t cmd_length, uint16_t ret_length)
{
    RET_CODE ret = RET_SUCCESS;

    cmd_buff[0] = TUNER_CMD;
    cmd_buff[1] = tuner_id;

    ERR_CHECKING(si479x_command(4+cmd_length, cmd_buff, ret_length, rsp_buff));

    return ret;	
}

RET_CODE si479x_tuner_set_property(uint8_t tuner_id, uint16_t prop_id, uint16_t prop_value)
{
    RET_CODE ret = RET_SUCCESS;
    
    clear_cmd_memory();
    
    cmd_buff[COMMAND_OFFSET+0] = SET_PROPERTY;

    cmd_buff[COMMAND_OFFSET+2] = (uint8_t)prop_id;
    cmd_buff[COMMAND_OFFSET+3] = (uint8_t)(prop_id >> 8);
    
    cmd_buff[COMMAND_OFFSET+4] = (uint8_t)prop_value;
    cmd_buff[COMMAND_OFFSET+5] = (uint8_t)(prop_value >> 8);
    
    ERR_CHECKING(si479x_tuner_cmd(tuner_id, 6, 4));

    return ret;
}

RET_CODE si479x_tuner_set_mode(uint8_t tuner_id, uint8_t mode)
{
    RET_CODE ret = RET_SUCCESS;
    
    clear_cmd_memory();
    
    cmd_buff[COMMAND_OFFSET+0] = SET_MODE;
    cmd_buff[COMMAND_OFFSET+1] = mode;
    
    ERR_CHECKING(si479x_tuner_cmd(tuner_id, 2, 4));

    //wait for the STC bit set
    ERR_CHECKING(si479x_waitForSTC(tuner_id));

    return ret;
}


RET_CODE si479x_fmpd_tuner_set_mode(uint8_t mode)
{
    RET_CODE ret = RET_SUCCESS;
    
    clear_cmd_memory();
    
    cmd_buff[COMMAND_OFFSET+0] = SET_MODE;
    cmd_buff[COMMAND_OFFSET+1] = mode;
    
    ERR_CHECKING(si479x_tuner_cmd(TUNER0, 2, 4));
    ERR_CHECKING(si479x_tuner_cmd(TUNER1, 2, 4));

    //wait for the STC bit set
    ERR_CHECKING(si479x_waitForSTC(TUNER0));
    ERR_CHECKING(si479x_waitForSTC(TUNER1));

    return ret;
}

/**
uint8_t si479x_tuner_get_mode(uint8_t tuner_id)
{
    RET_CODE ret = RET_SUCCESS;
    
    clear_cmd_memory();
    
    cmd_buff[COMMAND_OFFSET+0] = GET_MODE;

    ret = si479x_tuner_cmd(tuner_id, 2, 5);

    if (ret != RET_SUCCESS)
        return 0xff;

    return (rsp_buff[4] & 0x07);
}

RET_CODE si479x_tuner_setvolume(uint8_t tuner_id, uint8_t value)
{    
    RET_CODE ret = RET_SUCCESS;
	
	return si479x_tuner_set_property(tuner_id, RADIO_ANALOG_VOLUME, value&0x7F);
}
*/

RET_CODE si479x_tuner_mute(uint8_t tuner_id, uint8_t value)
{
    if (value != 0)
        return si479x_tuner_set_property(tuner_id, RADIO_MUTE, 0x03);
    else
        return si479x_tuner_set_property(tuner_id, RADIO_MUTE, 0x0);
}

RET_CODE si479x_tuner_stauts_datalist(uint8_t tuner_id, uint8_t cmd, uint8_t para, uint8_t maxlength, uint8_t buflength, uint8_t datalist[])
{
    RET_CODE ret = RET_SUCCESS;
    uint8_t totallength = buflength <= maxlength ? buflength : maxlength;
    uint8_t i;
   
    clear_cmd_memory();
    
    cmd_buff[COMMAND_OFFSET+0] = cmd;
    cmd_buff[COMMAND_OFFSET+1] = para;
    
    ERR_CHECKING(si479x_tuner_cmd(tuner_id, 2, maxlength+4));

    for (i=0; i<totallength; i++)
    {
        datalist[i] = rsp_buff[4+i];
    }
       
    return ret;
}

//FM command
RET_CODE si479x_tuner_fm_tune(uint8_t tuner_id, uint8_t tune_mode, uint8_t injection, uint16_t freq)
{
    RET_CODE ret = RET_SUCCESS;
    
    clear_cmd_memory();
    
    cmd_buff[COMMAND_OFFSET+0] = FM_TUNE_FREQ;
    cmd_buff[COMMAND_OFFSET+1] = (tune_mode & 0x07) << 4 | (injection & 0x03);
    
    _si479x_fill_cmd_uint16_t(cmd_buff, COMMAND_OFFSET+2, freq);

    ERR_CHECKING(si479x_tuner_cmd(tuner_id, 4, 4));

    //wait for the STC bit set    
    ERR_CHECKING(si479x_waitForSTC(tuner_id));    
    
    return ret;
}

//FM command
RET_CODE si479x_fmpd_tuner_fm_tune(uint8_t tune_mode, uint8_t injection, uint16_t freq)
{
    RET_CODE ret = RET_SUCCESS;
    
    clear_cmd_memory();
    
    cmd_buff[COMMAND_OFFSET+0] = FM_TUNE_FREQ;
    cmd_buff[COMMAND_OFFSET+1] = (tune_mode & 0x07) << 4 | (injection & 0x03);
    
    _si479x_fill_cmd_uint16_t(cmd_buff, COMMAND_OFFSET+2, freq);

    ERR_CHECKING(si479x_tuner_cmd(TUNER0, 4, 4));
    ERR_CHECKING(si479x_tuner_cmd(TUNER1, 4, 4));

    //wait for the STC bit set    
    ERR_CHECKING(si479x_waitForSTC(TUNER0));    
    ERR_CHECKING(si479x_waitForSTC(TUNER1));    
    
    return ret;
}


RET_CODE si479x_tuner_fm_rsq_status(uint8_t tuner_id, uint8_t mode, tuner_metrics_t * p_tuner_metrics)
{
    RET_CODE ret = RET_SUCCESS;
   
    clear_cmd_memory();
    
    cmd_buff[COMMAND_OFFSET+0] = FM_RSQ_STATUS;
    cmd_buff[COMMAND_OFFSET+1] = mode;
    
    ERR_CHECKING(si479x_tuner_cmd(tuner_id, 2, FM_RSQ_STATUS_RET_DATA_LENGTH));

    p_tuner_metrics->Valid = !!(rsp_buff[5] & 0x01);
	p_tuner_metrics->Freq = ((uint16_t)rsp_buff[6] & 0xff) | (uint16_t)rsp_buff[7] << 8;
    p_tuner_metrics->FREQOFF = rsp_buff[8];

    p_tuner_metrics->RSSI= rsp_buff[9];
    p_tuner_metrics->SNR= rsp_buff[10];
    p_tuner_metrics->ASSI100= rsp_buff[14];
    p_tuner_metrics->ASSI200= rsp_buff[17];
    p_tuner_metrics->MULTPATH= rsp_buff[18];
	p_tuner_metrics->FREQOFFWB= rsp_buff[25];
       
    return ret;
}

RET_CODE si479x_tuner_fm_rsq_datalist(uint8_t tuner_id, uint8_t length, uint8_t datalist[])
{
    return si479x_tuner_stauts_datalist(tuner_id, FM_RSQ_STATUS, 0, FM_RSQ_STATUS_RET_DATA_LENGTH-4, length, datalist);
}

uint8_t si479x_tuner_fm_current_metrics(uint8_t tuner_id, uint8_t metricsId)
{
    if (metricsId >= FM_MAX_METRICS)
        return 0xff;
   
    clear_cmd_memory();
    
    cmd_buff[COMMAND_OFFSET+0] = FM_RSQ_STATUS;
    cmd_buff[COMMAND_OFFSET+1] = 0;
    
    si479x_tuner_cmd(tuner_id, 2, FM_RSQ_STATUS_RET_DATA_LENGTH);

    return rsp_buff[8+metricsId];
}

uint8_t si479x_tuner_fm_current_servo(uint8_t tuner_id, uint8_t servoId)
{
    if (servoId >= FM_MAX_SERVOS)
        return 0xff;
   
    clear_cmd_memory();
    
    cmd_buff[COMMAND_OFFSET+0] = FM_SERVO_STATUS;
    cmd_buff[COMMAND_OFFSET+1] = 0;
    
    si479x_tuner_cmd(tuner_id, 2, FM_SERVO_STATUS_RET_DATA_LENGTH);

    return rsp_buff[6+servoId];
}

RET_CODE si479x_tuner_fm_servo_status(uint8_t tuner_id, fm_servo_status_t *status)
{
    RET_CODE ret = RET_SUCCESS;
    
    clear_cmd_memory();
    
    cmd_buff[COMMAND_OFFSET+0] = FM_SERVO_STATUS;

    ERR_CHECKING(si479x_tuner_cmd(tuner_id, 2, FM_SERVO_STATUS_RET_DATA_LENGTH));

    status->digblend = rsp_buff[6];
    status->chanbw = rsp_buff[7];
    status->softmute = rsp_buff[8];
    status->highcut = rsp_buff[9];
    status->lowcut = rsp_buff[10];
    status->stereoblend = rsp_buff[11];
    status->highblend = rsp_buff[12];
    status->nbusn1thr = rsp_buff[13];
    status->nbusn2thr = rsp_buff[14];
    status->cust1 = rsp_buff[15];
    status->cust2 = rsp_buff[16];
    status->fadethresh = rsp_buff[17];

    return ret;
}

RET_CODE si479x_tuner_fm_servo_datalist(uint8_t tuner_id, uint8_t length, uint8_t datalist[])
{
    return si479x_tuner_stauts_datalist(tuner_id, FM_SERVO_STATUS, 0, FM_SERVO_STATUS_RET_DATA_LENGTH-4, length, datalist);
}
//AM command
 RET_CODE si479x_tuner_am_tune(uint8_t tuner_id, uint8_t tune_mode, uint8_t smoothmetrics, uint16_t freq)
{
    RET_CODE ret = RET_SUCCESS;
    
    clear_cmd_memory();
    
    cmd_buff[COMMAND_OFFSET+0] = AM_TUNE_FREQ;
    cmd_buff[COMMAND_OFFSET+1] = (tune_mode & 0x03) << 4 |(smoothmetrics & 0x01) << 2;
    
    _si479x_fill_cmd_uint16_t(cmd_buff, COMMAND_OFFSET+2, freq);

    ERR_CHECKING(si479x_tuner_cmd(tuner_id, 4, 4));

    //wait for the STC bit set
    ERR_CHECKING(si479x_waitForSTC(tuner_id));
        
    return ret;
}

RET_CODE si479x_tuner_am_rsq_status(uint8_t tuner_id, uint8_t mode, tuner_metrics_t * p_tuner_metrics)
{
    RET_CODE ret = RET_SUCCESS;
    
    if (p_tuner_metrics == NULL)
        return RET_INVALIDINPUT;
    
    clear_cmd_memory();
    
    cmd_buff[COMMAND_OFFSET+0] = AM_RSQ_STATUS;
    cmd_buff[COMMAND_OFFSET+1] = mode;
    
    ERR_CHECKING(si479x_tuner_cmd(tuner_id, 2, AM_RSQ_STATUS_RET_DATA_LENGTH));

    p_tuner_metrics->Valid = !!(rsp_buff[5] & 0x01);
	p_tuner_metrics->Freq = ((uint16_t)rsp_buff[6] & 0xff) | (uint16_t)rsp_buff[7] << 8;
    p_tuner_metrics->FREQOFF = rsp_buff[8];
    p_tuner_metrics->RSSI= rsp_buff[9];
    p_tuner_metrics->SNR= rsp_buff[10];
        
    return ret;
}


RET_CODE si479x_tuner_am_rsq_datalist(uint8_t tuner_id, uint8_t length, uint8_t datalist[])
{
    return si479x_tuner_stauts_datalist(tuner_id, AM_RSQ_STATUS, 0, AM_RSQ_STATUS_RET_DATA_LENGTH-4, length, datalist);
}

uint8_t si479x_tuner_am_current_metrics(uint8_t tuner_id, uint8_t metricsId)
{
    if (metricsId >= AM_MAX_METRICS)
        return 0xff;
    
    clear_cmd_memory();
    
    cmd_buff[COMMAND_OFFSET+0] = AM_RSQ_STATUS;
    cmd_buff[COMMAND_OFFSET+1] = 0;
    
    si479x_tuner_cmd(tuner_id, 2, AM_RSQ_STATUS_RET_DATA_LENGTH);

    return rsp_buff[8+metricsId];
}


uint8_t si479x_tuner_am_current_servo(uint8_t tuner_id, uint8_t servoId)
{
    if (servoId >= AM_MAX_SERVOS)
        return 0xff;
    
    clear_cmd_memory();
    
    cmd_buff[COMMAND_OFFSET+0] = AM_SERVO_STATUS;
    cmd_buff[COMMAND_OFFSET+1] = 0;
    
    si479x_tuner_cmd(tuner_id, 2, AM_SERVO_STATUS_RET_DATA_LENGTH);

    return rsp_buff[6+servoId];
}

RET_CODE si479x_tuner_am_servo_datalist(uint8_t tuner_id, uint8_t length, uint8_t datalist[])
{
    return si479x_tuner_stauts_datalist(tuner_id, AM_SERVO_STATUS, 0, AM_SERVO_STATUS_RET_DATA_LENGTH-4, length, datalist);
}


uint16_t si479x_tuner_am_signal_event_count(uint8_t tuner_id, uint8_t eventIndex)
{
    uint16_t tdata;
    
    if (eventIndex >= 4)
        return 0xff;
   
    clear_cmd_memory();
    
    cmd_buff[COMMAND_OFFSET+0] = AM_SIGNAL_EVENT_COUNT;
    cmd_buff[COMMAND_OFFSET+1] = 0;
    cmd_buff[COMMAND_OFFSET+2] = 0;
    
    si479x_tuner_cmd(tuner_id, 3, AM_SIGNAL_EVENT_COUNT_RET_DATA_LENGTH);

    tdata = (((uint16_t)rsp_buff[4+eventIndex*2]) & 0x00ff) | (((uint16_t)rsp_buff[4+eventIndex*2+1]) << 8);

    return tdata;
}


RET_CODE si479x_tuner_am_signal_event_datalist(uint8_t tuner_id, uint8_t length, uint8_t datalist[])
{
    return si479x_tuner_stauts_datalist(tuner_id, AM_SIGNAL_EVENT_COUNT, 0, AM_SIGNAL_EVENT_COUNT_RET_DATA_LENGTH-4, length, datalist);
}


//FM servo command
RET_CODE si479x_tuner_fm_set_servo(uint8_t tuner_id, fm_servo_t *pServo)
{
    RET_CODE ret = RET_SUCCESS;

    if (pServo == NULL)
    {
        return RET_INVALIDINPUT;
    }
    
    clear_cmd_memory();
    
    cmd_buff[COMMAND_OFFSET+0] = FM_SET_SERVO;
    cmd_buff[COMMAND_OFFSET+1] = pServo->servo_id;

    _si479x_fill_cmd_uint16_t(cmd_buff, COMMAND_OFFSET+2, pServo->min_limits);
    _si479x_fill_cmd_uint16_t(cmd_buff, COMMAND_OFFSET+4, pServo->max_limits);
    _si479x_fill_cmd_uint16_t(cmd_buff, COMMAND_OFFSET+6, pServo->init);

    cmd_buff[COMMAND_OFFSET+8] = pServo->acc;
    
    ERR_CHECKING(si479x_tuner_cmd(tuner_id, 9, 4));
    
    return ret;
}

//FM servo command
RET_CODE si479x_tuner_fm_get_servo(uint8_t tuner_id, FM_SERVO_ID servo_id, fm_servo_t *pServo)
{
    RET_CODE ret = RET_SUCCESS;

    if (pServo == NULL)
    {
        return RET_INVALIDINPUT;
    }
    
    clear_cmd_memory();
    
    cmd_buff[COMMAND_OFFSET+0] = FM_GET_SERVO;
    cmd_buff[COMMAND_OFFSET+1] = servo_id;
	
    ERR_CHECKING(si479x_tuner_cmd(tuner_id, 2, 12));
	
	pServo->min_limits = (((uint16_t)rsp_buff[5]) & 0xff) | (((uint16_t)rsp_buff[6]) << 8);
	pServo->max_limits = (((uint16_t)rsp_buff[7]) & 0xff) | (((uint16_t)rsp_buff[8]) << 8);
	pServo->init = (((uint16_t)rsp_buff[9]) & 0xff) | (((uint16_t)rsp_buff[10]) << 8);
	pServo->acc = rsp_buff[11];    
    
    return ret;
}


//type: 0 fm
//type: 1 am
RET_CODE _si479x_set_servo_transform(uint8_t tuner_id, uint8_t type, uint8_t transform_id, transform_t *pTransform)
{
    RET_CODE ret = RET_SUCCESS;

    if (pTransform == NULL)
    {
        return RET_INVALIDINPUT;
    }
    
    clear_cmd_memory();

    if (type == 0)
        cmd_buff[COMMAND_OFFSET+0] = FM_SET_SERVO_TRANSFORM;
    else
        cmd_buff[COMMAND_OFFSET+0] = AM_SET_SERVO_TRANSFORM;
    
    cmd_buff[COMMAND_OFFSET+1] = transform_id & 0x7f;

	cmd_buff[COMMAND_OFFSET+2] = (uint8_t)pTransform->metric_id;
	cmd_buff[COMMAND_OFFSET+3] = (uint8_t)pTransform->servo_id;
    _si479x_fill_cmd_uint16_t(cmd_buff, COMMAND_OFFSET+4, (uint16_t)pTransform->x1);
    _si479x_fill_cmd_uint16_t(cmd_buff, COMMAND_OFFSET+6, (uint16_t)pTransform->y1);
    _si479x_fill_cmd_uint16_t(cmd_buff, COMMAND_OFFSET+8, (uint16_t)pTransform->x2);
    _si479x_fill_cmd_uint16_t(cmd_buff, COMMAND_OFFSET+10, (uint16_t)pTransform->y2);
    _si479x_fill_cmd_uint16_t(cmd_buff, COMMAND_OFFSET+12, pTransform->p1top2rate);
    _si479x_fill_cmd_uint16_t(cmd_buff, COMMAND_OFFSET+14, pTransform->p2top1rate);
    
    cmd_buff[COMMAND_OFFSET+16] = ((pTransform->enable_other0&0x01) << 7) | (pTransform->other_transform_at_ymax&0x7f);
    cmd_buff[COMMAND_OFFSET+17] = ((pTransform->enable_other1&0x01) << 7) | (pTransform->other_transform_at_ymin&0x7f);
    cmd_buff[COMMAND_OFFSET+18] = ((pTransform->enable_trigger&0x01) << 7) | ((pTransform->invert_trigger&0x01) << 6) | (pTransform->servo_trigger_id&0x3f);
    
    ERR_CHECKING(si479x_tuner_cmd(tuner_id, 19, 4));

    return ret;
}


RET_CODE si479x_tuner_fm_set_servo_transform(uint8_t tuner_id, uint8_t transform_id, fm_transform_t *pTransform)
{
    return _si479x_set_servo_transform(tuner_id, 0, transform_id, (transform_t*)pTransform);
}


RET_CODE si479x_tuner_fm_set_fast_servo_transform(uint8_t tuner_id, fm_fast_transform_t *pFastTransform)
{
    RET_CODE ret = RET_SUCCESS;

    if (pFastTransform == NULL)
    {
        return RET_INVALIDINPUT;
    }
    
    clear_cmd_memory();

    cmd_buff[COMMAND_OFFSET+0] = FM_SET_FAST_SERVO_TRANSFORM;
    cmd_buff[COMMAND_OFFSET+1] = pFastTransform->fast_transform_id & 0x7f;
	cmd_buff[COMMAND_OFFSET+2] = (uint8_t)pFastTransform->enable;
	
    _si479x_fill_cmd_uint16_t(cmd_buff, COMMAND_OFFSET+4, (uint16_t)pFastTransform->x1);
    _si479x_fill_cmd_uint16_t(cmd_buff, COMMAND_OFFSET+6, (uint16_t)pFastTransform->y1);
    _si479x_fill_cmd_uint16_t(cmd_buff, COMMAND_OFFSET+8, (uint16_t)pFastTransform->x2);
    _si479x_fill_cmd_uint16_t(cmd_buff, COMMAND_OFFSET+10, (uint16_t)pFastTransform->y2);
    _si479x_fill_cmd_uint16_t(cmd_buff, COMMAND_OFFSET+12, pFastTransform->p1top2rate);
    _si479x_fill_cmd_uint16_t(cmd_buff, COMMAND_OFFSET+14, pFastTransform->p2top1rate);

    ERR_CHECKING(si479x_tuner_cmd(tuner_id, 16, 4));

    return ret;
}


RET_CODE _si479x_set_servo_trigger(uint8_t tuner_id, uint8_t type, uint8_t trigger_id, servo_trigger_t *ptrigger)
{
    RET_CODE ret = RET_SUCCESS;

    if (ptrigger == NULL)
    {
        return RET_INVALIDINPUT;
    }
    
    clear_cmd_memory();

    if (type == 0)
        cmd_buff[COMMAND_OFFSET+0] = FM_SET_SERVO_TRIGGER;
    else
        cmd_buff[COMMAND_OFFSET+0] = AM_SET_SERVO_TRIGGER;
    
    cmd_buff[COMMAND_OFFSET+1] = trigger_id & 0x3f;
    cmd_buff[COMMAND_OFFSET+2] = ((ptrigger->inactive_int & 0x01) << 2) | ((ptrigger->active_int & 0x01) << 1);
    cmd_buff[COMMAND_OFFSET+3] = ptrigger->metric_id;
    
    _si479x_fill_cmd_uint16_t(cmd_buff, COMMAND_OFFSET+4, (uint16_t)ptrigger->lower);
    _si479x_fill_cmd_uint16_t(cmd_buff, COMMAND_OFFSET+6, (uint16_t)ptrigger->upper);
    
    ERR_CHECKING(si479x_tuner_cmd(tuner_id, 8, 4));

    return ret;
}

RET_CODE si479x_tuner_fm_set_servo_trigger(uint8_t tuner_id, uint8_t trigger_id, servo_trigger_t *ptrigger)
{
    return _si479x_set_servo_trigger(tuner_id, 0, trigger_id, ptrigger);
}

RET_CODE si479x_tuner_am_set_servo_trigger(uint8_t tuner_id, uint8_t trigger_id, servo_trigger_t *ptrigger)
{
    return _si479x_set_servo_trigger(tuner_id, 1, trigger_id, ptrigger);
}

//AM servo command
RET_CODE si479x_tuner_am_set_servo(uint8_t tuner_id, am_servo_t *pservo)
{
    RET_CODE ret = RET_SUCCESS;
    
    clear_cmd_memory();
    
    cmd_buff[COMMAND_OFFSET+0] = AM_SET_SERVO;
    cmd_buff[COMMAND_OFFSET+1] = pservo->servo_id;

    _si479x_fill_cmd_uint16_t(cmd_buff, COMMAND_OFFSET+2, pservo->min_limits);
    _si479x_fill_cmd_uint16_t(cmd_buff, COMMAND_OFFSET+4, pservo->max_limits);
    _si479x_fill_cmd_uint16_t(cmd_buff, COMMAND_OFFSET+6, pservo->init);

    cmd_buff[COMMAND_OFFSET+8] = pservo->acc;
    
    ERR_CHECKING(si479x_tuner_cmd(tuner_id, 9, 4));
    
    return ret;
}


RET_CODE si479x_tuner_am_set_servo_transform(uint8_t tuner_id, uint8_t transform_id, am_transform_t *pTransform)
{   
    return _si479x_set_servo_transform(tuner_id, 1, transform_id, (transform_t*)pTransform);
}


uint16_t si479x_tuner_fm_signal_event_count(uint8_t tuner_id, uint8_t eventIndex)
{
    uint16_t tdata;
    
    if (eventIndex >= 1)
        return 0xff;
   
    clear_cmd_memory();
    
    cmd_buff[COMMAND_OFFSET+0] = FM_SIGNAL_EVENT_COUNT;
    cmd_buff[COMMAND_OFFSET+1] = 0;
    
    si479x_tuner_cmd(tuner_id, 2, FM_SIGNAL_EVENT_COUNT_RET_DATA_LENGTH);

    tdata = (((uint16_t)rsp_buff[4+eventIndex*2]) & 0x00ff) | (((uint16_t)rsp_buff[4+eventIndex*2+1]) << 8);

    return tdata;
}

RET_CODE si479x_tuner_fm_signal_event_datalist(uint8_t tuner_id, uint8_t length, uint8_t datalist[])
{
    return si479x_tuner_stauts_datalist(tuner_id, FM_SIGNAL_EVENT_COUNT, 0, FM_SIGNAL_EVENT_COUNT_RET_DATA_LENGTH-4, length, datalist);
}

#ifdef SUPPORT_FMAMDAB
//FM command
RET_CODE si479x_tuner_dab_tune(uint8_t tuner_id, uint8_t detectmode, uint8_t freq_index)
{
    RET_CODE ret = RET_SUCCESS;
    
    clear_cmd_memory();
    
    cmd_buff[COMMAND_OFFSET+0] = DAB_TUNE_FREQ;
    cmd_buff[COMMAND_OFFSET+1] = detectmode;
	cmd_buff[COMMAND_OFFSET+2] = freq_index;
   
    ERR_CHECKING(si479x_tuner_cmd(tuner_id, 3, 4));

    //wait for the STC bit set    
    ERR_CHECKING(si479x_waitForSTC(tuner_id));    
    
    return ret;
}

uint8_t si479x_tuner_dab_current_metrics(uint8_t tuner_id, uint8_t metricsId)
{
    RET_CODE ret = RET_SUCCESS;
   
    clear_cmd_memory();
    
    cmd_buff[COMMAND_OFFSET+0] = DAB_RSQ_STATUS;
    cmd_buff[COMMAND_OFFSET+1] = 0;
    
    ERR_CHECKING(si479x_tuner_cmd(tuner_id, 2, 17));
      
	return rsp_buff[12+metricsId];
}

RET_CODE si479x_tuner_dab_rsq_status(uint8_t tuner_id, uint8_t attune, uint8_t cancel, uint16_t stcack, dab_tuner_metrics_t * p_tuner_metrics)
{
    RET_CODE ret = RET_SUCCESS;
   
    clear_cmd_memory();
    
    cmd_buff[COMMAND_OFFSET+0] = DAB_RSQ_STATUS;
    cmd_buff[COMMAND_OFFSET+1] = (attune & 0x01) << 2 | (cancel & 0x01) << 1 | (stcack & 0x01) ;
    
    ERR_CHECKING(si479x_tuner_cmd(tuner_id, 2, 17));
	
    p_tuner_metrics->valid = !!(rsp_buff[5] & 0x01);
    p_tuner_metrics->freq_index = rsp_buff[6];
	p_tuner_metrics->freq_hz = ((uint32_t)rsp_buff[8])&0xff | ((uint32_t)rsp_buff[9]<<8)&0xff00 | ((uint32_t)rsp_buff[10]<<16)&0xff0000 | ((uint32_t)rsp_buff[11]<<24)&0xff000000; 
    p_tuner_metrics->sqi = rsp_buff[12];
    p_tuner_metrics->rssi= rsp_buff[13];
	p_tuner_metrics->dab_detect= rsp_buff[14];
	p_tuner_metrics->rssi_adj= rsp_buff[15];
       
    return ret;
}
#endif
