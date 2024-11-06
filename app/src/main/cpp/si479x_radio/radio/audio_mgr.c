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

#include "audio_mgr.h"
#include "audio_data.h"

#include "si479x_audio_api.h"

static AUDIO_SOURCE _audio_mode;

i2s_config i2s_conf;

RET_CODE si479x_turn_audio(AUDIO_SOURCE mode);

RET_CODE si479x_switch_auxin();

RET_CODE si479x_switch_radio();

RET_CODE si479x_switch_i2s();

RET_CODE si479x_switch_off();

static RET_CODE si479x_disconnect_input_connections();

void Audio_mgr_init()
{	
#ifdef SUPPORT_WAV_PLAY
	//load click clark
	// Audio_load_wav();
#endif		

	si479x_turn_audio(OFF_AUDIO);
}


void Audio_switch(uint8_t mode)
{    
	if (RADIO_AUDIO == mode)
	{
		si479x_switch_radio();
	}
	else if (AUXIN_AUDIO == mode)
	{
		si479x_switch_auxin();
	} 
	else if (I2S_AUDIO == mode)
	{
		si479x_switch_i2s();
	}
	else 
	{
		si479x_switch_off();
	}
}

#ifdef SUPPORT_AUDIO_EVENT_TONE

void Audio_tonegen(uint8_t tone_id)
{
	uint8_t id = tone_id&0x3;
	uint16_t sleeptime = 0;
	uint8_t i = 0;
	uint8_t respbuff[32];
	uint16_t mixerGains[MIXER_GAIN_CNT];

	if (tone_id >= 3)
		return;
	
	//read current mixer value
	uint8_t length;
	#ifdef SUPPORT_HUB_OUTPUT
	uint16_t backupGain[2][HIFI_HUB_OUTPUT_CHANNELS];
	uint8_t mixers[HIFI_HUB_OUTPUT_CHANNELS] = {HIFI_HUB_MIXER0_ID, HIFI_HUB_MIXER1_ID};
	for (i=0; i<HIFI_HUB_OUTPUT_CHANNELS; i++)
	#endif

	#ifdef SUPPORT_4CHAN_OUTPUT
	uint16_t backupGain[2][HIFI_4CHAN_OUTPUT_CHANNELS];
	uint8_t mixers[HIFI_4CHAN_OUTPUT_CHANNELS] = {HIFI_4CHAN_LEFT_FRONT_MIXER_ID, HIFI_4CHAN_LEFT_REAR_MIXER_ID, HIFI_4CHAN_RIGHT_FRONT_MIXER_ID, HIFI_4CHAN_RIGHT_REAR_MIXER_ID};
	for (i=0; i<HIFI_4CHAN_OUTPUT_CHANNELS; i++)
	#endif

	#ifdef SUPPORT_6CHAN_OUTPUT
	uint16_t backupGain[2][HIFI_6CHAN_OUTPUT_CHANNELS];
	uint8_t mixers[HIFI_6CHAN_OUTPUT_CHANNELS] = {HIFI_6CHAN_LEFT_FRONT_MIXER_ID, HIFI_6CHAN_LEFT_REAR_MIXER_ID, HIFI_6CHAN_RIGHT_FRONT_MIXER_ID, HIFI_6CHAN_RIGHT_REAR_MIXER_ID, HIFI_6CHAN_CENTER_MIXER_ID, HIFI_6CHAN_SUBVOF_MIXER_ID};
	for (i=0; i<HIFI_6CHAN_OUTPUT_CHANNELS; i++)
	#endif
	{
		si479x_get_mixer(mixers[i], mixerGains);
		
		backupGain[0][i] = mixerGains[0];
		backupGain[1][i] = mixerGains[tone_id+1];
		
		mixerGains[tone_id+1] = mixerGains[0];
		mixerGains[0] -= 0x0600; // - 6.0 db
		
		si479x_set_mixer(mixers[i], mixerGains);
	}

	if (tone_id <= 1)
	{
#define AMP_ABS			0x586
#define REPEAT_TIMES	5
		si479x_hifi_tonegen_request(tone_id, AMP_ABS, REPEAT_TIMES, 2, &g_tone_descriptor_t[2*id]);

		sleeptime = (g_tone_descriptor_t[id].start_time + g_tone_descriptor_t[id].attack_time + g_tone_descriptor_t[id].peak_time 
				+ g_tone_descriptor_t[id].decay_time + g_tone_descriptor_t[id].fall_time) * REPEAT_TIMES + 500;

		sleep_ms(sleeptime);
	}
	else if (tone_id == 2)
	{        
#ifdef SUPPORT_WAV_PLAY
		si479x_hifi_wave_play(0, 4, 2, 0, wave_list);
		sleep_ms(3000);

		si479x_hifi_wave_stop(0);
#endif
	}

	#ifdef SUPPORT_HUB_OUTPUT
	for (i=0; i<HIFI_HUB_OUTPUT_CHANNELS; i++)
	#endif

	#ifdef SUPPORT_4CHAN_OUTPUT
	for (i=0; i<HIFI_4CHAN_OUTPUT_CHANNELS; i++)
	#endif

	#ifdef SUPPORT_6CHAN_OUTPUT
	for (i=0; i<HIFI_6CHAN_OUTPUT_CHANNELS; i++)
	#endif
	{
		mixerGains[0] = backupGain[0][i];
		mixerGains[tone_id+1] = backupGain[1][i];
				
		si479x_set_mixer(mixers[i], mixerGains);
	}
}
#endif

