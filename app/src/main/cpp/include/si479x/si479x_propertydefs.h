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
 *@file this header file define the property address for si479x headunit.
 *
 * They are from AN543.pdf released by silicom labs broadcast BU. You can
 *
 * add other property into this document if necessary.
 *
 * @author Michael.Yi@silabs.com
*/

#ifndef _SI479X_PROPERTY_DEFS_H_
#define _SI479X_PROPERTY_DEFS_H_

#define INTERRUPT_ENABLE0				0x0000	//0x0000	Interrupt enable control	
#define INTERRUPT_ENABLE1				0x0001	//0x0000	Interrupt enable control	
#define INTERRUPT_REPEAT0				0x0002	//0x0000	Interrupt repeat control	
#define INTERRUPT_REPEAT1				0x0003	//0x0000	Interrupt repeat control	
#define INTERRUPT_PULSEWIDTH			0x0004	//0x000a	Interrupt pulse width	
#define INTERRUPT_OPTIONS				0x0005	//0x0000	This property sets miscellaneous options for the interrupt

#define AGC_TEST_MODE					0x0500	//0x0000	AGC test mode	
#define AGC_AM_RF_INDEX_OVERRIDE		0x0501	//0x0000	AGC index override	
#define AGC_FM_RF_INDEX_OVERRIDE		0x0502	//0x0000	AGC index override	
#define AGC_B3_RF_INDEX_OVERRIDE		0x0503	//0x0000	AGC index override	
#define AGC_LB_RF_INDEX_OVERRIDE		0x0504	//0x0000	AGC index override	
#define AGC_AM_MS_INDEX_OVERRIDE		0x0505	//0x0000	AGC index override	
#define AGC_IF_INDEX_OVERRIDE			0x0508	//0x0000	AGC index override	
#define AGC_AM_RF_THRESHOLD				0x050a	//0x0006	AM_RF AGC threshold	
#define AGC_FM_RF_THRESHOLD				0x050b	//0x0000	FM_RF AGC threshold	
#define AGC_B3_RF_THRESHOLD				0x050c	//0x0003	B3_RF AGC threshold	
#define AGC_LB_RF_THRESHOLD				0x050d	//0xfffd	LB_RF AGC threshold	
#define AGC_AM_MS_THRESHOLD				0x050e	//0x0012	AM_MS AGC threshold	
#define AGC_AM_IF_THRESHOLD				0x0511	//0x0000	AM_IF AGC threshold	
#define AGC_FM_IF_THRESHOLD				0x0512	//0x0000	FM_IF AGC threshold	
#define AGC_B3_IF_THRESHOLD				0x0513	//0x0000	B3_IF AGC threshold	
#define AGC_LB_IF_THRESHOLD				0x0514	//0x0000	LB_IF AGC threshold	
#define AGC_AM_RF_HYSTERESIS			0x0518	//0x0008	AM_RF AGC hysteresis	
#define AGC_FM_RF_HYSTERESIS			0x0519	//0x0006	FM_RF AGC hysteresis	
#define AGC_B3_RF_HYSTERESIS			0x051a	//0x0007	B3_RF AGC hysteresis	
#define AGC_LB_RF_HYSTERESIS			0x051b	//0x0007	LB_RF AGC hysteresis	
#define AGC_AM_MS_HYSTERESIS			0x051c	//0x0010	AM_MS AGC hysteresis	
#define AGC_AM_IF_HYSTERESIS			0x051f	//0x0011	AM_IF AGC hysteresis	
#define AGC_FM_IF_HYSTERESIS			0x0520	//0x0008	FM_IF AGC hysteresis	
#define AGC_B3_IF_HYSTERESIS			0x0521	//0x0008	B3_IF AGC hysteresis	
#define AGC_LB_IF_HYSTERESIS			0x0522	//0x0008	LB_IF AGC hysteresis	
#define AGC_AM_RF_LOWER_LIMIT			0x0526	//0x0000	AM_RF AGC Lower Limit	
#define AGC_FM_RF_LOWER_LIMIT			0x0527	//0x0000	FM_RF AGC Lower Limit	
#define AGC_B3_RF_LOWER_LIMIT			0x0528	//0x0000	B3_RF AGC Lower Limit	
#define AGC_LB_RF_LOWER_LIMIT			0x0529	//0x0000	LB_RF AGC Lower Limit	
#define AGC_AM_MS_LOWER_LIMIT			0x052a	//0x0000	AM_MS AGC Lower Limit	
#define AGC_AM_IF_LOWER_LIMIT			0x052d	//0x0000	AM_IF AGC Lower Limit	
#define AGC_FM_IF_LOWER_LIMIT			0x052e	//0x0000	FM_IF AGC Lower Limit	
#define AGC_B3_IF_LOWER_LIMIT			0x052f	//0x0000	B3_IF AGC Lower Limit	
#define AGC_LB_IF_LOWER_LIMIT			0x0530	//0x0000	LB_IF AGC Lower Limit	
#define AGC_AM_RF_LIMITER				0x0539	//0x0064	Property to limit how far 60hz energy can engage RF AGC	

