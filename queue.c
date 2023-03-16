/* THIS FILE WAS CONTRIBUTED BY INSTRUCTOR, DR. JESSICA SHERETTE. */

#include "queue.h"

/* createQueue
 * input: none
 * output: a pointer to a queue (this is malloc-ed so must be freed eventually!)
 *
 * Creates a new empty queue and returns a pointer to it.
 */
Queue *createQueue( )
{
    Queue *pq = (Queue *)malloc( sizeof(Queue) );
    if( pq==NULL )
    {
        printf("ERROR - malloc of Queue failed\n");
        exit(-1);
    }
    pq->qFront = NULL;
    pq->qRear = NULL;
    pq->numElements = 0;

    return pq;
}

/* freeQueue
 * input: a pointer to a Queue
 * output: none
 *
 * frees the given Queue pointer.  Also, you should remember to free the elements in the linked list!
 */
void freeQueue( Queue *pq )
{
    while( !isEmpty(pq) )
        dequeue( pq );
    free( pq );
}

/* getNext
 * input: a pointer to a Queue
 * output: a pointer to queueType
 *
 * Returns the queueType stored at the front of the Queue.  It does not remove the element from the queue.
 */
queueType getNext( Queue *pq )
{
    if( isEmpty( pq ) )
    {
        /* no element to return */
        return NULL;
    }
    return pq->qFront->qt;
}

/* dequeue
 * input: a pointer to a Queue
 * output: a pointer to queueType
 *
 * Dequeues and returns the queueType stored at the front of the Queue.  It does not free the dequeue-ed element.
 */
queueType dequeue( Queue *pq )
{
    LLNode *temp;
    queueType qt;

    if( isEmpty( pq ) )
    {
        /* no element to return */
        return NULL;
    }
    temp = pq->qFront;
    pq->qFront = pq->qFront->pNext;
    if( pq->qFront==NULL ){
        pq->qRear=NULL;
    }
    qt = temp->qt;
    free(temp);
    pq->numElements--;

    return qt;
}

/* front
 * input: a pointer to a Queue
 * output: a pointer to queueType
 *
 * Returns the queueType stored at the front of the Queue.  It does not dequeue or free the front element.
 */
queueType front( Queue *pq )
{
    if( isEmpty( pq ) )
    {
        /* no element to return */
        return NULL;
    }

    return pq->qFront->qt;
}

/* enqueue
 * input: a pointer to a Queue, a queueType
 * output: none
 *
 * Inserts the queueType on the rear of the given Queue.
 */
void enqueue( Queue *pq, queueType qt )
{
    LLNode *node = (LLNode*)malloc(sizeof(LLNode));
    if(node==NULL){
        fprintf( stderr, "enqueue: Failed to allocate memory");
        exit(-1);
    }
    pq->numElements++;

    node->pNext=NULL;
    node->qt = qt;
    if( isEmpty(pq) ){
        pq->qFront = node;
        pq->qRear = node;
        return;
    }
    pq->qRear->pNext = node;
    pq->qRear = node;
}

/* mergeQueues
 * input: a pointer to 2 Queues
 * output: none
 *
 * Merges the Queue 'from' onto the end of the Queue 'to'.
 */
void mergeQueues( Queue *to, Queue *from )
{
    while( !isEmpty(from) )
        enqueue( to, dequeue(from) );
}

/* isEmpty
 * input: a pointer to a Queue
 * output: a boolean
 *
 * returns TRUE if the Queue is empty and FALSE otherwise
 */
bool isEmpty( Queue *pq )
{
    if( pq->qFront==NULL && pq->qRear==NULL )
    {
        return true;
    }
    else if( pq->qFront==NULL || pq->qRear==NULL )
    {
        fprintf( stderr, "isEmpty: Queue had inconsistent values for front and rear.\n" );
        exit(-1);
    }
    return false;
}

/* getNumElements
 * input: a pointer to a Queue
 * output: an int
 *
 * Returns the numElements of the provided Queue.
 */
int getNumElements( Queue *pq )
{
    return pq->numElements;
}
