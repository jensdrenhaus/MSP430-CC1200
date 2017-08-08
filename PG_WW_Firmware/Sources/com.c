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

#include "com.h"
#include "serial.h"
#include "rf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"

//#############################################################################
// globals
//#############################################################################

static COM_CB   g_com_callback;
com_data_fix_t  receive_data_fix;
com_frame_t     receive_frame;


//#############################################################################
// private function prototypes
//#############################################################################
static void interpreter_fix(uint8* frame, com_src_t src);



////////////////////////////////////////////////////////////////////////////

//!  PUBLIC com_init()
//!
////////////////////////////////////////////////////////////////////////////
void com_init(COM_CB callback) {

	g_com_callback = callback;
	serial_init(callback);
	rf_init(interpreter_fix);

}


////////////////////////////////////////////////////////////////////////////

//!  PUBLIC com_send()
//!
////////////////////////////////////////////////////////////////////////////
void com_send_fix(com_frame_t* data, com_dest_t dest) {
	if (dest == DEST_SERIAL)
		serial_send_fix(data);
	if (dest == DEST_RF)
		rf_send_fix(data);
}


////////////////////////////////////////////////////////////////////////////

//! PRIVATE interpreter()
//!
////////////////////////////////////////////////////////////////////////////
static void interpreter_fix(uint8* frame, com_src_t src){
    uint16 i;
    for(i=0; i<COM_FRAME_LEN; i++){
        receive_frame.array[i] = frame[19-i];
    }

	g_com_callback(&receive_frame, src);
	return;
}
