/*------------------------------------------------------------------------------
| File: rf.h
|
| Implements functionality for initialise CC1200 as well as sending and
| recieving using SPI module.
| Defines Makros for CC1200 register names
| Defines Packetlength
|
| Note: - uses PORT 3.4 int. and PORT 3.5 to process feedback from CC1200 in
|         addition to SPI
|       - implements PORT 3.4 pin ISR
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
static uint8 txBuffer[RF_PAYLOADLEN+1] = {0}; // + addr byte
static uint8 rxBuffer[RF_PAYLOADLEN+3] = {0}; // + addr byte + 2 status bytes
typedef enum e_csma_state {BUSY, SUCCESS}csma_state_t;
csma_state_t csma_state;
static uint8 rssi;


uint8 rxfirst_A = 0;
uint8 rxfirst_B = 0;
uint8 rxlast_A = 0;
uint8 rxlast_B = 0;
uint8 num_rxbytes_A = 0;
uint8 num_rxbytes_B = 0;
uint8 fifo_num_rxbytes_A = 0;
uint8 fifo_num_rxbytes_B = 0;

uint8 int_status = 0;


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

	dev_addr = 0x01;


	rf_status_t status;
	csma_state = BUSY;

	spi_init(8);                    // SMCLK(1MHz) / 8 = 125kH // !!! prescaler not used !!!

	//------------------------------------------
    // configure Timer A3 used for CSMA
    //------------------------------------------

    TA3CTL |= (TASSEL__SMCLK | MC__UP | ID__8);          // SMCLK/8 , UP mode
    TA3CTL |= TACLR;                 // clear to acticate new clock settings
    TA3CCR0 = 125;                   // SMCLK/8/125 = 1kHz => 1ms
    TA3CCTL0 &= ~CCIE;               // TACCR3 interrupt disabled

#ifdef PHYNODE
    //------------------------------------------
    // configure P1.1 for CC1200 GPIO 2 (multi purpose)
    //------------------------------------------
    P1DIR &= ~BIT1;                 // Set P3.5 to input direction
    P1REN |= BIT1;                  // Set P3.5 pullup/down Resistor
    P1OUT &= ~BIT1;                 // Select P3.5 pull-down
    P1IE  &= ~BIT1;                 // Disable Interrupt on P3.5
    P1IES &= ~BIT1;                 // rising edge
    P1IFG &= ~BIT1;                 // clear P3.5 interrupt flag
#else
    //------------------------------------------
    // configure P3.5 for CC1200 GPIO 2 (multi purpose)
    //------------------------------------------
    P3DIR &= ~BIT5;                 // Set P3.5 to input direction
    P3REN |= BIT5;                  // Set P3.5 pullup/down Resistor
    P3OUT &= ~BIT5;                 // Select P3.5 pull-down
    P3IE  &= ~BIT5;                 // Disable Interrupt on P3.5
    P3IES &= ~BIT5;                 // rising edge
    P3IFG &= ~BIT5;                 // clear P3.5 interrupt flag
#endif

	// ------------------------------------
	// CC1200 configuration
	// ------------------------------------
	uint8 writeByte = 0;
	uint8 readByte  = 0;
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

#ifdef PHYNODE
	// ------------------------------------
	// register configuration for CC1200 GPIO 0 (PKT OK)
	// ------------------------------------
	// signal connected to P1.2 looks like this:
	//
	//in RX          pkt OK
	//               ___________________________
	// _____________|                           |_________________
	//
	P1DIR &= ~BIT2;                 // Set P3.4 to input direction
	P1REN |= BIT2;                  // Set P3.4 pullup/down Resistor
	P1OUT &= ~BIT2;                 // Select P3.4 pull-down
	P1IE  |= BIT2;                  // Enable Interrupt on P3.4
	P1IES &= ~BIT2;                 // ### rising edge ###
	P1IFG &= ~BIT2;                 // clear P3.4 interrupt flag
#else
	// ------------------------------------
    // register configuration for CC1200 GPIO 0 (PKT OK)
    // ------------------------------------
    // signal connected to P3.4 looks like this:
    //
    //in RX          pkt OK
    //               ___________________________
    // _____________|                           |_________________
    //
    P3DIR &= ~BIT4;                 // Set P3.4 to input direction
    P3REN |= BIT4;                  // Set P3.4 pullup/down Resistor
    P3OUT &= ~BIT4;                 // Select P3.4 pull-down
    P3IE  |= BIT4;                  // Enable Interrupt on P3.4
    P3IES &= ~BIT4;                 // ### rising edge ###
    P3IFG &= ~BIT4;                 // clear P3.4 interrupt flag
#endif



#ifdef AP
    writeByte = RF_AP_ADDR;
#else
    writeByte = RF_STD_ADDR;
#endif
    status = write_reg(RF_DEV_ADDR, &writeByte, 1); // set dev addr


	spi_cmd_strobe(RF_SRX);

}

////////////////////////////////////////////////////////////////////////////

//!  PUBLIC rf_send()
//!
////////////////////////////////////////////////////////////////////////////
void rf_send_fix(com_frame_t* frame) {

	uint8  status = 0;
	uint8  cca_state;
	uint16 tx_on_cca_failed;
	uint8  writeByte;
	uint8  rnd;
	uint16 backoff;


#ifdef PHYNODE
	// Configure CC1200 GPIO 0 Interrupt (PKT_SYNC_RXTX)
	// no ISR no INT enable just set the right edge select
	// for chekcing the ISR flag on P1.2 later for end of transmission.
	// signal connected to P1.2 looks like this:
	//
	//          start sending          sending complete
	//               ___________________________
	// _____________|                           |_________________
	//
	P1IE  &= ~BIT2;                 // Disable Interrupt on P1.2
	P1IES |= BIT2;                  // ### falling edge ###
	P1IFG &= ~BIT2;                 // clear P1.2 interrupt flag
#else
	// Configure CC1200 GPIO 0 Interrupt (PKT_SYNC_RXTX)
    // no ISR no INT enable just set the right edge select
    // for chekcing the ISR flag on P3.4 later for end of transmission.
    // signal connected to P3.4 looks like this:
    //
    //          start sending          sending complete
    //               ___________________________
    // _____________|                           |_________________
    //
    P3IE  &= ~BIT4;                 // Disable Interrupt on P3.4
    P3IES |= BIT4;                  // ### falling edge ###
    P3IFG &= ~BIT4;                 // clear P3.4 interrupt flag
#endif

    // change GPIO0 signal to indicate TX complete
    writeByte = 0x06;             // 06->PKT_SYNC_RXTX
    status = write_reg(RF_IOCFG0, &writeByte, 1);

	// copy data frame into txBuffer
	// add address byte
#ifdef AP
	txBuffer[0] = RF_BC_ADDR; // set destination addr to broadcast
#else
	txBuffer[0] = RF_AP_ADDR; // set destination addr to AccessPoint addr
#endif
	uint16 i;
	for(i=0; i<RF_PAYLOADLEN; i++){
		txBuffer[i+1] = frame->array[(RF_PAYLOADLEN)-1-i];
	}



	// enter CSMA
	while(csma_state == BUSY){

	    // choose random backoff
	    status = spi_cmd_strobe(RF_SRX); // RX state for further randomized number
	    status = read_reg(RF_RNDGEN, &rnd, 1);
	    backoff = (uint16) (rnd & 0b0000000001111111); // use all 7 Bits -> 0 to 127
	    status = read_reg(RF_RNDGEN, &rnd, 1);
        backoff += (uint16) (rnd & 0b0000000001111111); // use all 7 Bits again -> 0 to 255

	    // put CC1200 into SLEEP while backoff
	    status = spi_cmd_strobe(RF_SIDLE);
	    status = spi_cmd_strobe(RF_SPWD);  // until CS goes LOW again

        // backoff (blocking)
        TA3CCR0 = 0;                       // stop timer
        TA3CTL |= TACLR;                   // clear count value
        TA3CCTL0 &= ~CCIFG;                // clear TACCR3 interrupt flag
        TA3CCR0 = 125;                     // start timer SMCLK/8/125 = 1kHz => 1ms
        while(backoff != 0){               // wait for backoff counter
            while(!(TA3CCTL0 & CCIFG));    // wait for interrupt flag -> (ISR disabled)
            TA3CCTL0 &= ~CCIFG;            // clear TACCR3 interrupt flag
            backoff --;
        }

        //wake up
        status = spi_cmd_strobe(RF_SIDLE); // wake up CC1200
        status = spi_cmd_strobe(RF_SNOP);  // debugging
        writeByte = 0xFF;
        status = write_reg(RF_RNDGEN, &writeByte, 1); // reactivate RNDGEN, no retention reg!


        // ensure RX mode and CARRIER_SENSE_VALID
        writeByte = 0x10;                  // 16->CARRIER_SENSE_VALID
        status = write_reg(RF_IOCFG2, &writeByte, 1);
#ifdef PHYNODE
        P1IFG &= ~BIT1;                    // clear P1.1 interrupt flag
#else
        P3IFG &= ~BIT5;                    // clear P3.5 interrupt flag
#endif
        status = spi_cmd_strobe(RF_SRX);   // ensure RX to perform CCA
        status = spi_cmd_strobe(RF_SNOP);  // debugging
#ifdef PHYNODE
        while(!(P1IFG & BIT1));            // wait for CS to be valid -> interrupt an P3.5 (ISR disabled)
        P1IFG &= ~BIT1;                    // clear P1.1 interrupt flag
#else
        while(!(P3IFG & BIT5));            // wait for CS to be valid -> interrupt an P3.5 (ISR disabled)
        P3IFG &= ~BIT5;                    // clear P3.5 interrupt flag
#endif

        // Write packet to TX FIFO
        status = write_tx_fifo(txBuffer, sizeof(txBuffer));

        // try to send
        writeByte = 0x0F;                  // 15->TXONCCA_DONE
        status = write_reg(RF_IOCFG2, &writeByte, 1);
#ifdef PHYNODE
        P1IFG &= ~BIT1;                    // clear P1.1 interrupt flag
        status = spi_cmd_strobe(RF_STX);   // try to send
        while(!(P1IFG & BIT1));            // wait for CCA decision -> interrupt an P1.1 (ISR disabled)
        P1IFG &= ~BIT1;                    // clear P1.1 interrupt flag
#else
        P3IFG &= ~BIT5;                    // clear P3.5 interrupt flag
        status = spi_cmd_strobe(RF_STX);   // try to send
        while(!(P3IFG & BIT5));            // wait for CCA decision -> interrupt an P3.5 (ISR disabled)
        P3IFG &= ~BIT5;                    // clear P3.5 interrupt flag
#endif

        // debugging
        status = read_reg(RF_RSSI1, &rssi, 1);

        //check CCA state
        status = read_reg(RF_MARC_STATUS0, &cca_state, 1);
        tx_on_cca_failed = (cca_state & 0b00000100);
        if(!tx_on_cca_failed){ // NOT TXONCCA_FAILED
            csma_state = SUCCESS;
        }
	}
	csma_state = BUSY;

#ifdef PHYNODE
	// Wait for interruptflag that packet has been sent.
    // Assuming the CC1200-GPIO connected to P1.2 is
    // set to GPIOx_CFG = 0x06 -> CC1200 PKT_SYNC_RXTX interrupt
    while(!(P1IFG & BIT2));
    status = spi_cmd_strobe(RF_SNOP);
    P1IFG &= ~BIT2;                 // clear P1.2 interrupt flag

    // change GPIO0 signal to indicate RX PKT OK
    P1IFG &= ~BIT2;                 // clear P1.2 interrupt flag
    writeByte = 0x13;               // PKT_CRC_OK
    status = write_reg(RF_IOCFG0, &writeByte, 1);

    P1IFG &= ~BIT2;                 // clear P1.2 interrupt flag
    P1IES &= ~BIT2;                 // rising edge
    P1IE  |= BIT2;                  // Enable Interrupt on P1.2
    P1IFG &= ~BIT2;                 // clear P1.2 interrupt flag

#else
	// Wait for interruptflag that packet has been sent.
	// Assuming the CC1200-GPIO connected to P3.4 is
	// set to GPIOx_CFG = 0x06 -> CC1200 PKT_SYNC_RXTX interrupt
	while(!(P3IFG & BIT4));
	status = spi_cmd_strobe(RF_SNOP);
	P3IFG &= ~BIT4;                 // clear P3.4 interrupt flag

	// change GPIO0 signal to indicate RX PKT OK
	P3IFG &= ~BIT4;                 // clear P3.4 interrupt flag
    writeByte = 0x13;               // PKT_CRC_OK
    status = write_reg(RF_IOCFG0, &writeByte, 1);

	P3IFG &= ~BIT4;                 // clear P3.4 interrupt flag
	P3IES &= ~BIT4;                 // rising edge
	P3IE  |= BIT4;                  // Enable Interrupt on P3.4
	P3IFG &= ~BIT4;                 // clear P3.4 interrupt flag
#endif
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


#ifdef PHYNODE
////////////////////////////////////////////////////////////////////////////

//!  PORT1.2 ISR for indicating 'PKT OK' in RX
//!
////////////////////////////////////////////////////////////////////////////

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    P1IFG &= ~BIT2;                             // clear P1.2 interrupt flag

    int_status = read_rx_fifo(rxBuffer, sizeof(rxBuffer));

    g_callback((rxBuffer), SRC_RF);         // pointer of secound byte, skip address byte
}

#else
////////////////////////////////////////////////////////////////////////////

//!  PORT3.4 ISR for indicating 'PKT OK' in RX
//!
////////////////////////////////////////////////////////////////////////////

#pragma vector=PORT3_VECTOR
__interrupt void Port_3(void)
{
    P3IFG &= ~BIT4;                             // clear P3.4 interrupt flag

    int_status = read_rx_fifo(rxBuffer, sizeof(rxBuffer));

    g_callback((rxBuffer), SRC_RF);         // pointer of secound byte, skip address byte
}

#endif

////////////////////////////////////////////////////////////////////////////

//!  Timer A3 ISR for CSMA backoff
//!
////////////////////////////////////////////////////////////////////////////
//#pragma vector=TIMER3_A0_VECTOR
//__interrupt void Timer3_A0(void)
//{
//    wait_for_backoff = 0;
//}



