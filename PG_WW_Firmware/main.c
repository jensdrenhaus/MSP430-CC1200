/*------------------------------------------------------------------------------
| File: main.c
|
| Initializes all software modules.
  Triggers periodic events by timer interrupt.
  Implements callback functions for each module.

  Current state:
  	  communication via serial only!
  	  -> responds to PAGE command with toggel Status LED
  	  -> echos WEIGHT commands
  	  -> sends dummy WEIGHT every 10 sec
|
| Note: - uses Timer A1 of MSP430 to trigger periodic events
|		- implements Timer A1 ISR
 -----------------------------------------------------------------------------*/

#include "msp430fr5969.h"
#include "serial.h"
#include "ui.h"
#include "com.h"
#include "sensor.h"

// Globals
int pressed = 0;
const int MY_BOX_ID = 42;
com_data_t send_data = {WEIGHT, 42, 100.12};
char* error_msg_command = "No valid command recieved\n";
char* error_msg_id      = "That's not me\n";

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

void process (com_data_t* receive_data) {
	if(receive_data->command == PAGE)
		if(receive_data->id == MY_BOX_ID)
			ui_toggle_status();
		else
			serial_send(error_msg_id);
	else if(receive_data->command == WEIGHT) {
		com_send(receive_data);
	}
	else
		serial_send(error_msg_command);
}

void update_weight(int val) {
	send_data.arg = (double)val;
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
	static int tick     = 1;        // set state for full secound
	static int send_cnt = 0;

	if (tick){
		//check sensor
		sen_request();
		// count to 10 secounds
		send_cnt++;
		if(send_cnt == 10){
			com_send(&send_data);
			send_cnt = 0;
		}
	}
	tick ^= 1;                   // toggel tick state
	ui_tick();
}

