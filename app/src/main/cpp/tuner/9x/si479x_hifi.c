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

#include "type.h"

#include "si479x_internel.h"
#include "dc_bootup.h"
#include "si479x_hifi.h"
#include "si479x_commanddefs.h"

hifi_resp_t _hifi_resp;

RET_CODE _si479x_waitForACTS(uint8_t ACTS)
{
    uint8_t i = 50;

	do
	{
		si479x_readReply(4, rsp_buff);
        
        if ((rsp_buff[2] & ACTS) == ACTS)
        {
            return RET_SUCCESS;
        }
            
        sleep_ms(10);
	}while (--i);

    return RET_TIMEOUT;    
}

RET_CODE si479x_waitForACTS0()
{
    return _si479x_waitForACTS(ACTS0);
}

RET_CODE si479x_waitForACTS1()
{
    return _si479x_waitForACTS(ACTS1);
}

/**
RET_CODE si479x_waitForACTS2()
{
    return _si479x_waitForACTS(ACTS2);
}
*/

RET_CODE si479x_hifi_cmd(uint8_t cmd, uint8_t count, uint16_t length)
{
    RET_CODE ret = RET_SUCCESS;

    cmd_buff[0] = cmd;
    cmd_buff[1] = count;

    _si479x_fill_cmd_uint16_t(cmd_buff, 2, length);

    ERR_CHECKING(si479x_command(4+length, cmd_buff, 4, rsp_buff));

    //delay 10ms to make sure the hifi is availble for next command, work round solution....
    sleep_ms(10);

    return ret;
}



RET_CODE si479x_hifi_resp(uint8_t resp_cmd, hifi_resp_t *presp)
{    
    RET_CODE ret = RET_SUCCESS;
    
    uint8_t i;
    
    clear_cmd_memory();

    cmd_buff[0] = resp_cmd;

    ERR_CHECKING(si479x_command(2, cmd_buff, 8, rsp_buff));

    presp->pipeid = rsp_buff[4];
    presp->count = rsp_buff[5];
    presp->length = ((uint16_t)rsp_buff[6] & 0x00ff) | 
        (((uint16_t)rsp_buff[7] << 8) & 0xff00);

    if (presp->length > RSQ_BUFF_SIZE-8)
        return RET_INVALIDOUTPUT;

	ERR_CHECKING(si479x_command(2, cmd_buff, 8+presp->length, rsp_buff));
	
    for (i=0; i<presp->length; i++)
    {
        presp->rdata[i] = rsp_buff[i+8];
    }

    return ret;
}

RET_CODE si479x_hifi_cmd0(uint8_t count, uint16_t length)
{
    return si479x_hifi_cmd(HIFI_CMD0, count, length);
}

RET_CODE si479x_hifi_cmd1(uint8_t count, uint16_t length)
{
    return si479x_hifi_cmd(HIFI_CMD1, count, length);
}

/**
RET_CODE si479x_hifi_cmd2(uint8_t count, uint16_t length)
{
    return si479x_hifi_cmd(HIFI_CMD2, count, length);
}
*/

RET_CODE si479x_hifi_resp0(hifi_resp_t *presp)
{    
    return si479x_hifi_resp(HIFI_RESP0, presp);
}

RET_CODE si479x_hifi_resp1(hifi_resp_t *presp)
{    
    return si479x_hifi_resp(HIFI_RESP1, presp);
}

/**
RET_CODE si479x_hifi_resp2(hifi_resp_t *presp)
{    
    return si479x_hifi_resp(HIFI_RESP2, presp);
}
*/

