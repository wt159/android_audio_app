/***************************************************************************************
                  Skyworks MSS Automotive Tuner + Demod SDK

   EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND CONDITIONS OF
     THE SOFTWARE LICENSE AGREEMENT IN THE DOCUMENTATION FILE CORRESPONDING
     TO THIS SOURCE FILE.
   IF YOU DO NOT AGREE TO THE LIMITED LICENSE AND CONDITIONS OF SUCH AGREEMENT,
     PLEASE RETURN ALL SOURCE FILES TO SKYWORKS.

   SDK DAB service list decoder - decodes the available services and adds them to the overall service database
   FILE: DAB_Service_List_Handler.c
   Supported IC : Si479xx + Si461x
   Date: September 21, 2015
  (C) Copyright 2015, Skyworks, Inc. All rights reserved.
****************************************************************************************/

#include "DAB_service_list_Handler.h"
#include "type.h"
#include "ui_callbacks.h"
#include "c_core.h"
#include "DAB_Common.h"


#include <string.h>
// IMPORTANT: may have to reverse the bytes of multi byte fields,
// if your platform is not little endian.

// These structs can be memory mapped to sections of the
// data returned by the demod command GET_DIGITAL_SERVICE_LIST()

// See Si4614 demod AN911 for the source of these defintions.
// DO NOT MODIFY, ORDERING and SIZE matters for memory mapping.
//#pragma pack(push, 1)
typedef struct dab_list_header {
    uint16_t version;
    uint8_t service_count;
    // no data
    uint8_t align_pad[3];
} dab_list_header;

typedef struct dab_svc_header {
    uint32_t service_id;
    uint8_t service_info1;
    uint8_t service_info2;
    uint8_t service_info3;
    // no data
    uint8_t align_pad;
    uint8_t service_label[DAB_SERVICE_LIST_SERVICE_LABEL__SIZE];
} dab_service;

// This is repeated for each component in a service.
// DO NOT MODIFY, ORDERING and SIZE matters for memory mapping.
typedef struct dab_svc_component {
    uint16_t component_id;
    uint8_t component_info;
    uint8_t valid_flags;
} dab_svc_component;

//#pragma pack(pop)

//-----------------------------------------------------------------------------
// local defines
//-----------------------------------------------------------------------------

#define DAB_SERVICE_LIST_VERSION__OFFSET 0
#define DAB_SERVICE_LIST_NUM_SERVICES__OFFSET 2
#define DAB_SERVICE_LIST_ELEMENT_START__OFFSET 6

#define DAB_SERVICE_LIST_SERVICE_ID__OFFSET 0
#define DAB_SERVICE_LIST_SERVICE_INFO1__OFFSET 4
#define DAB_SERVICE_LIST_SERVICE_INFO1__PTY_MASK 0x3E
#define DAB_SERVICE_LIST_SERVICE_INFO1__PTY_SHIFT 1
#define DAB_SERVICE_LIST_SERVICE_INFO1__PDFLAG_MASK 0x01
#define DAB_SERVICE_LIST_SERVICE_INFO1__PDFLAG_AUDIO 0
#define DAB_SERVICE_LIST_SERVICE_INFO1__PDFLAG_DATA 1

#define DAB_SERVICE_LIST_SERVICE_INFO2__OFFSET 5
#define DAB_SERVICE_LIST_SERVICE_INFO2__NUM_COMP_MASK 0x0F
#define DAB_SERVICE_LIST_SERVICE_INFO2__CAID_MASK 0x70
#define DAB_SERVICE_LIST_SERVICE_INFO2__CAID_SHIFT 4
#define DAB_SERVICE_LIST_SERVICE_INFO2__LOCAL_MASK 0x80
#define DAB_SERVICE_LIST_SERVICE_INFO2__LOCAL_SHIFT 7

#define DAB_SERVICE_LIST_SERVICE_INFO3__OFFSET 6
#define DAB_SERVICE_LIST_SERVICE_INFO3__SICHARSET_MASK 0x0F

#define DAB_SERVICE_LIST_SERVICE_LABEL__OFFSET 8

//#define DAB_SERVICE_LIST_SERVICE_LABEL__SIZE 16  //Defined in feature_types.h

