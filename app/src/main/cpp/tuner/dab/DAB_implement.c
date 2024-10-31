/***************************************************************************************
                  Skyworks MSS Automotive Tuner + Demod SDK

   EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND CONDITIONS OF
     THE SOFTWARE LICENSE AGREEMENT IN THE DOCUMENTATION FILE CORRESPONDING
     TO THIS SOURCE FILE.
   IF YOU DO NOT AGREE TO THE LIMITED LICENSE AND CONDITIONS OF SUCH AGREEMENT,
     PLEASE RETURN ALL SOURCE FILES TO SKYWORKS.

  Implementation of IFirmwareApi for an FMHD, AMHD, DAB capable board.
   FILE: Implementation_DAB.cpp
   Date:  June 28, 2016
  (C) Copyright 2015, Skyworks, Inc. All rights reserved.
****************************************************************************************/

#include "DAB_implement.h"

#include<ctype.h>
#include<stdio.h>
#include <string.h>

#include "dc_hal.h"

#include "si461x_dab_api_constants.h"
#include "si479xx_tuner_api_constants.h"

#include "c_util.h"
#include "c_core.h"
#include "si479x_tuner.h"

#include "DAB_nvm_storage.h"

#ifdef SUPPORT_DAB_DLS	
#include "DAB_DLS_handler.h"
#endif

dab_system_data xdata g_dab_state;
dab_service_list xdata g_dab_svc_list;
dab_service_list_element xdata g_dab_svc_list_elem;

cb_dab_scan xdata g_cb; //{ .bg = FALSE, };

uint32_t code dab_default_freq_list[] = {
    174928000,
    176640000,
    178352000,
    180064000,
    181936000,
    183648000,
    185360000,
    187072000,
    188928000,
    190640000,
    192352000,
    194064000,
    195936000,
    197648000,
    199360000,
    201072000,
    202928000,
    204640000,
    206352000,
    208064000,
    209936000,
    210096000,
    211648000,
    213360000,
    215072000,
    216928000,
    217088000,
    218640000,
    220352000,
    222064000,
    223936000,
    224096000,
    225648000,
    227360000,
    229072000,
    230784000,
    232496000,
    234208000,
    235776000,
    237488000,
    239200000,
};

void empty_dab_metrics_tuner(dab_tuner_metrics* met)
{
    met->freq_hz = 174928000;
    met->sqi = 0;
    met->rssi = 0;
    met->rssi_adj = 0;
    met->dab_detect = 0;
    met->valid = 0;
}

void empty_dab_metrics_demod(dab_demod_metrics* met)
{
    memset(met, 0, sizeof(dab_demod_metrics));
}

void empty_dab_metrics(dab_metrics* metrics)
{
    empty_dab_metrics_tuner(&metrics->tuner);
    empty_dab_metrics_demod(&metrics->demod);

    metrics->current_svc_svclist_index = DAB_METRIC_AUDIO_SERVICE_LIST__INDEX_NOT_SET;
}



void dab_system_init(dab_system_data* d)
{
#ifdef SUPPORT_DAB_DLS	
    dls_init(&d->dls_decoder);
#endif	
    empty_dab_metrics(&d->metrics);
	
	g_cb.bg = FALSE;
}



void DAB_init()
{
	dabsl_init_list(&g_dab_svc_list);
	
	dab_system_init(&g_dab_state);
		
	LoadServiceList_DAB();
}

static void copy_dab_tuner_metrics(dab_tuner_metrics* tuner, tuner_dab_metrics_t* rsq)
{
    if (!tuner || !rsq)
        return;

    tuner->sqi = rsq->SQI;
    tuner->rssi = rsq->RSSI;
    tuner->valid = rsq->VALID;
    tuner->dab_detect = rsq->DABDETECT;
    tuner->freq_hz = rsq->FREQ;
}

void copy_dab_metrics(dab_metrics* dab, dab_demod_digrad_status_t* digrad, tuner_dab_metrics_t* rsq)
{
    if (!dab)
        return;

    if (rsq)
        copy_dab_tuner_metrics(&dab->tuner, rsq);

    if (digrad)
        dab->demod.digrad = *digrad;
}