RET_CODE si479x_hifi_xbar_connect_node(hifi_port_node_connct_t connections[], uint8_t con_count)
{
    RET_CODE ret = RET_SUCCESS;
    uint8_t offset = 4;
	uint8_t i;

    if (4 + con_count * 12 > CMD_BUFF_SIZE)
    {
        return RET_INVALIDINPUT;
    }

    clear_cmd_memory();
    for (i=0; i<con_count; i++)
    {
        cmd_buff[offset++] = XBAR_CONNECT_NOTE;
        cmd_buff[offset++] = 0;
        
        _si479x_fill_cmd_uint16_t(cmd_buff, offset, 8);
        offset += 2;

        cmd_buff[offset++] = connections[i].port;
        cmd_buff[offset++] = connections[i].channel&0x0f;
        cmd_buff[offset++] = connections[i].node;
        cmd_buff[offset++] = 0;

        _si479x_fill_cmd_uint16_t(cmd_buff, offset, connections[i].gain);
        offset += 4;
    }

    ERR_CHECKING(si479x_hifi_cmd0(con_count, offset-4));

    ERR_CHECKING(si479x_waitForACTS0());

    ERR_CHECKING(si479x_hifi_resp0(&_hifi_resp));
    
    return ret;
}

RET_CODE si479x_hifi_xbar_disconnect_node(hifi_port_node_connct_t connections[], uint8_t con_count)
{    
    RET_CODE ret = RET_SUCCESS;
    uint8_t offset = 4;
	uint8_t i;

    if (4 + con_count * 8 > CMD_BUFF_SIZE)
    {
        return RET_INVALIDINPUT;
    }

    clear_cmd_memory();
    for (i=0; i<con_count; i++)
    {
        cmd_buff[offset++] = XBAR_DISCONNECT_NOTE;
        cmd_buff[offset++] = 0;
        
        _si479x_fill_cmd_uint16_t(cmd_buff, offset, 4);
        offset += 2;

        cmd_buff[offset++] = connections[i].port;
        cmd_buff[offset++] = connections[i].channel&0x0f;
        cmd_buff[offset++] = connections[i].node;
        cmd_buff[offset++] = 0;
    }

    ERR_CHECKING(si479x_hifi_cmd0(con_count, offset-4));

    ERR_CHECKING(si479x_waitForACTS0());

    ERR_CHECKING(si479x_hifi_resp0(&_hifi_resp));
    
    return ret;
}

#if 0
RET_CODE si479x_hifi_xbar_query(uint8_t count, uint16_t length, uint8_t * pData)
{
}
#endif

RET_CODE si479x_hifi_xbar_connect_port(hifi_port_2_port_connct_t connections[], uint8_t con_count)
{
    RET_CODE ret = RET_SUCCESS;
    uint8_t offset = 4;
	uint8_t i;

    if (4 + con_count * 12 > CMD_BUFF_SIZE)
    {
        return RET_INVALIDINPUT;
    }

    clear_cmd_memory();

    for (i=0; i<con_count; i++)
    {
    
        cmd_buff[offset++] = XBAR_CONNECT_PORT;
        cmd_buff[offset++] = 0;
        
        _si479x_fill_cmd_uint16_t(cmd_buff, offset, 8);
        offset += 2;

        cmd_buff[offset++] = connections[i].inport;
        cmd_buff[offset++] = connections[i].inchannel;
        cmd_buff[offset++] = connections[i].outport;
        cmd_buff[offset++] = connections[i].outchannel;

        _si479x_fill_cmd_uint16_t(cmd_buff, offset, connections[i].gain);
        offset += 2;
        
        _si479x_fill_cmd_uint16_t(cmd_buff, offset, 0);
        offset += 2;
    }

    ERR_CHECKING(si479x_hifi_cmd0(con_count, offset-4));
    
    ERR_CHECKING(si479x_waitForACTS0());

    ERR_CHECKING(si479x_hifi_resp0(&_hifi_resp));
    
    return ret;
}

RET_CODE si479x_hifi_xbar_disconnect_port(hifi_port_2_port_connct_t connections[], uint8_t con_count)
{
    RET_CODE ret = RET_SUCCESS;
    uint8_t offset = 4;
	uint8_t i;

    if (4 + con_count * 8 > CMD_BUFF_SIZE)
    {
        return RET_INVALIDINPUT;
    }

    clear_cmd_memory();

    for (i=0; i<con_count; i++)
    {
    
        cmd_buff[offset++] = XBAR_DISCONNECT_PORT;
        cmd_buff[offset++] = 0;
        
        _si479x_fill_cmd_uint16_t(cmd_buff, offset, 4);
        offset += 2;

        cmd_buff[offset++] = connections[i].inport;
        cmd_buff[offset++] = connections[i].inchannel;
        cmd_buff[offset++] = connections[i].outport;
        cmd_buff[offset++] = connections[i].outchannel;
    }

    ERR_CHECKING(si479x_hifi_cmd0(con_count, offset-4));
    
    ERR_CHECKING(si479x_waitForACTS0());

    ERR_CHECKING(si479x_hifi_resp0(&_hifi_resp));
    
    return ret;
}


