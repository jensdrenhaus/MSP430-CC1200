///*------------------------------------------------------------------------------
//| File: phy.c
//|
//| Implemts functionality for wired serial communication.
//|
//| Note: - uses UART A0 (backchannel UART) of MSP430 to commuinicate
//|		- implements UART AO ISR
// -----------------------------------------------------------------------------*/
//
//#include "phy.h"
//#include "msp430fr5969.h"
//#include <string.h>
//
//
//
////#############################################################################
//// globals
//
//static char      buf [PHY_MAX_BUF];
//static PHY_CB    g_callback;
//
//
//
////#############################################################################
//// module methods implementation:
////#############################################################################
//
//
//////////////////////////////////////////////////////////////////////////////
//
////!  PUBLIC PHY_init()
////!
//////////////////////////////////////////////////////////////////////////////
//void phy_init(PHY_CB callback) {
//
//	// ------------------------------------
//	// save funtion ptr to callback func.
//	// ------------------------------------
//	g_callback = callback;
//
//
//	// comment
//
//	P2SEL1 |= BIT0 | BIT1;          // Set port function to UART
//	P2SEL0 &= ~(BIT0 | BIT1);       // Set port function to UART
//
//	UCA0CTLW0 = UCSWRST;            // Put eUSCI in reset
//	UCA0CTLW0 |= UCSSEL__SMCLK;     // CLK = SMCLK
//	// Baud Rate calculation
//	// 1000000/(16*9600) = 6.510
//	// Fractional portion = 0.510
//	// User's Guide Table 21-4: UCBRSx = 0xAA
//	// UCBRFx = int ( (6.510-6)*16) = 8
//	UCA0BR0 = 6;                    // 8000000/16/9600
//	UCA0BR1 = 0x00;                 // UCA0BR is a word register, set high byte
//	UCA0MCTLW |= UCOS16 | UCBRF_8 | 0xAA00;
//	UCA0CTLW0 &= ~UCSWRST;          // Initialize eUSCI
//	UCA0IE |= UCRXIE;               // Enable RX Interrupt
//
//
//}
//
//////////////////////////////////////////////////////////////////////////////
//
////!  PUBLIC PHY_send()
////!
//////////////////////////////////////////////////////////////////////////////
//void phy_send(char *string) {
//	int n = 0;
//	while(1) {
//		while(!(UCA0IFG&UCTXIFG));
//		UCA0TXBUF = string[n];
//		n++;
//		if (string[n] == '\0') break;
//
//	}
//}
//
//
//
////#############################################################################
//// interrupt service routines:
////#############################################################################
//
//
//////////////////////////////////////////////////////////////////////////////
//
////! UART A0 ISR for communication intup
////!
//////////////////////////////////////////////////////////////////////////////
////#pragma vector=USCI_A0_VECTOR
////__interrupt void USCIA0RX_ISR(void)
////{
////	switch(__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG)) // check UART IFGs
////	  {
////	    case USCI_NONE: break;
////	    case USCI_UART_UCRXIFG:
////	    	strcat(buf, (const char*)&UCA0RXBUF);
////			if(UCA0RXBUF == '\n'){
////				g_callback(buf);
////				strcpy(buf,"");
////			}
////	        break;
////	    case USCI_UART_UCTXIFG: break;
////	    case USCI_UART_UCSTTIFG: break;
////	    case USCI_UART_UCTXCPTIFG: break;
////	  }
////
////}
//
//
