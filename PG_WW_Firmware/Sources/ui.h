/*------------------------------------------------------------------------------
| File: name.h
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

#ifndef _NAME_H_
#define _NAME_H_

#define NAME_SOMEDEFS		42                    1


//#############################################################################
// datatypes





//#############################################################################
// callback function definition
typedef void (*UI_CB)();

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
extern void ui_init(UI_CB callback);

extern void ui_tick();

extern void ui_toggle_status();



#endif // _UI_H_
