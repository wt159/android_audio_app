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
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>

#include "dc_config.h"
#include "dc_bootup.h"
#include "audio_mgr.h"
#include "radio_mgr.h"

#ifdef SUPPORT_DTS_CS
#include "audio_dts.h"
#endif


static void print_usage()
{
	printf("Command Usage: (case nonsensitivy)\n");
	
	puts("  Tune nFrequency -- Tune to $nFrequency, units is 10kHz for FM and kHz for AM.\n"
	     "  SeekUp -- Find a valid frequency toward high band.\n"
	     "  SeekDown -- Find a valid frequency toward low band.\n"
	     "  AutoScan -- Band scan and save the frequency.\n"
	     "  FM -- Switch band to FM.\n"
	     "  AM -- Switch band to AM.\n"
	     "  Mono -- Force mono on.\n"
	     "  Stereo -- Enable stereo off.\n"
	     "  IsStereo -- Check stereo seperation status.\n"
	     "  Mute -- Mute the audio output.\n"
	     "  Unmute -- Unmute the audio output.\n"
		 "  Volume nVolume -- Set the volume value (0~40).\n"
#ifdef SUPPORT_HIFI		 
		 "  Source nSource -- Switch audio source. (0: radio, 1: auxin, 2: i2s)\n"
#ifdef SUPPORT_AUDIO_EVENT_TONE
	     "  Tone nId -- Generator a tone, 0 or 1.\n"
#ifdef SUPPORT_WAV_PLAY		 
	     "  Wave nId -- Play a short wave file.\n"
#endif		 
#endif		 
#ifdef SUPPORT_PRESET_EQ
	     "  EQ nType -- Set the EQ type. 0 - JAZZ; 1 - POP; 2 - ROCK; 3 - CLASSIC; 4 - INDOOR;  5 - EQ off; .\n"		 
	     "  Bass gain -- Set the bass.\n"
	     "  Middle gain -- Set the middle.\n"
	     "  Treble gain -- Set the treble.\n"
	     "  FadeFront gain -- Set the front fade (-12 ~ 0) db.\n"
	     "  FadeRear gain -- Set the rear fade (-12 ~ 0) db.\n"
	     "  BalanceLeft left -- Set the left banlance (-12 ~ 0) db.\n"
	     "  BalanceRight right -- Set the right banlance (-12 ~ 0) db.\n"
#endif		 
#ifdef SUPPORT_DTS_CS
	     "  DTS onoff -- Set the DTS on/off. 0 - off; 1 - on.\n"
#endif
#endif		 
	     "  Exit -- Exit the application.\n"
		 );
}

void seek_process(uint16_t freq)
{
	printf("Checking %d\n", freq);
}

void found_process(uint16_t freq)
{
	printf("Valid frequency %d\n", freq);
}