RET_CODE _si479x_hifi_set_connection_gain(uint8_t cnx_id, uint8_t byte0, uint8_t byte1, 
    uint8_t byte2, uint8_t byte3, uint16_t gain)
{
    RET_CODE ret = RET_SUCCESS;
    uint8_t offset = 4;

    clear_cmd_memory();

    cmd_buff[offset] = SET_CONNECTION_GAIN;
    cmd_buff[offset+1] = cnx_id;
    
    _si479x_fill_cmd_uint16_t(cmd_buff, offset+2, 8);

    cmd_buff[offset+4] = byte0;
    cmd_buff[offset+5] = byte1;
    cmd_buff[offset+6] = byte2;
    cmd_buff[offset+7] = byte3;

    _si479x_fill_cmd_uint16_t(cmd_buff, offset+8, gain);
    _si479x_fill_cmd_uint16_t(cmd_buff, offset+10, 0);

    ERR_CHECKING(si479x_hifi_cmd0(1, 12));

    ERR_CHECKING(si479x_waitForACTS0());
    
    ERR_CHECKING(si479x_hifi_resp0(&_hifi_resp));
    
    return ret;
}


#if 0
RET_CODE si479x_hifi_set_connection_gain_port_2_node(uint8_t i_portid, uint8_t i_chid, 
    uint8_t i_nodeid, uint16_t gain)
{
    return _si479x_hifi_set_connection_gain(0, i_nodeid, 0, i_chid, i_portid, gain);
}

RET_CODE si479x_hifi_set_connection_gain_node_2_port(uint8_t o_portid, uint8_t o_chid, 
    uint8_t o_nodeid, uint16_t gain)
{
    return _si479x_hifi_set_connection_gain(1, o_chid, o_portid, 0, o_nodeid, gain);
}
#endif

#if 0
RET_CODE si479x_hifi_set_connection_gain_port_2_port(uint8_t i_portid, uint8_t i_chid, 
    uint8_t o_portid, uint8_t o_chid, uint16_t gain)
{
    return _si479x_hifi_set_connection_gain(2, o_chid, o_portid, i_chid, i_portid, gain);
}
#endif

RET_CODE si479x_hifi_port_mute(uint8_t portid, uint8_t chid, uint8_t mutectl)
{
    RET_CODE ret = RET_SUCCESS;
    uint8_t offset = 4;

    clear_cmd_memory();

    cmd_buff[offset] = XBAR_PORT_MUTE;
    cmd_buff[offset+1] = 0;
    
    _si479x_fill_cmd_uint16_t(cmd_buff, offset+2, 4);

    cmd_buff[offset+4] = portid;
    cmd_buff[offset+5] = chid;
    cmd_buff[offset+6] = mutectl;
    cmd_buff[offset+7] = 0;

    ERR_CHECKING(si479x_hifi_cmd0(1, 8)); 
    
    ERR_CHECKING(si479x_waitForACTS0());

    ERR_CHECKING(si479x_hifi_resp0(&_hifi_resp));
    
    return ret;   
}
//#endif

#if 0
RET_CODE si479x_hifi_port_query(PORT_ID port_id, PORT_STATE* pStatus)
{
    RET_CODE ret = RET_SUCCESS;
    uint8_t offset = 4;

    clear_cmd_memory();

    cmd_buff[offset] = XBAR_PORT_QUERY;
    cmd_buff[offset+1] = port_id;

    ERR_CHECKING(si479x_hifi_cmd0(1, 4)); 

    ERR_CHECKING(si479x_waitForACTS0());
    ERR_CHECKING(si479x_hifi_resp0(&_hifi_resp));

    if (port_id != _hifi_resp.rdata[4])
    {
        return RET_INVALIDOUTPUT;
    }

    pStatus->port_state = _hifi_resp.rdata[5]&0x03;
    pStatus->chan_state = _hifi_resp.rdata[6];
    
    return ret;       
}
#endif