#define FRONTEND_LT_ENABLE				0x0600	//0x0000	Loop-through enable	
#define FRONTEND_ANTENNA_INACTIVE		0x0604	//0x0055	Inactive antenna state	

#define RADIO_ANALOG_VOLUME			    0x0700	//0x0060	Sets the radio dac volume.
#define RADIO_MUTE						0x0701	//0x0000	Radio left/right mute control. (Property Deprecated) use tuner::radio_mute	
#define RADIO_IBOC_CONTROL				0x0702  //0x0000
#define RADIO_ANTIALIAS_FILTER			0x0705	//0x0000	Configure the audio anti-aliasing 6th LPF ctrls for audio output. (Property Deprecated) use tuner::radio_antialias_filter	
#define RADIO_SQ_OPTIONS				0x0706	//0x1000	Configure audio signal quality options. (Property Deprecated) use tuner::radio_sq_options	

#define I2S_ZIF_OUTPUT					0x0902	//0x0010	Controls ZIF output sources.	
#define I2S_ISTART						0x0903	//0x0000	Sets start point of I path of ZIF test pattern.	
#define I2S_QSTART						0x0904	//0x0000	Sets start point of Q path of ZIF test pattern.	
#define I2S_MCLK_OUTPUT				    0x0906	//0x0000	Controls the MCLK output.

#define AUDIO_DITHER_CONTROL			0x0a00	//0xc03f	Dither enable/disable and Scale control.	

#define FM_SEEK_BAND_BOTTOM				0x2100	//0x222e	Sets the lower seek boundary of the FM band. (Property Deprecated) use tuner::fm_seek_band_bottom	
#define FM_SEEK_BAND_TOP				0x2101	//0x2a26	Sets the upper seek boundary of the FM band. (Property Deprecated) use tuner::fm_seek_band_top	
#define FM_SEEK_SPACING					0x2102	//0x000a	Sets the frequency spacing for the FM band. (Property Deprecated) use tuner::fm_seek_spacing	

#define FM_RSQ_FREQOFF_FILTER			0x2200	//0x0064	Changes the amount that FM FREQOFF is filtered. (Property Deprecated) use tuner::fm_rsq_FREQOFF_filter	
#define FM_RSQ_RSSI_OFFSET				0x2201	//0x0000	Offset FM RSSI. (Property Deprecated) use tuner::fm_rsq_rssi_offset	