RETURN_CODE DAB_tune(uint32_t freq)
{
    RETURN_CODE xdata ret = SUCCESS;

#ifdef OPTION__DAB_USE_FAST_DETECT
    bool_t xdata fast_detect = false;
    fast_detect = true;
#endif

    //Tune with DAB_Detect enabled for Mode 1
    ret |= si479x_tuner_dab_tune( TUNER0, 3, freq); 

    empty_dab_metrics(&g_dab_state.metrics);
	
    if (ret == SUCCESS) {
        g_dab_state.current.freq = freq;
        g_dab_svc_list.list_updated_for_freq = false;
    }

    return ret;
}


RETURN_CODE DAB_confirm_tune_complete(bool_t full_check, uint8_t* dab_detect, dab_metrics* metrics)
{
    RETURN_CODE xdata ret = SUCCESS;

    tuner_dab_metrics_t xdata dab_rsq = { 0 };
	
	ret |= si479x_tuner_dab_rsq_status( TUNER0, false, false, false, &dab_rsq);
    if (ret != SUCCESS)
        return ret;

    if (full_check) {
        ret |= si479x_tuner_dab_rsq_status( TUNER0, false, false, false, &dab_rsq);

        if (ret == SUCCESS) {
            if (dab_detect)
                *dab_detect = dab_rsq.DABDETECT;

            if (metrics)
                copy_dab_metrics(metrics, NULL, &dab_rsq);

#ifdef OPTION__DAB_USE_FAST_DETECT
            if (!((dab_rsq.DABDETECT > 0) && (dab_rsq.VALID > 0))) {
                // Bad freq
                return TIMEOUT;
            }
#else
            if (dab_rsq.VALID == 0) {
                return TIMEOUT;
            }
#endif
        }
    }

    return ret;
}

RETURN_CODE DAB_tune_chips(uint32_t freq, dab_tune_status* status)
{
    RETURN_CODE xdata ret = SUCCESS;

    dab_metrics* xdata metrics = &(g_dab_state.metrics);
    // Tune primary to new station.
    uint8_t xdata dab_detect = 0;
	
    if (status) {
        status->tune_complete = false;
        status->demod_acquired = false;
    };
    ret |= DAB_tune(freq);
    ret |= DAB_confirm_tune_complete(true, &dab_detect, metrics);
    if (ret != SUCCESS) {
        return ret;
    }

    if (ret == SUCCESS && status)
        status->tune_complete = true;

    return ret;
}


static void update_ensemble_freq_cnr(uint32_t freq, uint8_t cnr)
{
	dab_service_list* xdata sl = &g_dab_svc_list;
	
	uint8_t xdata i, j;
    for (i = 0; i < sl->ensemble_count; i++) {
        dab_ensemble_info* xdata ens = &sl->ensembles[i];
        for (j = 0; j < ens->freq_count; j++) {
            if (ens->freqs[j].freq_hz == freq) {
                ens->freqs[j].cnr = cnr;
            }
        }
    }
}

RETURN_CODE DAB_acquire()
{
	return si461x_demod_dab_acquire(0);
}


RETURN_CODE DAB_confirm_acquisition_complete(bool_t full_check, dab_metrics* metrics, uint16_t* eid)
{
    RETURN_CODE xdata ret = SUCCESS;
    // Check ACQ once STC set
    dab_demod_digrad_status_t xdata dab_ds = { 0 };

    if (metrics && &g_cb) {
        g_cb.freq = metrics->tuner.freq_hz;
    }

    ret = si461x_demod_dab_digrad_status(false, false, false, false, false, &dab_ds);
    if (ret != SUCCESS)
        return ret;

    if (metrics)
        copy_dab_metrics(metrics, &dab_ds, NULL);

    if (dab_ds.ACQ == SI461X_DAB_CMD_DAB_DIGRAD_STATUS_REP_ACQ_ENUM_NOACQ) {
        return TIMEOUT;
    }

    if (full_check) {
        dab_demo_ensemble_info_t xdata dgeid = { 0 };
		int8_t xdata try_times = 3;
		do {
	        ret = si461x_get_ensemble_info(&dgeid);
	        if (ret != SUCCESS)
	            return ret;

			if (dgeid.EID != 0xFFFF)
			{
		        if (eid)
		            *eid = dgeid.EID;
				
				return SUCCESS;
			}

			Wait_Ms(100);
			try_times --;
		} while (try_times >= 0);

        // Can occasionally get 0xFFFF for EID after acquiring if signal is weak and demod loses signal.
        // In this case we consider the station too weak with TIMEOUT return code;
        //if (dgeid.EID == 0xFFFF)
        return TIMEOUT;
    }

    return SUCCESS;
}