#define DAB_SERVICE_LIST_SERVICE_DATA__LENGTH (DAB_SERVICE_LIST_SERVICE_LABEL__OFFSET + DAB_SERVICE_LIST_SERVICE_LABEL__SIZE)

#define DAB_COMPONENT_LIST_COMPONENT_ID__OFFSET 0
#define DAB_COMPONENT_LIST_COMPONENT_ID__TMID_MASK 0xC000 //TMID = 0 means audio service, 1 = data(stream), 2 = FIDC, 3 = data(packet)
#define DAB_COMPONENT_LIST_COMPONENT_ID__TMID_SHIFT 14

#define DAB_COMPONENT_LIST_COMPONENT_INFO__OFFSET 2
// Anything with CA = 1 can be thrown out - we dont support CA
#define DAB_COMPONENT_LIST_COMPONENT_INFO__CAFLAG_MASK 0x01
#define DAB_COMPONENT_LIST_COMPONENT_INFO__PS_MASK 0x02 // Primary (0), Secondary (1) - ignored
#define DAB_COMPONENT_LIST_COMPONENT_INFO__PS_SHIFT 1
#define DAB_COMPONENT_LIST_COMPONENT_INFO__ASCTY_DSCTY_MASK 0xFC
#define DAB_COMPONENT_LIST_COMPONENT_INFO__ASCTY_DSCTY_SHIFT 2
// ASCTY/DSCTY determined by TMID
// ASCTY:   0 = foreground
//          1 = background
//          2 = multichannel extension
// DSCTY:   0 = Unspecified data
//          1 = Traffic message Channel (TMC)
//          2 = Emergency Warning System (EWS)
//          3 = Interactive Text Transmission System (ITTS)
//          4 = Paging
//          5 = Transparent Data Channel (TDC)
//          24 = MPEG-2 Transport Stream, see [29]
//          59 = Embedded IP packets
//          60 = Multimedia Object Transfer (MOT)
//          61 = Proprietary service: no DSCTy signalled
//          62 = Not used
//          63 = Not used

#define DAB_COMPONENT_LIST_VALID_FLAGS__OFFSET 3

#define DAB_COMPONENT_LIST_COMPONENT_DATA__LENGTH 4

typedef enum TMID {
    TMID_STREAM_AUDIO = 0,
    TMID_STREAM_DATA = 1,
    TMID_RESERVED = 2,
    TMID_PACKET_DATA = 3,
} TMID;

typedef enum AUDIO_SERVICE_COMPONENT_TYPE {
    ASCTY_DAB = 0,
    ASCTY_DABPLUS = 63,
} AUDIO_SERVICE_COMPONENT_TYPE;

typedef enum DATA_SERVICE_COMPONENT_TYPE {
    DSCTY_MPEG2 = 24,
    DSCTY_MOT = 60,
} DATA_SERVICE_COMPONENT_TYPE;

typedef enum UATYPE {
    UATYPE_TPEG = 4,
    UATYPE_SPI = 7,
    UATYPE_DMB = 9,
} UATYPE;

#define ARRAY_SZ(x) (sizeof(x)/sizeof(x[0]))


void dab_service_info_copy(dab_service_info* s1, dab_service_info* s2)
{
	s1->service_id = s2->service_id;
    s1->component_id = s2->component_id;
    s1->ensemble_id = s2->ensemble_id;
    s1->freq = s2->freq;
}


//-----------------------------------------------------------------------------
// After a scan the service list needs to be flushed
//-----------------------------------------------------------------------------
void dabsl_init_list(dab_service_list* list)
{
	uint8_t xdata i;

    if (!list)
        return;

    list->service_list_version = DAB_LIST_VERSION_UNKNOWN;
    list->list_storage_version = DAB_SERVICE_LIST_OBJECT_VERSION;
    list->service_count = 0;
    list->ensemble_count = 0;

    for (i = 0; i < ARRAY_SZ(list->ensembles); i++) {
        memset(&list->ensembles[i], 0, sizeof(dab_ensemble_info));
        list->ensembles[i].offset = DAB_SERVICE_LIST__LINKED_INDEX_DEFAULT;
    }

    //c_log_svc_list(list);
}

