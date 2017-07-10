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
|     serial_debug_byte -- can by used for sending debug info to terminal
|     serial_debug_word -- can by used for sending debug info to terminal
 -----------------------------------------------------------------------------*/

#ifndef _SERIAL_H_
#define _SERIAL_H_

#include "com.h"
#include "types.h"

#define SERIAL_MAX_BUF		50
#define SERIAL_FIX_BUF          19


//#############################################################################
// datatypes



//#############################################################################
// callback function definition
//typedef void (*SERIAL_CB)(char* string, com_src_t src);
typedef void (*SERIAL_CB)(com_frame_t* frame, com_src_t src);

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
extern void serial_send_fix(com_frame_t *frame);

/*------------------------------------------------------------------------------
| serial_debug_byte -- can by used for sending debug info to terminal
|
| Parameter:
|     one byte of data
|
| Return:
|     -
|
 -----------------------------------------------------------------------------*/
extern void serial_debug_byte(uint8 byte);

/*------------------------------------------------------------------------------
| serial_debug_word -- can by used for sending debug info to terminal
|
| Parameter:
|     two byte of data
|
| Return:
|     -
|
 -----------------------------------------------------------------------------*/
extern void serial_debug_word(uint16 byte);



#endif // _SERIAL_H_