void Audio_mute(uint8_t mute)
{
	RET_CODE ret = RET_SUCCESS;
	uint8_t mutectl;
	
#ifdef SUPPORT_HIFI
	//mute all DACs
	uint8_t dacx = PORT_DAC0;
	mutectl = mute != 0 ? 1 : 0;

	for (; dacx<PORT_DAC5; dacx++) 
	{
		si479x_hifi_port_mute(dacx, 0, mutectl);
	}
#else //radio only port
	si479x_tuner_mute(0, mute);
#endif	
}

#define MAX_VOLUME	40

double volume_gains[MAX_VOLUME+1] = 
{
	-128.0,
	-80.0,
	-75.0,
	-65.0,
	-60.0,
	-55.0,
	-50.0,
	-45.0,
	-40.0,
	-37.0,
	-35.0,
	-33.0,
	-31.0,
	-29.0,
	-27.0,
	-26.0,
	-25.0,
	-24.0,
	-23.0,
	-22.0,
	-21.0,
	-20.0,
	-19.0,
	-18.0
	-17.0,
	-16.0,
	-15.0,
	-14.0,
	-13.0,
	-12.0,
	-11.0,
	-10.0,
	-9.0,
	-8.0,
	-7.5,
	-6.5,
	-5.5,
	-4.5,
	-3.0,
	-1.5,
	-0.5,
	0,
};

//volume range: 0 ~ MAX_VOLUME
void Audio_volume(uint8_t volume)
{	
	uint8_t tvolume = LIMIT(volume, 0, MAX_VOLUME);
	
	si479x_set_output_gain(volume_gains[tvolume]);
}

void DAC_compensate (uint8_t on)
{
	if (on != 0) 
	{
		//si479x_hifi_coeff_write(0, 16, 64, DAC_filter3_on_cmd);
		si479x_hifi_coeff_write(3, 16, 64, DAC_filter3_on_cmd);
	}
	else
	{
		//si479x_hifi_coeff_write(0, 16, 64, DAC_filter3_off_cmd);
		si479x_hifi_coeff_write(2, 52, 208, DAC_filter2_off_cmd);
		si479x_hifi_coeff_write(3, 16, 64, DAC_filter3_off_cmd);
	}
}

#ifdef SUPPORT_PRESET_EQ