#define FM_VALID_TIMER1					0x2300	//0x0006	Sets the amount of time in milliseconds for first timer to disqualify a station based on the metrics associated with this timer. (Property Deprecated) use tuner::fm_valid_timer1	
#define FM_VALID_TIMER1_METRICS			0x2301	//0x0002	Associates metric thresholds with FM_VALID_TIMER1. (Property Deprecated) use tuner::fm_valid_timer1_metrics	
#define FM_VALID_TIMER2					0x2302	//0x000c	Sets the amount of time in milliseconds for second timer to disqualify a station based on the metrics associated with this timer. (Property Deprecated) use tuner::fm_valid_timer2	
#define FM_VALID_TIMER2_METRICS			0x2303	//0x0007	Associates metric thresholds with FM_VALID_TIMER2. (Property Deprecated) use tuner::fm_valid_timer2_metrics	
#define FM_VALID_TIMER3					0x2304	//0x0000	Sets the amount of time in milliseconds for third timer to disqualify a station based on the metrics associated with this timer. (Property Deprecated) use tuner::fm_valid_timer3	
#define FM_VALID_TIMER3_METRICS			0x2305	//0x0000	Associates metric thresholds with FM_VALID_TIMER3. (Property Deprecated) use tuner::fm_valid_timer3_metrics	
#define FM_VALID_AF_TIMER1				0x2307	//0x0002	Sets the amount of time in milliseconds for first timer to disqualify a station based on the metrics associated with this timer. (Property Deprecated) use tuner::fm_valid_af_timer1	
#define FM_VALID_AF_TIMER1_METRICS		0x2308	//0x0006	Associates metric thresholds with FM_VALID_AF_TIMER1. (Property Deprecated) use tuner::fm_valid_af_timer1_metrics	
#define FM_VALID_AF_TIMER2				0x2309	//0x0000	Sets the amount of time in milliseconds for second timer to disqualify a station based on the metrics associated with this timer. (Property Deprecated) use tuner::fm_valid_af_timer2	
#define FM_VALID_AF_TIMER2_METRICS		0x230a	//0x0000	Associates metric thresholds with FM_VALID_AF_TIMER2. (Property Deprecated) use tuner::fm_valid_af_timer2_metrics	
#define FM_VALID_AF_TIMER3				0x230b	//0x0000	Sets the amount of time in milliseconds for third timer to disqualify a station based on the metrics associated with this timer. (Property Deprecated) use tuner::fm_valid_af_timer3	
#define FM_VALID_AF_TIMER3_METRICS		0x230c	//0x0000	Associates metric thresholds with FM_VALID_AF_TIMER3. (Property Deprecated) use tuner::fm_valid_af_timer3_metrics	
#define FM_VALID_AF_RDS_TIME			0x230e	//0x0078	Sets the amount of time in milliseconds to allow the RDS to acquire the PI code. (Property Deprecated) use tuner::fm_valid_af_rds_time	
#define FM_VALID_AF_PI					0x230f	//0x0000	Sets the PI code used for validating an AF check. (Property Deprecated) use tuner::fm_valid_af_pi	
#define FM_VALID_AF_PI_MASK				0x2310	//0xffff	Sets the bit mask for comparing to the PI code. (Property Deprecated) use tuner::fm_valid_af_pi_mask	
#define FM_VALID_FREQOFF_THRESHOLD		0x2311	//0x000f	Maximum frequency error in FM band. (Property Deprecated) use tuner::fm_valid_FREQOFF_threshold	
#define FM_VALID_RSSI_THRESHOLD			0x2312	//0x000c	Sets the RSSI threshold for a valid FM Seek/Tune. (Property Deprecated) use tuner::fm_valid_rssi_threshold	
#define FM_VALID_SNR_THRESHOLD			0x2313	//0x0008	Sets the SNR threshold for a valid FM Seek/Tune. (Property Deprecated) use tuner::fm_valid_snr_threshold	
#define FM_VALID_ISSI_THRESHOLD			0x2314	//0x001e	Sets the ISSI threshold for a valid FM Seek/Tune. (Property Deprecated) use tuner::fm_valid_issi_threshold	
#define FM_VALID_ASSI100_THRESHOLD		0x2315	//0x003c	Sets the ASSI100 threshold for a valid FM Seek/Tune. (Property Deprecated) use tuner::fm_valid_assi100_threshold	
#define FM_VALID_MULTIPATH_THRESHOLD	0x2316	//0x0064	Sets the multipath threshold for a valid FM Seek/Tune. (Property Deprecated) use tuner::fm_valid_multipath_threshold	
#define FM_VALID_ASSI200_THRESHOLD		0x2317	//0x0050	Sets the ASSI200 threshold for a valid FM Seek/Tune. (Property Deprecated) use tuner::fm_valid_assi200_threshold	
#define FM_VALID_USN_THRESHOLD			0x2318	//0x0014	Sets the USN threshold for a valid FM Seek/Tune. (Property Deprecated) use tuner::fm_valid_usn_threshold	
#define FM_VALID_RSSI_ADJ_THRESHOLD		0x231a	//0x000c	Sets the RSSI_ADJ threshold for a valid FM Seek/Tune. (Property Deprecated) use tuner::fm_valid_rssi_adj_threshold	
#define FM_VALID_RSSI_NC_THRESHOLD		0x231b	//0x000c	Sets the RSSI_NC threshold for a valid FM Seek/Tune. (Property Deprecated) use tuner::fm_valid_rssi_nc_threshold	
#define FM_VALID_RSSI_NC_ADJ_THRESHOLD	0x231c	//0x000c	Sets the RSSI_NC_ADJ threshold for a valid FM Seek/Tune. (Property Deprecated) use tuner::fm_valid_rssi_nc_adj_threshold	
#define FM_VALID_USN_WB_THRESHOLD		0x231e	//0x0014	Sets the USN_WB threshold for a valid FM Seek/Tune. (Property Deprecated) use tuner::fm_valid_usn_threshold