RETURN_CODE DAB_update_current_svc_metrics(bool_t* lost_svc)
{
    RETURN_CODE xdata ret = SUCCESS;
    int16_t xdata svc_index;

    ret = dabsl_find_service(g_dab_state.current.service_id,
        g_dab_state.current.component_id, &svc_index);

    if (ret != SUCCESS)
        return ret;

    if (svc_index >= 0) {
        // found the service in list
        g_dab_state.metrics.current_svc_svclist_index = (uint8_t)svc_index;
        if (lost_svc)
            *lost_svc = false;
    } else {
        // did not find service in list
        g_dab_state.metrics.current_svc_svclist_index = DAB_METRIC_AUDIO_SERVICE_LIST__INDEX_NOT_SET;
        if (lost_svc)
            *lost_svc = true;
        CALLBACK_Updated_Data(SCB_EVENT_CURRENT_SERVICE_NO_LONGER_AVAILABLE);
    }

    g_dab_state.browse_index = g_dab_state.metrics.current_svc_svclist_index;
    return ret;
}


static RETURN_CODE handle_svrlist(uint16_t svc_list_version)
{
    RETURN_CODE xdata ret = SUCCESS;
    bool_t xdata lost_svc = false;
    dab_freq xdata current_freq;
    dab_demod_digrad_status_t xdata digrad;
    demo_digital_service_list_t xdata gdsl = { 0 };

    // DAB only has one type of list.
    // This allocates memory to gdsl.DATA which we must free!
    ret = si461x_demod_dab_get_digital_service_list(0, &gdsl);
    if (ret != SUCCESS)
        return ret;

    current_freq.freq_hz = g_dab_state.current.freq;
    ret = si461x_demod_dab_digrad_status(false, false, false, false, false, &digrad);
    if (ret != SUCCESS)
        return ret;

    current_freq.cnr = digrad.CNR;
		
    dabsl_update_list(&gdsl, svc_list_version, &current_freq, g_dab_state.current.ensemble_id);
    //free(gdsl.DATA);

    // If list changes, we have to keep our current index up-to-date
    ret = DAB_update_current_svc_metrics(&lost_svc);
    return ret;
}


RETURN_CODE DAB_event_handler()
{
    RETURN_CODE xdata ret = SUCCESS;

    demod_dab_event_status_t xdata dges = { 0 };
	
    ret = si461x_demod_dab_get_event_status(false, true, &dges);
    if (ret != SUCCESS)
        return ret;

    g_dab_state.metrics.demod.event = dges;

    // Checking both the INT bits and current bit prevents
    //  weird possible situations where the INT bits have not been ACKed,
    //   but the data is not available.  May be slightly overkill.
    // (e.g. changed frequencies without ACK'ing the INT bits)
    if (dges.SVRLISTINT && dges.SVRLIST) {
        ret |= handle_svrlist(dges.SVRLISTVER);
        //c_log_svc_list(&g_dab_svc_list);
    }

    if (dges.ANNOINT && dges.ANNO) {
        //TODO: process announcements
    }

    if (dges.RECFGINT) {
        // Manually increase the service list debounce time as we have exited the reconfig window
        //ret |= set_prop(
        //SI479XX_PROP_DAB_EVENT_MIN_SVRLIST_PERIOD_ADDR,
        //DAB_EVENT_MIN_SVRLIST_PERIOD__NORMAL_VALUE__IN_100MS);

        // Issue callback that the service list has reconfigured
        CALLBACK_Updated_Data(SCB_EVENT_ENSEMBLE_RECONFIGURATION);
    }

    if (dges.RECFGWRNINT) {
        //Manually reduce the service list debounce time to ensure the new list is as soon as possible after the update
        //ret |= set_prop(
        //SI479XX_PROP_DAB_EVENT_MIN_SVRLIST_PERIOD_ADDR,
        //DAB_EVENT_MIN_SVRLIST_PERIOD__DURING_ENSEMBLE_RECONFIG__IN_100MS);

        //Issue callback that the service list reconfiguration is pending
        CALLBACK_Updated_Data(SCB_EVENT_ENSEMBLE_RECONFIGURATION_WARNING);
    }

    if (dges.FREQINFOINT && dges.FREQINFO) {
        demod_dab_freq_info_t xdata fi = { 0 };
        ret |= si461x_demod_dab_get_freq_info(&fi);

        //free(fi.FI);
    }
    return ret;
}