void Audio_EQ (uint8_t id)
{
#ifdef SUPPORT_HUB_OUTPUT //hub does not support EQ
	return;
#else //for 4chan and 6chan reference design
		
	uint8_t i, j;
	#ifdef SUPPORT_9BANDS_TONE_CONTROL //a unnique 9 band 
	#define MAX_BAND_PER_MODULE 9
		
	switch (id) 
	{
		case EQ_OFF:
			for (i=0; i<MAX_BAND_PER_MODULE; i++)
			{
				for (j=0; j<15; j++)
				{
					bulk_buff[i*15+j] = EQ_1band_no_filter_coefficients[j];
				}

				bulk_buff[MAX_BAND_PER_MODULE*15+i] = EQ_1band_no_filter_coefficients[15];
			}
			break;

			break;
		case JAZZ:
		case POP:
		case ROCK:
		case CLASSIC:
		case INDOOR:
			for (i=0; i<MAX_BAND_PER_MODULE; i++)
			{
				for (j=0; j<15; j++)
					bulk_buff[i*15+j] = EQ_9band_filter_coefficients[id][i][j];

				bulk_buff[MAX_BAND_PER_MODULE*15+i] = EQ_9band_filter_coefficients[id][i][15];
			}
			break;
		default:
			return;
	}

	#ifdef SUPPORT_4CHAN_OUTPUT
	si479x_hifi_module_parameter_write(HIFI_4CHAN_LEFT_EQ_ID, 0x1, MAX_BAND_PER_MODULE*16, bulk_buff);
	si479x_hifi_module_parameter_write(HIFI_4CHAN_RIGHT_EQ_ID, 0x1, MAX_BAND_PER_MODULE*16, bulk_buff);
	#endif
	
	#ifdef SUPPORT_6CHAN_OUTPUT
	si479x_hifi_module_parameter_write(HIFI_6CHAN_LEFT_EQ_ID, 0x1, MAX_BAND_PER_MODULE*16, bulk_buff);
	si479x_hifi_module_parameter_write(HIFI_6CHAN_RIGHT_EQ_ID, 0x1, MAX_BAND_PER_MODULE*16, bulk_buff);
	#endif
	#endif //SUPPORT_9BANDS_TONE_CONTROL

	#ifdef SUPPORT_3X3BANDS_TONE_CONTROL //3 module with 3band for each module
	#define MAX_BAND_PER_MODULE 3
	switch (id) 
	{
		case EQ_OFF:
			for (i=0; i<MAX_BAND_PER_MODULE; i++)
			{
				for (j=0; j<15; j++)
				{
					bulk_buff[i*15+j] = EQ_1band_no_filter_coefficients[j];
				}

				bulk_buff[MAX_BAND_PER_MODULE*15+i] = EQ_1band_no_filter_coefficients[15];
			}
			si479x_hifi_module_parameter_write(HIFI_BASS_ID, 0x1, MAX_BAND_PER_MODULE*16, bulk_buff);
			si479x_hifi_module_parameter_write(HIFI_MIDS_ID, 0x1, MAX_BAND_PER_MODULE*16, bulk_buff);
			si479x_hifi_module_parameter_write(HIFI_TREB_ID, 0x1, MAX_BAND_PER_MODULE*16, bulk_buff);
			break;
		case JAZZ:
		case POP:
		case ROCK:
		case CLASSIC:
		case INDOOR:
			for (i=0; i<3; i++)
			{
				for (j=0; j<15; j++)
					bulk_buff[i*15+j] = EQ_9band_filter_coefficients[id][i][j];

				bulk_buff[MAX_BAND_PER_MODULE*15+i] = EQ_9band_filter_coefficients[id][i][15];
			}
			si479x_hifi_module_parameter_write(HIFI_BASS_ID, 0x1, MAX_BAND_PER_MODULE*16, bulk_buff);

			for (; i<6; i++)
			{
				for (j=0; j<15; j++)
					bulk_buff[(i%3)*15+j] = EQ_9band_filter_coefficients[id][i][j];

				bulk_buff[MAX_BAND_PER_MODULE*15+(i%3)] = EQ_9band_filter_coefficients[id][i][15];
			}
			si479x_hifi_module_parameter_write(HIFI_MIDS_ID, 0x1, MAX_BAND_PER_MODULE*16, bulk_buff);

			for (; i<9; i++)
			{
				for (j=0; j<15; j++)
					bulk_buff[(i%3)*15+j] = EQ_9band_filter_coefficients[id][i][j];

				bulk_buff[MAX_BAND_PER_MODULE*15+(i%3)] =EQ_9band_filter_coefficients[id][i][15];
			}
			si479x_hifi_module_parameter_write(HIFI_TREB_ID, 0x1, MAX_BAND_PER_MODULE*16, bulk_buff);
			break;
		default:
			return;
	}
	#endif

#endif
}

#endif


RET_CODE si479x_disconnect_input_connections()
{
    RET_CODE ret = RET_SUCCESS;
	
	if (RADIO_AUDIO == _audio_mode)
	{
		ERR_CHECKING(si479x_hifi_xbar_disconnect_node(radio_port2node_connections, RADIO_PORT2NODE_NUM));
	}
	else if (AUXIN_AUDIO == _audio_mode)
	{
		ERR_CHECKING(si479x_hifi_xbar_disconnect_node(auxin_port2node_connections, AUXIN_PORT2NODE_NUM));
	} 
	else if (I2S_AUDIO == _audio_mode)
	{
		ERR_CHECKING(si479x_hifi_xbar_disconnect_node(i2s_port2node_connections, I2S_PORT2NODE_NUM));
	}
	
    return ret;
}

