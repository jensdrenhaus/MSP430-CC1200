/*------------------------------------------------------------------------------
| File: com.c
|
| Say something about the module. Copy that part from name.h
|
| Note: important things
 -----------------------------------------------------------------------------*/

#include "com.h"
#include "phy.h"




//#############################################################################
// globals

static COM_CB g_com_callback;

///* structure for something */
//typedef struct s_name {
//		int   one;
//}Name_t;




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

void com_send(com_data_t data) {

}


////////////////////////////////////////////////////////////////////////////

//! PRIVATE interpreter()
//!
////////////////////////////////////////////////////////////////////////////
static void interpreter(char* string){
	g_com_callback(string);
}

