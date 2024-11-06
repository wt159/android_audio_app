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

#ifndef _HIFI_DTSCS_CONFIG_H_
#define _HIFI_DTSCS_CONFIG_H_

#ifndef SUPPORT_6CHAN_OUTPUT
#define SUPPORT_6CHAN_OUTPUT
#endif

#ifndef SUPPORT_HIFI
#define SUPPORT_HIFI
#endif

#ifndef SUPPORT_AUDIO_EVENT_TONE
#define SUPPORT_AUDIO_EVENT_TONE /// Define it when support tone output
#endif

#ifndef SUPPORT_PRESET_EQ
#define SUPPORT_PRESET_EQ
#endif

#ifndef SUPPORT_WAV_PLAY
#define SUPPORT_WAV_PLAY
#endif

#define HIFI_6CHAN_OUTPUT_CHANNELS 6

#define SUPPORT_DTS_CS
#define DTS_CS_MODULE_ID 39

#define HIFI_6CHAN_LEFT_FRONT 0
#define HIFI_6CHAN_LEFT_REAR 1
#define HIFI_6CHAN_RIGHT_FRONT 2
#define HIFI_6CHAN_RIGHT_REAR 3
#define HIFI_6CHAN_CENTER 4
#define HIFI_6CHAN_SUBVOF 5

#define AUDIO_SAMPLE_RATE 48000

/////////////////////////////////////////////////////////////////////////////////////////
#define AUDIO_FREQ_MIN 20
#define AUDIO_FREQ_MAX 20000

// Sets 'filter' to the biquad params for a low-shelving filter.
#define AUDIO_BASS_MAX 12
#define AUDIO_BASS_MIN -12
#define AUDIO_BASS_S_VALUE_MAX 10
#define AUDIO_BASS_S_VALUE_MIN 0.1

// Sets 'filter' to the biquad params for a high-shelving filter.
#define AUDIO_TREBLE_MAX 12
#define AUDIO_TREBLE_MIN -12
#define AUDIO_TREBLE_S_VALUE_MAX 10
#define AUDIO_TREBLE_S_VALUE_MIN 0.1

// Sets 'filter' to the biquad params for a peaking/trough filter.
#define AUDIO_MID_MAX 12
#define AUDIO_MID_MIN -12
#define AUDIO_MID_Q_VALUE_MAX 10
#define AUDIO_MID_Q_VALUE_MIN 0.1

#define SUPPORT_9BANDS_TONE_CONTROL

#define HIFI_6CHAN_LEFT_EQ_ID 10
#define HIFI_6CHAN_RIGHT_EQ_ID 13

/////////////////////////////////////////////////////////////////////////////////////////
#define HIFI_6CHAN_LEFT_FRONT_MIXER_ID 32
#define HIFI_6CHAN_LEFT_REAR_MIXER_ID 35
#define HIFI_6CHAN_RIGHT_FRONT_MIXER_ID 38
#define HIFI_6CHAN_RIGHT_REAR_MIXER_ID 41
#define HIFI_6CHAN_CENTER_MIXER_ID 29
#define HIFI_6CHAN_SUBVOF_MIXER_ID 44

#define HIFI_6CHAN_LEFT_FRONT_EQ_ID 24
#define HIFI_6CHAN_LEFT_REAR_EQ_ID 31
#define HIFI_6CHAN_RIGHT_FRONT_EQ_ID 40
#define HIFI_6CHAN_RIGHT_REAR_EQ_ID 43
#define HIFI_6CHAN_CENTER_EQ_ID 34
#define HIFI_6CHAN_SUBWOF_EQ_ID 37

#define HIFI_6CHAN_LEFT_FRONT_DELAY_ID 19
#define HIFI_6CHAN_LEFT_REAR_DELAY_ID 20
#define HIFI_6CHAN_RIGHT_FRONT_DELAY_ID 21
#define HIFI_6CHAN_RIGHT_REAR_DELAY_ID 22
#define HIFI_6CHAN_CENTER_DELAY_ID 18
#define HIFI_6CHAN_SUBWOF_DELAY_ID 23

#define AUDIO_FADE_MIN -12
#define AUDIO_FADE_MAX 0

#define HIFI_6CHAN_LEFT_VOLUME_ID 12
#define HIFI_6CHAN_RIGHT_VOLUME_ID 15

#define AUDIO_BALANCE_MIN -12
#define AUDIO_BALANCE_MAX 0

#define MIXER_GAIN_CNT 5

#define MIXER_PARA_ID 2

#endif
