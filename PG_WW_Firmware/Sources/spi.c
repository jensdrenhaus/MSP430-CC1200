/*------------------------------------------------------------------------------
| File: spi.h
|
| Implemts spi access specific to the CC1200 RF chip.
|
| Note: - uses UART B0 of MSP430 to commuinicate
|-------------------------------------------------------------------------------
| Datatypes:
|     -
|-------------------------------------------------------------------------------
| Functions:
|     spi_init           -- init MSP hardware module
|     spi_reg_access     -- R/W single/burst access to CC1200 Reg Space
|     spi_ext_reg_acces  -- R/W single/burst access to CC1200 Ext Reg Space
|     spi_cmd_strobe     -- send command strobe
 -----------------------------------------------------------------------------*/


#include <spi.h>
#include "msp430fr5969.h"


//#############################################################################
// globals
//#############################################################################


//#############################################################################
// private function prototypes
static void data_transfer(uint8 header, uint8 *data, uint16 len);


////////////////////////////////////////////////////////////////////////////

//!  PUBLIC spi_init()
//!
////////////////////////////////////////////////////////////////////////////
void spi_init(uint8 prescaler) {

#ifdef PHYNODE
    // reset pin of CC1200 is connected to J.7
	PJDIR |= BIT7;
	PJOUT |= BIT7; // set P1.2 HIGH -> cc1200 reset HIGH

	UCA1CTLW0 |= UCSWRST; // SPI module in reset state

	/* MOSI     -> P2.5   MOSI is secoundary module function on port 1
	 * MISO     -> P2.6   MISO is secoundary module function on port 1
	 * SCLK     -> P2.4   SCLK is secoundary module function on port 1
	 * STE(CS)  -> P3.0   not used, set CS line manually before communication
	 *
	 *  PxSEL1  | PxSEL0 | mod-func
	 *     0        0       GPIO
	 *     0        1       prim
	 *     1        0       sec
	 *     1        1       ter
	 */

	// set module function for MISO, MOSI
	P2SEL1 |= ( BIT5 | BIT6);
	P2SEL0 &= ~( BIT5 | BIT6);
	// set module function for SCLK
	P2SEL1 |= BIT4;
	P2SEL0 &= ~(BIT4);
	// set MOSI, STE, SCLK as OUTPUT
	P2DIR |= BIT5 | BIT4;
	P3DIR |= BIT0;
	// set MISO as INPUT
	P2DIR &= ~BIT6;
	// set Pull-UP on MISO
	P2REN |= BIT6;
	P2OUT |= BIT6;

	//STE(CS) set to 1 (CS is aktive low)
	P3OUT |= BIT0;


	/* Configure
	 * SPI                                    -> UCSYNC    set
	 * 3 pin mode CS has to be set manually   -> UCMODE_0  set
	 * Master mode                            -> UCMST     set
	 * 8 Bit mode                             -> UC7BIT    clear
	 * Most sicnificant bit first             -> UCMSB     set
	 * Clock polarity: inactive stae is low   -> UCCPL     clear
	 * Clock phase: capture on first edge     -> UCCKPH    set
	 * Clock source SMCLK 1MHz                -> UCSSEL_2  set
	 */
	UCA1CTLW0 &= ~(UC7BIT | UCCKPL);
	UCA1CTLW0 |= UCSYNC | UCMODE_0 | UCMST | UCMSB | UCCKPH | UCSSEL_2;

	//UCB0BR0 = prescaler;  // BitClk = ModuleClk(SMCLK = 1MHz) / prescaler
	UCA1BR0 = 0;            // no prescaler -> BitClk = Moduleclk(SMCLK = 1MHz)

	// Release for operation
	UCA1CTL1 &= ~UCSWRST;
#else
	// reset pin of CC1200 is connected to MSP P1.2
    P1DIR |= BIT2;
    P1OUT |= BIT2; // set P1.2 HIGH -> cc1200 reset HIGH

    UCB0CTLW0 |= UCSWRST; // SPI module in reset state

    /* MOSI     -> P1.6   MOSI is secoundary module function on port 1
     * MISO     -> P1.7   MISO is secoundary module function on port 1
     * SCLK     -> P2.2   SCLK is secoundary module function on port 1
     * STE(CS)  -> P1.3   not used, set CS line manually before communication
     *
     *  PxSEL1  | PxSEL0 | mod-func
     *     0        0       GPIO
     *     0        1       prim
     *     1        0       sec
     *     1        1       ter
     */

    // set module function for STE, MISO, MOSI
    P1SEL1 |= ( BIT6 | BIT7);
    P1SEL0 &= ~( BIT6 | BIT7);
    // set module function for SCLK
    P2SEL1 |= BIT2;
    P2SEL0 &= ~(BIT2);
    // set MOSI, STE, SCLK as OUTPUT
    P1DIR |= BIT6 | BIT3;
    P2DIR |= BIT2;
    // set MISO as INPUT
    P1DIR &= ~BIT7;
    // set Pull-UP on MISO
    P1REN |= BIT7;
    P1OUT |= BIT7;

    //STE(CS) set to 1 (CS is aktive low)
    P1OUT |= BIT3;


    /* Configure
     * SPI                                    -> UCSYNC    set
     * 3 pin mode CS has to be set manually   -> UCMODE_0  set
     * Master mode                            -> UCMST     set
     * 8 Bit mode                             -> UC7BIT    clear
     * Most sicnificant bit first             -> UCMSB     set
     * Clock polarity: inactive stae is low   -> UCCPL     clear
     * Clock phase: capture on first edge     -> UCCKPH    set
     * Clock source SMCLK 1MHz                -> UCSSEL_2  set
     */
    UCB0CTLW0 &= ~(UC7BIT | UCCKPL);
    UCB0CTLW0 |= UCSYNC | UCMODE_0 | UCMST | UCMSB | UCCKPH | UCSSEL_2;

    //UCB0BR0 = prescaler;  // BitClk = ModuleClk(SMCLK = 1MHz) / prescaler
    UCB0BR0 = 0;            // no prescaler -> BitClk = Moduleclk(SMCLK = 1MHz)

    // Release for operation
    UCB0CTL1 &= ~UCSWRST;
#endif

}