RETURN_CODE DAB_confirm_service_list_complete()
{
    RETURN_CODE xdata ret = SUCCESS;
    //Confirm service list received
    int32_t xdata timeout_ms = OPTION__DAB_SERVICE_LIST_SCVLIST_MS;

    while (timeout_ms > OPTION__DAB_SERVICE_LIST_TIMEOUT_CALLBACK_RATE_MS) {
        ret |= DAB_event_handler();
        if (ret != SUCCESS)
            return ret;

        if (g_dab_svc_list.list_updated_for_freq)
            return SUCCESS;

        if (&g_cb) {
            // Hands off control to the MMI code for UI update (button checks) - opportunity to cancel the scan
            CALLBACK_Updated_Data_ctx(SCB_DAB_TUNE_SCAN_PROCESS_UPDATE, &g_cb);
        }

        Wait_Ms(OPTION__DAB_SERVICE_LIST_TIMEOUT_CALLBACK_RATE_MS);
        timeout_ms -= OPTION__DAB_SERVICE_LIST_TIMEOUT_CALLBACK_RATE_MS;
    }

    return TIMEOUT;
}


RETURN_CODE DAB_tune_full_process(uint32_t freq, bool_t update_svclist, dab_tune_status* status)
{
    RETURN_CODE xdata ret = SUCCESS;
    dab_metrics* xdata metrics = NULL;
    uint16_t* xdata eid = NULL;

    // Invalidate current frequency
    if (&g_dab_svc_list)
        update_ensemble_freq_cnr(freq, SI461X_DAB_CMD_DAB_DIGRAD_STATUS_REP_CNR_MIN);

    ret = DAB_tune_chips(freq, status);
    if (ret != SUCCESS || (status && !status->tune_complete))
        return ret;

    ret = DAB_acquire();
    if (ret != SUCCESS)
        return ret;

    metrics = &g_dab_state.metrics;
    eid = &g_dab_state.current.ensemble_id;

    ret = DAB_confirm_acquisition_complete(true,metrics, eid);
    if (ret == SUCCESS) {
        if (metrics && &g_dab_svc_list) {
            // acquired, update with new CNR
            update_ensemble_freq_cnr(freq, metrics->demod.digrad.CNR);
        }

        if (status)
            status->demod_acquired = true;

        if (update_svclist)
            ret = DAB_confirm_service_list_complete();
    }

    return ret;
}


RETURN_CODE ScanBand_DAB()
{
	RETURN_CODE xdata ret = SUCCESS;
	RETURN_CODE xdata r = SUCCESS;
    bool_t xdata found_service = false;
	uint8_t xdata i;
    // Do complete scan of the current frequency list.
	
    g_dab_state.browse_index = DAB_METRIC_AUDIO_SERVICE_LIST__INDEX_NOT_SET;
	g_cb.continue_scan = TRUE;
				
	//clear the service list
	dabsl_init_list(&g_dab_svc_list);
	
    for (i = 0; i < DAB_DEFAULT_FREQ_LIST_SIZE; i++) {
        dab_tune_status xdata status;
		
		g_cb.freq = dab_default_freq_list[i];
        g_cb.index = i;
		CALLBACK_Updated_Data_ctx(SCB_DAB_TUNE_SCAN_ABOUT_TO_PROCESS_NEW_FREQ, &g_cb);
        r = DAB_tune_full_process(dab_default_freq_list[i], true, &status);
        if (!IS_BIT_SET(r, TIMEOUT)) {
            found_service = true;

		}
		//g_cb.continue_scan is notify and updted in DAB_tune_full_process
		if (!g_cb.continue_scan) 
			break;
    }
	
	//update the service list
	SaveServiceList_DAB();

    CALLBACK_Updated_Data(SCB_DAB_TUNE_SCAN_PROCESS_COMPLETE);

    if (found_service)
        return SUCCESS;
    else
        return TIMEOUT;
}



static RETURN_CODE bubble_sort_weighted(uint16_t elem_count, uint32_t* list_start, uint8_t* weights_start)
{
    bool_t xdata swapped;

    if (elem_count <= 1)
        return SUCCESS;

    do {
        uint16_t xdata i;
        swapped = false;
        for (i = 1; i < elem_count; i++) {
            uint8_t* xdata s_b = weights_start + i;
            uint8_t* xdata s_a = s_b - 1;

            uint8_t* xdata b = list_start + i;
            uint8_t* xdata a = b - 1;
			
            if (*s_a > *s_b) {
				c_swap__uint8_t(s_a, s_b);
				c_swap__uint8_t(a, b);
                swapped = true;
            }
        }
    } while (swapped);

    return SUCCESS;
}

