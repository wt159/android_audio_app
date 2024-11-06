/***************************************************************************************
                  Skyworks MSS Automotive Tuner + Demod SDK

   EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND CONDITIONS OF
     THE SOFTWARE LICENSE AGREEMENT IN THE DOCUMENTATION FILE CORRESPONDING
     TO THIS SOURCE FILE.
   IF YOU DO NOT AGREE TO THE LIMITED LICENSE AND CONDITIONS OF SUCH AGREEMENT,
     PLEASE RETURN ALL SOURCE FILES TO SKYWORKS.

   SDK DAB DLS decoder
   FILE: DAB_DLS_Handler.c
   Supported IC : Si479xx + Si461x
   Date: September 21, 2015
  (C) Copyright 2015, Skyworks, Inc. All rights reserved.
****************************************************************************************/

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "DAB_DLS_handler.h"
#include "ui_callbacks.h"
#include "si461x_dab_api_constants.h"

#ifdef SUPPORT_DAB_DLS

//-----------------------------------------------------------------------------
// local defines
//-----------------------------------------------------------------------------
#define PREFIX0__PAYLOAD_INDEX 0
#define PREFIX0__TOGGLE_MASK 0x80
#define PREFIX0__TOGGLE_SHIFT 7
#define PREFIX0__COMMAND_MASK 0x10
#define PREFIX0__COMMAND_TYPE_MASK 0x0F
#define PREFIX0__COMMAND_DLS_MESSAGE 0x00
#define PREFIX0__COMMAND_DLS_CLEAR 0x01
#define PREFIX0__COMMAND_DL_PLUS_COMMAND 0x10

#define PREFIX1__PAYLOAD_INDEX 1
#define PREFIX1__DLS_CHARSET_MASK 0xF0
#define PREFIX1__DLS_CHARSET_SHIFT 4
#define PREFIX1__COMMAND_LINK_MASK 0x10

#define DLS_MESSAGE_DATA__PAYLOAD_INDEX 2

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------

// DLS Basic String Data
static void update_generic_dls_string(dab_dls_string* dest, uint8_t* src, uint8_t len)
{
    D_ASSERT(dest);
    dest->LENGTH = len;
    if (len > DAB_DLS_BUFFER_SIZE) {
        dest->LENGTH = DAB_DLS_BUFFER_SIZE;
    } else {
        dest->LENGTH = len;
    }
    CpyMemory(dest->DATA, src, dest->LENGTH);
    CALLBACK_Updated_Data(SCB_UPDATED_DLS_STRING);
}

//-----------------------------------------------------------------------------
// After changing services, the DLS tracking variables need to be initialized.
//-----------------------------------------------------------------------------
void dls_init(dab_dls_string* dls)
{
    D_ASSERT(dls);
    dls->ENCODING = DLS_ENCODING_RESERVED;
    dls->TOGGLE = 0;
    dls->LENGTH = 0;
    ClearMemory(dls->DATA, sizeof(dls->DATA));
}

//-----------------------------------------------------------------------------
// After a DLS interrupt, process the data.
//-----------------------------------------------------------------------------

void dls_update(dab_dls_string* dls, demod_dab_digital_service_t dlsData)
{
    D_ASSERT(dls);

    if (dlsData.DATA_SRC != SI461X_DAB_CMD_GET_DIGITAL_SERVICE_DATA_REP_DATA_SRC_ENUM_PAD_DLS)
        return;

    if (((dlsData.PAYLOAD[PREFIX0__PAYLOAD_INDEX] & PREFIX0__COMMAND_MASK) == 0)) {
        dls->TOGGLE = (dlsData.PAYLOAD[PREFIX0__PAYLOAD_INDEX] & PREFIX0__TOGGLE_MASK)
            >> PREFIX0__TOGGLE_SHIFT;
        dls->ENCODING = (DAB_DLS_ENCODING_TYPE)((dlsData.PAYLOAD[PREFIX1__PAYLOAD_INDEX] & PREFIX1__DLS_CHARSET_MASK)
            >> PREFIX1__DLS_CHARSET_SHIFT);

        if (dls->ENCODING == 4)
            dls->ENCODING = 6;

        update_generic_dls_string(dls,
            &dlsData.PAYLOAD[DLS_MESSAGE_DATA__PAYLOAD_INDEX],
            (uint8_t)(dlsData.BYTE_COUNT - DLS_MESSAGE_DATA__PAYLOAD_INDEX));
    } else {
        //We have a DL+ command
        if ((dlsData.PAYLOAD[PREFIX0__PAYLOAD_INDEX] & PREFIX0__COMMAND_TYPE_MASK) == PREFIX0__COMMAND_DLS_CLEAR) {
            dls_init(dls);
            CALLBACK_Updated_Data(SCB_CLEAR_DLS_COMMAND);
        } else {
            //TODO: add DL+ support for larger memory hosts
        }
    }
}

#endif //OPTION__DECODE_DLS
