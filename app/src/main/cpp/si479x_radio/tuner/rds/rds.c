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
#include <stdio.h>
#include <string.h>

#include "dc_config.h"

#include "rds.h"
//#include "core.h"
//#include "commanddefs.h"
//#include "propertydefs.h"
//#include "flashload.h"
//#include "hifi.h"
//#include "audio.h"
//#include "rds.h"

#ifdef SUPPORT_RDS

#include "si479x_rds.h"
#include "si479x_internel.h"

rds_status_t g_rds_status;
rds_data_t g_rds_data;
rds_ps_helper_t g_rds_ps_helper;
rds_rt_helper_t g_rds_rt_helper;
rds_time_t g_rds_time;
uint8_t bler[4];

static RET_CODE get_rds_status(rds_status_t* pStauts)
{
	RET_CODE ret = RET_SUCCESS;

	ERR_CHECKING(si479x_rds_status(1, 0, 0));

	pStauts->ints = rsp_buff[4] & 0x1f;
	pStauts->flags = rsp_buff[5] & 0x1f;
	pStauts->tp = (rsp_buff[6] & 0x20) >> 5;
	pStauts->pty = rsp_buff[6] & 0x1f;
	pStauts->pi = (uint16_t)rsp_buff[8] | (uint16_t)rsp_buff[9] << 8;
	pStauts->fifoused = rsp_buff[10];

	pStauts->blera = (rsp_buff[11] & 0xc0) >> 6;
	pStauts->blerb = (rsp_buff[11] & 0x30) >> 4;
	pStauts->blerc = (rsp_buff[11] & 0x0c) >> 2;
	pStauts->blerd = (rsp_buff[11] & 0x03) ;

	pStauts->blocka = (uint16_t)rsp_buff[12] | (uint16_t)rsp_buff[13] << 8;
	pStauts->blockb = (uint16_t)rsp_buff[14] | (uint16_t)rsp_buff[15] << 8;
	pStauts->blockc = (uint16_t)rsp_buff[16] | (uint16_t)rsp_buff[17] << 8;
	pStauts->blockd = (uint16_t)rsp_buff[18] | (uint16_t)rsp_buff[19] << 8;

	return ret;
}

static void update_pi()
{
	g_rds_data.pi = g_rds_status.pi;
}

static void update_pty()
{
	g_rds_data.pty = g_rds_status.pty;
}

static void init_alt_freq(void)
{
	uint8_t i;
	g_rds_data.af_count = 0;
	for (i = 0; i < sizeof(g_rds_data.af_list)/sizeof(g_rds_data.af_list[0]); i++)
	{
		g_rds_data.af_list[i] = 0;
	}
}


static void update_alt_freq(uint16_t current_alt_freq)
{
	// Currently this only works well for AF method A, though AF method B
	// data will still be captured.
	uint8_t dat;
	uint8_t i;
	uint16_t freq;

	// the top 8 bits is either the AF Count or AF Data
	dat = (uint8_t)(current_alt_freq >> 8);
	// look for the AF Count indicator
	if ((dat >= AF_COUNT_MIN) && (dat <= AF_COUNT_MAX) && 
			((dat - AF_COUNT_MIN) != g_rds_data.af_count))
	{
		init_alt_freq();  // clear the alternalte frequency list
		g_rds_data.af_count = (dat - AF_COUNT_MIN); // set the count
		dat = (uint8_t)current_alt_freq;
		if (dat >= AF_FREQ_MIN && dat <= AF_FREQ_MAX)
		{
			freq = AF_FREQ_TO_U16F(dat);
			g_rds_data.af_list[0]= freq;
		}
	}
	// look for the AF Data
	else if (g_rds_data.af_count && dat >= AF_FREQ_MIN && dat <= AF_FREQ_MAX)
	{
		uint8_t foundSlot = 0;
		static uint8_t clobber=1;  // index to clobber if no empty slot is found
		freq = AF_FREQ_TO_U16F(dat);
		for (i=1; i < g_rds_data.af_count; i+=2)
		{
			// look for either an empty slot or a match
			if ((!g_rds_data.af_list[i]) || (g_rds_data.af_list[i] == freq))
			{
				g_rds_data.af_list[i] = freq;
				dat = (uint8_t)current_alt_freq;
				freq = AF_FREQ_TO_U16F(dat);
				g_rds_data.af_list[i+1] = freq;
				foundSlot = 1;
				break;
			}
		}
		// If no empty slot or match was found, overwrite a 'random' slot.
		if (!foundSlot)
		{
			clobber += (clobber&1) + 1; // this ensures that an odd slot is always chosen.
			clobber %= (g_rds_data.af_count);       // keep from overshooting the array
			g_rds_data.af_list[clobber] = freq;
			dat = (uint8_t)current_alt_freq;
			freq = AF_FREQ_TO_U16F(dat);
			g_rds_data.af_list[clobber+1] = freq;
		}
	}

}