////////////////////////////////////////////////////////////////////////////

//!  PUBLIC spi_reg_access()
//!
////////////////////////////////////////////////////////////////////////////
uint8 spi_reg_access(uint8 access, uint8 addr, uint8 *data, uint16 len) {
#ifdef PHYNODE
	uint8 status;
	// set CS low
	P3OUT &= ~BIT0;
	// wait for MISO to go low
	while(P2IN & BIT6);
	// send header byte
	UCA1IFG &= ~UCRXIFG;
	UCA1TXBUF= (access | addr);
	//wait for answer / status byte
	while(!(UCA1IFG & UCRXIFG));
	status = UCA1RXBUF;
	data_transfer((access | addr), data, len);
	// set CS high
	P3OUT |= BIT0;
	return status;
#else
	uint8 status;
    // set CS low
    P1OUT &= ~BIT3;
    // wait for MISO to go low
    while(P1IN & BIT7);
    // send header byte
    UCB0IFG &= ~UCRXIFG;
    UCB0TXBUF= (access | addr);
    //wait for answer / status byte
    while(!(UCB0IFG & UCRXIFG));
    status = UCB0RXBUF;
    data_transfer((access | addr), data, len);
    // set CS high
    P1OUT |= BIT3;
    return status;
#endif
}

////////////////////////////////////////////////////////////////////////////

//!  PUBLIC spi_ext_reg_access()
//!
////////////////////////////////////////////////////////////////////////////
uint8 spi_ext_reg_access(uint8 access, uint8 ext_addr, uint8 addr,
		uint8* data, uint16 len){
#ifdef PHYNODE
	uint8 status;
	// set CS low
	P3OUT &= ~BIT0;
	// wait for MISO to go low
	while(P2IN & BIT6);
	// send header byte with extended address byte
	UCA1IFG &= ~UCRXIFG;
	UCA1TXBUF= (access | ext_addr);
	//wait for answer / status byte
	while(!(UCA1IFG & UCRXIFG));
	status = UCA1RXBUF;
	// send register address
	UCA1TXBUF= (addr);
	//wait for answer / status byte
	while(!(UCA1IFG & UCRXIFG));
	data_transfer((access | ext_addr), data, len);
	// set CS high
	P3OUT |= BIT0;
	return status;
#else
	uint8 status;
    // set CS low
    P1OUT &= ~BIT3;
    // wait for MISO to go low
    while(P1IN & BIT7);
    // send header byte with extended address byte
    UCB0IFG &= ~UCRXIFG;
    UCB0TXBUF= (access | ext_addr);
    //wait for answer / status byte
    while(!(UCB0IFG & UCRXIFG));
    status = UCB0RXBUF;
    // send register address
    UCB0TXBUF= (addr);
    //wait for answer / status byte
    while(!(UCB0IFG & UCRXIFG));
    data_transfer((access | ext_addr), data, len);
    // set CS high
    P1OUT |= BIT3;
    return status;
#endif
}