RETURN_CODE dabsl_find_service(uint32_t service_id, /**bool_t match_component, */uint16_t component_id,
    /**uint8_t start_svc_addr_index, */int16_t* found_svc_addr_index/**, dab_service_list_element** found_svc*/)
{
    RETURN_CODE xdata ret = SUCCESS;
	uint8_t xdata i;
	uint8_t start_svc_addr_index = 0;
	
	dab_service_list* xdata list = &g_dab_svc_list;

    if (found_svc_addr_index == NULL)
        return INVALID_INPUT;

    // starts out as invalid;
    *found_svc_addr_index = -1;

    for (i = start_svc_addr_index; i < list->service_count; i++) {
        dab_service_list_element* tmp_elem = &list->services[i];

        //if (ret != SUCCESS)
        //    return ret;

        if ((tmp_elem->info.service_id == service_id)
			&& (tmp_elem->info.component_id == component_id)) {
            /**&& (!match_component || (tmp_elem->info.component_id == component_id))) {*/
            *found_svc_addr_index = i;
            //if (found_svc)
            //    *found_svc = tmp_elem;

            return SUCCESS;
        }
    }

    // found_svc_addr_index indicates if svc was found, not return code.
    return SUCCESS;
}

RETURN_CODE dabsl_find_ensemble(uint16_t ensemble_id, int16_t* ensemble_index)
{
	uint8_t xdata ens;
	dab_service_list_element* xdata tmp_elem;
	dab_service_list* xdata list = &g_dab_svc_list;

    if (ensemble_index == NULL) {
        //SDK_PRINTF("ERROR: dabsl_find_ensemble() : invalid input.");
        return INVALID_INPUT;
    }

    // default to invalid index;
    *ensemble_index = -1;

    for (ens = 0; ens < list->ensemble_count; ens++) {
        uint16_t xdata svc_index = list->ensembles[ens].offset;
        if (svc_index >= list->service_count) {
            //SDK_PRINTF("WARN: ensemble indices not up-to-date, referenced service past end of list.");
            continue;
        }

        tmp_elem = &list->services[svc_index];
        if (tmp_elem->info.ensemble_id == ensemble_id) {
            *ensemble_index = ens;
            return SUCCESS;
        }
    }

    // Matching ensemble was not found! (indicated by ensemble_index)
    return SUCCESS;
}
static uint8_t to_upper_ASCII(uint8_t character)
{
    if ((character <= 0x7A) && (character >= 0x61)) {
        // Convert characters to upper case for comparison
        return (character - 0x20);
    } else {
        return character;
    }
}
// Compares two ASCII strings (case insensitive).
// Result:
//   o 0 = same (will only occur when lengths are the same)
//   o negative = str0 before str1 in dictionary
//   o positive = str0 after str1 in dictionary
static int8_t strcmp_ASCII(uint8_t* str0, uint8_t str0_len, uint8_t* str1, uint8_t str1_len)
{
	uint8_t xdata i;

    for (i = 0; (i < str0_len) && (i < str1_len); i++) {
        uint8_t xdata char0 = to_upper_ASCII(str0[i]);
        uint8_t xdata char1 = to_upper_ASCII(str1[i]);

        if (char0 < char1)
            return -1;
        if (char0 > char1)
            return 1;

        // same, check next char
    }

    // Result is based on lengths
    if (str0_len < str1_len)
        return -1;
    else if (str0_len == str1_len)
        return 0;
    else
        return 1;
}


// Sorts services in range [svc_start, svc_end) based on ASCII service name.
// Will not produce sorted list for non-sorted list.
static RETURN_CODE insertion_sort_alphabetical_list(uint8_t svc_start, uint8_t svc_end)
{
	uint8_t xdata svc;
	int16_t xdata i_sort;
	dab_service_list_element xdata tmp;
	dab_service_list* xdata list = &g_dab_svc_list;

    for (svc = svc_start + 1; svc < svc_end; svc++) {
        dab_service_list_element* xdata new_elem = &list->services[svc];

        // Insert each element into start (sorted) of list
        for (i_sort = svc - 1; i_sort >= svc_start; i_sort--) {
            dab_service_list_element* xdata test_elem = &list->services[i_sort];
            int8_t xdata test = strcmp_ASCII(new_elem->service_name.name, DAB_SERVICE_LIST_SERVICE_LABEL__SIZE,
                test_elem->service_name.name, DAB_SERVICE_LIST_SERVICE_LABEL__SIZE);

            if (test < 0) {
                // swap
                tmp = *new_elem;
                *new_elem = *test_elem;
                list->services[i_sort] = tmp;
                new_elem = &list->services[i_sort];
            } else {
                // in correct spot, continue to next service
                break;
            }
        }
    }

    return SUCCESS;
}