static void update_ps(uint8_t addr, uint8_t byte)
{
	uint8_t i;
	uint8_t textChange = 0; // indicates if the PS text is in transition
	uint8_t psComplete = 1; // indicates that the PS text is ready to be displayed

	if(g_rds_ps_helper.ps_high[addr] == byte)
	{
		// The new byte matches the high probability byte
		if(g_rds_ps_helper.ps_cnt[addr] < PS_VALIDATE_LIMIT)
		{
			g_rds_ps_helper.ps_cnt[addr]++;
		}
		else
		{
			// we have recieved this byte enough to max out our counter
			// and push it into the low probability array as well
			g_rds_ps_helper.ps_cnt[addr] = PS_VALIDATE_LIMIT;
			g_rds_ps_helper.ps_low[addr] = byte;
		}
	}
	else if(g_rds_ps_helper.ps_low[addr] == byte)
	{
		// The new byte is a match with the low probability byte. Swap
		// them, reset the counter and flag the text as in transition.
		// Note that the counter for this character goes higher than
		// the validation limit because it will get knocked down later
		if(g_rds_ps_helper.ps_cnt[addr] >= PS_VALIDATE_LIMIT)
		{
			textChange = 1;
			g_rds_ps_helper.ps_cnt[addr] = PS_VALIDATE_LIMIT + 1;
		}
		else
		{
			g_rds_ps_helper.ps_cnt[addr] = PS_VALIDATE_LIMIT;
		}
		g_rds_ps_helper.ps_low[addr] = g_rds_ps_helper.ps_high[addr];
		g_rds_ps_helper.ps_high[addr] = byte;
	}
	else if(!g_rds_ps_helper.ps_cnt[addr])
	{
		// The new byte is replacing an empty byte in the high
		// proability array
		g_rds_ps_helper.ps_high[addr] = byte;
		g_rds_ps_helper.ps_cnt[addr] = 1;
	}
	else
	{
		// The new byte doesn't match anything, put it in the
		// low probablity array.
		g_rds_ps_helper.ps_low[addr] = byte;
	}

	if(textChange)
	{
		// When the text is changing, decrement the count for all
		// characters to prevent displaying part of a message
		// that is in transition.
		for(i=0; i<8; i++)
		{
			if(g_rds_ps_helper.ps_cnt[i] > 1)
			{
				g_rds_ps_helper.ps_cnt[i]--;
			}
		}
	}

	// The PS text is incomplete if any character in the high
	// probability array has been seen fewer times than the
	// validation limit.
	for (i=0; i<8; i++)
	{
		if(g_rds_ps_helper.ps_cnt[i] < PS_VALIDATE_LIMIT)
		{
			psComplete = 0;
			break;
		}
	}

	// If the PS text in the high probability array is complete
	// copy it to the display array
	if (psComplete)
	{
		for (i=0; i<8; i++)
		{
			g_rds_data.ps[i] = g_rds_ps_helper.ps_high[i];
		}

		g_rds_data.ps_valid = 1;
	}
}

static void display_rt(void)
{
	uint8_t rtComplete = 1;
	uint8_t i;

	// The Radio Text is incomplete if any character in the high
	// probability array has been seen fewer times than the
	// validation limit.
	for (i=0; i<64; i++)
	{
		if(g_rds_rt_helper.rt_cnt[i] < RT_VALIDATE_LIMIT)
		{
			rtComplete = 0;
			break;
		}

		if(g_rds_rt_helper.rt_high[i] == 0x0d)
		{
			// The array is shorter than the maximum allowed
			break;
		}
	}

	// If the Radio Text in the high probability array is complete
	// copy it to the display array
	if (rtComplete)
	{
		g_rds_data.rt_valid = 1;
		for (i=0; i<64; i++)
		{
			g_rds_data.rt[i] = g_rds_rt_helper.rt_high[i];
			if(g_rds_rt_helper.rt_high[i] == 0x0d)
			{
				break;
			}
		}

		// Wipe out everything after the end-of-message marker
		for (i++; i<64; i++)
		{
			g_rds_data.rt[i] = 0;
			g_rds_rt_helper.rt_cnt[i]     = 0;
			g_rds_rt_helper.rt_high[i]    = 0;
			g_rds_rt_helper.rt_low[i]    = 0;
		}
	}
}


