/*-----------------------------------------------------------------------------
| File: main.c
|
| Initializes all software modules.
| Triggers periodic events by timer interrupt.
| Implements callback functions for each module.
|
| Current state:
|
|
|
|
|
| Note: - uses Timer A1 of MSP430 to trigger periodic events
|		- implements Timer A1 ISR
 ----------------------------------------------------------------------------*/

#include "msp430fr5969.h"
#include "ui.h"
#include "com.h"
#include "scale.h"
#include "queue.h"
#include "types.h"


//#############################################################################
// globals
//#############################################################################
typedef enum e_state{active, calibrating_zero, calibrating_load, waiting}state_t;

state_t        state      = active;
uint16         pressed    = 0;
com_frame_t    send_frame;
uint8          mac[6];
uint64         my_box_id;
uint64         my_product_id = 1;

//#############################################################################
// function prototypes
//#############################################################################
void data_recieved_event(com_data_t* recieve_data, com_src_t src);
void data_recieved_fix_event(com_frame_t* receive_frame, com_src_t src);
void weight_changed_event(int val);
void button1_pressed_event();
void button2_pressed_event();
void conf_powmgmt();


//#############################################################################
// MAIN FUNCTION
//#############################################################################
void main(void) {
	//------------------------------------------
	// set Watchdogtimer to 1 sek!
	//------------------------------------------
    //WDTCTL = WDTPW | WDTSSEL_1 | WDTCNTCL | WDTIS_4;
    WDTCTL = WDTPW | WDTHOLD;

    PM5CTL0 &=  ~LOCKLPM5;          // Disable the GPIO power-on default
                                    //               high-impedance mode

#ifdef PHYNODE
    // Configure Power Managementchip via SPI UCB0
    conf_powmgmt();
#endif

    //------------------------------------------
	// configure Timer A1 used for periodic events
	//------------------------------------------

	TA1CTL |= (TASSEL__SMCLK | MC__UP | ID__8);          // SMCLK/8 , UP mode
	TA1CTL |= TACLR;                 // clear to acticate new clock settings
	TA1CCR0 = 62500;                 // SMCLK/8/62500 = 2Hz => 0,5s
	TA1CCTL0 |= CCIE;                // TACCR0 interrupt enabled

	//------------------------------------------
	// read out unique device ID
	//------------------------------------------

    mac[0] = *(uint8_t*)0x1A0E;
    mac[1] = *(uint8_t*)0x1A10;
    mac[2] = *(uint8_t*)0x1A0A;
    mac[3] = *(uint8_t*)0x1A0B;
    mac[4] = *(uint8_t*)0x1A0C;
    mac[5] = *(uint8_t*)0x1A0D;

    my_box_id |= (uint64_t)mac[0];
    my_box_id |= (uint64_t)mac[1] << 8;
    my_box_id |= (uint64_t)mac[2] << 16;
    my_box_id |= (uint64_t)mac[3] << 24;
    my_box_id |= (uint64_t)mac[4] << 32;

    //------------------------------------------
	// init modules
	//------------------------------------------


	send_frame.frame.command = COM_WEIGHT_CMD;
    send_frame.frame.box_id = my_box_id;
    send_frame.frame.product_id = my_product_id;
    send_frame.frame.arg = 33;

    ui_init(button1_pressed_event, button2_pressed_event);
    com_init(data_recieved_fix_event);
    //scale_init(); // XXX Fix: analog input port is in use on PhyNode
    queue_init();



    _EINT();                         // global interrupt enable

    //------------------------------------------
	// MAIN LOOP
	//------------------------------------------

    while(1)
	{
//		while(!queue_isEmty()){
//			com_frame_t* tmp = queue_first_fix();
//			com_send_fix(tmp, DEST_RF);
//			queue_delete();
//			ui_marker_off();
//		}
		// do nothing
		// wait for interrupts
	}
}


//#############################################################################
// callback funktions
//#############################################################################

void data_recieved_fix_event (com_frame_t* receive_frame, com_src_t src) {
	if(state == active){
		switch(src){
		case SRC_RF:
			if(receive_frame->frame.command == COM_PAGE_CMD){
				if(receive_frame->frame.product_id == my_product_id){
					ui_marker_on();
				    //queue_insert_fix(&send_frame);
					com_send_fix(&send_frame, DEST_RF);
					ui_marker_off();
				    return;
				}
			}
			else if(receive_frame->frame.command == COM_WEIGHT_CMD)
				com_send_fix(receive_frame, DEST_SERIAL);
			break;

		case SRC_SERIAL:
			if(receive_frame->frame.command == COM_PAGE_CMD)
				com_send_fix(receive_frame, DEST_RF);
			break;

		}
	}
}

