#ifndef _queue_h
#define _queue_h
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "car.h"

typedef Car* queueType;

typedef struct LLNode
{
    queueType qt;               //queueType contained in this node
    struct LLNode *pNext;       //pointer to the next node in the linked list
}  LLNode;

typedef struct Queue
{
    LLNode *qFront;             //pointer to the first element of the queue
    LLNode *qRear;              //pointer to the last element of the queue
    int numElements;            //number of elements in this Queue
} Queue;

Queue *createQueue( );
void freeQueue( Queue *pq );

queueType getNext( Queue *pq );
queueType dequeue( Queue *pq );
queueType front( Queue *pq );
void enqueue( Queue *pq, queueType qt );

void mergeQueues( Queue *to, Queue *from );

bool isEmpty( Queue *pq );
int getNumElements( Queue *pq );

#endif
