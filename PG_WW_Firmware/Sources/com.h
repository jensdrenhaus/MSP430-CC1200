/*------------------------------------------------------------------------------
| File: com.h
|
| Implements funktionality for converting incoming byte data into textoriented
| command structure and vice versa
|
| Note: -
|-------------------------------------------------------------------------------
| Datatypes:
|     com_command_t    -- used to store command key words
|     com_data_t       -- used to store command set
|-------------------------------------------------------------------------------
| Functions:
|     com_init  -- init subjacent modules, saves functionpointer for callback
|     com_send  -- passes command set to subjacent module
 -----------------------------------------------------------------------------*/

#ifndef _COM_H_
#define _COM_H_



//#############################################################################
// datatypes

typedef enum e_commands {NONE = 0, PAGE = 1, WEIGHT = 2}com_command_t;

/* structure to hold commands */
typedef struct s_com_data {
		com_command_t command;
		int           id;
		double	      arg;
}com_data_t;




//#############################################################################
// callback function definition
typedef void (*COM_CB)(com_data_t* data);

//#############################################################################
// functions (prototypes)

/*------------------------------------------------------------------------------
| com_init  -- init subjacent modules, saves functionpointer for callback
|
| Parameter:
|     function ptr to callback
|
| Return:
|     -
|
 -----------------------------------------------------------------------------*/
extern void com_init(COM_CB callback);

/*------------------------------------------------------------------------------
| com_send  -- passes command set to subjacent module
|
| Parameter:
|     command data structure
|
| Return:
|     -
|
 -----------------------------------------------------------------------------*/
extern void com_send(com_data_t* data);



#endif // _COM_H_