// Sort by strength ascending. (weakest -> strongest)
static RETURN_CODE sort_candidate_freqs(uint32_t* freqs, uint8_t freq_count)
{
    RETURN_CODE xdata r = SUCCESS;
    int8_t xdata rssi[DAB_SERVICE_LIST__NUM_CANDIDATE_FREQS];
	uint8_t xdata i;
	
    D_ASSERT(chips && dab && freqs);

    if (freq_count <= 1)
        // nothing to sort
        return SUCCESS;

    CALLBACK_Updated_Data(SCB_DAB_TUNE_REQUIRED_UPDATE);
    // We need to do this as fast as possible - specific code for this RF check
    //r = DAB_demod_adjust_sync_valid_threshold(true);
    //if (r != SUCCESS)
    //    return r;

    for (i = 0; i < freq_count; i++) {
        r = DAB_tune(freqs[i]);
        r |= DAB_confirm_tune_complete(true, NULL, &g_dab_state.metrics);
        if (IS_BIT_SET(r, TIMEOUT)) {
            rssi[i] = SI479XX_TUNER_CMD_DAB_RSQ_STATUS_REP_RSSI_MIN;
        } else {
            rssi[i] = g_dab_state.metrics.tuner.rssi;
        }
    }

    //r = DAB_demod_adjust_sync_valid_threshold(false);

    // Sort the candidate ensembles by strongest RF (ascending)
    r |= bubble_sort_weighted(freq_count, freqs, rssi);

    return r;
}


RETURN_CODE DAB_start_processing_channel(uint32_t service_id, uint32_t component_id)
{
    RETURN_CODE xdata ret = SUCCESS;

#ifdef SUPPORT_DAB_DLS	
    dls_init(&g_dab_state.dls_decoder);
#endif
    ret = si461x_demod_dab_start_digital_service(SI461X_DAB_CMD_START_DIGITAL_SERVICE_ARG_SERTYPE_ENUM_AUDIO,
        service_id, component_id);

    if (ret == SUCCESS) {
        g_dab_state.current.service_id = service_id;
        g_dab_state.current.component_id = component_id;
    }

    return ret;
}



RETURN_CODE DAB_start_service_worker(uint32_t current_freq, dab_start_service_info *p_dab_start_service_info, bool_t force_retune)
{
    RETURN_CODE xdata ret = SUCCESS;
    dab_demod_digrad_status_t xdata dab_ds = { 0 };
	int32_t xdata i;
	uint16_t xdata timeout;    
	bool_t xdata lost_svc;

    if (force_retune || (current_freq != p_dab_start_service_info->freq)) {
        dab_tune_status xdata status;
        CALLBACK_Updated_Data(SCB_DAB_TUNE_REQUIRED_UPDATE);
        ret = DAB_tune_full_process(p_dab_start_service_info->freq, false, &status);
        if (ret != SUCCESS)
            return ret;

		Wait_Ms(50);//unkonwn reason
        ////c_log_comment(DCTAG "DAB_start_service_worker() - acquisition OK");
    }

    // Check for weak signals before starting
    for (i = 0; true; i++) {
        if (i >= OPTION__DAB_LINK_DISQUALIFIER_CNR_SETTLING_TIME_MS) {
            //SDK_PRINTF("DAB CNR too low to start service...\n");
            return TIMEOUT;
        }

        ret |= si461x_demod_dab_digrad_status(false, false, false, false, false, &dab_ds);
        if (ret != SUCCESS)
            return ret;

        // Is current service good enough to start ?
        // //SDK_PRINTF("DAB CNR: %d\n", dab_ds.CNR);
        if (dab_ds.CNR >= OPTION__DAB_LINK_DISQUALIFIER_CNR_THRESHOLD)
            break;

        Wait_Ms(1);
    }
	
    timeout = OPTION__DAB_SERVICE_START_TIMEOUT_MS;
    while (true) {
        ret = DAB_start_processing_channel(p_dab_start_service_info->service_id, p_dab_start_service_info->component_id);
        if (ret == SUCCESS) {
			demod_dab_event_status_t xdata demod_event_status;
            // clear any prior audio error bits
            si461x_demod_dab_get_event_status(true, false, &demod_event_status);
            break;
        }

        Wait_Ms(OPTION__DAB_SERVICE_START_ATTEMPT_RATE_MS);
        timeout -= OPTION__DAB_SERVICE_START_ATTEMPT_RATE_MS;
        if (timeout < OPTION__DAB_SERVICE_START_ATTEMPT_RATE_MS)
            return TIMEOUT;
    }


    ret = DAB_update_current_svc_metrics(&lost_svc);
    return ret;
}


