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
#include "DAB_nvm_storage.h"

static uint16_t xdata _file_pos = 0;

static size_t eeprom_rw_pos(bool_t read, uint8_t* buf, size_t buflen)
{
	uint16_t xdata cur = _file_pos + DAB_SERVICE_STORE_ADDR;
	uint16_t xdata i;
	
    if (!buf || _file_pos + buflen + DAB_SERVICE_STORE_ADDR > DAB_SERVICE_STORE_ADDR_TAIL) {
        //D_PRINTF("ERROR: invalid input fwrite_pos()\n");
        return 0;
    }
	
	for (i=0; i<buflen; i++)
	{
		if (read == true)
			buf[i] = I2cReadByte(cur);
		else
			I2cWriteByte(cur, buf[i]);
		
		cur ++;
	}
	
	_file_pos = cur-DAB_SERVICE_STORE_ADDR;
	
    return buflen;
}

size_t f_write(uint8_t* buf, size_t buflen)
{
	return eeprom_rw_pos(false, buf, buflen);
}

size_t f_read(uint8_t* buf, size_t buflen)
{
	return eeprom_rw_pos(true, buf, buflen);
}

void f_seek(uint8_t npos)
{
	_file_pos = npos; //;
}