#define FM_DEMOD_DE_EMPHASIS			0x2400	//0x0000	Sets the FM de-emphasis time constant. (Property Deprecated) use tuner::fm_demod_de_emphasis	

#define FM_NB_CONTROL					0x2500	//0x0000	Noise blanker control. (Property Deprecated) use tuner::fm_nb_control	
#define FM_NB_AUDIO_DETECTOR_SELECTOR	0x2501	//0x5a5a	Configures thresholds for switching between usn1 input and usn2 input to impulse detector. (Property Deprecated) use tuner::fm_nb_audio_detector_selector	
#define FM_NB_AUDIO_NUM_SAMPLES			0x2502	//0x0402	Set blanking duration. (Property Deprecated) use tuner::fm_nb_audio_num_samples	
#define FM_NB_AUDIO_LPF_THRESHOLD		0x2503	//0x0000	Configures the threshold for switching estimation filters. (Property Deprecated) use tuner::fm_nb_audio_lpf_threshold	
#define FM_NB_AUDIO_LPF_SLOW			0x2504	//0x0186	Slow low pass filter coefficient. (Property Deprecated) use tuner::fm_nb_audio_lpf_slow	
#define FM_NB_AUDIO_LPF_FAST			0x2505	//0x0186	Fast low pass filter coefficient. (Property Deprecated) use tuner::fm_nb_audio_lpf_fast	
#define FM_NB_AGC_BLANKING				0x2507	//0x0013	Defines the number of DSP frames during which the AGC step blanking is enabled. (Property Deprecated) use tuner::fm_nb_agc_blanking	
#define FM_NB_AGC_BLANK_THRSHOLD		0x2508	//0x1080	Defines the threshold for impulse detection used during AGC blank interval. (Property Deprecated) use tuner::fm_nb_agc_blank_threshold	
#define FM_NB_FALSE_DETECTION_THRESHOLD	0x2509	//0x7f00	Used to prevent false firing of the impulse blanker. (Property Deprecated) use tuner::fm_nb_false_detection_threshold	

#define FM_HD_CHANNEL_FILTER			0x2600	//0x0001	Configure bandwidth for HD channel filter. (Property Deprecated) use tuner::fm_hd_channel_filter	

