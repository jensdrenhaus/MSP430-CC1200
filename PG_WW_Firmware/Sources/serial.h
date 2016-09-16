/*------------------------------------------------------------------------------
| File: serial.h
|
| Implemts functionality for wired serial communication.
|
| Note: - uses UART A0 (backchannel UART) of MSP430 to commuinicate
|		- implements UART AO ISR
|-------------------------------------------------------------------------------
| Datatypes:
|     -
|-------------------------------------------------------------------------------
| Functions:
|     serial_init       -- init UART hardware, saves functionpointer for callback
|     serial_send       -- sends string via serial port
|     USCIA0RX_ISR      -- ISR for incoming bytes
 -----------------------------------------------------------------------------*/

#ifndef _SERIAL_H_
#define _SERIAL_H_

#include "com.h"
#include "types.h"

#define SERIAL_MAX_BUF		50


//#############################################################################
// datatypes



//#############################################################################
// callback function definition
typedef void (*SERIAL_CB)(char* string, com_src_t src);

//#############################################################################
// function prototypes

/*------------------------------------------------------------------------------
| serial_init       -- init UART hardware, saves functionpointer for callback
|
| Parameter:
|     function ptr to callback
|
| Return:
|     -
|
 -----------------------------------------------------------------------------*/
extern void serial_init(SERIAL_CB callback);

/*------------------------------------------------------------------------------
| serial_send       -- sends string via serial port
|
| Parameter:
|     pointer to string to be transmitted
|
| Return:
|     -
|
 -----------------------------------------------------------------------------*/
extern void serial_send(char *string);

extern void serial_debug_byte(uint8 byte);
extern void serial_debug_word(uint16 byte);



#endif // _SERIAL_H_