#if 0
RET_CODE si479x_disconnect_output_connections()
{
    RET_CODE ret = RET_SUCCESS;
#ifdef SUPPORT_HUB_OUTPUT
    ERR_CHECKING(si479x_hifi_xbar_disconnect_node(output_node2port_hub_connections, HIFI_HUB_OUTPUT_CHANNELS));
#endif    
#ifdef SUPPORT_4CHAN_OUTPUT
    ERR_CHECKING(si479x_hifi_xbar_disconnect_node(output_node2port_4ch_connections, HIFI_4CHAN_OUTPUT_CHANNELS));
#endif    
#ifdef SUPPORT_6CHAN_OUTPUT
    ERR_CHECKING(si479x_hifi_xbar_disconnect_node(output_node2port_6ch_connections, HIFI_6CHAN_OUTPUT_CHANNELS));
#endif    
    return ret;
}
#endif


RET_CODE si479x_turn_audio(uint8_t mode)
{
    RET_CODE ret = RET_SUCCESS;
    
    set_chip_id(CHIP_ID_0);
	
	//enable DACs
    #ifdef SUPPORT_HUB_OUTPUT
        ERR_CHECKING(si479x_DAC_config(1, 0, 0, 1));
	#endif
	
	#ifdef SUPPORT_4CHAN_OUTPUT
        // ERR_CHECKING(si479x_DAC_config(1, 1, 0, 1));
	#endif
	
	#ifdef SUPPORT_6CHAN_OUTPUT
        // ERR_CHECKING(si479x_DAC_config(1, 1, 1, 1));
	#endif

	//enable DAC
//    ERR_CHECKING(si479x_DAC_config(2, 0, 0, 1));
	
    //config i2s tx 0
	i2s_conf.sample_rate = FS16KS;
    i2s_conf.purpose = PURPOSE_AUDIO;
    i2s_conf.master = I2S_SLAVE;
    i2s_conf.clkfs = OUT0;
    i2s_conf.data0 = DOUT0;
    i2s_conf.fill = ZEROS;
    i2s_conf.slot_size = THIRTYTWO;
    i2s_conf.sample_size = SIXTEEN;
    i2s_conf.bitorder = MSBFIRST;
    i2s_conf.swap = LEFTRIGHT;
    i2s_conf.clkinv = CLK_NORMAL;
    i2s_conf.frame_mode = I2S;
    i2s_conf.clock_domain = 0x1;
//    i2s_conf.length =TDM_OFF;
    ERR_CHECKING(si479x_I2STX0_config(&i2s_conf));
    printf("%s: sample rate :%d\n", __func__, i2s_conf.sample_rate);
    //enable i2s Tx 0
    ERR_CHECKING(si479x_I2S_control(TX0, PORT_ENABLE, NULL)); 
    
    if (mode == RADIO_AUDIO)
	{       
		// ERR_CHECKING(si479x_hifi_xbar_connect_node(radio_port2node_connections, RADIO_PORT2NODE_NUM));
		ERR_CHECKING(si479x_hifi_xbar_connect_node(radio_port2port_connections, RADIO_PORT2PORT_NUM));
	}
    else if (mode == AUXIN_AUDIO)
	{
        ERR_CHECKING(si479x_hifi_xbar_connect_node(auxin_port2node_connections, AUXIN_PORT2NODE_NUM));
	} 
	else if (mode == I2S_AUDIO)
	{
		ERR_CHECKING(si479x_hifi_xbar_connect_node(i2s_port2node_connections, I2S_PORT2NODE_NUM));
	}
    
#ifdef SUPPORT_AUDIO_EVENT_TONE       
	#ifdef SUPPORT_HUB_OUTPUT
	{
	    ERR_CHECKING(si479x_hifi_xbar_connect_node(tone_port2node_hub_connections, TONE_REQUEST_NUM));
	    ERR_CHECKING(si479x_hifi_xbar_connect_node(wav_port2node_hub_connections, WAVE_REQUEST_NUM));
	}
	#endif
	#if (defined SUPPORT_4CHAN_OUTPUT) || (defined SUPPORT_6CHAN_OUTPUT)	
	{ 
	    // ERR_CHECKING(si479x_hifi_xbar_connect_node(tone_port2node_multichan_connections, TONE_REQUEST_NUM));
	    // ERR_CHECKING(si479x_hifi_xbar_connect_node(wav_port2node_multichan_connections, WAVE_REQUEST_NUM));
	}
	#endif
#endif

#ifdef SUPPORT_HUB_OUTPUT
        ERR_CHECKING(si479x_hifi_xbar_connect_node(output_node2port_hub_connections, HIFI_HUB_OUTPUT_CHANNELS));
#endif
#ifdef SUPPORT_4CHAN_OUTPUT	
//        ERR_CHECKING(si479x_hifi_xbar_connect_node(output_node2port_4ch_connections, HIFI_4CHAN_OUTPUT_CHANNELS));
#endif
#ifdef SUPPORT_6CHAN_OUTPUT	
        // ERR_CHECKING(si479x_hifi_xbar_connect_node(output_node2port_6ch_connections, HIFI_6CHAN_OUTPUT_CHANNELS));
#endif

	_audio_mode = mode;
	printf("_audio_mode=%d\n", _audio_mode);
    return ret;
}

