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
#include "scale.h"





typedef enum e_state{active, calibrating_zero, calibrating_load, waiting}state_t;

// Globals
int pressed = 0;
com_data_t send_data;
char* error_msg_command = "No valid command recieved\n";
char* error_msg_id      = "That's not me\n";

state_t state = active;

uint8_t mac[6];
uint64_t my_id;

// Prototypes
void data_recieved_event(com_data_t* recieve_data, com_src_t src);
void weight_changed_event(int val);
void button1_pressed_event();
void button2_pressed_event();




void main(void) {

    WDTCTL = WDTPW | WDTHOLD;       // stop watchdog timer
//    WDTCTL = WDTPW | WDTHOLD;
//    WDTCTL = WDTPW | WDTHOLD;

    mac[0] = *(uint8_t*)0x1A0E;
    mac[1] = *(uint8_t*)0x1A10;
    mac[2] = *(uint8_t*)0x1A0A;
    mac[3] = *(uint8_t*)0x1A0B;
    mac[4] = *(uint8_t*)0x1A0C;
    mac[5] = *(uint8_t*)0x1A0D;

    my_id |= (uint64_t)mac[0];
    my_id |= (uint64_t)mac[1] << 8;
    my_id |= (uint64_t)mac[2] << 16;
    my_id |= (uint64_t)mac[3] << 24;
    my_id |= (uint64_t)mac[4] << 32;

    send_data.command = WEIGHT;
    send_data.id = my_id;
    send_data.arg = 0.0;


    ui_init(button1_pressed_event, button2_pressed_event);
    com_init(data_recieved_event);
    sen_init(weight_changed_event);
    scale_init();


    _EINT();                        // global interrupt enable

     while(1)
    {

    	// do nothing
    	// wait for interrupts


    	// spi test
//    	uint8 addr = 0x01; // IOCFG2
//    	uint8 data = 0x06; // some value
//    	spi_reg_access(SPI_WRITE_SINGLE, addr, &data, 1);

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

void data_recieved_event (com_data_t* receive_data, com_src_t src) {
	if(state == active){
		switch(src){
		case SRC_RF:
			if(receive_data->command == PAGE){
				if(receive_data->id == my_id)
					ui_marker_on();
			}
			else if(receive_data->command == WEIGHT)
				com_send(receive_data, DEST_SERIAL);
			break;

		case SRC_SERIAL:
			if(receive_data->command == PAGE)
				com_send(receive_data, DEST_RF);
			break;

		}
	}
}

void weight_changed_event(int val) {
	send_data.arg = (double)val;
}

void button1_pressed_event(){
	ui_marker_off();
}

void button2_pressed_event(){
	switch(state){
	case active:
		state = calibrating_zero;
		break;
	case waiting:
		state = calibrating_load;
		break;
	}
}


//#############################################################################
// Timer A1 ISR for periodic events every half secound
//#############################################################################
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer1_A0(void)
{
	static int tick      = 1;        // set state for full secound
	static int send_cnt  = 0;
	static int calib_cnt = 0;
	static uint16 calib_val = 0;

	switch(state){
	case active:
		if (tick){
			//check sensor
			//sen_request();

			//Test analoge waage
			uint16 weight = scale_request();
			send_data.arg = (double)weight;


			send_cnt++;
			if(send_cnt == 4){
				com_send(&send_data, DEST_RF);
				send_cnt = 0;
			}
		}
		tick ^= 1;                   // toggel tick state
		ui_tick();					 // blik red led to show programm is ok and working
		break;
	case calibrating_zero:
		ui_red_on();
		ui_toggle_status();
		calib_cnt++;
		calib_val += scale_get_adc();
		if(calib_cnt == 12){
			scale_set_zero_offset(calib_val/12);
			state = waiting;
			ui_red_off();
			calib_cnt = 0;
			calib_val = 0;
		}
		break;
	case waiting:
		ui_toggle_status();
		break;
	case calibrating_load:
		ui_red_on();
		ui_toggle_status();
		calib_cnt++;
		calib_val += scale_get_adc();
		if(calib_cnt == 12){
			scale_set_calib_load_value(calib_val/12);
			state = active;
			ui_red_off();
			calib_cnt = 0;
			calib_val = 0;
		}
		break;
	}
}


