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

#include "dc_config.h"
#include "dc_bootup.h"

#include "si479x_core.h"
#include "si479x_tuner.h"
#include "radio_mgr.h"

static uint8_t s_max_stereo = 0;

void Radio_ForceMono()
{
	fm_servo_t servo;
	
	si479x_tuner_fm_get_servo(TUNER0, FM_SERVO_STBLEND, &servo);
	
	if (s_max_stereo == 0)
	{
		s_max_stereo = servo.max_limits;
	}
	        
	servo.servo_id = FM_SERVO_STBLEND;
    servo.max_limits = 0;

    si479x_tuner_fm_set_servo(TUNER0, &servo);
}

void Radio_EnableStereo()
{
	fm_servo_t servo;
	
	if (s_max_stereo == 0)
		return;
	
	si479x_tuner_fm_get_servo(TUNER0, FM_SERVO_STBLEND, &servo);
	
	if (servo.max_limits == s_max_stereo)
		return;
	
	servo.servo_id = FM_SERVO_STBLEND;
	servo.max_limits = s_max_stereo;
	
	si479x_tuner_fm_set_servo(TUNER0, &servo);
}

uint8_t Radio_CheckStereo()
{
	fm_servo_status_t fm_servo_status;
	
	si479x_tuner_fm_servo_status(TUNER0, &fm_servo_status);
	
	return fm_servo_status.stereoblend;
}

