/***************************************************************************************
                  Skyworks MSS Automotive Tuner + Demod SDK

   EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND CONDITIONS OF
     THE SOFTWARE LICENSE AGREEMENT IN THE DOCUMENTATION FILE CORRESPONDING
     TO THIS SOURCE FILE.
   IF YOU DO NOT AGREE TO THE LIMITED LICENSE AND CONDITIONS OF SUCH AGREEMENT,
     PLEASE RETURN ALL SOURCE FILES TO SKYWORKS.

  DAB implmentation helper functions.
   FILE: DAB_common.h
   Date: June 28, 2016
  (C) Copyright 2015, Skyworks, Inc. All rights reserved.
****************************************************************************************/
#ifndef DAB_COMMON_H_
#define DAB_COMMON_H_

#include "DAB_service_list_handler.h"
#include "feature_types.h"
#include "si461x_core.h"
#include "type.h"

typedef struct dab_metrics dab_metrics;
typedef struct dab_demod_metrics dab_demod_metrics;
// typedef struct sfc_handle sfc_handle;
typedef struct dab_system_data dab_system_data;
typedef struct dab_tune_status dab_tune_status;

struct dab_tune_status {
    bool_t tune_complete;
    bool_t demod_acquired;
};

typedef struct _DAB_DEMOD_dab_get_audio_info__data {
    uint8_t PBER_VALID;
    uint8_t DEC_VAILD;
    uint16_t AUDIO_BIT_RATE;
    uint16_t AUDIO_SAMPLE_RATE;
    uint8_t AUDIO_XPAD_IND;
    uint8_t AUDIO_PS_FLAG;
    uint8_t AUDIO_SBR;
    uint8_t AUDIO_MODE;
    uint8_t AUDIO_DRC_GAIN;
    uint32_t PSEUDO_BER_BIT_CNT;
    uint32_t PSEUDO_BER_ERR_CNT;
} DAB_DEMOD_dab_get_audio_info__data;

struct dab_demod_metrics {
    // These could be trimmed down to only metrics we actually want  to track.
    dab_demod_digrad_status_t digrad;
    DAB_DEMOD_dab_get_audio_info__data audio;
    demod_dab_event_status_t event;
};

struct dab_metrics {
    dab_tuner_metrics tuner;
    dab_demod_metrics demod;

    // overall 'quality' (0,100), derived
    uint8_t quality;

    // none selected = 0xFF
    uint8_t current_svc_svclist_index;
};

struct dab_system_data {
    uint8_t browse_index;
    bool_t only_browse_favorites;

    dab_service_info current;

    // metrics for current service
    dab_metrics metrics;
#ifdef SUPPORT_DAB_DLS
    dab_dls_string dls_decoder;
#endif

    // Service List - possibly shared between multiple DAB systems
    // dab_service_list* svc_list;
};

extern dab_system_data xdata g_dab_state;

#define DAB_DEFAULT_FREQ_LIST_SIZE 41
extern uint32_t code dab_default_freq_list[];

// Similar to DAB_start_service_worker, with different argument format.
typedef struct _dab_start_service_info {
    uint32_t service_id;
    uint16_t component_id;
    uint32_t freq;
} dab_start_service_info;

RETURN_CODE DAB_start_service(dab_service_info* p_svc);

extern dab_service_list xdata g_dab_svc_list;
#endif // !DAB_COMMON_H_
