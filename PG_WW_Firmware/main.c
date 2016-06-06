#include "msp430fr5969.h"
#include "phy.h"
#include <string.h>
#include "ui.h"
#include "com.h"
#include "sensor.h"

// Globals
int pressed = 0;
const int MY_BOX_ID = 42;
com_data_t send_data = {WEIGHT, 42, 100.12};
const char* error_msg_command = "No valid command recieved\n";
const char* error_msg_id      = "That's not me\n";

// Prototypes
void process(com_data_t* recieve_data);
void update_weight(int val);
void enter();





void main(void) {

    WDTCTL = WDTPW | WDTHOLD;       // stop watchdog timer

    ui_init(enter);
    com_init(process);
    sen_init(update_weight);

    _EINT();                        // global interrupt enable

    while(1)
    {
    	// do nothing
    	// wait for interrupts

    	// test f�r softwaregruppe
//    	com_send(&send_data);
//    	send_data.id++;
//    	if(send_data.id >10)
//    		send_data.id = 1;
    }
}

//#############################################################################
// callback funktions
//#############################################################################

void process (com_data_t* recieve_data) {
	if(recieve_data->command == PAGE)
		if(recieve_data->id == MY_BOX_ID)
			com_send(&send_data);
		else
			phy_send(error_msg_id);
	else if(recieve_data->command == WEIGHT) {
		com_send(recieve_data);
//		send_data.arg = recieve_data->arg;
//		com_send(&send_data);
	}
	else
		phy_send(error_msg_command);
}

void update_weight(int val) {
	send_data.arg = val;
}

void enter(){
	ui_toggle_status();
}



//#############################################################################
// Timer A1 ISR for periodic events
//#############################################################################
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer0_A1(void)
{
	static int tick = 1;        // set state for full secound

	if (tick)
		//
		sen_request();
	tick ^= 1;                   // toggel tick state
	ui_tick();
}