RET_CODE si479x_hifi_coeff_write(uint8_t filterid, uint8_t ntaps, uint8_t datalen, const uint8_t * datalist)
{
    RET_CODE ret = RET_SUCCESS;
    uint8_t offset = 4;
    uint8_t i;

    if (datalen > 208) 
    {
        return RET_INVALIDINPUT;
    }

    clear_cmd_memory();

    cmd_buff[offset] = COEFF_WRITE;
    cmd_buff[offset+1] = 0;
    
    _si479x_fill_cmd_uint16_t(cmd_buff, offset+2, datalen+4);
	
    cmd_buff[offset+4] = filterid&0x07;
    cmd_buff[offset+5] = ntaps;
    cmd_buff[offset+6] = 0;
    cmd_buff[offset+7] = 0;

    for (i=0; i<datalen; i++)
    {
        cmd_buff[offset+8+i] = datalist[i];
    }

    ERR_CHECKING(si479x_hifi_cmd0(1, 8+datalen)); 
    
    ERR_CHECKING(si479x_waitForACTS0());

    ERR_CHECKING(si479x_hifi_resp0(&_hifi_resp));
    
    return ret;   
}


RET_CODE si479x_set_sleep_mode(uint16_t mode)
{
    RET_CODE ret = RET_SUCCESS;
    uint8_t offset = 4;

    clear_cmd_memory();

    cmd_buff[offset] = SET_SLEEP_MODE;
    cmd_buff[offset+1] = mode&0x03;

    ERR_CHECKING(si479x_hifi_cmd0(1, 4));
    ERR_CHECKING(si479x_waitForACTS0());

    ERR_CHECKING(si479x_hifi_resp0(&_hifi_resp));

	return ret;	
}

RET_CODE si479x_set_dac_offset(uint32_t dac_offset)
{
    RET_CODE ret = RET_SUCCESS;
    uint8_t offset = 4;

    clear_cmd_memory();

    cmd_buff[offset] = SET_DAC_OFFSET;
    cmd_buff[offset+1] = 0;
    _si479x_fill_cmd_uint16_t(cmd_buff, offset+2, 4);
    _si479x_fill_cmd_uint32_t(cmd_buff, offset+4, dac_offset);

    ERR_CHECKING(si479x_hifi_cmd0(1, 8));
    ERR_CHECKING(si479x_waitForACTS0());

    ERR_CHECKING(si479x_hifi_resp0(&_hifi_resp));

	return ret;	
}

#if 0
RET_CODE si479x_hifi_query_connection_gains(uint8_t count, uint16_t length, uint8_t * pData)
{
}


RET_CODE si479x_hifi_node_query(uint8_t count, uint16_t length, uint8_t * pData)
{
}


RET_CODE si479x_hifi_port_mute(uint8_t count, uint16_t length, uint8_t * pData)
{
}


RET_CODE si479x_hifi_clock_domain_sync(uint8_t count, uint16_t length, uint8_t * pData)
{
}

RET_CODE si479x_hifi_clock_domain_query(uint8_t count, uint16_t length, uint8_t * pData)
{
}

RET_CODE si479x_hifi_tonegen_request(uint8_t count, uint16_t length, uint8_t * pData)
{
}

RET_CODE si479x_hifi_parameter_write(uint8_t count, uint16_t length, uint8_t * pData)
{
}

RET_CODE si479x_hifi_parameter_read(uint8_t count, uint16_t length, uint8_t * pData)
{
}

RET_CODE si479x_hifi_process_schedule(uint8_t count, uint16_t length, uint8_t * pData)
{
}

#endif