// Removes all services from the specified ensemble index.
static RETURN_CODE remove_ensemble(uint8_t ens_index, uint8_t* removed_count)
{
    uint8_t xdata remove_svc_count = 0;
	uint8_t xdata i;
	dab_service_list* xdata list = &g_dab_svc_list;

    if (ens_index == (list->ensemble_count - 1)) {
        // last ensemble, so remove rest of services.
        remove_svc_count = list->service_count - list->ensembles[ens_index].offset;
    } else {
        // remove services from this ensemble (stop at next ensemble)
        remove_svc_count = list->ensembles[ens_index + 1].offset
            - list->ensembles[ens_index].offset;
    }
    if (removed_count)
        *removed_count = remove_svc_count;

    // We can skip copying services if the removed set was at the end of the current list
    if (ens_index < (list->ensemble_count - 1)) {
        uint16_t xdata first_svc_of_ens = list->ensembles[ens_index].offset;

        // Remove current index, shift remaining down
        for (i = ens_index; (i + 1) < list->ensemble_count; i++) {
            list->ensembles[i] = list->ensembles[i + 1];
            list->ensembles[i].offset -= remove_svc_count;
        }

        // Copy/Repack the remaining services down.
        for (i = first_svc_of_ens; i < list->service_count; i++) {
            list->services[i] = list->services[i + remove_svc_count];
        }
    }

    // Update the counts.
    list->ensemble_count--;
    list->service_count -= remove_svc_count;

    return SUCCESS;
}


static void remove_ensemble_freq(uint32_t freq)
{
	uint8_t xdata i, j;
	dab_service_list* xdata list = &g_dab_svc_list;

    for (i = 0; i < list->ensemble_count;) {
        bool_t xdata removed_ensemble = false;
        for (j = 0; j < list->ensembles[i].freq_count; j++) {
            if (list->ensembles[i].freqs[j].freq_hz == freq) {
                // remove frequency by shifting others down
                remove_array_elem(j, (uint8_t*)list->ensembles[i].freqs,
                    list->ensembles[i].freq_count--,
                    sizeof(list->ensembles[i].freqs[0]));

                if (list->ensembles[i].freq_count == 0) {
                    // If all frequencies for an ensemble are gone, stop keeping track of it.
                    remove_ensemble(i, NULL);
                    removed_ensemble = true;
                    break;
                }
            }
        }

        if (!removed_ensemble)
            i++;
    }
}



static bool_t remove_old_ensemble()
{
	uint8_t xdata i, f;
	dab_service_list* xdata list = &g_dab_svc_list;

    // find an ensemble with all frequencies set to 0 strength
    for (i = 0; i < list->ensemble_count; i++) {
        uint8_t xdata valid_freqs = 0;
        for (f = 0; f < list->ensembles[i].freq_count; f++) {
            if (list->ensembles[i].freqs[f].cnr > 0) {
                valid_freqs++;
            }
        }

        if (valid_freqs == 0) {
            // ensemble no longer has frequencies, remove to make room.
            remove_ensemble(i, NULL);
            return true;
        }
    }
    return false;
}


static void error_in_list(uint16_t listVer)
{
	dab_service_list* xdata list = &g_dab_svc_list;

    list->list_updated_for_freq = true;
    list->service_list_version = listVer;
    CALLBACK_Updated_Data(SCB_SERVICE_LIST_BUFFER_FULL_ERROR);
}
static uint8_t get_svc_comp_count(uint32_t service_info2)
{
    return service_info2 & DAB_SERVICE_LIST_SERVICE_INFO2__NUM_COMP_MASK;
}

static uint32_t get_svc_id(uint32_t service_id)
{
    return get_32bit_le((uint8_t*)&service_id);
}

