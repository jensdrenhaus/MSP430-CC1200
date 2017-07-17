/*-----------------------------------------------------------------------------
| File: ui.c
|
| Say something about the module. Copy that part from name.h
|
| Note: important things
 -----------------------------------------------------------------------------*/

#include "ui.h"
#include "msp430fr5969.h"
#include "types.h"

//#############################################################################
// globals

static UI_CB g_ui_callback1;
static UI_CB g_ui_callback2;
static int pressed1 = 0;
static int pressed2 = 0;
static uint8 cnt = 0;



//#############################################################################
// private function prototypes



//#############################################################################
// module methods implementation:
//#############################################################################


////////////////////////////////////////////////////////////////////////////

//!  PUBLIC module_somefunction()
//!
////////////////////////////////////////////////////////////////////////////
void ui_init(UI_CB callback1, UI_CB callback2){

	g_ui_callback1 = callback1;
	g_ui_callback2 = callback2;

	//------------------------------------------
	// Configure Timer A0 used for debouncing button 1
	//------------------------------------------

	TA0CTL |= (TASSEL__SMCLK | MC__UP | ID__8);          // SMCLK/8 , UP mode
	TA0CTL |= TACLR;                 // clear to acticate new clock settings
	TA0CCR0 = 5000;                  // SMCLK/8/5000 = 25Hz => 40ms
	TA0CCTL0 &= ~CCIE;               // TACCR0 interrupt disabled

	//------------------------------------------
	// Configure Timer A2 used for debouncing button 2
	//------------------------------------------

	TA2CTL |= (TASSEL__SMCLK | MC__UP | ID__8);          // SMCLK/8 , UP mode
	TA2CTL |= TACLR;                 // clear to acticate new clock settings
	TA2CCR0 = 5000;                  // SMCLK/8/5000 = 25Hz => 40ms
	TA2CCTL0 &= ~CCIE;               // TACCR0 interrupt disabled

	//------------------------------------------
	// Configure GPIO
	//------------------------------------------

	PM5CTL0	&=	~LOCKLPM5;			// Disable the GPIO power-on default
									//               high-impedance mode
#ifdef PHYNODE
	// Configure GPIO LEDs
    P3DIR |= BIT5;                  // Set P3.5 to output direction
    P4DIR |= BIT2;                  // Set P4.2 to output direction

    P3OUT &= ~BIT5;                // reset green LED

    // Configure GPIO Buttons
    P3DIR &= ~BIT6;                 // Set P3.6 to input direction
    P3REN |= BIT6;                  // Set P3.6 pullup/down Resistor
    P3OUT |= BIT6;                  // Select P3.6 pull-up
    P3IE  |= BIT6;                  // Enable Interrupt on P3.6
    P3IES |= BIT6;                  // High/Low Edge
    P3IFG &= ~BIT6;                 // clear P3.6 interrupt flag

    P3DIR &= ~BIT7;                 // Set P3.7 to input direction
    P3REN |= BIT7;                  // Set P3.7 pullup/down Resistor
    P3OUT |= BIT7;                  // Select P3.7 pull-up
    P3IE  |= BIT7;                  // Enable Interrupt on P3.7
    P3IES |= BIT7;                  // High/Low Edge
    P3IFG &= ~BIT7;                 // clear P3.7 interrupt flag
#else
	// Configure GPIO LEDs
	P1DIR |= BIT0;                  // Set P1.0 to output direction
	P4DIR |= BIT6;                  // Set P4.6 to output direction

	P1OUT &= ~BIT0;                // reset green LED

	// Configure GPIO Buttons
	P1DIR &= ~BIT1;                 // Set P1.1 to input direction
	P1REN |= BIT1;                  // Set P1.1 pullup/down Resistor
	P1OUT |= BIT1;                  // Select P1.1 pull-up
	P1IE  |= BIT1;                  // Enable Interrupt on P1.1
	P1IES |= BIT1;                  // High/Low Edge
	P1IFG &= ~BIT1;                 // clear P1.1 interrupt flag

	P4DIR &= ~BIT5;                 // Set P4.5 to input direction
	P4REN |= BIT5;                  // Set P4.5 pullup/down Resistor
	P4OUT |= BIT5;                  // Select P4.5 pull-up
	P4IE  |= BIT5;                  // Enable Interrupt on P1.1
	P4IES |= BIT5;                  // High/Low Edge
	P4IFG &= ~BIT5;                 // clear P4.5 interrupt flag
#endif
}

