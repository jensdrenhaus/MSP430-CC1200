/*------------------------------------------------------------------------------
| File: name.c
|
| Simple ring buffer to store com_data elements. Fix size.
|
| Note: --
 -----------------------------------------------------------------------------*/

#include "queue.h"



//#############################################################################
// globals


/* array to store elements */
static com_data_t array[MAX];
static com_frame_t array_fix[MAX];

/* array pointer */
static uint16 front;
static uint16 rear;
static uint16 count;


//#############################################################################
// module methods implementation:
//#############################################################################


////////////////////////////////////////////////////////////////////////////

//!  PUBLIC queue_init()
//!
////////////////////////////////////////////////////////////////////////////
void queue_init(){
    front  = 0;
    rear   = 0;
    count  = 0;
}


////////////////////////////////////////////////////////////////////////////

//!  PUBLIC queue_insert()
//!
////////////////////////////////////////////////////////////////////////////
queue_status_t queue_insert_fix(com_frame_t* element){
    if(count == MAX)
        return OVERFLOW;
    else {
        array_fix[rear].frame.command    = element->frame.command;
        array_fix[rear].frame.box_id     = element->frame.box_id;
        array_fix[rear].frame.product_id = element->frame.product_id;
        array_fix[rear].frame.arg        = element->frame.arg;

        rear++;
        if(rear == MAX)
            rear = 0;
        count++;

        return OK;
    }
}

queue_status_t queue_insert(com_data_t* element){
    if(count == MAX)
        return OVERFLOW;
    else {
        array[rear].command    = element->command;
        array[rear].box_id     = element->box_id;
        array[rear].product_id = element->product_id;
        array[rear].arg        = element->arg;

        rear++;
        if(rear == MAX)
            rear = 0;
        count++;

        return OK;
    }
}


////////////////////////////////////////////////////////////////////////////

//!  PUBLIC queue_first()
//!
////////////////////////////////////////////////////////////////////////////
com_frame_t* queue_first_fix(){
    if(count == 0)
        return NULL;
    else
        return &array_fix[front];
}

com_data_t* queue_first(){
    if(count == 0)
        return NULL;
    else
        return &array[front];
}


////////////////////////////////////////////////////////////////////////////

//!  PUBLIC queue_delete()
//!
////////////////////////////////////////////////////////////////////////////
queue_status_t queue_delete(){
    if(count == 0)
        return UNDERFLOW;
    else{
        front++;
        if(front == MAX)
            front = 0;
        count--;

        return OK;
    }
}


////////////////////////////////////////////////////////////////////////////

//!  PUBLIC queue_isEmty()
//!
////////////////////////////////////////////////////////////////////////////
uint16 queue_isEmty(){
    if(count == 0)
        return 1;
    else
        return 0;
}






