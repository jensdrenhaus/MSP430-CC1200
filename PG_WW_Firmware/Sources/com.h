
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
#include "types.h"

#define COM_FRAME_LEN   19 // 1+8+8+2 Byte

#define COM_PAGE_CMD    0x01
#define COM_WEIGHT_CMD  0x02

//#############################################################################
// datatypes
//#############################################################################
typedef enum e_source {SRC_RF, SRC_SERIAL}com_src_t;
typedef enum e_destination {DEST_RF, DEST_SERIAL}com_dest_t;
typedef enum e_commands {NONE = 0, PAGE = 1, WEIGHT = 2}com_command_t;

/* structure to hold commands */
typedef struct s_com_data {
		com_command_t command;
		uint64_t      box_id;
		uint64_t      product_id;
		double	      arg;
}com_data_t;

/* structure to hold commands */
typedef struct s_com_data_fix {
        uint16      arg;
        uint64      product_id;
        uint64      box_id;
		uint8       command;
}__attribute__ ((packed))com_data_fix_t;


typedef union u_com_frame{
    uint8 array[COM_FRAME_LEN];
    com_data_fix_t frame;
}com_frame_t;



//#############################################################################
// callback function definition
//#############################################################################
//typedef void (*COM_CB)(com_data_t* data, com_src_t src);
typedef void (*COM_CB)(com_frame_t* frame, com_src_t src);

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
extern void com_send_fix(com_frame_t* data, com_dest_t dest);



#endif // _COM_H_
