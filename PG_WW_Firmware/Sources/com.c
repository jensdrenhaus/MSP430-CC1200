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
static char     send_str[SERIAL_MAX_BUF];
static uint8    send_frame[COM_FRAME_LEN];
com_data_t      receive_data;
com_data_fix_t  receive_data_fix;
com_frame_t     receive_frame;


//#############################################################################
// private function prototypes
//#############################################################################
static void interpreter(char* string, com_src_t src);
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


void com_send(com_data_t* data, com_dest_t dest) {
	switch (data->command){
	case PAGE:
		sprintf(send_str, "PAGE %lld \n", data->product_id);
		break;
	case WEIGHT:
		sprintf(send_str, "WEIGHT %lld %lld %.3f \n", data->box_id, data->product_id, data->arg);
		break;
	default:
		strcpy(send_str, "ERROR\n");
	}
	if (dest == DEST_SERIAL)
		serial_send(send_str);
	if (dest == DEST_RF)
		rf_send(send_str);
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
}

static void interpreter(char* string, com_src_t src){
	char *ptr;
	ptr = strtok(string, " ");
	if(!strcmp(ptr, "PAGE"))
		receive_data.command = PAGE;
	else if(!strcmp(ptr, "WEIGHT"))
		receive_data.command = WEIGHT;
	else
		receive_data.command = NONE;

	switch(receive_data.command){
	case PAGE:
	    ptr = strtok(NULL, " ");
        if(ptr){
            receive_data.product_id = atoll(ptr);
            receive_data.box_id     = 0;
            receive_data.arg        = 0.0;
        }
	    break;
	case WEIGHT:
	    ptr = strtok(NULL, " ");
        if(ptr)
            receive_data.box_id = atoll(ptr);
        ptr = strtok(NULL, " ");
        if(ptr)
            receive_data.product_id = atoll(ptr);
        ptr = strtok(NULL, " ");
        if(ptr)
            receive_data.arg = atof(ptr);
	    break;
	default:
	    receive_data.box_id     = 0;
	    receive_data.product_id = 0;
	    receive_data.arg        = 0.0;

	}

	g_com_callback(&receive_data, src);
}

