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

#ifndef _SI479X_CORE_H_
#define _SI479X_CORE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "si479x_commanddefs.h"
#include "si479x_propertydefs.h"
#include "type.h"

#define MAX_SERIAL_NUM_LENGTH 15

#define FM_MAX_METRICS 34
#define FM_MAX_SERVOS 12
#define AM_MAX_METRICS 26
#define AM_MAX_SERVOS 13

#define COMMAND_OFFSET 4
#define TUNER0 0
#define TUNER1 1

void _swap_16(uint16_t* pValue);
void _swap_32(uint32_t* pValue);

typedef uint8_t RADIO_STATE;
enum {
    BOOTLOADER,
    APPLICATION,
    UNKNOWN,
};

typedef uint8_t CLK_MODE_T;
enum {
    OFF,
    XTAL,
    SINGLE,
    SINGLE_AC,
    SINGLE_DC,
    DIFFERENTIAL
};

typedef struct _power_up_args_t {
    uint8_t CTSIEN;
    uint8_t CLKO_CURRENT;
    uint8_t VIO;
    uint8_t CLKOUT;
    CLK_MODE_T CLK_MODE;
    uint8_t TR_SIZE;
    uint8_t CTUN;
    uint32_t XTAL_FREQ; // crystal clock
    uint8_t IBIAS; // only for crytal oscillator
    uint8_t AFS; // 0: 48kS/s 1:44.1kS/s
    uint8_t DIGDAT; // 0: HD 1: DAB
    uint8_t CHIPID;
    uint8_t ISFSMODE;

    uint8_t ICLINK;

    uint8_t IQCHANNELS;
    uint8_t IQFMT;
    uint8_t IQMUX;
    uint8_t IQRATE;

    uint8_t IQSWAP1;
    uint8_t IQSWAP0;
    uint8_t IQSLOT;
    uint8_t IQEARLY;
    uint8_t IQOUT;
} power_up_args_t;

typedef uint8_t POWERUP_STATE;
enum {
    POWERUP_RESET = 0x01,
    POWERUP_BOOTLOADER = 0x02,
    POWERUP_BOOTREADY = 0x04,
    POWERUP_APPLICATION = 0x08,
};

typedef struct _load_config_t {
    uint8_t numchips;
    uint8_t chipid;
    uint8_t role;
    uint8_t icn_in; // FLOWCTRLIN bits
    uint8_t nvmosi_out; // FLOWCTRLOUT bits

    uint32_t clksel8;
    uint32_t spicfg3;
    uint32_t drvdel;
    uint32_t capdel;
    uint32_t spisctrl;
    uint32_t nvsclk;

    uint32_t nvssb;
    uint32_t nvmosi;
    uint32_t nvmiso;
    uint32_t diclk0;
    uint32_t difs0;
    uint32_t icn;
    uint32_t icp;

    uint32_t debugp;
    uint32_t debugn;
    uint32_t isfsp;
    uint32_t isfsn;
    uint32_t zifiout;
    uint32_t zifclk;

    uint32_t zifqout;
    uint32_t ziffs;
} load_config_t;

/**
 *   @breif part info structure
 *
 *   This structure includes the tuner part information
 *
 */
typedef struct _part_info_t {
    uint8_t chipRW; ///< chip revision
    uint8_t ROMId; /// ROM ID
    uint16_t PartNum; ///< part number eg. 21
    char Major; ///< FW major eg. A
} part_info_t;

/**
 *   @breif function info structure for si479xx
 *
 *   This structure includes the tuner function information, you can fetch the patch id by structure
 *
 */
typedef struct func_info_t {

    uint8_t VERINFO[5]; ///< version
    uint8_t HIFIINFO[4]; ///< hifi
    uint8_t CUSTINFO[4]; ///< customer
    uint8_t SVNINFO; ///< svn info
    uint32_t SVNID; ///< svn id
} func_info_t;

RET_CODE si479x_checkPowerupStatus(POWERUP_STATE state);

RET_CODE si479x_waitForCTS();

RET_CODE si479x_waitForSTC(uint8_t tuner_id);

RET_CODE si479x_get_part_info(part_info_t* p_part_info);

RET_CODE si479x_get_func_info(func_info_t* p_si479x_func_info);

/**
 *   @breif command sent to tuner
 *
 *   The command sent to tuner is by i2c interface. Refer the tuner document to know the expected data reply.
 *
 *   @param cmd_size: the command data size to sent to tuner.
 *   @param cmd: the sent command data.
 *   @param reply_size: the reply data expect to read from tuner.
 *   @param reply: the reply data buffer.
 *
 *   @retval RET_CODE
 */
RET_CODE si479x_command(uint16_t cmd_size, uint8_t* cmd, uint16_t reply_size, uint8_t* reply);

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
RET_CODE si479x_set_property(uint16_t propNumber, uint16_t propValue);

/**
 *   @breif Power up function.
 *
 *   After call this function, the tuner should start to work.
 *
 *   @param band: The band type, BAND_FM or BAND_AM.
 *
 *   @retval void
 */
RET_CODE si479x_powerup(const power_up_args_t* pPowerup_args);

RET_CODE si479x_loader_init(uint8_t guid, uint32_t _guid_table[]);

RET_CODE si479x_load_config(load_config_t* p_load_config_t);

RET_CODE si479x_boot();

typedef struct _phase_div_status_t {
    uint8_t audio;
    uint8_t comb;
    uint8_t crosscorr;
} phase_div_status_t;

RET_CODE si479x_fm_phase_div(uint8_t audio_output, uint8_t combile_mode, int8_t weight);

RET_CODE si479x_fm_phase_div_status(phase_div_status_t* p_phase_div_status);

RET_CODE si479x_fm_phase_div_datalist(uint8_t length, uint8_t datalist[]);

RET_CODE si479x_agc_stauts_datalist(uint8_t length, uint8_t datalist[]);

uint8_t si479x_agc(uint8_t agcIndex);

RET_CODE si479x_agc_debug_set_property(uint16_t prop_id, uint16_t prop_value);

RET_CODE si479x_dab_set_freq_list(uint8_t count, uint32_t* freqlist);

#ifdef __cplusplus
}
#endif

#endif
