/*------------------------------------------------------------------------------
| File: sensor.c
|
| Say something about the module. Copy that part from name.h
|
| Note: important things
 -----------------------------------------------------------------------------*/

#include "sensor.h"
#include "msp430fr5969.h"



//#############################################################################
// globals

static SEN_CB g_sen_callback;



//#############################################################################
// private function prototypes
//static void foo();


//#############################################################################
// module methods implementation:
//#############################################################################


////////////////////////////////////////////////////////////////////////////

//!  PUBLIC module_somefunction()
//!
////////////////////////////////////////////////////////////////////////////
void sen_init(SEN_CB callback){
	g_sen_callback = callback;

	//----------------------------------------------
	// Configure UART A1 for measurement datat input
	//----------------------------------------------

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
}

////////////////////////////////////////////////////////////////////////////

//!  PUBLIC module_somefunction()
//!
////////////////////////////////////////////////////////////////////////////
void sen_request() {
	UCA0TXBUF = 'w';
}



//#############################################################################
// interrupt service routines:
//#############################################################################

////////////////////////////////////////////////////////////////////////////

//! UART A1 ISR for measurement data intup
//!
////////////////////////////////////////////////////////////////////////////
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
  switch(__even_in_range(UCA1IV, USCI_UART_UCTXCPTIFG)) // check UART IFGs
  {
    case USCI_NONE: break;
    case USCI_UART_UCRXIFG:
      while(!(UCA1IFG&UCTXIFG));
      // read out serial data UCA1RXBUF
      // convert to int
      // g_sen_callback(val)
      break;
    case USCI_UART_UCTXIFG: break;
    case USCI_UART_UCSTTIFG: break;
    case USCI_UART_UCTXCPTIFG: break;
  }
}


