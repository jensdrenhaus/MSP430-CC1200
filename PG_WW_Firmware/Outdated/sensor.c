/*------------------------------------------------------------------------------
| File: sensor.c
|
| Say something about the module. Copy that part from name.h
|
| Note: important things
 -----------------------------------------------------------------------------*/

#include "sensor.h"
#include "msp430fr5969.h"
#include <string.h>
#include <stdlib.h>



//#############################################################################
// globals

static char      buf[25];
static SEN_CB    g_sen_callback;



//#############################################################################
// private function prototypes
static int converter();


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

	P2SEL0 &= ~(BIT5 | BIT6);		// Set port function to UART
	P2SEL1 |= BIT5 | BIT6;          // Set port function to UART

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

//!  PUBLIC sen_request()
//!
////////////////////////////////////////////////////////////////////////////
void sen_request() {
	while(!(UCA1IFG&UCTXIFG));
	UCA1TXBUF = 's';
}

////////////////////////////////////////////////////////////////////////////

//!  PRIVATE converter()
//!
////////////////////////////////////////////////////////////////////////////
int converter() {
	char tmp_str[15] = "";
	int i = 11;
	int j = 0;
	while(buf[i] != ' '){
		i--;
	}
	i++;
	while(i <= 11){
		tmp_str[j] = buf[i];
		i++;
		j++;
	}
	tmp_str[j] = '\0';
	return atoi(tmp_str);
}

//#############################################################################
// interrupt service routines:
//#############################################################################

////////////////////////////////////////////////////////////////////////////

//! UART A1 ISR for measurement data intup
//!
////////////////////////////////////////////////////////////////////////////
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void){
  switch(__even_in_range(UCA1IV, USCI_UART_UCTXCPTIFG)) // check UART IFGs
  {
    case USCI_NONE: break;
    case USCI_UART_UCRXIFG:
      while(!(UCA1IFG&UCTXIFG));
      // read out serial data UCA1RXBUF
      strcat(buf, (const char*)&UCA1RXBUF);
      if(UCA1RXBUF == '\n'){
    	  g_sen_callback(converter());
    	  strcpy(buf,"");
      }
      break;
    case USCI_UART_UCTXIFG: break;
    case USCI_UART_UCSTTIFG: break;
    case USCI_UART_UCTXCPTIFG: break;
  }
}