static uint8_t get_svc_pty(uint8_t service_info1)
{
    return (service_info1 & DAB_SERVICE_LIST_SERVICE_INFO1__PTY_MASK) >> DAB_SERVICE_LIST_SERVICE_INFO1__PTY_SHIFT;
}

static uint8_t get_svc_charset(uint8_t service_info3)
{
    return service_info3 & DAB_SERVICE_LIST_SERVICE_INFO3__SICHARSET_MASK;
}

static uint16_t get_comp_id(dab_svc_component* c)
{
    return get_16bit_le((uint8_t*)&c->component_id);
}

static uint8_t get_comp_tmid(dab_svc_component* c)
{
    return (c->component_id & DAB_COMPONENT_LIST_COMPONENT_ID__TMID_MASK)
        >> DAB_COMPONENT_LIST_COMPONENT_ID__TMID_SHIFT;
}

static bool_t get_comp_ca_flag(dab_svc_component* c)
{
    return (c->component_info) & DAB_COMPONENT_LIST_COMPONENT_INFO__CAFLAG_MASK;
}

static uint8_t get_comp_type(dab_svc_component* c)
{
    return (c->component_info & DAB_COMPONENT_LIST_COMPONENT_INFO__ASCTY_DSCTY_MASK)
        >> DAB_COMPONENT_LIST_COMPONENT_INFO__ASCTY_DSCTY_SHIFT;
}

static DIGITAL_COMPONENT_TYPE get_component_type(dab_service_list_element* svc, dab_svc_component* component)
{
	TMID xdata tmid;
	uint8_t xdata svc_type;
	bool_t xdata ca;
	DIGITAL_COMPONENT_TYPE xdata output = DCT_UNKNOWN;
    demod_dab_component_info_t xdata ci = { 0 };
    RETURN_CODE xdata ret;

    if (!svc || !component)
        return DCT_UNKNOWN;

    // Does not support CA
    ca = get_comp_ca_flag(component);
    if (ca)
        return DCT_UNKNOWN;

    tmid = get_comp_tmid(component);
    svc_type = get_comp_type(component);

    if (tmid == TMID_STREAM_AUDIO) {
        if (svc_type == ASCTY_DAB)
            return DCT_AUDIO_DAB;
        if (svc_type == ASCTY_DABPLUS)
            return DCT_AUDIO_DABPLUS;
        return DCT_AUDIO_UNKNOWN;
    }
    if (tmid == TMID_RESERVED)
        return DCT_UNKNOWN;

    // Some kind of DATA service....
    ret = si461x_get_component_info(svc->info.service_id, svc->info.component_id, &ci);
    if (ret != SUCCESS)
        goto free_uadata;

    if (ci.UADATALEN > 0) {
        if (ci.UATYPE == UATYPE_DMB)
            output = DCT_DATA_DMB;
    } else {
        // No component info
        switch (svc_type) {
        case DSCTY_MPEG2:
            // can decode audio portion ?
            output = DCT_DATA_VIDEO;
        }
    }

free_uadata:
    //free(ci.UADATA);
    return output;
}

static bool_t is_component_type_supported(DIGITAL_COMPONENT_TYPE type)
{
    switch (type) {
    case DCT_AUDIO_DAB:
    case DCT_AUDIO_DABPLUS:
    // DMB not supported yet
    case DCT_DATA_DMB:
        // case DCT_DATA_VIDEO:
        return true;

    default:
        return false;
    }
}