inline void ui_tick(){
#ifdef PHYNODE
    P4OUT ^= BIT2;               // toggel red LED
#else
	P4OUT ^= BIT6;				 // toggel red LED
#endif
}


inline void ui_toggle_status(){
#ifdef PHYNODE
    P3OUT ^= BIT5;              // toggle green LED
#else
	P1OUT ^= BIT0;              // toggle green LED
#endif
}


inline void ui_marker_on(){
#ifdef PHYNODE
    P3OUT |= BIT5;              // set green LED
#else
	P1OUT |= BIT0;              // set green LED
#endif
}


inline void ui_marker_off(){
#ifdef PHYNODE
    P3OUT &= ~BIT5;             // reset green LED
#else
	P1OUT &= ~BIT0;             // reset green LED
#endif
}


inline void ui_red_on(){
#ifdef PHYNODE
    P4OUT |= BIT2;
#else
	P4OUT |= BIT6;
#endif
}


inline void ui_red_off(){
#ifdef PHYNODE
    P4OUT &= ~BIT2;
#else
	P4OUT &= ~BIT6;
#endif
}


//#############################################################################
// interrupt service routines:
//#############################################################################

#ifndef PHYNODE
////////////////////////////////////////////////////////////////////////////

//!  Port 1.1 ISR for button 1 debounce
//!
////////////////////////////////////////////////////////////////////////////
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    P1IE &= ~BIT1;                  // Disable Interrupt on P1.1
    P1IFG &= ~BIT1;                 // clear P1.1 interrupt flag
    TA0CCTL0 |= CCIE;               // TACCR0 interrupt enabled
    TA0CTL |= TACLR;                // start Timer TA0
    pressed1 = 1;                    // save buttonstate
}
#endif

////////////////////////////////////////////////////////////////////////////

//!  Timer A0 ISR for button 1 debounce
//!
////////////////////////////////////////////////////////////////////////////
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0(void)
{
    // check if button is still pressed
    if (!(P1IN & BIT1) && pressed1) {
        TA0CTL |= TACLR;            // reset Timer TA0
    }
    // button released ?
    else if ((P1IN & BIT1) && pressed1) {

        // ### button released action ###
        g_ui_callback1();
        // ###############################

        pressed1 = 0;
        TA0CCTL0 &= ~CCIE;          // TACCR0 interrupt disabled
        P1IFG &= ~BIT1;             // clear P1.1 interrupt flag
        P1IE |= BIT1;               // Enable Interrupt on P1.1
    }
}

#ifndef PHYNODE
////////////////////////////////////////////////////////////////////////////

//!  Port 4.5 ISR for button 2 debounce
//!
////////////////////////////////////////////////////////////////////////////
#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void)
{
    P4IE &= ~BIT5;                  // Disable Interrupt on P1.1
    P4IFG &= ~BIT5;                 // clear P4.5 interrupt flag
    TA2CCTL0 |= CCIE;               // TACCR0 interrupt enabled
    TA2CTL |= TACLR;                // start Timer TA2
    pressed2 = 1;                    // save buttonstate
}
#endif

////////////////////////////////////////////////////////////////////////////

//!  Timer A2 ISR for button 2 debounce
//!
////////////////////////////////////////////////////////////////////////////
#pragma vector=TIMER2_A0_VECTOR
__interrupt void Timer2_A0(void)
{
    // check if button is still pressed
    if (!(P4IN & BIT5) && pressed2) {
    	cnt++;
    	if(cnt>100){
    		// ### button hold for 4 sec action ###
			g_ui_callback2();
			// ####################################

			pressed2 = 0;
			TA2CCTL0 &= ~CCIE;          // TACCR0 interrupt disabled
			P4IFG &= ~BIT5;             // clear P4.5 interrupt flag
			P4IE |= BIT5;               // Enable Interrupt on P4.5

			cnt = 0;
    	}
    	TA2CTL |= TACLR;            // reset Timer TA2, another 40ms
    }
    // button released ?
    else if ((P4IN & BIT5) && pressed2) {

    	cnt = 0;

        // ### button released action ###
        // g_ui_callback2();
        // ###############################

        pressed2 = 0;
        TA2CCTL0 &= ~CCIE;          // TACCR0 interrupt disabled
        P4IFG &= ~BIT5;             // clear P4.5 interrupt flag
        P4IE |= BIT5;               // Enable Interrupt on P4.5
    }
}