static void update_rt_advance(uint8_t abFlag, uint8_t count, uint8_t addr, uint8_t * byte)
{

	uint8_t i;
	uint8_t textChange = 0; // indicates if the Radio Text is changing

	if (abFlag != g_rds_rt_helper.ab_flag && g_rds_rt_helper.ab_flag_valid)
	{
		// If the A/B message flag changes, try to force a display
		// by increasing the validation count of each byte
		// and stuffing a space in place of every NUL char
		for (i=0; i<sizeof(g_rds_rt_helper.rt_cnt); i++)
		{
			if (!g_rds_rt_helper.rt_high[i])
			{
				g_rds_rt_helper.rt_high[i] = ' ';
				g_rds_rt_helper.rt_cnt[i]++;
			}
		}

		for (i=0; i<sizeof(g_rds_rt_helper.rt_cnt); i++)
		{
			g_rds_rt_helper.rt_cnt[i]++;
		}

		display_rt();

		// Wipe out the cached text
		for (i=0; i<sizeof(g_rds_rt_helper.rt_cnt); i++)
		{
			g_rds_rt_helper.rt_cnt[i]  = 0;
			g_rds_rt_helper.rt_high[i] = 0;
			g_rds_rt_helper.rt_low[i] = 0;
		}
	}

	g_rds_rt_helper.ab_flag = abFlag;    // Save the A/B flag
	g_rds_rt_helper.ab_flag_valid = 1;    // Our copy of the A/B flag is now valid

	for (i=0; i<count; i++)
	{
		uint8_t errCount;
		uint8_t blerMax;
		// Choose the appropriate block. Count > 2 check is necessary for 2B groups
		if ((i < 2) && (count > 2))
		{
			errCount = g_rds_status.blerc;
			blerMax = RDS_BLOCKC_TOLERRENCE;
		}
		else
		{
			errCount = g_rds_status.blerd;
			blerMax = RDS_BLOCKD_TOLERRENCE;
		}

		if (errCount <= blerMax)
		{
			if(!byte[i])
			{
				byte[i] = ' '; // translate nulls to spaces
			}

			// The new byte matches the high probability byte
			if(g_rds_rt_helper.rt_high[addr+i] == byte[i])
			{
				if(g_rds_rt_helper.rt_cnt[addr+i] < RT_VALIDATE_LIMIT)
				{
					g_rds_rt_helper.rt_cnt[addr+i]++;
				}
				else
				{
					// we have recieved this byte enough to max out our counter
					// and push it into the low probability array as well
					g_rds_rt_helper.rt_cnt[addr+i] = RT_VALIDATE_LIMIT;
					g_rds_rt_helper.rt_low[addr+i] = byte[i];
				}
			}
			else if(g_rds_rt_helper.rt_low[addr+i] == byte[i])
			{
				// The new byte is a match with the low probability byte. Swap
				// them, reset the counter and flag the text as in transition.
				// Note that the counter for this character goes higher than
				// the validation limit because it will get knocked down later
				if(g_rds_rt_helper.rt_cnt[addr+i] >= RT_VALIDATE_LIMIT)
				{
					textChange = 1;
					g_rds_rt_helper.rt_cnt[addr+i] = RT_VALIDATE_LIMIT + 1;
				}
				else
				{
					g_rds_rt_helper.rt_cnt[addr+i] = RT_VALIDATE_LIMIT;
				}
				g_rds_rt_helper.rt_low[addr+i] = g_rds_rt_helper.rt_high[addr+i];
				g_rds_rt_helper.rt_high[addr+i] = byte[i];
			}
			else if(!g_rds_rt_helper.rt_cnt[addr+i])
			{
				// The new byte is replacing an empty byte in the high
				// proability array
				g_rds_rt_helper.rt_high[addr+i] = byte[i];
				g_rds_rt_helper.rt_cnt[addr+i] = 1;
			}
			else
			{
				// The new byte doesn't match anything, put it in the
				// low probablity array.
				g_rds_rt_helper.rt_low[addr+i] = byte[i];
			}
		}
	}

	if(textChange)
	{
		// When the text is changing, decrement the count for all
		// characters to prevent displaying part of a message
		// that is in transition.
		for(i=0; i<sizeof(g_rds_rt_helper.rt_cnt); i++)
		{
			if(g_rds_rt_helper.rt_cnt[i] > 1)
			{
				g_rds_rt_helper.rt_cnt[i]--;
			}
		}
	}

	// Display the Radio Text
	display_rt();
}


static void update_clock(uint16_t b, uint16_t c, uint16_t d)
{
	if (g_rds_status.blerb <= RDS_BLOCKB_TOLERRENCE &&
			g_rds_status.blerc <= RDS_BLOCKC_TOLERRENCE &&
			g_rds_status.blerd <= RDS_BLOCKD_TOLERRENCE &&
			(g_rds_status.blerb + g_rds_status.blerc + g_rds_status.blerd) <= RDS_BLOCKB_TOLERRENCE) 
	{
		g_rds_time.day_high= (b >> 1) & 1;
		g_rds_time.day_low = (b << 15) | (c >> 1);
		g_rds_time.hour = ((c&1) << 4) | (d >> 12);
		g_rds_time.minute= (d>>6) & 0x3F;
		g_rds_time.offset = d & 0x1F;
		if (d & (1<<5))
		{
			g_rds_time.offset = -g_rds_time.offset;
		}
	}
}

