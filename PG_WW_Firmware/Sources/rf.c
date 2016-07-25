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
#include "rf_reg_config.h"


#define PKTLEN              8 // 1 < PKTLEN < 126
#define ISR_ACTION_REQUIRED 1
#define ISR_IDLE            0

//#############################################################################
// globals

static RF_CB    g_callback;
static uint8  packetSemaphore;
static uint32 packetCounter = 0;

//#############################################################################
// private function prototypes
static rfStatus_t read_reg(uint16 addr, uint8 *data, uint8 len);
static rfStatus_t write_reg(uint16 addr, uint8 *data, uint8 len);
static rfStatus_t write_tx_fifo(uint8 *data, uint8 len);
static rfStatus_t read_rx_fifo(uint8 *data, uint8 len);
static rfStatus_t get_tx_status();
static rfStatus_t get_rx_status();
static void create_packet(uint8 txBuffer[]);


//#############################################################################
// module methods implementation:
//#############################################################################


////////////////////////////////////////////////////////////////////////////

//!  PUBLIC rf_init()
//!
////////////////////////////////////////////////////////////////////////////
void rf_init() {

	// ------------------------------------
	// save funtion ptr to callback func.
	// ------------------------------------
	//g_callback = callback;

	rfStatus_t status;

	spi_init(8);                    // SMCLK(1MHz) / 8 = 125kH

	// ------------------------------------
	// register configuration
	// ------------------------------------
	uint8 writeByte;
	uint8 readByte = 0;
	uint16 i;

	// Reset radio
	spi_cmd_strobe(RF_SRES);

//	// Read registers
//	for(i = 0;
//		i < (sizeof(preferredSettings)/sizeof(rfSetting_t)); i++) {
//		status = read_reg(preferredSettings[i].addr, &readByte, 1);
//	}

	// Write registers to radio
	for(i = 0;
	    i < (sizeof(preferredSettings)/sizeof(rfSetting_t)); i++) {
	    writeByte = preferredSettings[i].data;
	    status = write_reg(preferredSettings[i].addr, &writeByte, 1);
	}

	// Read registers
	for(i = 0;
		i < (sizeof(preferredSettings)/sizeof(rfSetting_t)); i++) {
		status = read_reg(preferredSettings[i].addr, &readByte, 1);
	}





}

////////////////////////////////////////////////////////////////////////////

//!  PUBLIC rf_send()
//!
////////////////////////////////////////////////////////////////////////////
void rf_send() {

	// Initialize packet buffer of size PKTLEN + 1
	uint8 txBuffer[PKTLEN+1] = {0};

	rfStatus_t status;

//	// Connect ISR function to GPIO2
//	ioPinIntRegister(IO_PIN_PORT_1, GPIO2, &radioTxISR);
//
//	// Interrupt on falling edge
//	ioPinIntTypeSet(IO_PIN_PORT_1, GPIO2, IO_PIN_FALLING_EDGE);
//
//	// Clear ISR flag
//	ioPinIntClear(IO_PIN_PORT_1, GPIO2);
//
//	// Enable interrupt
//	ioPinIntEnable(IO_PIN_PORT_1, GPIO2);

	// Configure GPIO Interrupt
	P3DIR &= ~BIT5;                 // Set P3.5 to input direction
	P3REN |= BIT5;                  // Set P3.5 pullup/down Resistor
	P3OUT &= ~BIT5;                 // Select P3.5 pull-down
	P3IE  |= BIT5;                  // Enable Interrupt on P3.5
	P3IES &= ~BIT5;                  // rising edge
	P3IFG &= ~BIT5;                 // clear P3.5 interrupt flag




		// Update packet counter
		packetCounter++;

		// Create a random packet with PKTLEN + 2 byte packet
		// counter + n x random bytes
		create_packet(txBuffer);

		// Write packet to TX FIFO
		status = write_tx_fifo(txBuffer, sizeof(txBuffer));

		// Strobe TX to send packet
		spi_cmd_strobe(RF_STX);

		// Wait for interrupt that packet has been sent.
		// (Assumes the GPIO connected to the radioRxTxISR function is
		// set to GPIOx_CFG = 0x06)
		while(packetSemaphore != ISR_ACTION_REQUIRED);

		// Clear semaphore flag
		packetSemaphore = ISR_IDLE;
}


rfStatus_t read_reg(uint16 addr, uint8 *data, uint8 len){

	uint8 tmp_ext  = (uint8)(addr>>8);
	uint8 tmp_addr = (uint8)(addr & 0x00FF);
	uint8 status;

	/* Checking if this is a FIFO access -> returns chip not ready  */
	if((RF_SINGLE_TXFIFO<=tmp_addr)&&(tmp_ext==0)) return 0;

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

rfStatus_t write_reg(uint16 addr, uint8 *data, uint8 len){

	uint8 tmp_ext  = (uint8)(addr>>8);
	uint8 tmp_addr = (uint8)(addr & 0x00FF);
	uint8 status;

	/* Checking if this is a FIFO access -> returns chip not ready  */
	if((RF_SINGLE_TXFIFO<=tmp_addr)&&(tmp_ext==0)) return 0;

	/* Decide what register space is accessed */
	if(!tmp_ext)
	{
	  status = spi_reg_access(SPI_WRITE_BURST, tmp_addr, data, len);
	}
	else if (tmp_ext == 0x2F)
	{
	  status = spi_ext_reg_access(SPI_WRITE_BURST,tmp_ext,tmp_addr,data,len);
	}
	return (status);
}

static rfStatus_t write_tx_fifo(uint8 *data, uint8 len){
	uint8 status;
	status = spi_reg_access(0x00,RF_BURST_TXFIFO, data, len);
	return (status);
}

static rfStatus_t read_rx_fifo(uint8 *data, uint8 len){
	uint8 status;
	status = spi_reg_access(0x00,RF_BURST_RXFIFO, data, len);
	return (status);
}

static rfStatus_t get_tx_status(){
	return(spi_cmd_strobe(RF_SNOP));
}

static rfStatus_t get_rx_status(){
	return(spi_cmd_strobe(RF_SNOP | SPI_READ_SINGLE));
}

static void create_packet(uint8 txBuffer[]) {

    txBuffer[0] = PKTLEN;                           // Length byte
    txBuffer[1] = (uint8) (packetCounter >> 8);     // MSB of packetCounter
    txBuffer[2] = (uint8)  packetCounter;           // LSB of packetCounter

    // Fill rest of buffer with random bytes
    uint8 i;
    for(i = 3; i < (PKTLEN + 1); i++) {
        txBuffer[i] = 0x11;
    }
}

//#############################################################################
// interrupt service routines:
//#############################################################################


//////////////////////////////////////////////////////////////////////////

// ??? ISR for RXTX

//////////////////////////////////////////////////////////////////////////
#pragma vector=PORT3_VECTOR
__interrupt void Port_3(void)
{
    P3IE &= ~BIT5;                  // Disable Interrupt on P3.5
    // Set packet semaphore
	packetSemaphore = ISR_ACTION_REQUIRED;

    P3IFG &= ~BIT5;                 // clear P3.5 interrupt flag

}

























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
