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
	serial_init(interpreter_fix);
	rf_init(interpreter_fix);

}


////////////////////////////////////////////////////////////////////////////

//!  PUBLIC com_send()
//!
////////////////////////////////////////////////////////////////////////////
void com_send_fix(com_data_fix_t* data, com_dest_t dest) {
	send_frame[0] = data->command;
	send_frame[1]  = (uint8)(data->box_id >> 56);
	send_frame[2]  = (uint8)(data->box_id >> 48);
	send_frame[3]  = (uint8)(data->box_id >> 40);
	send_frame[4]  = (uint8)(data->box_id >> 32);
	send_frame[5]  = (uint8)(data->box_id >> 24);
	send_frame[6]  = (uint8)(data->box_id >> 16);
	send_frame[7]  = (uint8)(data->box_id >> 8);
	send_frame[8]  = (uint8)(data->box_id >> 0);
	send_frame[9]  = (uint8)(data->product_id >> 56);
	send_frame[10] = (uint8)(data->product_id >> 48);
	send_frame[11] = (uint8)(data->product_id >> 40);
	send_frame[12] = (uint8)(data->product_id >> 32);
	send_frame[13] = (uint8)(data->product_id >> 24);
	send_frame[14] = (uint8)(data->product_id >> 16);
	send_frame[15] = (uint8)(data->product_id >> 8);
	send_frame[16] = (uint8)(data->product_id >> 0);
	send_frame[17] = (uint8)(data->arg >> 8);
	send_frame[18] = (uint8)(data->arg >> 0);
	if (dest == DEST_SERIAL)
		serial_send_fix(send_frame);
	if (dest == DEST_RF)
		rf_send_fix(send_frame);
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
	receive_data_fix.command = *frame;
	receive_data_fix.box_id = 	frame[1] << 56 +
								frame[2] << 48 +
								frame[3] << 40 +
								frame[4] << 32 +
								frame[5] << 24 +
								frame[6] << 16 +
								frame[7] << 8+
								frame[8];
	receive_data_fix.product_id = 	frame[9] << 56 +
									frame[10] << 48 +
									frame[11] << 40 +
									frame[12] << 32 +
									frame[13] << 24 +
									frame[14] << 16 +
									frame[15] << 8+
									frame[16];
	receive_data_fix.arg = 	frame[17] << 8 +
							frame[18];

	g_com_callback(&receive_data_fix, src);
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

