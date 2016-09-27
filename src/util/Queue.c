#include <stdlib.h>
#include <string.h>
#include "Queue.h"

typedef struct _queue_data {
    void * data;
    struct _queue_data * next;
} queue_data;

struct _queue {
    int count;
    queue_data * first;
    queue_data * last;
};

Queue * Queue_create(){
    Queue * queue = (Queue*)malloc(sizeof(Queue));
    if(queue == NULL)
        return NULL;
    memset(queue, 0, sizeof(Queue));
    return queue;
}

int Queue_push(Queue * queue, void * data){
    queue_data * item = (queue_data*)malloc(sizeof(queue_data));
    if(item == NULL)
        return 0;
    item->data = data;
    item->next = NULL;
	if(queue->last != NULL)
		queue->last->next = item;
	else
		queue->first = item;
    queue->last = item;
    queue->count++;
    return 1;
}

void * Queue_pop(Queue * queue){
    void * data;
    queue_data * first;
    if(Queue_empty(queue))
        return NULL;
    first = queue->first;
    queue->first = first->next;
	if(queue->first == NULL)
		queue->last = NULL;
    data = first->data;
    free(first);
    queue->count--;
    return data;
}

void * Queue_front(Queue * queue){
    if(Queue_empty(queue))
        return NULL;
    return queue->first->data;
}

int Queue_empty(Queue * queue){
    return (queue->first == NULL);
}

void Queue_clear(Queue * queue){
    while(!Queue_empty(queue))
        Queue_pop(queue);
}

int Queue_count(Queue * queue){
    return queue->count;
}

void Queue_free(Queue * queue){
    Queue_clear(queue);
    free(queue);
}
