/*------------------------------------------------------------------------------
| File: scale.h
|
| Say something about WHAT the model does.
|
| Note: important things to know
|-------------------------------------------------------------------------------
| Datatypes:
|     NAME_datatype    -- used to store data and informatin of something
|     ...
|-------------------------------------------------------------------------------
| Functions:
|     name_somefunction  -- short explaination
|     ...
 -----------------------------------------------------------------------------*/

#ifndef _SCALE_H_
#define _SCALE_H_

#include "types.h"





//#############################################################################
// datatypes




//#############################################################################
// functions (prototypes)

/*------------------------------------------------------------------------------
| name_somefunction  -- short explaination
|
| Parameter:
|     ---
|
| Return:
|     ---
|
 -----------------------------------------------------------------------------*/


extern uint16_t scale_request();

extern void scale_set_zero_offset(uint16_t val);

extern void scale_set_calib_load_value(uint16_t val);

extern uint16_t scale_get_adc();


#endif // _SCALE_H_
