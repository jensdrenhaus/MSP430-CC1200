#include "msp430fr5969.h"
#include <string.h>

// Globals
int pressed = 0;
const int MY_BOX_ID = 42;

// Defines

typedef struct{
	char string [20];
} Data;
Data data = {""};

typedef struct{
	double weight;
}Sensor_Data;
Sensor_Data sensor_data = {0.0};

// Prototypes
void send_string(char *string);





void main(void) {

    WDTCTL = WDTPW | WDTHOLD;           // Stop watchdog timer

    //#########################################################################
    // Configure Timer A0 used for debouncing
    //#########################################################################

    TA0CTL |= (TASSEL__SMCLK | MC__UP | ID__8);          // SMCLK/8 , UP mode
    TA0CTL |= TACLR;                 // clear to acticate new clock settings
    TA0CCR0 = 5000;                  // SMCLK/8/5000 = 25Hz => 40ms
    TA0CCTL0 &= ~CCIE;               // TACCR0 interrupt disabled

    //#########################################################################
    // Configure Timer A1 used for periodic actions
    //#########################################################################

    TA1CTL |= (TASSEL__SMCLK | MC__UP | ID__8);          // SMCLK/8 , UP mode
    TA1CTL |= TACLR;                 // clear to acticate new clock settings
    TA1CCR0 = 62500;                 // SMCLK/8/62500 = 2Hz => 0,5s
    TA1CCTL0 |= CCIE;                // TACCR0 interrupt enabled

    //#########################################################################
    // Configure UART A0 for debugging (backchannel uart)
    //#########################################################################

    P2SEL1 |= BIT5 | BIT6;          // Set port function to UART
    P2SEL0 &= ~(BIT5 | BIT6);		// Set port function to UART

    UCA0CTLW0 = UCSWRST;            // Put eUSCI in reset
    UCA0CTLW0 |= UCSSEL__SMCLK;     // CLK = SMCLK
    // Baud Rate calculation
    // 1000000/(16*9600) = 6.510
    // Fractional portion = 0.510
    // User's Guide Table 21-4: UCBRSx = 0xAA
    // UCBRFx = int ( (6.510-6)*16) = 8
    UCA0BR0 = 6;                    // 8000000/16/9600
    UCA0BR1 = 0x00;				 // UCA0BR is a word register, set high byte
    UCA0MCTLW |= UCOS16 | UCBRF_8 | 0xAA00;
    UCA0CTLW0 &= ~UCSWRST;          // Initialize eUSCI


    //#########################################################################
    // Configure UART A1 for measurement datat input
    //#########################################################################

    P2SEL1 |= BIT0 | BIT1;          // Set port function to UART
    P2SEL0 &= ~(BIT0 | BIT1);		// Set port function to UART

    UCA1CTLW0 = UCSWRST;            // Put eUSCI in reset
    UCA1CTLW0 |= UCSSEL__SMCLK;     // CLK = SMCLK
    // Baud Rate calculation
    // 1000000/(16*9600) = 6.510
    // Fractional portion = 0.510
    // User's Guide Table 21-4: UCBRSx = 0xAA
    // UCBRFx = int ( (6.510-6)*16) = 8
    UCA1BR0 = 6;                    // 8000000/16/9600
    UCA1BR1 = 0x00;				    // UCA0BR is a word register, set high-byte
    UCA1MCTLW |= UCOS16 | UCBRF_8 | 0xAA00;
    UCA1CTLW0 &= ~UCSWRST;          // Initialize eUSCI
    UCA1IE |= UCRXIE;               // Enable USCI_A1 RX interrupt

    //#########################################################################
    // Configure GPIO basic user interface
    //#########################################################################

    PM5CTL0	&=	~LOCKLPM5;			// Disable the GPIO power-on default
    								//               high-impedance mode


    P2SEL1 |= BIT0 | BIT1;          // Set port function to UART
    P2SEL0 &= ~(BIT0 | BIT1);       // Set port function to UART

    UCA0CTLW0 = UCSWRST;            // Put eUSCI in reset
    UCA0CTLW0 |= UCSSEL__SMCLK;     // CLK = SMCLK
    // Baud Rate calculation
    // 1000000/(16*9600) = 6.510
    // Fractional portion = 0.510
    // User's Guide Table 21-4: UCBRSx = 0xAA
    // UCBRFx = int ( (6.510-6)*16) = 8
    UCA0BR0 = 6;                    // 8000000/16/9600
    UCA0BR1 = 0x00;                 // UCA0BR is a word register, set high byte
    UCA0MCTLW |= UCOS16 | UCBRF_8 | 0xAA00;
    UCA0CTLW0 &= ~UCSWRST;          // Initialize eUSCI
    UCA0IE |= UCRXIE;               // Enable RX Interrupt

    // Configure GPIO LEDs
    P1DIR |= BIT0;                  // Set P1.0 to output direction
    P4DIR |= BIT6;                  // Set P4.6 to output direction
    // Configure GPIO Button
    P1DIR &= ~BIT1;                 // Set P1.1 to input direction
    P1REN |= BIT1;                  // Set P1.1 pullup/down Resistor
    P1OUT |= BIT1;                  // Select P1.1 pull-up
    P1IE  |= BIT1;                  // Enable Interrupt on P1.1
    P1IES |= BIT1;                  // High/Low Edge
    P1IFG &= ~BIT1;                 // clear P1.1 interrupt flag



    _EINT();                        // Global interrupt enable


    while(1)
    {

    }
}