////////////////////////////////////////////////////////////////////////////

//!  PUBLIC spi_cmd_strobe()
//!
////////////////////////////////////////////////////////////////////////////
uint8 spi_cmd_strobe(uint8 cmd){
#ifdef PHYNODE
	uint8 status;
	// set CS low
	P3OUT &= ~BIT0;
	// wait for MISO to go low
	while(P2IN & BIT6);
	UCA1IFG &= ~UCRXIFG;
	UCA1TXBUF= (cmd);
	while(!(UCA1IFG & UCRXIFG));
	status = UCA1RXBUF;
	// set CS high
	P3OUT |= BIT0;
	return status;
#else
	uint8 status;
    // set CS low
    P1OUT &= ~BIT3;
    // wait for MISO to go low
    while(P1IN & BIT7);
    UCB0IFG &= ~UCRXIFG;
    UCB0TXBUF= (cmd);
    while(!(UCB0IFG & UCRXIFG));
    status = UCB0RXBUF;
    // set CS high
    P1OUT |= BIT3;
    return status;
#endif
}

////////////////////////////////////////////////////////////////////////////

//!  PRIVATE data_transfer()
//!
////////////////////////////////////////////////////////////////////////////
void data_transfer(uint8 header, uint8 *data, uint16 len){
#ifdef PHYNODE
	uint16 i;

	if((header&0b11000000) == SPI_READ_BURST){
		for(i = 0; i < len; i++){
			UCA1IFG &= ~UCRXIFG; 		 // clear RX flag
			UCA1TXBUF= 0x00;			 // send expected 0 Byte
			while(!(UCA1IFG & UCRXIFG)); // wait for slave to answer
			*data = UCA1RXBUF;			 // read RX buffer, RX flag is reset
			data++;						 //
		}
	}
	else if((header&0b11000000) == SPI_READ_SINGLE){
		UCA1IFG &= ~UCRXIFG; 		     // clear RX flag
		UCA1TXBUF= 0x00;			     // send expected 0 Byte
		while(!(UCA1IFG & UCRXIFG));     // wait for slave to answer
		*data = UCA1RXBUF;			     // read RX buffer, RX flag is reset
	}
	else if((header&0b11000000) == SPI_WRITE_BURST){
		for(i = 0; i < len; i++){
			UCA1IFG &= ~UCRXIFG;         // clear RX flag
			UCA1TXBUF= *data; 		     // write byte
			while(!(UCA1IFG & UCRXIFG)); // wait for slave to answer
			data++;
		}
	}
	else {
		UCA1IFG &= ~UCRXIFG;             // clear RX flag
		UCA1TXBUF= *data; 		         // write byte
		while(!(UCA1IFG & UCRXIFG));     // wait for slave to answer
	}
#else
	uint16 i;

    if((header&0b11000000) == SPI_READ_BURST){
        for(i = 0; i < len; i++){
            UCB0IFG &= ~UCRXIFG;         // clear RX flag
            UCB0TXBUF= 0x00;             // send expected 0 Byte
            while(!(UCB0IFG & UCRXIFG)); // wait for slave to answer
            *data = UCB0RXBUF;           // read RX buffer, RX flag is reset
            data++;                      //
        }
    }
    else if((header&0b11000000) == SPI_READ_SINGLE){
        UCB0IFG &= ~UCRXIFG;             // clear RX flag
        UCB0TXBUF= 0x00;                 // send expected 0 Byte
        while(!(UCB0IFG & UCRXIFG));     // wait for slave to answer
        *data = UCB0RXBUF;               // read RX buffer, RX flag is reset
    }
    else if((header&0b11000000) == SPI_WRITE_BURST){
        for(i = 0; i < len; i++){
            UCB0IFG &= ~UCRXIFG;         // clear RX flag
            UCB0TXBUF= *data;            // write byte
            while(!(UCB0IFG & UCRXIFG)); // wait for slave to answer
            data++;
        }
    }
    else {
        UCB0IFG &= ~UCRXIFG;             // clear RX flag
        UCB0TXBUF= *data;                // write byte
        while(!(UCB0IFG & UCRXIFG));     // wait for slave to answer
    }
#endif

}




