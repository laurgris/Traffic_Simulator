#include "priorityQueue.h"

/* createPQ
 * input: none
 * output: a pointer to a queue (this is malloc-ed so must be freed eventually!)
 *
 * Creates a new empty queue and returns a pointer to it.
 */
PriorityQueue *createPQ( )
{
    PriorityQueue *pq = (PriorityQueue *)malloc( sizeof(PriorityQueue) );
    if( pq!=NULL )
    {
        pq->qFront = NULL;
        pq->qRear = NULL;
    }

    return pq;
}

/* freePQ
 * input: a pointer to a PriorityQueue
 * output: none
 *
 * frees the given PriorityQueue pointer.  Also, you should remember to free the elements in the linked list!
 */
void freePQ( PriorityQueue *pq )
{
    free(pq);
}

/* getFrontPriority
 * input: a pointer to a PriorityQueue
 * output: the priority of the front element
 *
 * Returns the priority of the front of the PriorityQueue.  It does not remove the element from the PriorityQueue.
 */
int getFrontPriority( PriorityQueue *pq )
{
    if( isEmptyPQ( pq ) )
    {
        /* no element to return */
        printf( "ERROR - getFrontPriority - attempt to get priority of empty priority queue.\n" );
        exit(-1);
    }
    return pq->qFront->priority;
}

/* getNextPQ
 * input: a pointer to a PriorityQueue
 * output: a pointer to process
 *
 * Returns the process stored at the front of the PriorityQueue.  It does not remove the element from the PriorityQueue.
 */
priorityQueueType getNextPQ( PriorityQueue *pq )
{
    if( isEmptyPQ( pq ) )
    {
        /* no element to return */
        return NULL;
    }
    return pq->qFront->qt;
}

/* dequeuePQ
 * input: a pointer to a PriorityQueue
 * output: a pointer to process
 *
 * Dequeues and returns the process stored at the front of the PriorityQueue.  It does not free the dequeue-ed element.
 */
priorityQueueType dequeuePQ( PriorityQueue *pq )
{
    PQNode *temp;
    priorityQueueType qt;

    if( isEmptyPQ( pq ) )
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

    return qt;
}

/* enqueueByPriority
 * input: a pointer to a PriorityQueue, a priorityQueueType
 * output: none
 *
 * Inserts the process based on its priority.
 */
void enqueueByPriority( PriorityQueue *pq, priorityQueueType qt, int priority )
{
    PQNode *curr;
    PQNode *node = (PQNode*)malloc(sizeof(PQNode));
    if(node==NULL){
        fprintf( stderr, "enqueue: Failed to allocate memory\n");
        exit(-1);
    }

    node->pNext=NULL;
    node->qt = qt;
    node->priority = priority;

    //Insert in empty list
    if( isEmptyPQ(pq) ){
        pq->qFront = node;
        pq->qRear = node;
        return;
    }

    //Insert at front
    if( node->priority < pq->qFront->priority ){
        node->pNext = pq->qFront;
        pq->qFront = node;
        return;
    }

    curr = pq->qFront;
    while( curr->pNext!=NULL && curr->pNext->priority <= node->priority ){
        curr = curr->pNext;
    }

    //Insert after curr
    node->pNext = curr->pNext;
    curr->pNext = node;
}


/* isEmptyPQ
 * input: a pointer to a PriorityQueue
 * output: a boolean
 *
 * returns TRUE if the PriorityQueue is empty and FALSE otherwise
 */
bool isEmptyPQ( PriorityQueue *pq )
{
    if( pq->qFront==NULL && pq->qRear==NULL )
    {
        return true;
    }
    else if( pq->qFront==NULL || pq->qRear==NULL )
    {
        fprintf( stderr, "isEmpty: PriorityQueue had inconsistent values for front and rear.\n" );
        exit(-1);
    }
    return false;
}