//#############################################################################
// Port 1.1 ISR for button debounce
//#############################################################################
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    P1IE &= ~BIT1;                  // Disable Interrupt on P1.1
    P1IFG &= ~BIT1;                 // clear P1.1 interrupt flag
    TA0CCTL0 |= CCIE;               // TACCR0 interrupt enabled
    TA0CTL |= TACLR;                // start Timer TA0
    pressed = 1;                    // save buttonstate
}

//#############################################################################
// Timer A0 ISR for button debounce
//#############################################################################
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
        while(!(UCA0IFG&UCTXIFG));
        UCA0TXBUF = 0x0A;           // send NL
        while(!(UCA0IFG&UCTXIFG));
        UCA0TXBUF = 0x0D;           // send CR
        P1OUT ^= BIT0;              // toggle green LED
        // ###############################

        pressed = 0;
        TA0CCTL0 &= ~CCIE;          // TACCR0 interrupt disabled
        P1IFG &= ~BIT1;             // clear P1.1 interrupt flag
        P1IE |= BIT1;               // Enable Interrupt on P1.1
    }
}

//#############################################################################
// Timer A1 ISR for periodic events
//#############################################################################
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer0_A1(void)
{
	static int num = 0x30;
	static int tick = 1;        // set state for full secound

	if (tick) {

	}

	tick ^= 1;                   // toggel tick state
	P4OUT ^= BIT6;				 // toggel red LED

}

//#############################################################################
// UART A1 ISR for measurement data intup
//#############################################################################
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
  switch(__even_in_range(UCA1IV, USCI_UART_UCTXCPTIFG)) // check UART IFGs
  {
    case USCI_NONE: break;
    case USCI_UART_UCRXIFG:
      while(!(UCA1IFG&UCTXIFG));
      UCA0TXBUF = UCA1RXBUF;
      __no_operation();
      break;
    case USCI_UART_UCTXIFG: break;
    case USCI_UART_UCSTTIFG: break;
    case USCI_UART_UCTXCPTIFG: break;
  }
}




//#############################################################################
// UART A1 ISR for communication intup
//#############################################################################
#pragma vector=USCI_A0_VECTOR
__interrupt void USCIA0RX_ISR(void)
{
	switch(__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG)) // check UART IFGs
	  {
	    case USCI_NONE: break;
	    case USCI_UART_UCRXIFG:
	    	strcat(data.string, &UCA0RXBUF);
			if(UCA0RXBUF == '\0'){
				send_string(data.string);
				strcpy(data.string,"");
			}
	        break;
	    case USCI_UART_UCTXIFG: break;
	    case USCI_UART_UCSTTIFG: break;
	    case USCI_UART_UCTXCPTIFG: break;
	  }

}

void send_string(char *string) {
	int n = 0;
	while(1) {
		while(!(UCA0IFG&UCTXIFG));
		UCA0TXBUF = string[n];
		if (string[n] == '\0') break;
		n++;
	}
}
