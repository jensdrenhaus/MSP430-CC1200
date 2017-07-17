/*------------------------------------------------------------------------------
| File: serial.c
|
| Implemts functionality for wired serial communication.
|
| Note: - uses UART A0 (backchannel UART) of MSP430 to commuinicate
|		- implements UART AO ISR
 -----------------------------------------------------------------------------*/

#include <serial.h>
#include "msp430fr5969.h"
#include <string.h>
#include "types.h"



//#############################################################################
// globals

static char         buf [SERIAL_MAX_BUF];
static uint8        buf_fix [SERIAL_FIX_BUF];
static com_frame_t  buf_frame;
static uint16       rec_buf_cnt;
static SERIAL_CB    g_callback;



//#############################################################################
// module methods implementation:
//#############################################################################


////////////////////////////////////////////////////////////////////////////

//!  PUBLIC serial_init()
//!
////////////////////////////////////////////////////////////////////////////
void serial_init(SERIAL_CB callback) {

	// ------------------------------------
	// save funtion ptr to callback func.
	// ------------------------------------
	g_callback = callback;


	// comment
	P2SEL0 &= ~(BIT0 | BIT1);       // Set port function to UART
	P2SEL1 |= BIT0 | BIT1;          // Set port function to UART

	UCA0CTLW0 = UCSWRST;            // Put eUSCI in reset
	UCA0CTLW0 |= UCSSEL__SMCLK;     // CLK = SMCLK
	// Baud Rate calculation
	// 1000000/(16*9600) = 6.510
	// Fractional portion = 0.510
	// User's Guide Table 21-4: UCBRSx = 0xAA
	// UCBRFx = int ( (6.510-6)*16) = 8
//	UCA0BR0 = 6;                    // 1000000/16/9600
//	UCA0BR1 = 0x00;                 // UCA0BR is a word register, set high byte
//	UCA0MCTLW |= UCOS16 | UCBRF_8 | 0xAA00;

	// Baud Rate settings for 57600Baud (Table 24-5 Users Guid):
	// UCOS16 = 0 , UCBRx = 17, UCBRSx = 0x4A
	UCA0BRW = 17;
    UCA0MCTLW = 0x4A00;

	UCA0CTLW0 &= ~UCSWRST;          // Initialize eUSCI
	UCA0IE |= UCRXIE;               // Enable RX Interrupt

	rec_buf_cnt = SERIAL_FIX_BUF;
}

////////////////////////////////////////////////////////////////////////////

//!  PUBLIC serial_send()
//!
////////////////////////////////////////////////////////////////////////////
void serial_send_fix(com_frame_t *frame) {
	uint16 i;
	for(i=0; i<COM_FRAME_LEN; i++){
		while(!(UCA0IFG&UCTXIFG));
		UCA0TXBUF = frame->array[(COM_FRAME_LEN)-1-i];
	}
}

void serial_send(char *string) {
	int n = 0;
	while(1) {
		while(!(UCA0IFG&UCTXIFG));
		UCA0TXBUF = string[n];
		n++;
		if (string[n] == '\0') break;

	}
}

void serial_debug_byte(uint8 byte) {
	while(!(UCA0IFG&UCTXIFG));
	UCA0TXBUF = byte;
}

void serial_debug_word(uint16 word) {
	// send high-byte first
	uint8 byte = word>>8;
	while(!(UCA0IFG&UCTXIFG));
	UCA0TXBUF = byte;
	byte = (uint8) word;
	while(!(UCA0IFG&UCTXIFG));
	UCA0TXBUF = byte;
}



//#############################################################################
// interrupt service routines:
//#############################################################################


////////////////////////////////////////////////////////////////////////////

//! UART A0 ISR for communication intup
//!
////////////////////////////////////////////////////////////////////////////
#pragma vector=USCI_A0_VECTOR
__interrupt void USCIA0RX_ISR(void)
{
	// for fix packet length
	switch(__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG)) // check UART IFGs
	  {
		case USCI_NONE: break;
		case USCI_UART_UCRXIFG:
		    rec_buf_cnt--;
			buf_frame.array[rec_buf_cnt] = (uint8)UCA0RXBUF;
			if(rec_buf_cnt == 0){
				g_callback(&buf_frame, SRC_SERIAL);
				rec_buf_cnt = SERIAL_FIX_BUF;
			}
			break;
		case USCI_UART_UCTXIFG: break;
		case USCI_UART_UCSTTIFG: break;
		case USCI_UART_UCTXCPTIFG: break;
	  }


	// for older text based commands
//	switch(__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG)) // check UART IFGs
//	  {
//	    case USCI_NONE: break;
//	    case USCI_UART_UCRXIFG:
//	    	strcat(buf, (const char*)&UCA0RXBUF);
//			if(UCA0RXBUF == '\n'){
//				g_callback(buf, SRC_SERIAL);
//				strcpy(buf,"");
//			}
//	        break;
//	    case USCI_UART_UCTXIFG: break;
//	    case USCI_UART_UCSTTIFG: break;
//	    case USCI_UART_UCTXCPTIFG: break;
//	  }

}


