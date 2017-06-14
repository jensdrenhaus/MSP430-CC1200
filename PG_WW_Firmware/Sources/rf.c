/*------------------------------------------------------------------------------
| File: rf.h
|
| Implements functionality for initialise CC1200 as well as sending and
| recieving using SPI module.
| Defines Makros for CC1200 register names
| Defines Packetlength
|
| Note: - uses PORT 3.5 int. to process feedback from CC1200 in addition to SPI
|       - implements PORT 3.5 pin ISR
|------------------------------------------------------------------------------
| Datatypes:
|     rf_setting_t    -- used to store register address - content touple
|     rf_status_t     -- used to store feedback from CC1200
|------------------------------------------------------------------------------
| Functions:
|     rf_init  -- init subjacent modules, saves functionpointer for callback
|     rf_send  -- sends data to CC1200 using SPI module
 ----------------------------------------------------------------------------*/

#include "rf.h"
#include "spi.h"
#include "msp430fr5969.h"
#include "rf_reg_config.h"
#include "types.h"



//#############################################################################
// globals

static RF_CB  g_callback;
static uint32 packetCounter = 0;
// Initialize packet buffers of size RF_PKTLEN + additional information
static uint8 txBuffer[RF_PKTLEN+1] = {0}; // + legth byte
static uint8 rxBuffer[RF_PKTLEN+3] = {0}; // + legth byte + 2 status bytes
static char buf [RF_PKTLEN+1];            // legth byte
static uint16 wait_for_backoff;
typedef enum e_csma_state {BUSY, SUCCESS}csma_state_t;
csma_state_t csma_state;
static uint8 rssi;

//#############################################################################
// private function prototypes
static rf_status_t read_reg(uint16 addr, uint8 *data, uint8 len);
static rf_status_t write_reg(uint16 addr, uint8 *data, uint8 len);
static rf_status_t write_tx_fifo(uint8 *data, uint8 len);
static rf_status_t read_rx_fifo(uint8 *data, uint8 len);
static rf_status_t get_status();


////////////////////////////////////////////////////////////////////////////

