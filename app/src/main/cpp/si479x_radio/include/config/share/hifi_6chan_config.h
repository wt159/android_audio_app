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

#ifndef _HIFI_6CHAN_CONFIG_H_
#define _HIFI_6CHAN_CONFIG_H_

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

#define HIFI_6CHAN_LEFT_FRONT_EQ_ID 24
#define HIFI_6CHAN_LEFT_REAR_EQ_ID 31
#define HIFI_6CHAN_RIGHT_FRONT_EQ_ID 34
#define HIFI_6CHAN_RIGHT_REAR_EQ_ID 37
#define HIFI_6CHAN_CENTER_EQ_ID 40
#define HIFI_6CHAN_SUBWOF_EQ_ID 43

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

#define HIFI_BASS_ID 8
#define HIFI_MIDS_ID 9
#define HIFI_TREB_ID 10

/////////////////////////////////////////////////////////////////////////////////////////
#define HIFI_6CHAN_LEFT_FRONT_MIXER_ID 27
#define HIFI_6CHAN_LEFT_REAR_MIXER_ID 30
#define HIFI_6CHAN_RIGHT_FRONT_MIXER_ID 33
#define HIFI_6CHAN_RIGHT_REAR_MIXER_ID 36
#define HIFI_6CHAN_CENTER_MIXER_ID 39
#define HIFI_6CHAN_SUBVOF_MIXER_ID 42

#define AUDIO_FADE_MIN -12
#define AUDIO_FADE_MAX 0

#define HIFI_6CHAN_LEFT_VOLUME_ID 16
#define HIFI_6CHAN_RIGHT_VOLUME_ID 17

#define AUDIO_BALANCE_MIN -12
#define AUDIO_BALANCE_MAX 0

#endif