static RETURN_CODE process_dab_list(uint32_t freq, uint16_t current_ensemble_id,
    demo_digital_service_list_t* dab_svc_list)
{
	uint8_t xdata i, j;
    dab_service_list_element xdata tmp_elem = { 0 };
	uint16_t xdata sl_addr;
    dab_list_header* xdata list_info;
    uint8_t xdata svc_count;
	dab_service_list_element* xdata new_elem;
	dab_service_list* xdata list = &g_dab_svc_list;
	
    if (dab_svc_list == NULL)
        return INVALID_INPUT;

    // Code validity checks...
    //D_ASSERT(sizeof(dab_list_header) == DAB_SERVICE_LIST_ELEMENT_START__OFFSET);
    //D_ASSERT(sizeof(dab_service) == DAB_SERVICE_LIST_SERVICE_DATA__LENGTH);
    //D_ASSERT(sizeof(dab_svc_component) == DAB_COMPONENT_LIST_COMPONENT_DATA__LENGTH);
    //D_ASSERT(MEMBER_SZ(dab_service, service_label) == DAB_SERVICE_LIST_SERVICE_LABEL__SIZE);

    //Current Service List Addr in the current buffer - note this may be broken up and reset
    if (dab_svc_list->SIZE <= 0) {
        // nothing to update
        return SUCCESS;
    }

    sl_addr = 0;
    list_info = (dab_list_header*)(dab_svc_list->DATA + sl_addr);
    svc_count = list_info->service_count;
    sl_addr += sizeof(dab_list_header);
    // sl_addr now pointing to first service

    //Initialize the Linking Info items in the temp element, will be the same for all items in the list
    tmp_elem.info.ensemble_id = current_ensemble_id;

    for (i = 0; i < svc_count; i++) {
		uint8_t xdata component_count;
        // Use the structs for implicit offsets to members.
        dab_service* xdata svc = (dab_service*)(dab_svc_list->DATA + sl_addr);

        // Need to assign the number of components to know how much to skip if not audio
        component_count = get_svc_comp_count(svc->service_info2);

        // Parse audio service data
        //set the frequency index from the current metrics - also set during tune
        tmp_elem.info.freq = freq;

        tmp_elem.info.service_id = get_svc_id(svc->service_id);
        tmp_elem.program_type = get_svc_pty(svc->service_info1);

        //Hold the service label in seperate memory for later deciding to use component label or not
        tmp_elem.service_name.encoding = get_svc_charset(svc->service_info3);
        CpyMemory(tmp_elem.service_name.name, svc->service_label, sizeof(svc->service_label));

        // Increment the buffer address after reading service info
        sl_addr += sizeof(dab_service);
        // sl_addr now offset to the current service's first component

        for (j = 0; j < component_count; j++) {
            bool_t xdata have_space = true;
			DIGITAL_COMPONENT_TYPE xdata dctype;
            dab_svc_component* xdata component = (dab_svc_component*)(dab_svc_list->DATA + sl_addr);
            tmp_elem.info.component_id = get_comp_id(component);

            // Determine if this component can be started by the demod
            dctype = get_component_type(&tmp_elem, component);
            tmp_elem.component_type = dctype;

            if (!is_component_type_supported(dctype)) {
                // unsupported / error
                sl_addr += sizeof(dab_svc_component);
                continue;
            }

            //  This is a startable component type
            while (list->service_count >= DAB_SERVICE_LIST__MAX_SERVICES) {
                //SDK_PRINTF("Reached max services, trying to evict old ensembles...\n");
                bool_t xdata removed_elems = remove_old_ensemble();
                if (!removed_elems) {
                    have_space = false;
                    break;
                }
            }
            if (!have_space) {
                CALLBACK_Updated_Data(SCB_SERVICE_LIST_BUFFER_FULL_ERROR);
                break;
            }

            // Write the service info to the service list
            new_elem = &list->services[list->service_count];
            CpyMemory(new_elem, &tmp_elem, sizeof(dab_service_list_element));
            list->service_count++;

            // point sl_addr to the next component in the list.
            sl_addr += sizeof(dab_svc_component);
        }
    }

    return SUCCESS;
}

// Replace new service labels with the component labels
// DRUK indicates the component info is preferred over service info if present.
static RETURN_CODE replace_service_labels(uint8_t first_svc_index)
{
    RETURN_CODE xdata ret = SUCCESS;
	uint8_t xdata i;
	dab_service_list* xdata list = &g_dab_svc_list;
	
    for (i = first_svc_index; i < list->service_count; i++) {
        demod_dab_component_info_t xdata dgci = { 0 };
        dab_service_list_element* xdata tmp_elem = &list->services[i];

        // !allocates memory to dgci.UADATA pointer, we must free.
        ret |= si461x_get_component_info(
            tmp_elem->info.service_id,
            tmp_elem->info.component_id, &dgci);

        if ((ret == SUCCESS) && (dgci.LABEL[0] != 0)) {
            CpyMemory(tmp_elem->service_name.name, dgci.LABEL, DAB_SERVICE_LIST_SERVICE_LABEL__SIZE);
            tmp_elem->service_name.encoding = dgci.CHARSETID;
        }
        //free(dgci.UADATA);
    }
    return ret;
}

