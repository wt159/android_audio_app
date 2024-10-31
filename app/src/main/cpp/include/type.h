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

#ifndef _TYPE_H_
#define _TYPE_H_

#include <limits.h>
#include <stdint.h>

#include "dc_config.h"


typedef unsigned char	uint8_t;
typedef signed char		int8_t;
typedef unsigned short	uint16_t;
typedef signed short	int16_t;
typedef int8_t bool_t;

#ifdef __linux__
	#ifdef OPTION__32BIT_IS_LONG
	typedef unsigned long	uint32_t;
	#else
	typedef unsigned int	uint32_t;
	#endif
#else
#define OPTION__32BIT_IS_LONG
	#ifdef OPTION__32BIT_IS_LONG
	typedef unsigned long	uint32_t;
	typedef signed long		int32_t;
	#else
	typedef unsigned int	uint32_t;
	typedef signed int		int32_t;
	#endif
#endif

#define TRUE    1
#define FALSE   0

#ifndef NULL
#define NULL ((void *) 0)
#endif

#define BAND_FM 0x00   ///< fm band
#define BAND_AM 0x01   ///< am band
#define BAND_DAB 0x02  ///< dab band

#define RET_SUCCESS			0x00    //successful
#define RET_INVALIDINPUT	0x01    //invalid parameter
#define RET_INVALIDOUTPUT	0x02    //invalid resp
#define RET_TIMEOUT			0x04    //command time out
#define RET_ERROR	        0x08    //command error
#define RET_STATEERROR	    0x10    //command error
#define RET_NOT_FOUND		0xff	//not found

typedef uint8_t		    RET_CODE;

#define ERR_CHECKING(FUN) \
    ret = FUN; \
    if (ret != RET_SUCCESS) \
    { \
        printf("Error in %s(%d)\n", #FUN, __LINE__); \
        return ret; \
    }\


#define unused(x) {x = x;}

// Limit a number to within the given range
#define LIMIT(x, x_min, x_max) \
	(((x) < (x_min))? (x_min) : \
	(((x) > (x_max))? (x_max) : (x)))
	

#define MAX(a, b) ((a > b)? (a) : (b))

#define ABS(x) (((x) < 0)? -(x) : (x))

#endif