#define FM_CHANBW_CHANBW_CONTROL		0x2900	//0x0001	Configure the FM channel bandwidth control. (Property Deprecated) use tuner::fm_chanbw_chanbw_control	
#define FM_CHANBW_ACI100_THLD			0x2901	//0x000c	+-100kHz interferer power threshold for narrowing channel bandwidth from narrow to ACI setting. (Property Deprecated) use tuner::fm_chanbw_aci100_thld	
#define FM_CHANBW_ACI200_THLD			0x2902	//0x0001	+-200kHz interferer power threshold for narrowing channel bandwidth from wide to narrow. (Property Deprecated) use tuner::fm_chanbw_aci200_thld	
#define FM_CHANBW_ACI_HYST				0x2903	//0x0018	Close-in interferer power hysteresis for widening channel bandwidth. (Property Deprecated) use tuner::fm_chanbw_aci_hyst	
#define FM_CHANBW_ACI_HYST_TIME			0x2904	//0x00c8	Close-in interferer power hysteresis time for changing channel bandwidth. (Property Deprecated) use tuner::fm_chanbw_aci_hyst_time	
#define FM_CHANBW_CHANBW_ACI			0x2905	//0x0023	Adjacent interferer (ACI) channel bandwidth value. (Property Deprecated) use tuner::fm_chanbw_chanbw_aci	
#define FM_CHANBW_CHANBW_OVRD			0x2906	//0x0064	Channel bandwidth override value. (Property Deprecated) use tuner::fm_chanbw_chanbw_ovrd	
#define FM_CHANBW_BWSPEED				0x2907	//0x7fff	Channel bandwidth control loop speed. Larger values are faster. (Property Deprecated) use tuner::fm_chanbw_bwspeed	
#define FM_CHANBW_CHANBW_MAX			0x2908	//0x0096	Maximum channel bandwidth value. (Property Deprecated) use tuner::fm_chanbw_chanbw_max	

#define FM_RDS_INTERRUPT_SOURCE			0x2a00	//0x0000	Configure RDS interrupts. (Property Deprecated) use tuner::fm_rds_interrupt_source	
#define FM_RDS_INTERRUPT_FIFO_COUNT		0x2a01	//0x0000	Configures minimum received data groups in fifo before interrupt. (Property Deprecated) use tuner::fm_rds_interrupt_fifo_count	
#define FM_RDS_CONFIG					0x2a02	//0x0000	Enables RDS and configures acceptable block error threshold. (Property Deprecated) use tuner::fm_rds_config	
#define FM_RDS_CONFIDENCE				0x2a03	//0x1111	Configures rds block confidence threshold. (Property Deprecated) use tuner::fm_rds_confidence	
#define FM_RDS_CHANNEL_BW				0x2a04	//0x645f	Sets channel bandwidth limit for RDS. (Property Deprecated) use tuner::fm_rds_channel_bw	

#define FM_DCNR_CONTROL					0x2c00	//0xa023	Control the dynamic channel noise abatement. (Property Deprecated) use tuner::fm_dcnr_control	
#define FM_DCNR_HICUT_BRIGHTNESS		0x2c01	//0x5200	DCNR brightness control for hicut/hiblend second stage. (Property Deprecated) use tuner::fm_dcnr_hicut_brightness	
#define FM_DCNR_HICUT_DEPTH				0x2c03	//0x0000	DCNR depth control for hicut/hiblend second stage. (Property Deprecated) use tuner::fm_dcnr_hicut_depth	

#define FM_LIMITER_SCALE				0x2d00	//0x0037	FM pop limiter scale factor for RMS limit. (Property Deprecated) use tuner::fm_limiter_scale	
#define FM_LIMITER_BLENDING_MODE		0x2d02	//0x0005	Configure different modes of operation of pop filters. (Property Deprecated) use tuner::fm_limiter_blending_mode	

#define FM_VICS_CONFIG					0x2e00	//0x0000	Configure mode of operation for VICS. (Property Deprecated) use tuner::fm_vics_config	
#define FM_VICS_SCHMITT_THRESHOLD		0x2e01	//0x0000	Configure hysteresis threshold settings for schmitt trigger. (Property Deprecated) use tuner::fm_vics_schmitt_threshold	

