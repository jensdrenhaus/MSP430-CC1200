/*------------------------------------------------------------------------------
| File: com.h
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

#ifndef _COM_H_
#define _COM_H_




//#############################################################################
// datatypes

typedef enum e_commands {PAGE = 1, WEIGHT = 2}command_t;

/* structure to hold commands */
typedef struct s_com_data {
		command_t command;
		int       id;
		double	  arg;
}com_data_t;



//#############################################################################
// callback function definition
typedef void (*COM_CB)();

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
extern void com_init(COM_CB callback);

extern void com_send(com_data_t data);



#endif // _COM_H_
