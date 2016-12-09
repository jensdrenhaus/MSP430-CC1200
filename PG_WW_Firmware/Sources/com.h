/*-----------------------------------------------------------------------------
| File: com.h
|
| Implements funktionality for converting incoming byte data into
| command structure and vice versa
|
| Note: -
|------------------------------------------------------------------------------
| Datatypes:
|     com_command_t    -- used to store command key words
|     com_src_t        -- used to store data source
|     com_dest_t       -- used to store data destination
|     com_data_t       -- used to store command-set
|------------------------------------------------------------------------------
| Functions:
|     com_init  -- init subjacent modules, saves functionpointer for callback
|     com_send  -- passes command-set to subjacent module
 ----------------------------------------------------------------------------*/

#ifndef _COM_H_
#define _COM_H_

#include <stdint.h>



//#############################################################################
// datatypes
//#############################################################################
typedef enum e_source {SRC_RF, SRC_SERIAL}com_src_t;
typedef enum e_destination {DEST_RF, DEST_SERIAL}com_dest_t;
typedef enum e_commands {NONE = 0, PAGE = 1, WEIGHT = 2}com_command_t;

/* structure to hold commands */
typedef struct s_com_data {
		com_command_t command;
		uint64_t      id;
		double	      arg;
}com_data_t;




//#############################################################################
// callback function definition
//#############################################################################
typedef void (*COM_CB)(com_data_t* data, com_src_t src);

//#############################################################################
// functions prototypes
//#############################################################################

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
extern void com_send(com_data_t* data, com_dest_t dest);



#endif // _COM_H_
