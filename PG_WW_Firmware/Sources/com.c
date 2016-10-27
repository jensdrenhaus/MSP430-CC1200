/*------------------------------------------------------------------------------
| File: com.c
|
| Implements funktionality for converting incoming byte data into textoriented
| command structure and vice versa
|
| Note: -
 -----------------------------------------------------------------------------*/

#include "com.h"
#include "serial.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include <rf.h>
// zum test
#include "ui.h"




//#############################################################################
// globals

static COM_CB g_com_callback;
static char send_str[SERIAL_MAX_BUF];

com_data_t receive_data;




//#############################################################################
// private function prototypes
static void interpreter(char* string, com_src_t src);


//#############################################################################
// module methods implementation:
//#############################################################################


////////////////////////////////////////////////////////////////////////////

//!  PUBLIC com_init()
//!
////////////////////////////////////////////////////////////////////////////
void com_init(COM_CB callback) {

	g_com_callback = callback;
	serial_init(interpreter);
	rf_init(interpreter);

}


////////////////////////////////////////////////////////////////////////////

//!  PUBLIC com_send()
//!
////////////////////////////////////////////////////////////////////////////
void com_send(com_data_t* data, com_dest_t dest) {
	switch (data->command){
	case PAGE:
		sprintf(send_str, "PAGE %lld \n", data->id);
		break;
	case WEIGHT:
		sprintf(send_str, "WEIGHT %lld %.3f \n", data->id, data->arg);
//		dtoa(tempstr, data->arg);
//		strcat(send_str, tempstr);
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
static void interpreter(char* string, com_src_t src){
	char *ptr;
	ptr = strtok(string, " ");
	if(!strcmp(ptr, "PAGE"))
		receive_data.command = PAGE;
	else if(!strcmp(ptr, "WEIGHT"))
		receive_data.command = WEIGHT;
	else
		receive_data.command = NONE;

	ptr = strtok(NULL, " ");
	if(ptr)
		receive_data.id = atoll(ptr);
	ptr = strtok(NULL, " ");
	if(ptr)
		receive_data.arg = atof(ptr);

	g_com_callback(&receive_data, src);
}




