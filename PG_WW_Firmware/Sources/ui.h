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

#ifndef _UI_H_
#define _UI_H_


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
extern void ui_init(UI_CB callback1, UI_CB callback2);

extern void ui_tick();

extern void ui_toggle_status();
extern void ui_marker_on();
extern void ui_marker_off();
extern void ui_red_on();
extern void ui_red_off();



#endif // _UI_H_