static void clean_freqs(dab_freq* freqs, uint8_t* freq_count, uint8_t min_cnr)
{
	uint8_t xdata i, j;
    if (!freqs || !freq_count)
        return;

    for (i = 0; i < *freq_count;) {
        if (freqs[i].cnr <= min_cnr) {
            for (j = i; j + 1 < *freq_count; j++) {
                freqs[j] = freqs[j + 1];
            }
            *freq_count -= 1;
        } else {
            i++;
        }
    }
}

RETURN_CODE dabsl_update_list(demo_digital_service_list_t* list_data, uint16_t list_version,
    dab_freq* freq, uint16_t ensemble_id)
{
    RETURN_CODE xdata ret = SUCCESS;
    dab_ensemble_info xdata orig_ens;
    int16_t xdata ens_offset_index = -1;
	uint8_t xdata orig_svc_count;
	uint8_t xdata i, j;
	
	dab_service_list* xdata list = &g_dab_svc_list;

    // dabList must have data before call
    if (!list_data || !list_data->DATA)
        return INVALID_INPUT;

    if ((list_version == list->service_list_version)
        && (list->service_list_version != DAB_LIST_VERSION_UNKNOWN)) {
        // nothing to update
        return SUCCESS;
    }
    CALLBACK_Updated_Data(SCB_DAB_SERVICE_LIST_REQUIRED_UPDATE);

    // Remove current frequency index from other ensemble's linked freqs
    // NOTE: any ensembles that no longer have frequencies will be removed (along with their services)
    remove_ensemble_freq(freq->freq_hz);

    // Remove existing ensemble information.
	ens_offset_index = -1;
    ret = dabsl_find_ensemble(ensemble_id, &ens_offset_index);
    if (ret != SUCCESS)
        return ret;

    if (ens_offset_index >= 0) {
        uint8_t xdata num_removed = 0;
        // Remove ensembles' services from list
        orig_ens = list->ensembles[ens_offset_index];
        ret = remove_ensemble((uint8_t)ens_offset_index, &num_removed);
        if (ret != SUCCESS) {
            error_in_list(list_version);
            return ret;
        }
    } else {
        orig_ens.eid = ensemble_id;
        orig_ens.freq_count = 0;
    }

    // Variables for confirming playing service is present in updated list
    orig_svc_count = list->service_count;


    // Add new services to the list
    ret = process_dab_list(freq->freq_hz, ensemble_id, list_data);
    if (ret != SUCCESS)
        return ret;

    // Add ensemble information back to list
    for (i = 0; i < list->service_count; i++) {
        if (list->services[i].info.ensemble_id == ensemble_id) {
            orig_ens.offset = i;
            break;
        }
    }
    for (i = 0; i < orig_ens.freq_count;) {
        if (orig_ens.freqs[i].freq_hz == freq->freq_hz) {
            // remove index
            for (j = i; j + 1 < orig_ens.freq_count; j++) {
                orig_ens.freqs[j] = orig_ens.freqs[j + 1];
            }
            orig_ens.freq_count--;
        } else {
            i++;
        }
    }

    if (orig_ens.freq_count == DAB_SERVICE_LIST__MAX_NUMBER_OF_LINKED_FREQUENCIES) {
        // Must replace an existing frequency.
        // First try to clean out list - remove frequencies with 0 CNR
        clean_freqs(orig_ens.freqs, &orig_ens.freq_count, 0);
    }
    if (orig_ens.freq_count == DAB_SERVICE_LIST__MAX_NUMBER_OF_LINKED_FREQUENCIES) {
        // Remove those weaker than current
        clean_freqs(orig_ens.freqs, &orig_ens.freq_count, freq->cnr);
    }
    if (orig_ens.freq_count == DAB_SERVICE_LIST__MAX_NUMBER_OF_LINKED_FREQUENCIES) {
        // Must replace something.
        uint8_t xdata i_replace = DAB_SERVICE_LIST__MAX_NUMBER_OF_LINKED_FREQUENCIES - 1;
        //orig_ens.freqs[i_replace] = freq;
		memcpy(orig_ens.freqs+i_replace, freq, sizeof(dab_freq));
    } else {
		memcpy(orig_ens.freqs+orig_ens.freq_count++, freq, sizeof(dab_freq));
        //orig_ens.freqs[orig_ens.freq_count++] = freq;
    }
	
	memcpy(list->ensembles+list->ensemble_count, &orig_ens, sizeof(dab_ensemble_info));
    //list->ensembles[list->ensemble_count++] = orig_ens;
	list->ensemble_count++;

    // Use component labels if available
    ret |= replace_service_labels(orig_svc_count);

    // Sort the new services added by new labels.
    ret |= insertion_sort_alphabetical_list(orig_svc_count, list->service_count);

    // Keep track of this version to avoid reprocessing it
    list->list_updated_for_freq = true;
    list->service_list_version = list_version;

    //Tell the MMI we have a new service list with the new list
    CALLBACK_Updated_Data(SCB_UPDATED_SERVICE_LIST_DAB);

    return ret;
}
static bool_t freq_in_list(uint8_t test, uint32_t* freqs, uint8_t freq_count)
{
	uint8_t xdata i;
    if (!freqs)
        return false;

    for (i = 0; i < freq_count; i++) {
        if (freqs[i] == test)
            return true;
    }
    return false;
}