// Starts a service list element.
// Will try all the matching SIDs + all linked frequencies for those Ensembles.
RETURN_CODE DAB_start_service_from_list(uint8_t svc_list_index)
{
    RETURN_CODE xdata ret = SUCCESS;
    // All matching service indices.
    uint8_t xdata services[DAB_SERVICE_LIST__NUM_CANDIDATE_FREQS];
    uint8_t xdata service_count = 0;

    // Candidate frequencies carrying services.
    uint32_t xdata freqs[DAB_SERVICE_LIST__NUM_CANDIDATE_FREQS];
    uint8_t xdata freq_count = 0;
	int8_t xdata i;
	
	dab_service_list_element* xdata svc_to_start;
	
    if ((g_dab_svc_list.service_count <= 0) || (svc_list_index >= g_dab_svc_list.service_count))
        return INVALID_INPUT;


    // Consider all matching entries in the service list.
    dabsl_find_matching_services(svc_list_index, ARRAY_SZ(services), services, &service_count);

    // Create a list of frequencies for the candidates.
    for (i = 0; i < service_count; i++) {
        dabsl_find_candidate_freqs(services[i], ARRAY_SZ(freqs), freqs, &freq_count);
    }

    // Sort frequencies by signal strength.
    ret = sort_candidate_freqs(freqs, freq_count);
    if (ret != SUCCESS) {
        //c_log_commentf("ERROR: DAB_start_service_from_list() - failed to sort RF candidates.\n");
        return ret;
    }

    // Attempting component start. Go from end to beginning (strongest RF -> weakest RF)
    svc_to_start = &g_dab_svc_list.services[svc_list_index];
    g_dab_state.metrics.current_svc_svclist_index = svc_list_index;
    for (i = freq_count - 1; i >= 0; i--) {
		dab_start_service_info xdata start_service_info;
		start_service_info.service_id = svc_to_start->info.service_id;
		start_service_info.component_id = svc_to_start->info.component_id;
		start_service_info.freq = freqs[i];
        ret = DAB_start_service_worker(g_dab_state.metrics.tuner.freq_hz, &start_service_info, true);

        if (ret == SUCCESS)
            break;
    }

    return ret;
}



static void dab_browse_step_up_down_worker(bool_t stepup)
{
    if (stepup) {
        if (g_dab_state.browse_index < (g_dab_svc_list.service_count - 1)) {
            // Not at the end of the list - move service index up
            g_dab_state.browse_index++;
        } else {
            //We have reached the end of the list - wrap to bottom
            g_dab_state.browse_index = 0;
        }
    } else {
        if (g_dab_state.browse_index > 0) {
            // Not at the end of the list - move service index down
            g_dab_state.browse_index--;
        } else {
            //We have reached the end of the list - wrap to top
            g_dab_state.browse_index = (g_dab_svc_list.service_count - 1);
        }
    }
}



RETURN_CODE DAB_get_svc_list_element(dab_service_list* list, uint8_t service_index,
    dab_service_list_element* list_elem)
{
    if (!list_elem)
        return INVALID_INPUT;

    if (service_index >= list->service_count) {
        // SDK_PRINTF("ERROR: get_svc_list_element(): invalid svc index.");
        return INVALID_INPUT;
    }

    *list_elem = list->services[service_index];
    return SUCCESS;
}