//!  PUBLIC rf_init()
//!
////////////////////////////////////////////////////////////////////////////
void rf_init(RF_CB callback) {

	// ------------------------------------
	// save funtion ptr to callback func.
	// ------------------------------------
	g_callback = callback;

	rf_status_t status;
	wait_for_backoff = 1;
	csma_state = BUSY;

	spi_init(8);                    // SMCLK(1MHz) / 8 = 125kH

	//------------------------------------------
    // configure Timer A3 used for CSMA
    //------------------------------------------

    TA3CTL |= (TASSEL__SMCLK | MC__UP | ID__8);          // SMCLK/8 , UP mode
    TA3CTL |= TACLR;                 // clear to acticate new clock settings
    TA3CCR0 = 62500;               // SMCLK/8/62500 = 2Hz => 0,5s
    TA3CCTL0 &= ~CCIE;               // TACCR3 interrupt disabled

    //------------------------------------------
    // configure P3.6
    //------------------------------------------
    P3DIR &= ~BIT6;                 // Set P3.6 to input direction
    P3REN |= BIT6;                  // Set P3.6 pullup/down Resistor
    P3OUT &= ~BIT6;                 // Select P3.6 pull-down
    P3IE  &= ~BIT6;                 // Disable Interrupt on P3.6
    P3IES |= BIT6;                  // falling edge
    P3IFG &= ~BIT6;                // clear P3.6 interrupt flag

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
	    i < (sizeof(preferredSettings)/sizeof(rf_setting_t)); i++) {
	    writeByte = preferredSettings[i].data;
	    status = write_reg(preferredSettings[i].addr, &writeByte, 1);
	}

	// Read registers
	for(i = 0;
		i < (sizeof(preferredSettings)/sizeof(rf_setting_t)); i++) {
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
void rf_send(char* data) {

	uint8  status = 0;
	uint8  cca_state;
	uint16 tx_on_cca_failed;
	uint8  writeByte;


	// Configure GPIO Interrupt
	// no ISR no INT enable just set the right edge select
	// for chekcing the ISR flag on P3.5 later for end of transmission.
	// Line connected to P3.5 looks like this:
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

	// copy data string into txBuffer
	// add length byte

	txBuffer[0] = RF_PKTLEN;
	uint8 n = 1;
	while(1) {
		txBuffer[n] = data[n-1];
		n++;
		if (data[n-1] == '\0') break;
	}

	// Write packet to TX FIFO
	status = write_tx_fifo(txBuffer, sizeof(txBuffer));

	// enter CSMA
	while(csma_state == BUSY){

	    // backoff
//	    TA3CCR0 = 0;                    // stop timer
//        TA3CCTL0 |= CCIE;               // TACCR3 interrupt enabled
//        TA3CCR0 = 62500;                // start timer: SMCLK/8/62500 = 2Hz => 0,5s
//        while(wait_for_backoff);
//        TA3CCTL0 &= ~CCIE;              // TACCR3 interrupt disabled

        // backoff
        TA3CCR0 = 0;                    // stop timer
        TA3CCTL0 &= ~CCIFG;             // clear TACCR3 interrupt flag
        TA3CCR0 = 62500;                // start timer: SMCLK/8/62500 = 2Hz => 0,5s
        while(!(TA3CCTL0 & CCIFG));     // wait for interrupt flag -> (ISR disabled)
        TA3CCTL0 &= ~CCIFG;             // clear TACCR3 interrupt flag



        // ensure RX mode and CARRIER_SENSE_VALID
        writeByte = 0x10;                   // 16->CARRIER_SENSE_VALID
        status = write_reg(RF_IOCFG2, &writeByte, 1);
        P3IFG &= ~BIT6;                    // clear P3.6 interrupt flag
        status = spi_cmd_strobe(RF_SRX);   // ensure RX to perform CCA
        while(!(P3IFG & BIT6));            // wait for CS to be valid -> interrupt an P3.6 (ISR disabled)
        P3IFG &= ~BIT6;                    // clear P3.6 interrupt flag

        // try to send
        writeByte = 0x0F;                   // 15->TXONCCA_DONE
        status = write_reg(RF_IOCFG2, &writeByte, 1);
        P3IFG &= ~BIT6;                    // clear P3.6 interrupt flag
        status = spi_cmd_strobe(RF_STX);   // try to send
        while(!(P3IFG & BIT6));            // wait for CCA decision -> interrupt an P3.6 (ISR disabled)
        P3IFG &= ~BIT6;                    // clear P3.6 interrupt flag

        // for testing
        status = read_reg(RF_RSSI1, &rssi, 1);

        //check CCA state
        status = read_reg(RF_MARC_STATUS0, &cca_state, 1);
        tx_on_cca_failed = (cca_state & 0b00000100);
        if(!tx_on_cca_failed){ // NOT TXONCCA_FAILED
            csma_state = SUCCESS;
        }
	}
	csma_state = BUSY;

	// Wait for interruptflag that packet has been sent.
	// Assuming the CC1200-GPIO connected to P3.5 is
	// set to GPIOx_CFG = 0x06 -> CC1200 PKT_SYNC_RXTX interrupt
	while(!(P3IFG & BIT5));
	status = spi_cmd_strobe(RF_SNOP);

	//flush TX FIFO
	status = spi_cmd_strobe(RF_SIDLE);
	status = spi_cmd_strobe(RF_SFTX);

	status = spi_cmd_strobe(RF_SRX);
	P3IFG &= ~BIT5;                 // clear P3.5 interrupt flag
	P3IE  |= BIT5;                  // Enable Interrupt on P3.5
	P3IFG &= ~BIT5;                 // clear P3.5 interrupt flag
}

////////////////////////////////////////////////////////////////////////////

//!  PRIVATE read_reg()
//!
////////////////////////////////////////////////////////////////////////////
rf_status_t read_reg(uint16 addr, uint8 *data, uint8 len){

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


////////////////////////////////////////////////////////////////////////////

//!  PRIVATE write_reg()
//!
////////////////////////////////////////////////////////////////////////////
rf_status_t write_reg(uint16 addr, uint8 *data, uint8 len){

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

////////////////////////////////////////////////////////////////////////////

//!  PRIVATE write_tx_fifo()
//!
////////////////////////////////////////////////////////////////////////////
static rf_status_t write_tx_fifo(uint8 *data, uint8 len){
	uint8 status;
	status = spi_reg_access(0x00,RF_BURST_TXFIFO, data, len);
	return (status);
}


////////////////////////////////////////////////////////////////////////////

//!  PRIVATE read_rx_fifo()
//!
////////////////////////////////////////////////////////////////////////////
static rf_status_t read_rx_fifo(uint8 *data, uint8 len){
	uint8 status;
	status = spi_reg_access(0x00,RF_BURST_RXFIFO, data, len);
	return (status);
}

////////////////////////////////////////////////////////////////////////////

//!  PRIVATE get_tx_status()
//!
////////////////////////////////////////////////////////////////////////////
static rf_status_t get_status(){
	return(spi_cmd_strobe(RF_SNOP));
}


//#############################################################################
// interrupt service routines:
//#############################################################################


////////////////////////////////////////////////////////////////////////////

//!  PORT3.5 ISR for indicating 'TX complete' as well as 'new data arrived'
//!
////////////////////////////////////////////////////////////////////////////
#pragma vector=PORT3_VECTOR
__interrupt void Port_3(void)
{
    P3IE &= ~BIT5;                              // Disable Interrupt on P3.5

    uint8 status;
    status = read_rx_fifo(rxBuffer, sizeof(rxBuffer));

    if (rxBuffer[37] & 0b10000000) {            // chech CRC
    	uint8 n = 0;
    	    do{
    	    	buf[n] = rxBuffer[n+1];
    	    	n++;
    	    } while (rxBuffer[n+1] != '\n');
    	    g_callback(buf, SRC_RF);
    }
    // flush RX-FIFO
    status = spi_cmd_strobe(RF_SIDLE);
    status = spi_cmd_strobe(RF_SFRX);

    status = spi_cmd_strobe(RF_SRX);

    P3IE  |= BIT5;                              // Enable Interrupt on P3.5
    P3IFG &= ~BIT5;                             // clear P3.5 interrupt flag

}

////////////////////////////////////////////////////////////////////////////

//!  Timer A3 ISR for CSMA backoff
//!
////////////////////////////////////////////////////////////////////////////
//#pragma vector=TIMER3_A0_VECTOR
//__interrupt void Timer3_A0(void)
//{
//    wait_for_backoff = 0;
//}