////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
	//0: Host load, 1: Flash load
	int ret;
	int bootMode = 0; 
	char buff[256];
	int para;
	int band = 0; //0: FM; 1: AM 2: DAB
	
	//Step a. Platform hal init, GPIO, spi/i2c init, etc.
	platform_hal_init();

	if (argc > 1)
	{
		if (!memcmp("FM", argv[1], 2)) {
            band = 0;
        } else if (!memcmp("AM", argv[1], 2)) {
            band = 1;
        } else if (!memcmp("DAB", argv[1], 3)) {
            band = 2;
        }
	}
	
	//Step b. reset chipsets
	chips_reset();
	
	//Step c. host load or flash load
	if (bootMode == 0) 
	{
		//host load
		ret = dc_hostload_bootup();
		printf("Host load.\n");
	}
	else
	{
		//flash load
		ret = dc_flashload_bootup();
		printf("Flash load.\n");
	}
	
	if (ret != RET_SUCCESS)
	{
		printf("Bootup failed!\n");
		return -1;
	}

    Audio_mgr_init();
    printf("Audio manager init done.\n");
		
	//Step d. configurations post boot and tune
	if (band == BAND_FM) 
	{
		//configure audio settings
		 dc_post_bootup(BAND_FM);
		
		//tune some frequency
		 dc_fm_tune(10430);
		
		band = BAND_FM;
	}
	else if(band == BAND_AM) 
	{
		//configure audio settings
		 dc_post_bootup(BAND_AM);
		
		//tune some frequency
		 dc_am_tune(999);
		
		band = BAND_AM;
	} else if (band == BAND_DAB) {
        dc_post_bootup(BAND_DAB);
//        si479x_dab_set_freq_list();
//        si479x_tuner_dab_current_metrics();
    }
	printf("Bootup done. band:%d\n", band);
	//Step e. Init audio manager

	//Step f. Init DTS if neccessary.
	#ifdef SUPPORT_DTS_CS
	DTS_Init();
	#endif
	
	//Step g. setup radio connections and turn on the audio  
	Audio_switch(RADIO_AUDIO);
	printf("Audio output on.");
	//Step h. set the audio volume, range is 0-40
	// Audio_volume(8);
	// printf("Volume set to 8dB.\n");
	//Step i. monitor RSQ and wait for user commands
    return 0;
	while (1) 
	{
		print_usage();
		
		scanf("%s %d", buff, &para);
		int len = strlen(buff);
		len = len > 255 ? 255 : len;
		
		for (int i=0; i<len; i++) 
		{
			buff[i] = (char)tolower(buff[i]);
		}
			
		//radio related operation
		//////////////////////////////////////////////////////////////////////////////
		//radio related
		//seek up
		if (0 == strcmp(buff, "seekup")) {
			//mute the radio output
			si479x_tuner_mute(TUNER0, 1);
			if (band == BAND_FM)
			{
				printf("FM seek up.\n");
				dc_fm_seek(1, 0, seek_process);
			} 
			else
			{
				printf("AM seek up.\n");
				dc_am_seek(1, 0, seek_process);
			}
			si479x_tuner_mute(TUNER0, 0);
		//seek down
		} else if (0 == strcmp(buff, "seekdown")) {
			//mute/unmute the radio output
			si479x_tuner_mute(TUNER0, 1);
			if (band == BAND_FM)
			{
				printf("FM seek down.\n");
				dc_fm_seek(0, 0, seek_process);
			} 
			else
			{
				printf("AM seek down.\n");
				dc_am_seek(0, 0, seek_process);
			}
			si479x_tuner_mute(TUNER0, 0);
		//auto scan
		} else if (0 == strcmp(buff, "autoscan")) {
			//mute/unmute is done in dc_fm_autoseek/dc_am_autoseek
			if (band == BAND_FM)
			{
				printf("FM auto seek.\n");
				dc_fm_autoseek(seek_process, found_process);	
			}		
			else
			{
				printf("AM auto seek.\n");
				dc_am_autoseek(seek_process, found_process);	
			}
		//switch band to FM
		} else if (0 == strcmp(buff, "fm")) {
			if (band != BAND_FM)
			{
				printf("Switch to FM.\n");
				dc_set_mode(BAND_FM);
				dc_fm_tune(9810);
				band = BAND_FM;
			}
			else
			{
				printf("It's aleary in FM band.\n");
			}
		//switch band to AM
		} else if (0 == strcmp(buff, "am")) {
			if (band != BAND_AM)
			{
				printf("Switch to AM.\n");
				dc_set_mode(BAND_AM);
				dc_am_tune(999);
				band = BAND_AM;
			}			
			else
			{
				printf("It's aleary in AM band.\n");
			}
		//Force MONO on
		} else if (0 == strcmp(buff, "mono")) {
			printf("Force mono on.\n");
			Radio_ForceMono();
		//Enable Stereo
		} else if (0 == strcmp(buff, "stereo")) {
			printf("Enable stereo on.\n");
			Radio_EnableStereo();
		//Enable Stereo
		} else if (0 == strcmp(buff, "isstereo")) {
			uint8_t stereo_blend;
			stereo_blend = Radio_CheckStereo();
			printf("Stereo seperation: %d, it's %s.\n", stereo_blend, stereo_blend>0?"stereo":"mono");
		//Tune some frequency
		} else if (0 == strcmp(buff, "tune")) {
			if (band == BAND_FM) 
			{
				printf("FM tune %d.\n", para);
				dc_fm_tune(para); 
			}
			else
			{
				printf("AM tune %d.\n", para);
				dc_am_tune(para); 
			}
		//mute the audio
		} else if (0 == strcmp(buff, "mute")) {
			printf("Mute the audio output.\n");
			Audio_mute(1);
		//unmute the audio
		} else if (0 == strcmp(buff, "unmute")) {
			printf("Unmute the audio output.\n");
			Audio_mute(0);
		//////////////////////////////////////////////////////////////////////////////
		//audio related
		//volume
		} else if (0 == strcmp(buff, "volume")) {
			printf("Set volume to %d, (range: 0 - 40).\n", para);
			Audio_volume(para);
#ifdef SUPPORT_HIFI		 
		//switch audio source
		} else if (0 == strcmp(buff, "source")) {
			if (para > 2)
			{
				para = 0xff;
			}
				
			printf("switch to audio source %d.\n", para);
			Audio_switch(para);
#ifdef SUPPORT_AUDIO_EVENT_TONE
		//tone generation
		} else if (0 == strcmp(buff, "tone")) {
			if (para == 0) 
			{
				printf("Tone request from Tone0.\n");
				Audio_tonegen(0);
			}
			else
			{
				printf("Tone request from Tone1.\n");
				Audio_tonegen(1);
			}
		} else if (0 == strcmp(buff, "wave")) {
#ifdef SUPPORT_WAV_PLAY	
			printf("Wave play.\n");
			Audio_tonegen(2);
#endif
#endif
#ifdef SUPPORT_PRESET_EQ			
		//EQ
		} else if (0 == strcmp(buff, "eq")) {
			#ifdef SUPPORT_PRESET_EQ
				printf("Set EQ %d. (range: 0 - 5).\n", para);
				Audio_EQ(para);
			#else
				printf("Tone control EQ is not supported.\n");
			#endif
		} else if (0 == strcmp(buff, "bass")) {
			printf("Set Bass %d db. \n", para);
			Audio_Bass(para);
		} else if (0 == strcmp(buff, "treble")) {
			printf("Set Treble %d db. \n", para);
			Audio_Treble(para);
		} else if (0 == strcmp(buff, "middle")) {
			printf("Set Middle %d db. \n", para);
			Audio_Middle(para);		
		} else if (0 == strcmp(buff, "balanceleft")) {
			printf("Set left balance %d.. \n", para);
			Audio_BalanceLeft(para);
		} else if (0 == strcmp(buff, "balanceright")) {
			printf("Set right balance %d.. \n", para);
			Audio_BalanceRight(para);
		} else if (0 == strcmp(buff, "fadefront")) {
			printf("Set front fade %d.. \n", para);
			Audio_FadeFront(para);
		} else if (0 == strcmp(buff, "faderear")) {
			printf("Set rear fade %d.. \n", para);
			Audio_FadeRear(para);
#endif			
#ifdef SUPPORT_DTS_CS
		} else if (0 == strcmp(buff, "dts")) {
			printf("Set DTS %s. \n", para==0 ? "off" : "on");
			//on DTS means bypass off; off DTS means bypass on;
			uint8_t bypass = para==0 ? 1 : 0;
			
			DTS_Bypass(bypass);
			//Usually, bypass delay and cabin EQ with DTS block on the same time
			//Audio_Bypass_Cabin_EQ(bypass);
			//Audio_Bypass_Delay(bypass);
#endif
		//delay	
		//bypass cabin EQ and delay
		
		} else if (0 == strcmp(buff, "dac_offset")) {
			printf("Set DA offset %d.. \n", para);
			si479x_set_dac_offset(para);
		} else if (0 == strcmp(buff, "sleep_mode")) {
			printf("Set Sleep %d db. \n", para);
			si479x_set_sleep_mode(para);	
		
#endif	
		}else if (0 == strcmp(buff, "exit")) {
			break;
		}
	}
	
	return 0;
}