RET_CODE si479x_switch_auxin()
{
	RET_CODE ret = RET_SUCCESS;
	
	if (AUXIN_AUDIO == _audio_mode)
	{
		return ret;
	}
	
	if (OFF_AUDIO != _audio_mode)
		ERR_CHECKING(si479x_disconnect_input_connections());
    
    ERR_CHECKING(si479x_hifi_xbar_connect_node(auxin_port2node_connections, AUXIN_PORT2NODE_NUM));
	
	_audio_mode = AUXIN_AUDIO;
    
    return ret;
}

RET_CODE si479x_switch_radio()
{
	RET_CODE ret = RET_SUCCESS;
	if (RADIO_AUDIO == _audio_mode)
	{
		return ret;
	}
	
//	if (OFF_AUDIO != _audio_mode)
//		ERR_CHECKING(si479x_disconnect_input_connections());
        
	// ERR_CHECKING(si479x_hifi_xbar_connect_node(radio_port2node_connections, RADIO_PORT2NODE_NUM));
//	ERR_CHECKING(si479x_hifi_xbar_connect_port(radio_port2port_connections, RADIO_PORT2PORT_NUM));
	_audio_mode = RADIO_AUDIO;
	
    return ret;
}

RET_CODE si479x_switch_i2s()
{
	RET_CODE ret = RET_SUCCESS;
	
	if (I2S_AUDIO == _audio_mode)
	{
		return ret;
	}
	
	if (OFF_AUDIO != _audio_mode)
		ERR_CHECKING(si479x_disconnect_input_connections());
        
	ERR_CHECKING(si479x_hifi_xbar_connect_node(i2s_port2node_connections, I2S_PORT2NODE_NUM));
	
	_audio_mode = I2S_AUDIO;
	
    return ret;
}

RET_CODE si479x_switch_off()
{
	RET_CODE ret = RET_SUCCESS;
	
	if (OFF_AUDIO == _audio_mode)
	{
		return ret;
	}
	
	ERR_CHECKING(si479x_disconnect_input_connections());
	
	_audio_mode = OFF_AUDIO;
	
    return ret;
	
}


#ifdef SUPPORT_WAV_PLAY
RET_CODE Audio_load_wav()
{
	RET_CODE ret = RET_SUCCESS;

    ERR_CHECKING(si479x_hifi_wave_add(0, 0xe8, (uint8_t *)wave0_sec0));
    ERR_CHECKING(si479x_hifi_wave_append(0, 0xe8, (uint8_t *)wave0_sec1));
    ERR_CHECKING(si479x_hifi_wave_append(0, 0x70, (uint8_t *)wave0_sec2));

    ERR_CHECKING(si479x_hifi_wave_add(1, 0xe8, (uint8_t *)wave1_sec0));
    ERR_CHECKING(si479x_hifi_wave_append(1, 0xe8, (uint8_t *)wave1_sec1));
    ERR_CHECKING(si479x_hifi_wave_append(1, 0x7c, (uint8_t *)wave1_sec2));

    return ret;
}
#endif


void Audio_Treble(double gain)
{
	//si479x_set_tone_treble(0, gain, 0.4, 6000);
	//si479x_set_tone_treble(1, gain, 0.4, 8000);
	si479x_set_tone_treble(2, gain, 0.4, 10000);
}

void Audio_Middle(double gain)
{
	//si479x_set_tone_mid(0, gain, 1, 1000);
	si479x_set_tone_mid(1, gain, 1, 2000);
	//si479x_set_tone_mid(2, gain, 1, 3000);
}

