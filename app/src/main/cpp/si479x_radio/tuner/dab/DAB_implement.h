/***************************************************************************************
                  Skyworks MSS Automotive Tuner + Demod SDK

   EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND CONDITIONS OF
     THE SOFTWARE LICENSE AGREEMENT IN THE DOCUMENTATION FILE CORRESPONDING
     TO THIS SOURCE FILE.
   IF YOU DO NOT AGREE TO THE LIMITED LICENSE AND CONDITIONS OF SUCH AGREEMENT,
     PLEASE RETURN ALL SOURCE FILES TO SKYWORKS.

  Implementation of IFirmwareApi for the 3T2D
   FILE: Implementation_3T2D.cpp
   Date:  June 28, 2016
  (C) Copyright 2015, Skyworks, Inc. All rights reserved.
****************************************************************************************/
#ifndef DAB_IMPLEMENT_H_
#define DAB_IMPLEMENT_H_

#include "DAB_service_list_handler.h"
#include "dab_common.h"
#include "type.h"

// Tunes, acquires and optionally updates the global service list.
RETURN_CODE DAB_tune_full_process(uint32_t freq, bool_t update_svclist, dab_tune_status* status);

RETURN_CODE DAB_StartServiceByIndex(uint8_t svcIndex);

RETURN_CODE DAB_StartCurrentSelection();

RETURN_CODE DAB_StartNextSelection();

RETURN_CODE DAB_StartPrevSelection();

RETURN_CODE ScanBand_DAB();

RETURN_CODE SaveServiceList_DAB();

RETURN_CODE LoadServiceList_DAB();

void DAB_init();

dab_service_list_element* DAB_GetCurrentServiceElementInfo();

dab_service_list* DAB_GetServiceList();

#define DAB_STORAGE_MAX_ENS_COUNT 6
RETURN_CODE LoadEnsemblesTable_DAB(uint8_t* pnCount, uint16_t* pEnsIndsTable);

RETURN_CODE DAB_UpdateDataServiceData();

RETURN_CODE DAB_UpdateServiceList();

extern dab_system_data xdata g_dab_state;
extern dab_service_list xdata g_dab_svc_list;
extern dab_service_list_element xdata g_dab_svc_list_elem;

#endif