RET_CODE si479x_hifi_proc1_enable()
{
    RET_CODE ret = RET_SUCCESS;
    uint8_t offset = 4;

	//IMPORTANT note: wait ACST 
    ERR_CHECKING(si479x_waitForACTS1());

    clear_cmd_memory();

    cmd_buff[offset] = PROC_ENABLE;
    
    _si479x_fill_cmd_uint16_t(cmd_buff, offset+2, 0);
    
    ERR_CHECKING(si479x_hifi_cmd1(1, 4));

    ERR_CHECKING(si479x_waitForACTS1());

    ERR_CHECKING(si479x_hifi_resp1(&_hifi_resp));

    return ret;
}

/**
RET_CODE si479x_hifi_proc2_enable()
{
    RET_CODE ret = RET_SUCCESS;
    uint8_t offset = 4;
    
	//IMPORTANT note: wait ACST 
    ERR_CHECKING(si479x_waitForACTS2());

    clear_cmd_memory();

    cmd_buff[offset] = PROC_ENABLE;
    
    _si479x_fill_cmd_uint16_t(cmd_buff, offset+2, 0);
    
    ERR_CHECKING(si479x_hifi_cmd2(1, 4));

    ERR_CHECKING(si479x_waitForACTS2());

    ERR_CHECKING(si479x_hifi_resp2(&_hifi_resp));

    return ret;
}
*/

RET_CODE si479x_hifi_module_parameter_write(uint16_t module_id, uint16_t para_id, 
    uint16_t length, const uint8_t * pData)
{
    RET_CODE ret = RET_SUCCESS;
    uint8_t offset = 4;
    uint8_t i;

    clear_cmd_memory();

    cmd_buff[offset] = MODULE_PARAMETER_WRITE;
    cmd_buff[offset+1] = 0;
    _si479x_fill_cmd_uint16_t(cmd_buff, offset+2, length+4);
    _si479x_fill_cmd_uint16_t(cmd_buff, offset+4, module_id);
    _si479x_fill_cmd_uint16_t(cmd_buff, offset+6, para_id);
    
    for (i=0; i<length; i++)
    {
        cmd_buff[i+offset+8] = pData[i];
    }

    ERR_CHECKING(si479x_hifi_cmd1(1, 8+length));
    ERR_CHECKING(si479x_waitForACTS1());

    ERR_CHECKING(si479x_hifi_resp1(&_hifi_resp));

	return ret;
}

RET_CODE si479x_hifi_module_parameter_read(uint16_t module_id, uint16_t para_id, uint8_t * pLength, uint8_t * pData)
{
    RET_CODE ret = RET_SUCCESS;
    uint8_t offset = 4;
    uint8_t i;

    clear_cmd_memory();

    cmd_buff[offset] = MODULE_PARAMETER_READ;
    cmd_buff[offset+1] = 0;
    _si479x_fill_cmd_uint16_t(cmd_buff, offset+2, 4);
    _si479x_fill_cmd_uint16_t(cmd_buff, offset+4, module_id);
    _si479x_fill_cmd_uint16_t(cmd_buff, offset+6, para_id);
    
    ERR_CHECKING(si479x_hifi_cmd1(1, 8));
    ERR_CHECKING(si479x_waitForACTS1());

    ERR_CHECKING(si479x_hifi_resp1(&_hifi_resp));
	
	_hifi_resp.length = _hifi_resp.length < 252 ? _hifi_resp.length : 252;
	(* pLength) = _hifi_resp.length;
	
	for (i=0; i<_hifi_resp.length; i++)
	{
		pData[i] = _hifi_resp.rdata[i];
	}

	return ret;	
}


