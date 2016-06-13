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
|     -
 -----------------------------------------------------------------------------*/


#include <spi.h>
#include "msp430fr5969.h"



//#############################################################################
// globals


//#############################################################################
// private function prototypes
static void data_transfer(uint8 header, uint8 *data, uint16 len);

//#############################################################################
// module methods implementation:
//#############################################################################


////////////////////////////////////////////////////////////////////////////

//!  PUBLIC spi_init()
//!
////////////////////////////////////////////////////////////////////////////
void spi_init(uint8 prescaler) {

	UCB0CTLW0 |= UCSWRST; // SPI module in reset state

	/* MOSI -> P1.6   MOSI is secoundary module function on port 1
	 * MISO -> P1.7   MISO is secoundary module function on port 1
	 * SCLK -> P2.2   SCLK is secoundary module function on port 1
	 * STE  -> P1.3   STE  is secoundary module function on port 1
	 *
	 *  PxSEL0  | PxSEL1 | mod-func
	 *     0        0       GPIO
	 *     0        1       prim
	 *     1        0       sec
	 *     1        1       ter
	 */

	// Set module function for STE, MISO, MOSI
	P1SEL0 |= BIT3 | BIT6 | BIT7;
	P1SEL1 &= ~(BIT3 | BIT6 | BIT7);
	// Set module function for SCLK
	P2SEL0 |= BIT2;
	P2SEL1 &= ~(BIT2);
	// MOSI, STE, SCLK as OUTPUT
	P1DIR |= BIT6 | BIT3;
	P2DIR |= BIT2;
	// MISO as INPUT
	P1DIR &= BIT7;
	//Pull-UP on MISO
	P1OUT |= BIT7;


	/* Configure
	 * SPI                                    -> UCSYNC    set
	 * 4 pin mode Slave enabled when STE = 0  -> UCMODE_2  set
	 * Master mode                            -> UCMST     set
	 * 8 Bit mode                             -> UC7BIT    clear
	 * Most sicnificant bit first             -> UCMSB     set
	 * Clock polarity: inactive stae is low   -> UCCPL     clear
	 * Clock phase: capture on first edge     -> UCCKPH    set
	 * use STE pin for enable signal          -> UCSTEM    set
	 * Clock source SMCLK                     -> UCSSEL_2  set
	 */
	UCB0CTLW0 &= ~(UC7BIT | UCCKPL);
	UCB0CTLW0 |= UCSYNC | UCMODE_2 | UCMST | UCMSB | UCCKPH | UCSTEM | UCSSEL_2;

	UCB0BR0 = prescaler;

	/* Release for operation */
	  UCB0CTL1 &= ~UCSWRST;

}

////////////////////////////////////////////////////////////////////////////

//!  PUBLIC spi_reg_access()
//!
////////////////////////////////////////////////////////////////////////////
uint8 spi_reg_access(uint8 access, uint8 addr, uint8 *data, uint16 len) {
	uint8 status;
	// send header byte
	UCB0IFG &= ~UCRXIFG;
	UCB0TXBUF= (access | addr);
	//wait for answer / status byte
	while(!(UCB0IFG & UCRXIFG));
	status = UCB0RXBUF;
	data_transfer(access|addr, data, len);
	return status;
}

////////////////////////////////////////////////////////////////////////////

//!  PRIVATE data_transfer()
//!
////////////////////////////////////////////////////////////////////////////
void data_transfer(uint8 header, uint8 *data, uint16 len){

}