#define AM_SEEK_BAND_BOTTOM				0x4100	//0x0208	Sets the lower seek boundary of the AM band. (Property Deprecated) use tuner::am_seek_band_bottom	
#define AM_SEEK_BAND_TOP				0x4101	//0x06ae	Sets the upper seek boundary of the AM band. (Property Deprecated) use tuner::am_seek_band_top	
#define AM_SEEK_SPACING					0x4102	//0x000a	Sets the frequency spacing for the AM band. (Property Deprecated) use tuner::am_seek_spacing	

#define AM_RSQ_FREQOFF_FILTER			0x4200	//0x0064	Changes the amount that AM FREQOFF is filtered. (Property Deprecated) use tuner::am_rsq_FREQOFF_filter	
#define AM_RSQ_RSSI_OFFSET				0x4201	//0x0000	Offset AM RSSI. (Property Deprecated) use tuner::am_rsq_rssi_offset	
#define AM_RSQ_HD_DETECTION				0x4202	//0x0020	Configures the Fast AM HD Detection routine. (Property Deprecated) use tuner::am_rsq_hd_detection	
#define AM_RSQ_FILTER_TIME_CONSTANT		0x4203	//0x0000	Configures the Fast AM HD Detection routine. (Property Deprecated) use tuner::am_rsq_filter_time_constant

#define AM_VALID_TIMER1					0x4300	//0x0006	Sets the amount of time in milliseconds for first timer to disqualify a station based on the metrics associated with this timer. (Property Deprecated) use tuner::am_valid_timer1	
#define AM_VALID_TIMER1_METRICS			0x4301	//0x0002	Associates metric thresholds with AM_VALID_TIMER1. (Property Deprecated) use tuner::am_valid_timer1_metrics	
#define AM_VALID_TIMER2					0x4302	//0x0026	Sets the amount of time in milliseconds for second timer to disqualify a station based on the metrics associated with this timer. (Property Deprecated) use tuner::am_valid_timer2	
#define AM_VALID_TIMER2_METRICS			0x4303	//0x0007	Associates metric thresholds with AM_VALID_TIMER2. (Property Deprecated) use tuner::am_valid_timer2_metrics	
#define AM_VALID_TIMER3					0x4304	//0x0000	Sets the amount of time in milliseconds for third timer to disqualify a station based on the metrics associated with this timer. (Property Deprecated) use tuner::am_valid_timer3	
#define AM_VALID_TIMER3_METRICS			0x4305	//0x0007	Associates metric thresholds with AM_VALID_TIMER3. (Property Deprecated) use tuner::am_valid_timer3_metrics	
#define AM_VALID_FREQOFF_THRESHOLD		0x4306	//0x000f	Maximum frequency error in AM band. (Property Deprecated) use tuner::am_valid_FREQOFF_threshold	
#define AM_VALID_RSSI_THRESHOLD			0x4307	//0x000a	Sets the RSSI threshold for a valid AM Seek/Tune. (Property Deprecated) use tuner::am_valid_rssi_threshold	
#define AM_VALID_SNR_THRESHOLD			0x4308	//0x0005	Sets the SNR threshold for a valid AM Seek/Tune. (Property Deprecated) use tuner::am_valid_snr_threshold	
#define AM_VALID_ISSI_THRESHOLD			0x4309	//0x007f	Sets the ISSI threshold for a valid AM Seek/Tune. (Property Deprecated) use tuner::am_valid_issi_threshold	
#define AM_VALID_ASSI_THRESHOLD			0x430a	//0x0050	Sets the ASSI threshold for a valid AM Seek/Tune. (Property Deprecated) use tuner::am_valid_assi_threshold	
#define AM_VALID_HDLEVEL_THRESHOLD		0x430b	//0x001e	Sets the HDLEVEL threshold for a valid AM Seek/Tune. (Property Deprecated) use tuner::am_valid_hdlevel_threshold	
#define AM_VALID_RSSI_ADJ_THRESHOLD		0x430c	//0x000a	Sets the RSSI_ADJ threshold for a valid AM Seek/Tune. (Property Deprecated) use tuner::am_valid_rssi_adj_threshold	
#define AM_VALID_RSSI_NC_THRESHOLD		0x430d	//0x000a	Sets the RSSI_NC threshold for a valid AM Seek/Tune. (Property Deprecated) use tuner::am_valid_rssi_nc_threshold	
#define AM_VALID_RSSI_NC_ADJ_THRESHOLD	0x430e	//0x000a	Sets the RSSI_NC_ADJ threshold for a valid AM Seek/Tune. (Property Deprecated) use tuner::am_valid_rssi_nc_adj_threshold	
#define AM_VALID_FILTERED_HDLEVEL_THRESHOLD	0x430f	//0x001e	Sets the FILTERED_HDLEVEL threshold for a valid AM Seek/Tune. (Property Deprecated) use tuner::am_valid_filtered_hdlevel_threshold

