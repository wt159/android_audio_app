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

#ifndef _AUDIO_CONFIG_H_
#define _AUDIO_CONFIG_H_

#ifndef SUPPORT_HUB_OUTPUT
#define SUPPORT_HUB_OUTPUT
#endif

#ifdef SUPPORT_4CHAN_OUTPUT
#undef SUPPORT_4CHAN_OUTPUT
#endif

#ifdef SUPPORT_6CHAN_OUTPUT
#undef SUPPORT_6CHAN_OUTPUT
#endif

#define HIFI_HUB_OUTPUT_CHANNELS 2

#define HIFI_HUB_MIXER0_ID 0
#define HIFI_HUB_MIXER1_ID 2

#define HIFI_HUB_LEFT 0
#define HIFI_HUB_RIGHT 1

#endif