void data_recieved_event (com_data_t* receive_data, com_src_t src) {
	if(state == active){
		switch(src){
		case SRC_RF:
			if(receive_data->command == PAGE){
				if(receive_data->product_id == my_product_id){
					ui_marker_on();
				    queue_insert(&send_data);
//					com_send(&send_data, DEST_RF);
//					ui_marker_off();
				}
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
	static uint16 old_weight = 0;
	static uint16 weight = 0;

	switch(state){
	case active:
		if (tick){

			// analoge scale
//			weight = scale_request();
//			send_data.arg = (double)weight;
			int16 dif = weight - old_weight;
			// reset Paging LED
//			if (abs(dif) >=  9)
//				ui_marker_off();
//			old_weight = weight;


//			send_cnt++;
//			if(send_cnt == 3){
//				com_send(&send_data, DEST_RF);
//				send_cnt = 0;
//			}
		}
		tick ^= 1;                   // toggel tick state
 		ui_tick();					 // blink red led to show sw is working
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
			ui_marker_off();
			calib_cnt = 0;
			calib_val = 0;
		}
		break;
	}

	//WDTCTL = WDTPW | WDTSSEL_1 | WDTCNTCL | WDTIS_4; // reset WDT

}

void conf_powmgmt(){

    UCB0CTLW0 |= UCSWRST; // SPI module B0 in reset state
    /* MOSI     -> P1.6   MOSI is secoundary module function on port 1
     * MISO     -> P1.7   MISO is secoundary module function on port 1
     * SCLK     -> P2.2   SCLK is secoundary module function on port 1
     * STE(CS)  -> P3.3   not used, set CS line manually before communication
     */
    // set module function for MISO, MOSI
    P1SEL1 |= ( BIT6 | BIT7);
    P1SEL0 &= ~( BIT6 | BIT7);
    // set module function for SCLK
    P2SEL1 |= BIT2;
    P2SEL0 &= ~(BIT2);
    // set MOSI, SCLK as OUTPUT
    P1DIR |= BIT6;
    P2DIR |= BIT2;
    // set MISO as INPUT
    P1DIR &= ~BIT7;
    // set Pull-UP on MISO
    P1REN |= BIT7;
    P1OUT |= BIT7;
    //STE(CS) set to OUTPUT and 0 (CS is active high)
    P3DIR |= BIT3;
    P3OUT &= ~BIT3;
    /* Configure
     * SPI                                    -> UCSYNC    set
     * 3 pin mode CS has to be set manually   -> UCMODE_0  set
     * Master mode                            -> UCMST     set
     * 8 Bit mode                             -> UC7BIT    clear
     * Most sicnificant bit first             -> UCMSB     set
     * Clock polarity: inactive stae is low   -> UCCPL     clear
     * Clock phase: capture on first edge     -> UCCKPH    set
     * Clock source SMCLK 1MHz                -> UCSSEL_2  set
     */
    UCB0CTLW0 &= ~(UC7BIT | UCCKPL);
    UCB0CTLW0 |= UCSYNC | UCMODE_0 | UCMST | UCMSB | UCCKPH | UCSSEL_2;
    UCB0BR0 = 0;            // no prescaler -> BitClk = ModuleClk(SMCLK = 1MHz)
    // Release for operation
    UCB0CTL1 &= ~UCSWRST;

    // disable Buck-boost converter on PM-Chip
    P1DIR |= BIT4;
    P1OUT &= ~BIT4;
    // activate chip
    P1DIR |= BIT3;
    P1OUT |= BIT3;

    //write Enable
    // set CS high
    P3OUT |= BIT3;
    __delay_cycles(10000); // delay 10ms (1MHz -> cycle=1us, 10ms = 10000
    // send address
    UCB0IFG &= ~UCRXIFG;
    UCB0TXBUF= 0x01;
    //wait for completion
    while(!(UCB0IFG & UCRXIFG));
    // send 0 to indicate write operation
    UCB0IFG &= ~UCRXIFG;
    UCB0TXBUF= 0x00;
    //wait for completion
    while(!(UCB0IFG & UCRXIFG));
    // send value
    UCB0IFG &= ~UCRXIFG;
    UCB0TXBUF= 0x80;
    //wait for completion
    while(!(UCB0IFG & UCRXIFG));
    __delay_cycles(10000); // delay 10ms (1MHz -> cycle=1us, 10ms = 10000
    // set CS low
    P3OUT &= ~BIT3;

    //write Voltage
    // set CS high
    P3OUT |= BIT3;
    __delay_cycles(10000); // delay 10ms (1MHz -> cycle=1us, 10ms = 10000
    // send address
    UCB0IFG &= ~UCRXIFG;
    UCB0TXBUF= 0x03;
    //wait for completion
    while(!(UCB0IFG & UCRXIFG));
    // send 0 to indicate write operation
    UCB0IFG &= ~UCRXIFG;
    UCB0TXBUF= 0x00;
    //wait for completion
    while(!(UCB0IFG & UCRXIFG));
    // send value
    UCB0IFG &= ~UCRXIFG;
    UCB0TXBUF= 0x27;
    //wait for completion
    while(!(UCB0IFG & UCRXIFG));
    __delay_cycles(10000); // delay 10ms (1MHz -> cycle=1us, 10ms = 10000
    // set CS low
    P3OUT &= ~BIT3;

    //write BB1 enable
    // set CS high
    P3OUT |= BIT3;
    __delay_cycles(10000); // delay 10ms (1MHz -> cycle=1us, 10ms = 10000
    // send address
    UCB0IFG &= ~UCRXIFG;
    UCB0TXBUF= 0x00;
    //wait for completion
    while(!(UCB0IFG & UCRXIFG));
    // send 0 to indicate write operation
    UCB0IFG &= ~UCRXIFG;
    UCB0TXBUF= 0x00;
    //wait for completion
    while(!(UCB0IFG & UCRXIFG));
    // send value
    UCB0IFG &= ~UCRXIFG;
    UCB0TXBUF= 0x05;
    //wait for completion
    while(!(UCB0IFG & UCRXIFG));
    __delay_cycles(10000); // delay 10ms (1MHz -> cycle=1us, 10ms = 10000
    // set CS low
    P3OUT &= ~BIT3;
}
