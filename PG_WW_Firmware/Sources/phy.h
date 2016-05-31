/*------------------------------------------------------------------------------
| File: phy.h
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
|     PHY_init       -- init UART hardware, saves functionpointer for callback
|     PHY_send       -- sends string via serial port
|     USCIA0RX_ISR   -- ISR for incoming bytes
 -----------------------------------------------------------------------------*/

#ifndef _PHY_H_
#define _PHY_H_

#define PHY_MAX_BUF		50


//#############################################################################
// datatypes



//#############################################################################
// callback function definition
typedef void (*PHY_CB)(char* string);

//#############################################################################
// function prototypes

/*------------------------------------------------------------------------------
| PHY_init       -- init UART hardware, saves functionpointer for callback
|
| Parameter:
|     function ptr to callback
|
| Return:
|     -
|
 -----------------------------------------------------------------------------*/
extern void phy_init(PHY_CB callback);

/*------------------------------------------------------------------------------
| PHY_send       -- sends string via serial port
|
| Parameter:
|     pointer to string to be transmitted
|
| Return:
|     -
|
 -----------------------------------------------------------------------------*/
extern void phy_send(char *string);



#endif // _PHY_H_