RETURN_CODE DAB_browse_services_change_selection(bool_t stepup, dab_service_list_element* list_elem)
{
    RETURN_CODE xdata ret = SUCCESS;
	uint8_t xdata orig_browse_index;
	dab_service_list_element* xdata tmp_elem;

    if (!list_elem)
        return INVALID_INPUT;

    if (g_dab_svc_list.service_count == 0) {
        ClearMemory(list_elem, sizeof(*list_elem));
        return INVALID_INPUT;
    }

    if (g_dab_state.browse_index == DAB_METRIC_AUDIO_SERVICE_LIST__INDEX_NOT_SET) {
        //We are not tuned so we want to start at the top of the list
        g_dab_state.browse_index = 0;
    }

    orig_browse_index = g_dab_state.browse_index;
    tmp_elem = &g_dab_svc_list.services[g_dab_state.browse_index];

    do {
        dab_browse_step_up_down_worker(stepup);
        ret |= DAB_get_svc_list_element(&g_dab_svc_list, g_dab_state.browse_index, list_elem);

        if (ret != SUCCESS)
            return ret;

    } while ((CompareMemory(tmp_elem->service_name.name, list_elem->service_name.name, DAB_SERVICE_LIST_SERVICE_LABEL__SIZE) == 0)
        && (orig_browse_index != g_dab_state.browse_index));

    return ret;
}



RETURN_CODE DAB_StartServiceByIndex(uint8_t svcIndex)
{
    RETURN_CODE xdata ret = SUCCESS;
	
	if ((g_dab_svc_list.service_count > 0) && (svcIndex >= g_dab_svc_list.service_count))
		return INVALID_INPUT;

    g_dab_state.browse_index = svcIndex; 
    
    return DAB_start_service_from_list(g_dab_state.browse_index);
}


RETURN_CODE DAB_StartCurrentSelection()
{
    RETURN_CODE xdata ret = SUCCESS;

    if((g_dab_state.browse_index == DAB_METRIC_AUDIO_SERVICE_LIST__INDEX_NOT_SET) 
        && (g_dab_svc_list.service_count > 0))
    {
        g_dab_state.browse_index = 0; 
    }
	
   
    ret = DAB_start_service_from_list(g_dab_state.browse_index);
	
    return ret;
}


RETURN_CODE DAB_StartNextSelection()
{
	RETURN_CODE xdata ret = SUCCESS;
	dab_start_service_info xdata start_service_info;
	
	
	ret = DAB_browse_services_change_selection(true, &g_dab_svc_list_elem);
	if (SUCCESS != ret)
		return ret;
	
	start_service_info.service_id = g_dab_svc_list_elem.info.service_id;
	start_service_info.component_id = g_dab_svc_list_elem.info.component_id;
	start_service_info.freq = g_dab_svc_list_elem.info.freq;
	
	ret |= DAB_start_service_worker(g_dab_state.metrics.tuner.freq_hz, &start_service_info, false);
	
    return ret;
}


RETURN_CODE DAB_StartPrevSelection()
{
	RETURN_CODE xdata ret = SUCCESS;
	dab_start_service_info xdata start_service_info;
	
	ret = DAB_browse_services_change_selection(false, &g_dab_svc_list_elem);
	if (SUCCESS != ret)
		return ret;
	
	start_service_info.service_id = g_dab_svc_list_elem.info.service_id;
	start_service_info.component_id = g_dab_svc_list_elem.info.component_id;
	start_service_info.freq = g_dab_svc_list_elem.info.freq;
	
	ret |= DAB_start_service_worker(g_dab_state.metrics.tuner.freq_hz, &start_service_info, false);
	
    return ret;
}

RETURN_CODE DAB_UpdateDataServiceData()
{
	RETURN_CODE xdata ret = SUCCESS;

    demod_dab_int_status_t xdata int_status = { 0 };

    ret |= si461x_demod_dab_get_int_status(&int_status);

    // Check for valid data in the data service interface
    //**************************************************************************
    if (int_status.STATUS_DSRVINT) {
        demod_dab_digital_service_t xdata gdsd = { 0 };
        do {
            // Clear DSRVINTB
            ret |= si461x_demod_dab_get_digital_service_data(false, true, &gdsd);

            // Sometimes this loops gets in a state where the command to read digital service data always reports a command error.
            if ((gdsd.BYTE_COUNT > 0) && (ret == SUCCESS)) {
//Send to appropriate handler
//If PSD data we handle is seperately from the tracked service list
#ifdef SUPPORT_DAB_DLS
                // TODO current implementation will not work long term
                // - as we need to handle other PAD data types, TDC, MOT, etc
                if ((gdsd.SERVICE_ID == g_dab_state.current.service_id)
                    && (gdsd.COMP_ID == g_dab_state.current.component_id)) {
                    dls_update(&g_dab_state.dls_decoder, gdsd);
                }
#endif //SUPPORT_DAB_DLS

#ifdef OPTION__HANDLE_ADVANCED_SERVICES
                // Check if there are digital services to update
                ret |= adv_update(_service_list, &gdsd);
#endif
            }
        } while (ret == SUCCESS && gdsd.BUFF_COUNT > 0);
    }

	return ret;
}	


