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

#ifndef _SPI_H_
#define _SPI_H_

#include "types.h"

#define SPI_WRITE_SINGLE 0x00
#define SPI_WRITE_BURST  0x40
#define SPI_READ_SINGLE  0x80
#define SPI_READ_BURST   0xC0

//#############################################################################
// datatypes

typedef uint8 spi_status;



// function prototypes

/*------------------------------------------------------------------------------
| spi_init       -- ...
|
| Parameter:
|     -
|
| Return:
|     -
|
 -----------------------------------------------------------------------------*/
extern void spi_init(uint8 prescaler);
extern uint8 spi_reg_access(uint8 access_type, uint8 addr, uint8 *data, uint16 len);
extern uint8 spi_cmd_strobe(uint8 cmd);


#endif // _SERIAL_H_
