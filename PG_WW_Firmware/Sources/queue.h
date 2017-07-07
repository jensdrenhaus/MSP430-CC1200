/*------------------------------------------------------------------------------
| File: queue.h
|
| Simple ring buffer to store com_data elements. Fix size.
|
| Note: --
|-------------------------------------------------------------------------------
| Functions:
|     queue_init   -- init the queue
|     queue_insert -- insert new element (deep copy)
|     queue_first  -- return pinter to first element
|     queue_delete -- delete first element
|     queue_isEmty -- check state of the queue
 -----------------------------------------------------------------------------*/

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "com.h"
#include "types.h"

#define MAX 4

//#############################################################################
// datatypes
//#############################################################################
typedef enum e_status {OK, OVERFLOW, UNDERFLOW}queue_status_t;

//#############################################################################
// functions (prototypes)

/*------------------------------------------------------------------------------
| queue_init   -- init the queue
|
| Parameter:
|     ---
|
| Return:
|     ---
|
 -----------------------------------------------------------------------------*/
extern void queue_init();


/*------------------------------------------------------------------------------
| queue_insert -- insert new element (deep copy)
|
| Parameter:
|     pointer to com_data type
|
| Return:
|     status: OK, OVERFLOW, UNDERFLOW
|
 -----------------------------------------------------------------------------*/
extern queue_status_t queue_insert(com_data_t* element);
extern queue_status_t queue_insert_fix(com_data_fix_t* element);

/*------------------------------------------------------------------------------
| queue_first  -- return pinter to first element
|
| Parameter:
|     ---
|
| Return:
|     pointer to first element
|
 -----------------------------------------------------------------------------*/
extern com_data_t* queue_first();
extern com_data_fix_t* queue_first_fix();


/*------------------------------------------------------------------------------
| queue_delete -- delete first element
|
| Parameter:
|     ---
|
| Return:
|     status: OK, OVERFLOW, UNDERFLOW
|
 -----------------------------------------------------------------------------*/
extern queue_status_t queue_delete();


/*------------------------------------------------------------------------------
| queue_isEmty -- check state of the queue
|
| Parameter:
|     ---
|
| Return:
|     true, if no element is inserted
|
 -----------------------------------------------------------------------------*/
extern uint16 queue_isEmty();


#endif // _QUEUE_H_
