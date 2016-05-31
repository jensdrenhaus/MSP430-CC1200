/*------------------------------------------------------------------------------
| File: com.c
|
| Say something about the module. Copy that part from name.h
|
| Note: important things
 -----------------------------------------------------------------------------*/

#include "com.h"
#include "phy.h"
#include <stdio.h>




//#############################################################################
// globals

static COM_CB g_com_callback;
static char send_str[PHY_MAX_BUF];

com_data_t recieve_data;




//#############################################################################
// private function prototypes
static void interpreter(char* string);


//#############################################################################
// module methods implementation:
//#############################################################################


////////////////////////////////////////////////////////////////////////////

//!  PUBLIC module_somefunction()
//!
////////////////////////////////////////////////////////////////////////////
void com_init(COM_CB callback) {

	g_com_callback = callback;
	phy_init(interpreter);

}

void com_send(com_data_t* data) {
	switch (data->command){
	case PAGE:
		sprintf(send_str, "PAGE %d\n", data->id);
		break;
	case WEIGHT:
		sprintf(send_str, "WEIGHT %d %d\n", data->id, data->arg);
		break;
	default:
		strcpy(send_str, "ERROR\n");
	}
	phy_send(send_str);
}


////////////////////////////////////////////////////////////////////////////

//! PRIVATE interpreter()
//!
////////////////////////////////////////////////////////////////////////////
static void interpreter(char* string){
	char *ptr;
	ptr = strtok(string, " ");
	if(!strcmp(ptr, "PAGE"))
		recieve_data.command = PAGE;
	else if(!strcmp(ptr, "WEIGHT"))
		recieve_data.command = WEIGHT;
	else
		recieve_data.command = NONE;

	ptr = strtok(NULL, " ");
	if(ptr)
		recieve_data.id = atoi(ptr);
	ptr = strtok(NULL, " ");
	if(ptr)
		recieve_data.arg = atoi(ptr);


	g_com_callback(&recieve_data);
}