#ifdef SUPPORT_AUDIO_EVENT_TONE
RET_CODE si479x_hifi_tonegen_request(uint8_t tone_id, uint16_t amp_abs, 
	uint16_t repeat_count, uint8_t tone_count, hifi_tone_descriptor_t *tone)
{
    RET_CODE ret = RET_SUCCESS;
    uint8_t offset = 4;
	uint8_t i = 0;
	uint8_t count = tone_count <= 15 ? tone_count : 15;

    clear_cmd_memory();

    cmd_buff[offset] = TONEGEN_REQUEST;
    cmd_buff[offset+1] = tone_id&0x03;
    _si479x_fill_cmd_uint16_t(cmd_buff, offset+2, 4+count*16);
    _si479x_fill_cmd_uint16_t(cmd_buff, offset+4, amp_abs);
    _si479x_fill_cmd_uint16_t(cmd_buff, offset+6, repeat_count);
	
	for (i=0; i<=count; i++)
	{
		cmd_buff[offset+8+i*16] = 
			(tone[i].sum_or_mpy & 0x01) << 7 | 
			(tone[i].fallramp & 0x01) << 6 | 
			(tone[i].decayramp & 0x01) << 5 | 
			(tone[i].attackramp & 0x01) << 4 | 
			(tone[i].wavetype & 0x03) << 2 | 
			(tone[i].oscid & 0x03);
		cmd_buff[offset+9+i*16] = tone[i].mod_index & 0x7f;
		cmd_buff[offset+10+i*16] = tone[i].amp_peak;
		cmd_buff[offset+11+i*16] = tone[i].amp_decay;
		
		_si479x_fill_cmd_uint16_t(cmd_buff, offset+12+i*16, tone[i].frequency);
		_si479x_fill_cmd_uint16_t(cmd_buff, offset+14+i*16, tone[i].start_time);
		_si479x_fill_cmd_uint16_t(cmd_buff, offset+16+i*16, tone[i].attack_time);
		_si479x_fill_cmd_uint16_t(cmd_buff, offset+18+i*16, tone[i].peak_time);
		cmd_buff[offset+19+i*16] |= (tone[i].peak_duration << 7);
		_si479x_fill_cmd_uint16_t(cmd_buff, offset+20+i*16, tone[i].decay_time);
		_si479x_fill_cmd_uint16_t(cmd_buff, offset+22+i*16, tone[i].fall_time);
	}
	
    ERR_CHECKING(si479x_hifi_cmd0(1, 8+count*16)); 

    ERR_CHECKING(si479x_waitForACTS0());

    ERR_CHECKING(si479x_hifi_resp0(&_hifi_resp));
        
    return ret;       
}
#endif

#ifdef SUPPORT_WAV_PLAY
RET_CODE si479x_hifi_wave_play(uint8_t wave_gen_id, uint8_t wave_count, uint8_t loop_control, uint16_t loop_count, hifi_wave_type_t *wave_list)
{
    RET_CODE ret = RET_SUCCESS;
    uint8_t offset = 4;
    uint8_t i;

    clear_cmd_memory();

    cmd_buff[offset] = WAVE_PLAY;
    cmd_buff[offset+1] = wave_gen_id&0x01;
    _si479x_fill_cmd_uint16_t(cmd_buff, offset+2, 4+4*wave_count);
    
    cmd_buff[offset+4] = wave_count&0x0f;
    cmd_buff[offset+5] = loop_control;
    _si479x_fill_cmd_uint16_t(cmd_buff, offset+6, loop_count);
    
    for (i=0; i<wave_count; i++) 
    {
        cmd_buff[offset+8+4*i] = wave_list[i].wave_id;
        cmd_buff[offset+8+4*i+1] = wave_list[i].wave_gain;
        _si479x_fill_cmd_uint16_t(cmd_buff, offset+8+4*i+2, wave_list[i].wave_duration);
    }

    ERR_CHECKING(si479x_hifi_cmd0(1, 8+4*wave_count)); 

    ERR_CHECKING(si479x_waitForACTS0());

    ERR_CHECKING(si479x_hifi_resp0(&_hifi_resp));
        
    return ret;       
}

RET_CODE si479x_hifi_wave_stop(uint8_t wave_gen_id)
{
    RET_CODE ret = RET_SUCCESS;
    uint8_t offset = 4;
    
    clear_cmd_memory();
    
    cmd_buff[offset] = WAVE_STOP;
    cmd_buff[offset+1] = wave_gen_id&0x01;
    _si479x_fill_cmd_uint16_t(cmd_buff, offset+2, 0);
    
    ERR_CHECKING(si479x_hifi_cmd0(1, 4)); 

    ERR_CHECKING(si479x_waitForACTS0());

    ERR_CHECKING(si479x_hifi_resp0(&_hifi_resp));

	return ret;
}
#endif