RETURN_CODE DAB_UpdateServiceList()
{
	return DAB_event_handler();
}

dab_service_list_element* DAB_GetCurrentServiceElementInfo()
{
	RETURN_CODE xdata ret = SUCCESS;
	
	ret = DAB_get_svc_list_element(&g_dab_svc_list, g_dab_state.browse_index, &g_dab_svc_list_elem);
	
	if (SUCCESS != ret)
		return NULL;
	
    return &g_dab_svc_list_elem;
}

dab_service_list* DAB_GetServiceList()
{
	return &g_dab_svc_list;
}

// Saves the service list as binary to file 'filename'.
RETURN_CODE DAB_save_service_list(uint32_t freq, uint8_t nServIndex)
{
    RETURN_CODE xdata ret = SUCCESS;
	uint16_t xdata write_sz = 0;
	
	dab_service_list* xdata list = &g_dab_svc_list;
	
    if (!list)
        return INVALID_INPUT;
	
	f_seek(0);

	write_sz = sizeof(uint32_t);
    //Save the Last Played Service
    if (write_sz != f_write((uint8_t*)&freq, write_sz)) {
        ret = HAL_ERROR;
    }

	write_sz = sizeof(uint8_t);
    //Save the Last Played Service
    if (write_sz != f_write(&nServIndex, write_sz)) {
        ret = HAL_ERROR;
    }

    // Store our list
    write_sz = sizeof(dab_service_list);
    if (write_sz != f_write((uint8_t*)list, write_sz)) {
        ret = HAL_ERROR;
    }

    return ret;
}

RETURN_CODE SaveServiceList_DAB()
{
	return DAB_save_service_list(g_dab_state.current.freq, DAB_METRIC_AUDIO_SERVICE_LIST__INDEX_NOT_SET/*g_dab_state.metrics.current_svc_svclist_index*/);
}

// Loads the service list from binary file.
RETURN_CODE DAB_load_service_list(uint32_t* pFreqIndex, uint8_t* pServIndex)
{
    RETURN_CODE xdata ret = SUCCESS;
	size_t xdata bytes_read;
	dab_service_list* xdata list = &g_dab_svc_list;
	
    if (!pFreqIndex || !pServIndex || !list)
        return INVALID_INPUT;

    dabsl_init_list(list);
			
	f_seek(0);

    bytes_read = sizeof(uint32_t);
    if (bytes_read != f_read((uint8_t*)pFreqIndex, bytes_read)) {
        return HAL_ERROR;
    }

    bytes_read = 1;
    if (bytes_read != f_read(pServIndex, bytes_read)) {
        return HAL_ERROR;
    }

    bytes_read = sizeof(dab_service_list);
    if (bytes_read != f_read((uint8_t*)list, bytes_read)) {
        return HAL_ERROR;
    }

    if (list->list_storage_version != DAB_SERVICE_LIST_OBJECT_VERSION) {
        // The saved list is not valid for this software version - need to clear the list
        dabsl_init_list(list);
        return INVALID_INPUT;
    }

    //Need to override the internal list tracking variables so that the handler will process the next list recieved.
    list->service_list_version = DAB_LIST_VERSION_UNKNOWN;

    return ret;
}

RETURN_CODE LoadServiceList_DAB()
{
    return DAB_load_service_list(&g_dab_state.current.freq, &g_dab_state.browse_index);
}

RETURN_CODE LoadEnsemblesTable_DAB(uint8_t *pnCount, uint16_t *pEnsIndsTable)
{
	uint8_t xdata i;
	dab_service_list* xdata list = &g_dab_svc_list;
	
    if (!pEnsIndsTable || !pnCount || DAB_SERVICE_LIST__MAX_ENSEMBLES_FOR_LIST < list->ensemble_count)
        return INVALID_INPUT;
	
	
	for (i=0; i<list->ensemble_count; i++)
	{
		pEnsIndsTable[i] = list->ensembles[i].eid;
	}
	
	*pnCount = list->ensemble_count;
		
    return SUCCESS;
}