// Add all matching SID+CID entries from the service list to 'candidate' list.
// Only append, do not clear existing variable values.
void dabsl_find_matching_services(uint8_t svc_index,
    uint8_t max, uint8_t* candidates, uint8_t* candidate_count)
{
	uint8_t xdata i;
	dab_service_list* xdata sl = &g_dab_svc_list;

    // Find all matching SID+CID entries in service list.
    dab_service_info xdata target;// = sl->services[svc_index].info;
	
	dab_service_info_copy(&target, &sl->services[svc_index].info);
	
    for (i = 0; i < sl->service_count; i++) {
        if (sl->services[i].info.service_id == target.service_id
            && sl->services[i].info.component_id == target.component_id) {
            if (*candidate_count >= max) {
                //c_log_commentf("INFO: find_matching_services() reached max\n");
                return;
            }

            candidates[*candidate_count] = i;
            *candidate_count += 1;
        }
    }
}

// Add the services' ensemble known frequencies' indices to 'candidate' list.
// Only append, do not reset the current variables' values.
void dabsl_find_candidate_freqs(uint8_t svc_index,
    uint8_t max, uint32_t* candidates, uint8_t* candidate_count)
{
	uint8_t xdata i;
    dab_service_info xdata target;
    dab_ensemble_info* xdata ens = NULL;
	dab_service_list* xdata sl = &g_dab_svc_list;
	
	dab_service_info_copy(&target, &(sl->services[svc_index].info));
	
    // Get matching ensemble info
    for (i = 0; i < sl->ensemble_count; i++) {
        if (sl->ensembles[i].eid == target.ensemble_id) {
            ens = &sl->ensembles[i];
            break;
        }
    }

    if (!ens) {
        //c_log_commentf("ERROR: find_candidate_freqs() - failed to find ensemble 0x%x\n", target.ensemble_id);
        // Use services' stored freq as backup
        if (!freq_in_list(target.freq, candidates, *candidate_count)) {
            if (*candidate_count >= max) {
                //c_log_commentf("INFO: find_candidate_freqs(): reached max DAB test freqs\n");
                return;
            }
            candidates[*candidate_count] = target.freq;
            *candidate_count += 1;
        }

        return;
    }

    // Add unique frequency indices
    for (i = 0; i < ens->freq_count; i++) {
        uint32_t xdata test_freq = ens->freqs[i].freq_hz;
        if (!freq_in_list(test_freq, candidates, *candidate_count)) {
            if (*candidate_count >= max) {
                //c_log_commentf("INFO: find_candidate_freqs(): reached max DAB test freqs\n");
                return;
            }
            candidates[*candidate_count] = test_freq;
            *candidate_count += 1;
        }
    }
}