#define AM_DEMOD_OPTIONS				0x4400	//0x1402	Selects the method of AM demodulation. (Property Deprecated) use tuner::am_demod_options	
#define AM_DEMOD_PLL_MAX_FREQ_RANGE		0x4401	//0x0073	Configures AM PLL Freq range (max limit). (Property Deprecated) use tuner::am_demod_pll_max_freq_change	
#define AM_DEMOD_PLL_MIN_FREQ_RANGE		0x4402	//0xff8d	Configures AM PLL Freq range (min limit). (Property Deprecated) use tuner::am_demod_pll_min_freq_range	

#define AM_NB_OPTIONS					0x4500	//0x0018	AM noise blanker options. (Property Deprecated) use tuner::am_nb_options	
#define AM_NB_IQ1_SETTINGS				0x4501	//0x050f	Sets the number of samples and delay for the AM I/Q #1 noise blanker. (Property Deprecated) use tuner::am_nb_iq1_settings	
#define AM_NB_IQ2_SETTINGS				0x4502	//0x000a	Sets the number of samples and delay for the AM I/Q #2 noise blanker. (Property Deprecated) use tuner::am_nb_iq2_settings	
#define AM_NB_IQ3_SETTINGS				0x4503	//0x0808	Sets the number of samples and delay for the AM I/Q #3 noise blanker. (Property Deprecated) use tuner::am_nb_iq3_settings	
#define AM_NB_AUDIO_SETTINGS			0x4504	//0x000f	Sets the number of samples and delay for the AM audio noise blanker. (Property Deprecated) use tuner::am_nb_audio_settings	

#define AM_AVC_OPTIONS					0x4a00	//0x0000	Configure various AVC options. (Property Deprecated) use tuner::am_avc_options	
#define AM_AVC_REF						0x4a01	//0xf206	Sets the desired internal carrier reference level. (Property Deprecated) use tuner::am_avc_ref	

#define AM_AFC_MODE						0x4b00	//0x0000	Configure wide-band AM AFC mode. (Property Deprecated) use tuner::am_afc_mode	
#define AM_AFC_LOOP_GAIN				0x4b01	//0x4000	Configures AM AFC loop gain. (Property Deprecated) use tuner::am_afc_loop_gain	
#define AM_AFC_FREQ_RANGE				0x4b02	//0x0073	Configures the AM AFC Freq range. (Property Deprecated) use tuner::am_afc_freq_range

//si4796x
#define FRONTEND_TUNER0_ANTENNA_SELECT	0x0605
#define FRONTEND_TUNER1_ANTENNA_SELECT	0x0606

#define EZIQ_CONFIG         0x0e00
#define EZIQ_FIXED_SETTINGS 0x0e01
#define EZIQ_OUTPUT_SOURCE  0x0e02
#define EZIQ_DAB_CONFIG0    0x0e07
#define EZIQ_DAB_CONFIG1    0x0e08
#endif
