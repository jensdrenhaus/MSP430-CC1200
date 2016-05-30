/*------------------------------------------------------------------------------
| File: phy.h
|
| Say something about WHAT the model does.
|
| Note: uses UART A0 (backchannel UART) of MSP430 to commuinicate
		implements UART AO ISR
|-------------------------------------------------------------------------------
| Datatypes:
|     PHY_recbuf__t    -- used to store data and informatin of something
|     ...
|-------------------------------------------------------------------------------
| Functions:
|     PHY_init  -- init UART hardware, sets recieving behaviour
|     PHY_send  -- sends multiple bytes
|     ...
 -----------------------------------------------------------------------------*/

#ifndef _PHY_H_
#define _PHY_H_

#define PHY_MAX_BUF		50


//#############################################################################
// datatypes

/* structure to hold recieved chars */
//typedef struct s_recbuf {
//		char buf [PHY_MAX_BUF];
//}SER_recbuf_t;



//#############################################################################
// callback function definition
typedef void (*PHY_CB)(char* string);

//#############################################################################
// function prototypes

/*------------------------------------------------------------------------------
| name_somefunction  -- short explaination
|
| Parameter:
|     ---
|
| Return:
|     ---
|
 -----------------------------------------------------------------------------*/
extern void phy_init(PHY_CB callback);

extern void phy_send(char *string);



#endif // _PHY_H_
