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

#ifndef _SPI_H_
#define _SPI_H_

#include "types.h"

#define SPI_WRITE_SINGLE 0x00
#define SPI_WRITE_BURST  0x40
#define SPI_READ_SINGLE  0x80
#define SPI_READ_BURST   0xC0

//#############################################################################
// datatypes
//#############################################################################


//#############################################################################
// function prototypes
//#############################################################################

/*------------------------------------------------------------------------------
| spi_init           -- init MSP hardware module
|
| Parameter:
|     prescaler for SPI datarate-> BitClk = ModuleClk / prescaler
|
| Return:
|     status byte
|
 -----------------------------------------------------------------------------*/
extern void spi_init(uint8 prescaler);

/*------------------------------------------------------------------------------
| spi_reg_access     -- R/W single/burst access to CC1200 Reg Space
|
| Parameter:
|     access_type -> SPI_WRITE_SINGLE, SPI_WRITE_BURST, SPI_READ_SINGLE,
|					 SPI_READ_BURST
|     addr        -> regsiter address
|     data        -> pointer to byte/bytes to by transmitted
|     len         -> number of bytes to be transmitted
| Return:
|     status byte
|
 -----------------------------------------------------------------------------*/
extern uint8 spi_reg_access(uint8 access_type, uint8 addr, uint8 *data,
							uint16 len);

/*------------------------------------------------------------------------------
| spi_ext_reg_acces  -- R/W single/burst access to CC1200 Ext Reg Space
|
| Parameter:
|     access_type -> SPI_WRITE_SINGLE, SPI_WRITE_BURST, SPI_READ_SINGLE,
|					 SPI_READ_BURST
|     ext_addr    -> extended register address
|     addr        -> regsiter address
|     data        -> pointer to byte/bytes to by transmitted
|     len         -> number of bytes to be transmitted
| Return:
|     status byte
|
 -----------------------------------------------------------------------------*/
extern uint8 spi_ext_reg_access(uint8 access_type, uint8 ext_addr, uint8 addr,
								uint8* data, uint16 len);

/*------------------------------------------------------------------------------
| spi_cmd_strobe     -- send command strobe
|
| Parameter:
|     command strobe (see definitions in header file)
|
| Return:
|     status byte
|
 -----------------------------------------------------------------------------*/
extern uint8 spi_cmd_strobe(uint8 cmd);


#endif // _SERIAL_H_
