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
#include <types.h>
// zum test
#include "ui.h"




//#############################################################################
// globals

static COM_CB g_com_callback;
static char send_str[SERIAL_MAX_BUF];

com_data_t receive_data;




//#############################################################################
// private function prototypes
static void interpreter(char* string);
static void rf_interpreter(uint8* rxBuffer);


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
	rf_init(rf_interpreter);

}


////////////////////////////////////////////////////////////////////////////

//!  PUBLIC com_send()
//!
////////////////////////////////////////////////////////////////////////////
void com_send(com_data_t* data) {
	switch (data->command){
	case PAGE:
		sprintf(send_str, "PAGE %d\n", data->id);
		break;
	case WEIGHT:
		sprintf(send_str, "WEIGHT %d %.3f \n", data->id, data->arg);
//		dtoa(tempstr, data->arg);
//		strcat(send_str, tempstr);
		break;
	default:
		strcpy(send_str, "ERROR\n");
	}
	serial_send(send_str);
}


////////////////////////////////////////////////////////////////////////////

//! PRIVATE interpreter()
//!
////////////////////////////////////////////////////////////////////////////
static void interpreter(char* string){
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
		receive_data.id = atoi(ptr);
	ptr = strtok(NULL, " ");
	if(ptr)
		receive_data.arg = atof(ptr);


	g_com_callback(&receive_data);
}

static void rf_interpreter(uint8* rxBuffer){
	ui_toggle_status();
}




