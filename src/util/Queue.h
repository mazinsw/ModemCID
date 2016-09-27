#ifndef _STACK_H_
#define _STACK_H_
/*
    Queue - Queue data type
    ModemCID - Fax Modem CallerID Detection
    Copyright (C) 2010-2014 MZSW Creative Software

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    MZSW Creative Software
    contato@mzsw.com.br
*/

/** @file Queue.h
 *  Main include header for the Queue data type
 */
 
// queue data type
typedef struct _queue Queue;

#ifdef __cplusplus
extern "C" {
#endif

//create a new queue
Queue * Queue_create();
//push data to queue
int Queue_push(Queue * queue, void * data);
//pop data from queue return NULL if empty
void * Queue_pop(Queue * queue);
//get top queue data return NULL if empty
void * Queue_front(Queue * queue);
//check if queue is empty return 1 to true, 0 to false
int Queue_empty(Queue * queue);
//clear queue, no free data
void Queue_clear(Queue * queue);
//count queue data
int Queue_count(Queue * queue);
//clear and free queue
void Queue_free(Queue * queue);

#ifdef __cplusplus
}
#endif

#endif
