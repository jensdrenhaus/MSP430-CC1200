/*-----------------------------------------------------------------------------
| File: ui.c
|
| Say something about the module. Copy that part from name.h
|
| Note: important things
 -----------------------------------------------------------------------------*/

#include "ui.h"
#include "msp430fr5969.h"

//#############################################################################
// globals

static UI_CB g_ui_callback;
static int pressed = 0;



//#############################################################################
// private function prototypes



//#############################################################################
// module methods implementation:
//#############################################################################


////////////////////////////////////////////////////////////////////////////

//!  PUBLIC module_somefunction()
//!
////////////////////////////////////////////////////////////////////////////
void ui_init(UI_CB callback){

	g_ui_callback = callback;

	//------------------------------------------
	// Configure Timer A0 used for debouncing
	//------------------------------------------

	TA0CTL |= (TASSEL__SMCLK | MC__UP | ID__8);          // SMCLK/8 , UP mode
	TA0CTL |= TACLR;                 // clear to acticate new clock settings
	TA0CCR0 = 5000;                  // SMCLK/8/5000 = 25Hz => 40ms
	TA0CCTL0 &= ~CCIE;               // TACCR0 interrupt disabled

	//------------------------------------------
	// Configure Timer A1 used for periodic actions
	//------------------------------------------

	TA1CTL |= (TASSEL__SMCLK | MC__UP | ID__8);          // SMCLK/8 , UP mode
	TA1CTL |= TACLR;                 // clear to acticate new clock settings
	TA1CCR0 = 62500;                 // SMCLK/8/62500 = 2Hz => 0,5s
	TA1CCTL0 |= CCIE;                // TACCR0 interrupt enabled

	//------------------------------------------
	// Configure GPIO
	//------------------------------------------

	PM5CTL0	&=	~LOCKLPM5;			// Disable the GPIO power-on default
									//               high-impedance mode

	// Configure GPIO LEDs
	P1DIR |= BIT0;                  // Set P1.0 to output direction
	P4DIR |= BIT6;                  // Set P4.6 to output direction

	P1OUT &= ~BIT0;                // reset green LED

	// Configure GPIO Button
	P1DIR &= ~BIT1;                 // Set P1.1 to input direction
	P1REN |= BIT1;                  // Set P1.1 pullup/down Resistor
	P1OUT |= BIT1;                  // Select P1.1 pull-up
	P1IE  |= BIT1;                  // Enable Interrupt on P1.1
	P1IES |= BIT1;                  // High/Low Edge
	P1IFG &= ~BIT1;                 // clear P1.1 interrupt flag

}

void ui_tick(){
	P4OUT ^= BIT6;				 // toggel red LED
}

void ui_toggle_status(){
//	while(!(UCA0IFG&UCTXIFG));
//	UCA0TXBUF = 0x0A;           // send NL
//	while(!(UCA0IFG&UCTXIFG));
//	UCA0TXBUF = 0x0D;           // send CR
	P1OUT ^= BIT0;              // toggle green LED
}

void ui_marker_on(){
	P1OUT |= BIT0;              // set green LED
}
void ui_marker_off(){
	P1OUT &= ~BIT0;             // reset green LED
}

//#############################################################################
// interrupt service routines:
//#############################################################################


////////////////////////////////////////////////////////////////////////////

//!  Port 1.1 ISR for button debounce
//!
////////////////////////////////////////////////////////////////////////////
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    P1IE &= ~BIT1;                  // Disable Interrupt on P1.1
    P1IFG &= ~BIT1;                 // clear P1.1 interrupt flag
    TA0CCTL0 |= CCIE;               // TACCR0 interrupt enabled
    TA0CTL |= TACLR;                // start Timer TA0
    pressed = 1;                    // save buttonstate
}

////////////////////////////////////////////////////////////////////////////

//!  Timer A0 ISR for button debounce
//!
////////////////////////////////////////////////////////////////////////////
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0(void)
{
    // check if button is still pressed
    if (!(P1IN & BIT1) && pressed) {
        TA0CTL |= TACLR;            // reset Timer TA0
    }
    // button released ?
    else if ((P1IN & BIT1) && pressed) {

        // ### button released action ###
        g_ui_callback();
        // ###############################

        pressed = 0;
        TA0CCTL0 &= ~CCIE;          // TACCR0 interrupt disabled
        P1IFG &= ~BIT1;             // clear P1.1 interrupt flag
        P1IE |= BIT1;               // Enable Interrupt on P1.1
    }
}