void rds_process(void)
{
	RET_CODE ret = RET_SUCCESS;

	uint8_t rtblocks[4];
	uint8_t group_type;      // bits 4:1 = type,  bit 0 = version
	uint8_t addr;
	uint8_t abflag;

	ret = get_rds_status(&g_rds_status);
	if (ret != RET_SUCCESS)
		return;

	// Loop until all the RDS information has been read from the part.
	if(g_rds_status.fifoused)
	{
		// Update pi code.
		if (g_rds_status.flags & PIVALID_MASK)
		{
			update_pi();
		}

		if (g_rds_status.blerb <= RDS_BLOCKB_TOLERRENCE)
		{
			group_type = g_rds_status.blockb >> 11;  // upper five bits are the group type and version
		}
		else
		{
			// Drop the data if more than two errors were corrected in block B
			return;
		}

		// update pty code.
		if (g_rds_status.flags & TPPTYVALID_MASK)
		{
			update_pty();
		}

		switch (group_type) {
			case RDS_TYPE_0A:
				if (g_rds_status.blerc <= RDS_BLOCKC_TOLERRENCE)
				{
					update_alt_freq(g_rds_status.blockc);
				}
				// fallthrough
			case RDS_TYPE_0B:
				addr = (g_rds_status.blockb & 0x3) * 2;
				if (g_rds_status.blerd <= RDS_BLOCKD_TOLERRENCE)
				{
					update_ps(addr+0, g_rds_status.blockd >> 8  );
					update_ps(addr+1, g_rds_status.blockd & 0xff);
				}
				break;

			case RDS_TYPE_2A:
				{
					rtblocks[0] = (uint8_t)(g_rds_status.blockc >> 8);
					rtblocks[1] = (uint8_t)(g_rds_status.blockc & 0xFF);
					rtblocks[2] = (uint8_t)(g_rds_status.blockd >> 8);
					rtblocks[3] = (uint8_t)(g_rds_status.blockd & 0xFF);
					addr = (g_rds_status.blockb & 0xf) * 4;
					abflag = (g_rds_status.blockb & 0x0010) >> 4;

					update_rt_advance(abflag, 4, addr, rtblocks);
					break;
				}

			case RDS_TYPE_2B:
				{
					rtblocks[0] = (uint8_t)(g_rds_status.blockd >> 8);
					rtblocks[1] = (uint8_t)(g_rds_status.blockd & 0xFF);
					rtblocks[2] = 0;
					rtblocks[3] = 0;
					addr = (g_rds_status.blockb & 0xf) * 2;
					abflag = (g_rds_status.blockb & 0x0010) >> 4;
					// The last 32 bytes are unused in this format
					g_rds_rt_helper.rt_high[32]    = 0x0d;
					g_rds_rt_helper.rt_low[32]    = 0x0d;
					g_rds_rt_helper.rt_cnt[32]     = RT_VALIDATE_LIMIT;
					update_rt_advance(abflag, 2, addr, rtblocks);
					break;
				}
			case RDS_TYPE_4A:
				// Clock Time and Date
				update_clock(g_rds_status.blockb, g_rds_status.blockc, g_rds_status.blockd);
				break;
			default:
				break;
		}

		// Get the RDS status from the part.
		ret = get_rds_status(&g_rds_status);
		if (ret != RET_SUCCESS)
			return;
	}
}

void rds_reset()
{
	memset(&g_rds_status, 0, sizeof(g_rds_status));
	memset(&g_rds_data, 0, sizeof(g_rds_data));
	memset(&g_rds_ps_helper, 0, sizeof(g_rds_ps_helper));
	memset(&g_rds_rt_helper, 0, sizeof(g_rds_rt_helper));
	memset(&g_rds_time, 0, sizeof(g_rds_time));

	// Gather the latest BLER info
	bler[0] = g_rds_status.blera;
	bler[1] = g_rds_status.blerb;
	bler[2] = g_rds_status.blerc;
	bler[3] = g_rds_status.blerd;
}

uint8_t rds_ps_available()
{
	return g_rds_data.ps_valid;
}

uint8_t rds_rt_available()
{
	return g_rds_data.rt_valid;
}

uint8_t * rds_ps_data()
{
	return g_rds_data.ps;
}


uint8_t * rds_rt_data()
{
	return g_rds_data.rt;
}

#endif