#ifdef SUPPORT_WAV_PLAY
RET_CODE si479x_hifi_wave_add(uint8_t wave_id, uint16_t length, uint8_t *wavdata)
{
    RET_CODE ret = RET_SUCCESS;
    uint8_t offset = 4;
    uint8_t i;
    
    clear_cmd_memory();
    
    cmd_buff[offset] = WAVE_ADD;
    cmd_buff[offset+1] = wave_id&0x7f;
    _si479x_fill_cmd_uint16_t(cmd_buff, offset+2, length);
    
    for (i=0; i<length; i++)
    {
        cmd_buff[offset+4+i] = wavdata[i];
    }
    
    ERR_CHECKING(si479x_hifi_cmd0(1, 4+length)); 

    ERR_CHECKING(si479x_waitForACTS0());

    ERR_CHECKING(si479x_hifi_resp0(&_hifi_resp));

	return ret;
}


RET_CODE si479x_hifi_wave_append(uint8_t wave_id, uint16_t length, uint8_t *wavdata)
{
    RET_CODE ret = RET_SUCCESS;
    uint8_t offset = 4;
    uint8_t i;
    
    clear_cmd_memory();
    
    cmd_buff[offset] = WAVE_APPEND;
    cmd_buff[offset+1] = wave_id&0x7f;
    _si479x_fill_cmd_uint16_t(cmd_buff, offset+2, length);
    
    for (i=0; i<length; i++)
    {
        cmd_buff[offset+4+i] = wavdata[i];
    }
    
    ERR_CHECKING(si479x_hifi_cmd0(1, 4+length)); 

    ERR_CHECKING(si479x_waitForACTS0());

    ERR_CHECKING(si479x_hifi_resp0(&_hifi_resp));

	return ret;
}
#endif

RET_CODE si479x_hifi_cmd_exe(uint8_t cmd_size, uint8_t *cmd)
{
    RET_CODE ret = RET_SUCCESS;
    uint8_t acts = 0x01;
    
    ERR_CHECKING(si479x_command(cmd_size, cmd, 4, rsp_buff)); 

    _si479x_waitForACTS(acts << (cmd[0] - HIFI_CMD0));

    ERR_CHECKING(si479x_hifi_resp(HIFI_RESP0 + cmd[0] - HIFI_CMD0, &_hifi_resp));
    
    return ret;   
}

RET_CODE si479x_hifi_read_meter(uint16_t module_id, uint8_t meter_id)
{
    RET_CODE ret = RET_SUCCESS;
    uint8_t offset = 4;

    clear_cmd_memory();

    cmd_buff[offset] = READ_METER;
    cmd_buff[offset+1] = 0;
    _si479x_fill_cmd_uint16_t(cmd_buff, offset+2, 4);
    _si479x_fill_cmd_uint16_t(cmd_buff, offset+4, module_id);
	cmd_buff[offset+6] = meter_id;

    ERR_CHECKING(si479x_hifi_cmd1(1, 8+offset));
    ERR_CHECKING(si479x_waitForACTS1());

    ERR_CHECKING(si479x_hifi_resp1(&_hifi_resp));

	return ret;	
}


RET_CODE si479x_hifi_modify_control(uint16_t module_id, uint8_t control_id, 
	uint8_t op_oode, uint32_t control, uint32_t mask)
{
    RET_CODE ret = RET_SUCCESS;
    uint8_t offset = 4;

    clear_cmd_memory();

    cmd_buff[offset] = MODIFY_CONTROL;
    cmd_buff[offset+1] = 0x00;
	cmd_buff[offset+2] = 0x0c;
    _si479x_fill_cmd_uint16_t(cmd_buff, offset+4, module_id);
	cmd_buff[offset+6] = control_id&0x01;
	cmd_buff[offset+7] = op_oode;
    _si479x_fill_cmd_uint16_t(cmd_buff, offset+8, control);
    _si479x_fill_cmd_uint16_t(cmd_buff, offset+0x0c, mask);

    ERR_CHECKING(si479x_hifi_cmd1(1, 16+offset));
    ERR_CHECKING(si479x_waitForACTS1());

    ERR_CHECKING(si479x_hifi_resp1(&_hifi_resp));

	return ret;	
}