void Audio_Bass(double gain)
{
	si479x_set_tone_bass(0, gain, 0.4, 50);
	//si479x_set_tone_bass(1, gain, 0.4, 200);
	//si479x_set_tone_bass(2, gain, 0.4, 300);
}

void Audio_FadeFront(double front)
{
	front = LIMIT(front, AUDIO_FADE_MIN, AUDIO_FADE_MAX);

	si479x_set_fade(front, 0);
}

void Audio_FadeRear(double rear)
{
	rear = LIMIT(rear, AUDIO_FADE_MIN, AUDIO_FADE_MAX);

	si479x_set_fade(0, rear);
}

void Audio_BalanceLeft(double left)
{
	left = LIMIT(left, AUDIO_BALANCE_MIN, AUDIO_BALANCE_MAX);

	si479x_set_balance(left, 0);
}

void Audio_BalanceRight(double right)
{
	right = LIMIT(right, AUDIO_BALANCE_MIN, AUDIO_BALANCE_MAX);

	si479x_set_balance(0, right);
}

void Audio_Bypass_Cabin_EQ(int8_t onoff)
{
	uint8_t value = 0;

	if (onoff != 0)
	{
		value = 2;
	} 
	
	#ifdef SUPPORT_4CHAN_OUTPUT
	si479x_hifi_modify_control(HIFI_4CHAN_LEFT_FRONT_EQ_ID, 0, 0, value, 0x2);
	si479x_hifi_modify_control(HIFI_4CHAN_LEFT_REAR_EQ_ID, 0, 0, value, 0x2);
	si479x_hifi_modify_control(HIFI_4CHAN_RIGHT_FRONT_EQ_ID, 0, 0, value, 0x2);
	si479x_hifi_modify_control(HIFI_4CHAN_RIGHT_REAR_EQ_ID, 0, 0, value, 0x2);
	#endif
	
	#ifdef SUPPORT_6CHAN_OUTPUT
	si479x_hifi_modify_control(HIFI_6CHAN_LEFT_FRONT_EQ_ID, 0, 0, value, 0x2);
	si479x_hifi_modify_control(HIFI_6CHAN_LEFT_REAR_EQ_ID, 0, 0, value, 0x2);
	si479x_hifi_modify_control(HIFI_6CHAN_RIGHT_FRONT_EQ_ID, 0, 0, value, 0x2);
	si479x_hifi_modify_control(HIFI_6CHAN_RIGHT_REAR_EQ_ID, 0, 0, value, 0x2);
	si479x_hifi_modify_control(HIFI_6CHAN_CENTER_EQ_ID, 0, 0, value, 0x2);
	si479x_hifi_modify_control(HIFI_6CHAN_SUBWOF_EQ_ID, 0, 0, value, 0x2);
	#endif
}

void Audio_Bypass_Delay(int8_t onoff)
{
	uint8_t value = 0;

	if (onoff != 0)
	{
		value = 2;
	} 
		
	#ifdef SUPPORT_4CHAN_OUTPUT
	si479x_hifi_modify_control(HIFI_4CHAN_LEFT_FRONT_DELAY_ID, 0, 0, value, 0x2);
	si479x_hifi_modify_control(HIFI_4CHAN_LEFT_REAR_DELAY_ID, 0, 0, value, 0x2);
	si479x_hifi_modify_control(HIFI_4CHAN_RIGHT_FRONT_DELAY_ID, 0, 0, value, 0x2);
	si479x_hifi_modify_control(HIFI_4CHAN_RIGHT_REAR_DELAY_ID, 0, 0, value, 0x2);
	#endif
	
	#ifdef SUPPORT_6CHAN_OUTPUT
	si479x_hifi_modify_control(HIFI_6CHAN_LEFT_FRONT_DELAY_ID, 0, 0, value, 0x2);
	si479x_hifi_modify_control(HIFI_6CHAN_LEFT_REAR_DELAY_ID, 0, 0, value, 0x2);
	si479x_hifi_modify_control(HIFI_6CHAN_RIGHT_FRONT_DELAY_ID, 0, 0, value, 0x2);
	si479x_hifi_modify_control(HIFI_6CHAN_RIGHT_REAR_DELAY_ID, 0, 0, value, 0x2);
	si479x_hifi_modify_control(HIFI_6CHAN_CENTER_DELAY_ID, 0, 0, value, 0x2);
	si479x_hifi_modify_control(HIFI_6CHAN_SUBWOF_DELAY_ID, 0, 0, value, 0x2);
	#endif
}


