/***************************************************************************************
                  Skyworks MSS Automotive Tuner + Demod SDK

   EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND CONDITIONS OF
     THE SOFTWARE LICENSE AGREEMENT IN THE DOCUMENTATION FILE CORRESPONDING
     TO THIS SOURCE FILE.
   IF YOU DO NOT AGREE TO THE LIMITED LICENSE AND CONDITIONS OF SUCH AGREEMENT,
     PLEASE RETURN ALL SOURCE FILES TO SKYWORKS.

   SDK DAB service list decoder header - decodes the available services and adds them
   to the overall service database.

   FILE: DAB_Service_List_Handler.h
   Supported IC : Si479xx + Si461x
   Date: September 21, 2015
  (C) Copyright 2015, Skyworks, Inc. All rights reserved.
****************************************************************************************/

#ifndef DAB_SERVICE_LIST_HANDLER__
#define DAB_SERVICE_LIST_HANDLER__

#include "type.h"
#include "feature_types.h"
#include "si461x_core.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------
//Note: this needs to change if we ever change the format for DAB list storage
// so the persistant storage read will know to ignore anything which is of an older version.
#define DAB_SERVICE_LIST_OBJECT_VERSION 1

#define DAB_LIST_VERSION_UNKNOWN 0xFFFF

//-----------------------------------------------------------------------------
// Interface type definitions
//-----------------------------------------------------------------------------
typedef struct dab_service_list_element {
    dab_service_info info;

    DIGITAL_COMPONENT_TYPE component_type;
    PROGRAM_TYPE program_type;
    dab_service_label service_name;
}dab_service_list_element;

typedef struct dab_freq {
    // uint8_t index;
    uint32_t freq_hz;
    uint8_t cnr;
}dab_freq;

typedef struct dab_ensemble_info {
    // Offset into services[]
    uint8_t offset;
    // duplicated so we don't have to check a service[] each time
    uint16_t eid;

    // Frequency information if there are multiple broadcasts.
    uint8_t freq_count;
    struct dab_freq freqs[DAB_SERVICE_LIST__MAX_NUMBER_OF_LINKED_FREQUENCIES];
}dab_ensemble_info;

typedef struct dab_service_list {
    // Version of the storage list type layout (for saving/loading from memory)
    uint8_t list_storage_version;

    // Service list version #
    uint16_t service_list_version;

    // Flag checking if service list has been updated for current ensemble.
    bool_t list_updated_for_freq;

    uint8_t service_count;
    dab_service_list_element services[DAB_SERVICE_LIST__MAX_SERVICES];

    // These are offsets into services[].
    // This requires some handling to keep these two arrays always synced.
    uint8_t ensemble_count;
    dab_ensemble_info ensembles[DAB_SERVICE_LIST__MAX_ENSEMBLES_FOR_LIST];
}dab_service_list;

//-----------------------------------------------------------------------------
// Function prototypes
//-----------------------------------------------------------------------------
void dabsl_init_list(dab_service_list* list);

// This will search the list for the matching service.
// start_svc_addr_index is the starting point of the search.
//	- input 0 to start at the beginning of the list, input the current index + 1 to find alternates
// service was found if 'found_svc_addr_index' >= 0, o.w. the service was not found in the list.
// - if found_svc == NULL, it will be ignored
//		o.w. it will be set to the service at found_svc_addr_index
RETURN_CODE dabsl_find_service(uint32_t service_id,
    /**bool_t match_component,*/ uint16_t component_id,
    /**uint8_t start_svc_addr_index,*/ int16_t* found_svc_addr_index/**, dab_service_list_element** found_svc*/);

// If the ensemble for freq_index was not found : ensemble_addr_index  will be < 0 (invalid),
//  o.w.  ensemble_addr_index will be valid.
RETURN_CODE dabsl_find_ensemble(uint16_t ensemble_id, int16_t* ensemble_index);
RETURN_CODE dabsl_find_ensemble_at_freq(dab_service_list* list, uint32_t freq, int16_t* ensemble_index);

dab_ensemble_info* dabsl_get_ensemble(dab_service_list* sl, uint8_t service_index);
dab_ensemble_info* dabsl_get_ensemble_from_service(dab_service_list* sl, dab_service_info service);

RETURN_CODE dabsl_update_list(demo_digital_service_list_t* list_data, uint16_t list_version,
    dab_freq *freq, uint16_t ensemble_id);

// Add all matching SID+CID entries from the service list to 'candidate' list.
// Only appends to candidates, does not overwrite.
void dabsl_find_matching_services(uint8_t svc_index,
    uint8_t max, uint8_t* candidates, uint8_t* candidate_count);

// Add the services' ensemble known frequencies' indices to 'candidate' list.
// Only appends to candidates, does not overwrite.
void dabsl_find_candidate_freqs(uint8_t svc_index,
    uint8_t max, uint32_t* candidates, uint8_t* candidate_count);

// Uses dabsl_find_matching_services() to get a list of matching (hard-linked SID==SID) services.
// Then uses dabsl_find_candidate_freqs() to create find the candidate frequencies for these services.
// Only appends to links, does not overwrite.
void dabsl_get_linked(uint8_t selected_svc_index,
    uint8_t max, dab_service_info* links, uint8_t* link_count);

void dab_service_info_copy(dab_service_info* s1, dab_service_info* s2);

#endif
