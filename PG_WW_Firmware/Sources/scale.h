/*------------------------------------------------------------------------------
| File: scale.h
|
| Implements functionality for reading out and processing analog single from a
| strain gauge
|
| Note: uses P3.0 as analog input
 -------------------------------------------------------------------------------
| Datatypes:
|     -
|-------------------------------------------------------------------------------
| Functions:
|     scale_init                 -- set up port configuration and ADC registers
|     scale_request              --  get a calculated weight
|     scale_get_adc              -- just trigger a sampling period get raw data
|     scale_set_zero_offset      -- calibrate zero value of conversion function
|     scale_set_calib_load_value -- calibrate ref value of conversion function
 -----------------------------------------------------------------------------*/

#ifndef _SCALE_H_
#define _SCALE_H_

#include "types.h"


//#############################################################################
// datatypes
//#############################################################################



//#############################################################################
// functions (prototypes)
//#############################################################################

/*------------------------------------------------------------------------------
| scale_init                 -- set up port configuration and ADC registers
|
| Parameter:
|     ---
|
| Return:
|     ---
|
 -----------------------------------------------------------------------------*/
extern void scale_init();

/*------------------------------------------------------------------------------
| scale_request              --  get a calculated weight
|
| Parameter:
|     ---
|
| Return:
|     ---
|
 -----------------------------------------------------------------------------*/
extern uint16_t scale_request();

/*------------------------------------------------------------------------------
| scale_get_adc              -- just trigger a sampling period get raw data
|
| Parameter:
|     ---
|
| Return:
|     ---
|
 -----------------------------------------------------------------------------*/
extern uint16_t scale_get_adc();

/*------------------------------------------------------------------------------
| scale_set_zero_offset      -- calibrate zero value of conversion function
|
| Parameter:
|     ---
|
| Return:
|     ---
|
 -----------------------------------------------------------------------------*/
extern void scale_set_zero_offset(uint16_t val);

/*------------------------------------------------------------------------------
| scale_set_calib_load_value -- calibrate ref value of conversion function
|
| Parameter:
|     ---
|
| Return:
|     ---
|
 -----------------------------------------------------------------------------*/
extern void scale_set_calib_load_value(uint16_t val);



#endif // _SCALE_H_
