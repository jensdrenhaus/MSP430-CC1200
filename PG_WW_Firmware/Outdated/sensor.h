/*------------------------------------------------------------------------------
| File: sensor.h
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

#ifndef _SENSOR_H_
#define _SENSOR_H_



//#############################################################################
// datatypes




//#############################################################################
// callback function definition
typedef void (*SEN_CB)(int val);

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
extern void sen_init(SEN_CB callback);

extern void sen_request();



#endif // _SENSOR_H_
