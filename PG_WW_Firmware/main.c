#include <msp430.h> 
#include <driverlib.h>
#include "msp430fr5969.h"

// Globals
int pressed = 0;



void main(void) {
    WDTCTL = WDTPW | WDTHOLD;			// Stop watchdog timer

    // Configure Timer

    TA0CTL |= (TASSEL__SMCLK | MC__UP | ID__8);          // SMCLK/8 , UP mode
    TA0CTL |= TACLR;                 // clear to acticate new clock settings
    TA0CCR0 = 5000; 				 // SMCLK/8/10000 = 25Hz => 40ms
    TA0CCTL0 &= ~CCIE;               // TACCR0 interrupt disabled

    // Configure GPIO

    PM5CTL0	&=	~LOCKLPM5;			// Disable the GPIO power-on default high-impedance mode

    P1OUT &= 0x00;					// Shut down everything
    P1DIR &= 0x00;
    P4OUT &= 0x00;					// Shut down everything
    P4DIR &= 0x00;
    // Configure GPIO LEDs
    P1DIR |= BIT0;					// Set P1.0 to output direction
    P4DIR |= BIT6;					// Set P4.6 to output direction
    // Configure GPIO Button
    P1DIR &= ~BIT1;					// Set P1.1 to input direction
    P1REN |= BIT1;					// Set P1.1 pullup/down Resistor
    P1OUT |= BIT1;					// Select P1.1 pull-up
    P1IE |=	BIT1;					// Enable Interrupt on P1.1
    P1IES |= BIT1;					// High/Low Edge
    P1IFG &= ~BIT1;                 // clear P1.1 interrupt flag



    _EINT();						// Global interrupt enable

    while(1)
    {

    }
}


#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
 	P1IE &=	~BIT1;					// Disable Interrupt on P1.1
	P1IFG &= ~BIT1;                 // clear P1.1 interrupt flag
	TA0CCTL0 |= CCIE;               // TACCR0 interrupt enabled
	TA0CTL |= TACLR;                // start Timer TA0
	pressed = 1;                    // save buttonstate
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0(void)
{
	// check if button is still pressed
	if (!(P1IN & BIT1) && pressed) {
		TA0CTL |= TACLR;            // reset Timer TA0
	}
	// button released ?
	else if ((P1IN & BIT1) && pressed) {
		pressed = 0;
		P1OUT ^= BIT0;				// toggel green LED
		TA0CCTL0 &= ~CCIE;          // TACCR0 interrupt disabled
		P1IFG &= ~BIT1;             // clear P1.1 interrupt flag
		P1IE |=	BIT1;				// Enable Interrupt on P1.1
	}
}






















//#define ONE_SECOND 800000
//
//volatile unsigned int state = 0;
//
//
//
//int button_klicked();
//
//int main(void) {
//    WDT_A_hold(WDT_A_BASE);	   								// Stop watchdog timer
//
//    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
//    GPIO_setOutputLowOnPin( GPIO_PORT_P1, GPIO_PIN0 );
//    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
//
//    PMM_unlockLPM5();          								// unlock pins (fr5969 spezific pin state after reset)
//
//
//    while(1) {
//
//        if ( button_klicked()) {
//
//        	if (state == 0) {
//        		// turn on LED
//        		GPIO_setOutputHighOnPin( GPIO_PORT_P1, GPIO_PIN0 );
//        		state = 1;
//        		continue;
//        	}
//        	if (state == 1) {
//        		// turn off LED
//        		GPIO_setOutputLowOnPin( GPIO_PORT_P1, GPIO_PIN0 );
//        		state = 0;
//        		continue;
//        	}
//        }
//    }
//
//	return 0;
//}
//
//int button_klicked() {
//	static unsigned int pinVal;
//	pinVal = GPIO_getInputPinValue( GPIO_PORT_P1, GPIO_PIN1 );
//
//	if(pinVal == 0) {
//		_delay_cycles(1600);  												// ca 2ms debounce
//		do {
//			pinVal = GPIO_getInputPinValue( GPIO_PORT_P1, GPIO_PIN1 );		// wait for button to be released
//		} while (pinVal == 0);
//		_delay_cycles(1600);												// ca 2ms debounce
//		return 1;
//	}
//
//	return 0;
//}

