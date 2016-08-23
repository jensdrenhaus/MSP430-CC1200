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



#define ISR_ACTION_REQUIRED 1
#define ISR_IDLE            0

//#############################################################################
// globals

static RF_CB  g_callback;
static uint32 packetCounter = 0;
// Initialize packet buffers of size RF_PKTLEN + 1
uint8 txBuffer[RF_PKTLEN+1] = {0};
uint8 rxBuffer[RF_PKTLEN+1] = {0};

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
void rf_init(RF_CB callback) {

	// ------------------------------------
	// save funtion ptr to callback func.
	// ------------------------------------
	g_callback = callback;

	rfStatus_t status;

	spi_init(8);                    // SMCLK(1MHz) / 8 = 125kH

	// ------------------------------------
	// CC1200 configuration
	// ------------------------------------
	uint8 writeByte;
	uint8 readByte = 0;
	uint16 i;

	// Reset radio
	spi_cmd_strobe(RF_SRES);


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

	// ------------------------------------
	// register configuration for CC1200 GPIO
	// ------------------------------------
	// line connected to P3.5 looks like this:
	//
	//          start sending          sending complete
	//               ___________________________
	// _____________|                           |_________________
	//
	P3DIR &= ~BIT5;                 // Set P3.5 to input direction
	P3REN |= BIT5;                  // Set P3.5 pullup/down Resistor
	P3OUT &= ~BIT5;                 // Select P3.5 pull-down
	P3IE  |= BIT5;                  // Enable Interrupt on P3.5
	P3IES |= BIT5;                  // falling edge
	P3IFG &= ~BIT5;                 // clear P3.5 interrupt flag

	spi_cmd_strobe(RF_SRX);


}

////////////////////////////////////////////////////////////////////////////

//!  PUBLIC rf_send()
//!
////////////////////////////////////////////////////////////////////////////
void rf_send() {

	uint8 status = 0;


	// Configure GPIO Interrupt
	// no ISR no INT enable just set the right edge select
	// for chekcing the ISR flag on P3.5 later for end of transmission
	// line connected to P3.5 looks like this:
	//
	//          start sending          sending complete
	//               ___________________________
	// _____________|                           |_________________
	//
	P3DIR &= ~BIT5;                 // Set P3.5 to input direction
	P3REN |= BIT5;                  // Set P3.5 pullup/down Resistor
	P3OUT &= ~BIT5;                 // Select P3.5 pull-down
	P3IE  &= ~BIT5;                 // Disable Interrupt on P3.5
	P3IES |= BIT5;                  // falling edge
	P3IFG &= ~BIT5;                 // clear P3.5 interrupt flag


	// Update packet counter
	packetCounter++;

	// Create a random packet with RF_PKTLEN + 2 byte packet
	// counter + n x random bytes
	create_packet(txBuffer);

	// Write packet to TX FIFO
	status = write_tx_fifo(txBuffer, sizeof(txBuffer));


	// Strobe TX to send packet
	spi_cmd_strobe(RF_STX);

	// Wait for interruptflag that packet has been sent.
	// (Assumes the CC1200-GPIO connected to P3.5 is
	// set to GPIOx_CFG = 0x06)
	while(!(P3IFG & BIT5));
	status = spi_cmd_strobe(RF_SNOP);


	spi_cmd_strobe(RF_SRX);
	P3IFG &= ~BIT5;                 // clear P3.5 interrupt flag
	P3IE  |= BIT5;                  // Enable Interrupt on P3.5
	P3IFG &= ~BIT5;                 // clear P3.5 interrupt flag
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

    txBuffer[0] = RF_PKTLEN;                           // Length byte
    txBuffer[1] = (uint8) (packetCounter >> 8);     // MSB of packetCounter
    txBuffer[2] = (uint8)  packetCounter;           // LSB of packetCounter

    // Fill rest of buffer with random bytes
    uint8 i;
    for(i = 3; i < (RF_PKTLEN + 1); i++) {
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

    read_rx_fifo(rxBuffer, sizeof(rxBuffer));
    g_callback(rxBuffer);
    spi_cmd_strobe(RF_SRX);

    P3IE  |= BIT5;                  // Enable Interrupt on P3.5
    P3IFG &= ~BIT5;                 // clear P3.5 interrupt flag

}



