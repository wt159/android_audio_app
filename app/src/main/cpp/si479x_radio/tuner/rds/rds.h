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
 *@file this header file declear the core function for si47xx headunit.
 *
 * @author Michael.Yi@silabs.com
 */

#ifndef _RDS_H_
#define _RDS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "type.h"

#define RDS_TYPE_0A (0 * 2 + 0)
#define RDS_TYPE_0B (0 * 2 + 1)
#define RDS_TYPE_1A (1 * 2 + 0)
#define RDS_TYPE_1B (1 * 2 + 1)
#define RDS_TYPE_2A (2 * 2 + 0)
#define RDS_TYPE_2B (2 * 2 + 1)
#define RDS_TYPE_3A (3 * 2 + 0)
#define RDS_TYPE_3B (3 * 2 + 1)
#define RDS_TYPE_4A (4 * 2 + 0)
#define RDS_TYPE_4B (4 * 2 + 1)
#define RDS_TYPE_5A (5 * 2 + 0)
#define RDS_TYPE_5B (5 * 2 + 1)
#define RDS_TYPE_6A (6 * 2 + 0)
#define RDS_TYPE_6B (6 * 2 + 1)
#define RDS_TYPE_7A (7 * 2 + 0)
#define RDS_TYPE_7B (7 * 2 + 1)
#define RDS_TYPE_8A (8 * 2 + 0)
#define RDS_TYPE_8B (8 * 2 + 1)
#define RDS_TYPE_9A (9 * 2 + 0)
#define RDS_TYPE_9B (9 * 2 + 1)
#define RDS_TYPE_10A (10 * 2 + 0)
#define RDS_TYPE_10B (10 * 2 + 1)
#define RDS_TYPE_11A (11 * 2 + 0)
#define RDS_TYPE_11B (11 * 2 + 1)
#define RDS_TYPE_12A (12 * 2 + 0)
#define RDS_TYPE_12B (12 * 2 + 1)
#define RDS_TYPE_13A (13 * 2 + 0)
#define RDS_TYPE_13B (13 * 2 + 1)
#define RDS_TYPE_14A (14 * 2 + 0)
#define RDS_TYPE_14B (14 * 2 + 1)
#define RDS_TYPE_15A (15 * 2 + 0)
#define RDS_TYPE_15B (15 * 2 + 1)

typedef uint8_t BLER;
enum {
    BLER_NONE,
    BLER_COUPLE,
    BLER_SOME,
    BLER_ALOT
};

typedef struct _rds_status_t {
    uint8_t ints;
    uint8_t flags;
    uint8_t tp;
    uint8_t pty;
    uint16_t pi;
    uint8_t fifoused;
    BLER blera;
    BLER blerb;
    BLER blerc;
    BLER blerd;
    uint16_t blocka;
    uint16_t blockb;
    uint16_t blockc;
    uint16_t blockd;
} rds_status_t;

#if 0
#define RDS_BLOCKA_TOLERRENCE BLER_SOME
#define RDS_BLOCKB_TOLERRENCE BLER_COUPLE
#define RDS_BLOCKC_TOLERRENCE BLER_SOME
#define RDS_BLOCKD_TOLERRENCE BLER_SOME
#else
#define RDS_BLOCKA_TOLERRENCE BLER_NONE
#define RDS_BLOCKB_TOLERRENCE BLER_SOME
#define RDS_BLOCKC_TOLERRENCE BLER_NONE
#define RDS_BLOCKD_TOLERRENCE BLER_NONE
#endif

#define PS_VALIDATE_LIMIT 1
#define RT_VALIDATE_LIMIT 2

#define PIVALID_MASK 0x08
#define TPPTYVALID_MASK 0x10

#define AF_COUNT_MIN 225
#define AF_COUNT_MAX (AF_COUNT_MIN + 25)
#define AF_FREQ_MIN 0
#define AF_FREQ_MAX 204
#define AF_FREQ_TO_U16F(freq) (8750 + ((freq - AF_FREQ_MIN) * 10))

typedef struct _rds_ps_helper_t {
    uint8_t ps_high[8]; // Temporary PS text (high probability)
    uint8_t ps_low[8]; // Temporary PS text (low probability)
    uint8_t ps_cnt[8]; // Hit count of high probability PS text
} rds_ps_helper_t;

typedef struct _rds_rt_helper_t {
    uint8_t ab_flag;
    uint8_t ab_flag_valid;
    uint8_t rt_high[64]; // Temporary Radio Text (high probability)
    uint8_t rt_low[64]; // Temporary Radio text (low probability)
    uint8_t rt_cnt[64]; // Hit count of high probabiltiy Radio text
} rds_rt_helper_t;

typedef struct _rds_time_t {
    uint8_t day_high; // Modified Julian Day high bit
    uint16_t day_low; // Modified Julian Day low 16 bits
    uint8_t hour; // Hour
    uint8_t minute; // Minute
    int8_t offset; // Local Time Offset from UTC
} rds_time_t;

typedef struct _rds_data_t {
    uint16_t pi;

    uint8_t ta;

    uint8_t pty;

    uint8_t ps[8];
    uint8_t ps_valid;

    uint8_t rt[64];
    uint8_t rt_valid;

    uint8_t af_index;
    uint8_t af_check_times;
    uint8_t af_count;
    uint16_t af_list[25];
} rds_data_t;

void rds_process(void);

void rds_reset();

uint8_t rds_rt_available();

uint8_t rds_ps_available();

uint8_t* rds_ps_data();

uint8_t* rds_rt_data();

uint16_t rds_af_check();

uint8_t rds_af_tune(uint16_t freq);

#ifdef __cplusplus
}
#endif
#endif
