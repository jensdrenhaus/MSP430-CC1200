/*------------------------------------------------------------------------------
| File: rf.c
|
| --
|
| Note: --
 -----------------------------------------------------------------------------*/

#include "rf.h"
#include "spi.h"
#include "msp430fr5969.h"




//#############################################################################
// globals

static RF_CB    g_callback;

//#############################################################################
// private function prototypes
rfStatus_t read_reg(uint16 addr, uint8 *data, uint8 len);
rfStatus_t wrtie_reg(uint16 addr, uint8 *data, uint8 len);
rfStatus_t wrtie_tx_fifo(uint8 *data, uint8 len);
rfStatus_t read_rx_fifo(uint8 *data, uint8 len);
rfStatus_t get_tx_status();
rfStatus_t get_rx_statua();


//#############################################################################
// module methods implementation:
//#############################################################################


////////////////////////////////////////////////////////////////////////////

//!  PUBLIC PHY_init()
//!
////////////////////////////////////////////////////////////////////////////
void rf_init(RF_CB callback) {

	// ------------------------------------
	// save funtion ptr to callback func.
	// ------------------------------------
	g_callback = callback;




}

////////////////////////////////////////////////////////////////////////////

//!  PUBLIC PHY_send()
//!
////////////////////////////////////////////////////////////////////////////
void phy_send(char *string) {

}

rfStatus_t read_reg(uint16 addr, uint8 *data, uint8 len){

	uint8 tmp_ext  = (uint8)(addr>>8);
	uint8 tmp_addr = (uint8)(addr & 0x00FF);
	uint8 status;

	/* Checking if this is a FIFO access -> returns chip not ready  */
	if((RF_SINGLE_TXFIFO<=tmp_addr)&&(tmp_ext==0)) return -1;

	/* Decide what register space is accessed */
	if(!tmp_ext)
	{
	  status = spi_reg_access((SPI_READ_BURST), tmp_addr, data,len);
	}
	else if (tmp_ext == 0x2F)
	{
	  status = spi_ext_reg_access((SPI_READ_BURST),tmp_ext,tmp_addr,data,len);
	}
	return (status);
}

rfStatus_t wirte_reg(uint16 addr, uint8 *data, uint8 len){

	uint8 tmp_ext  = (uint8)(addr>>8);
	uint8 tmp_addr = (uint8)(addr & 0x00FF);
	uint8 status;

	/* Checking if this is a FIFO access -> returns chip not ready  */
	if((RF_SINGLE_TXFIFO<=tmp_addr)&&(tmp_ext==0)) return -1;

	/* Decide what register space is accessed */
	if(!tmp_ext)
	{
	  status = spi_reg_access(SPI_WRITE_BURST, tmp_addr, data,len);
	}
	else if (tmp_ext == 0x2F)
	{
	  status = spi_ext_reg_access(SPI_WRITE_BURST,tmp_ext,tmp_addr,data,len);
	}
	return (status);
}

rfStatus_t write_tx_buf(uint8 *data, uint8 len){
	uint8 status;
	status = spi_reg_access(0x00,RF_BURST_TXFIFO, data, len);
	return (status);
}

rfStatus_t read_rx_buf(uint8 *data, uint8 len){
	uint8 status;
	status = spi_reg_access(0x00,RF_BURST_RXFIFO, data, len);
	return (status);
}

rfStatus_t get_tx_status(){
	return(spi_cmd_strobe(RF_SNOP));
}

rfStatus_t get_rx_status(){
	return(spi_cmd_strobe(RF_SNOP | SPI_READ_SINGLE));
}

//#############################################################################
// interrupt service routines:
//#############################################################################


//////////////////////////////////////////////////////////////////////////

// ??? ISR for communication intup

//////////////////////////////////////////////////////////////////////////
//#pragma vector=USCI_A0_VECTOR
//__interrupt void USCIA0RX_ISR(void)
//{
//	switch(__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG)) // check UART IFGs
//	  {
//	    case USCI_NONE: break;
//	    case USCI_UART_UCRXIFG:
//	    	strcat(buf, (const char*)&UCA0RXBUF);
//			if(UCA0RXBUF == '\n'){
//				g_callback(buf);
//				strcpy(buf,"");
//			}
//	        break;
//	    case USCI_UART_UCTXIFG: break;
//	    case USCI_UART_UCSTTIFG: break;
//	    case USCI_UART_UCTXCPTIFG: break;
//	  }
//
//}

























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
