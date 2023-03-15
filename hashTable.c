#include "hashTable.h"
#include <math.h>
#include <time.h>

void freeList( hashTableEntry* pe );
void createAndInsert( long size, hashType type  );


/* createTable
 * input: long
 * output: hashTable*
 *
 *  Creates and returns a hashTable* with the specified size.
 */
hashTable* createTable( long tableSize ){
    long i;
    hashTable* ph = (hashTable*)malloc( sizeof(hashTable) );

    ph->tableSize = tableSize;
    ph->table = (hashTableEntry**)malloc( sizeof(hashTableEntry*)*ph->tableSize );
    if( ph->table==NULL ){
        printf("ERROR - createTable - Failed to malloc hash table\n");
        exit(-1);
    }

    for( i=0; i<tableSize; i++)
        ph->table[i] = NULL;

    ph->A = (sqrt(5)-1)/2;

    ph->type = FIBONACCI_HASH; /* Default to using the FIBONACCI_HASH */
    ph->reportCollisions = false;
    ph->numCollisions = 0;
    return ph;
}

hashTable* createTableType( long tableSize, hashType type, bool report ){
    long i;
    hashTable* ph = (hashTable*)malloc( sizeof(hashTable) );

    ph->tableSize = tableSize;
    ph->table = (hashTableEntry**)malloc( sizeof(hashTableEntry*)*ph->tableSize );
    if( ph->table==NULL ){
        printf("ERROR - createTable - Failed to malloc hash table\n");
        exit(-1);
    }

    for( i=0; i<tableSize; i++)
        ph->table[i] = NULL;

    ph->A = (sqrt(5)-1)/2;

    ph->type = type;
    ph->reportCollisions = report;
    ph->numCollisions = 0;
    return ph;
}

/* freeTable and freeList
 * input: hashTable*
 * output: N/A
 *
 *  Frees all of the data in the hashTable.
 */
void freeTable( hashTable* ph ){
    long i;

    for( i=0; i<ph->tableSize; i++ )
        if( ph->table[i] != NULL )
            freeList( ph->table[i] );

    if( ph->reportCollisions==true ){
        printf("Number of hash table collisions = %ld\n", ph->numCollisions);
    }

    free( ph->table );
    free( ph );
}

void freeList( hashTableEntry* pe ){
    hashTableEntry* temp;
    while( pe!=NULL ){
        temp = pe->nextEntry;
        free( pe );
        pe = temp;
    }
}

long hashCode( hashTable* ph, long key ){
    if( ph->type == NAIVE_HASH )
        return hashCodeNaive( ph, key );
    else if( ph->type == FIBONACCI_HASH )
        return hashCodeFibonacci( ph, key );
    else /* if( ph->type == UNSORTEDLL_HASH ) */
        return 0; /* All records collide in hash table */
}

long hashCodeNaive( hashTable* ph, long key ){
    if(key%ph->tableSize<0)
        return key%ph->tableSize + ph->tableSize;
    return key%ph->tableSize;
}

long hashCodeFibonacci( hashTable* ph, long key ){
    double product = key*ph->A;
    double fraction = product - (long)product;
    long slot = (long)(ph->tableSize*fraction);
    return slot;
}

/* searchTable
 * input: hashTable* ph, long key
 * output: void*
 *
 *  Searchs the hashTable based on the given key and returns data associated with this key
 */
void* searchTable( hashTable* ph, long key ){
    hashTableEntry* pe = ph->table[ hashCode(ph, key) ];
    while( pe!=NULL && pe->key!=key ){
        pe = pe->nextEntry;
    }
    if( pe!=NULL )
        return pe->data;
    return NULL;
}

/* insertTable
 * input: hashTable* ph, long key, void* data
 * output: N/A
 *
 *  Inserts the given void* into hashTable based on the given key
 */
void insertTable( hashTable* ph, long key, void* data ){
    if( searchTable(ph,key)==NULL ){
        hashTableEntry* pe = (hashTableEntry*)malloc( sizeof(hashTableEntry) );
        if( pe==NULL ){
            printf("ERROR - insertTable - Failed to malloc hash table entry\n");
            exit(-1);
        }

        pe->key = key;
        pe->data = data;

        if( ph->table[hashCode(ph, key)]!=NULL )
            ph->numCollisions++;

        pe->nextEntry = ph->table[ hashCode(ph, key) ];
        ph->table[ hashCode(ph, key) ] = pe;
    }
    else{
        printf("ERROR - insertTable - Attempting to insert duplicate key.\n");
    }
}

/*
//Functions to test hash table
int main( ){
    clock_t start, end;
    start = clock();
    createAndInsert( 100000, UNSORTEDLL_HASH );
    end = clock();
    printf( "UNSORTEDLL_HASH testing on 10,000 inserts took %lf seconds\n\n", (double)(end - start)/ CLOCKS_PER_SEC );

    start = clock();
    createAndInsert( 10000000, NAIVE_HASH );
    end = clock();
    printf( "NAIVE_HASH testing on 10,000,000 inserts took %lf seconds\n\n", (double)(end - start)/ CLOCKS_PER_SEC );

    start = clock();
    createAndInsert( 10000000, FIBONACCI_HASH );
    end = clock();
    printf( "FIBONACCI_HASH testing on 10,000,000 inserts took %lf seconds\n\n", (double)(end - start)/ CLOCKS_PER_SEC );
}

void createAndInsert( long size, hashType type  ){
    long i;
    hashTable* ph = createTable( size, type, true );

    for( i = 10000000; i<size+10000000; i++ )
        insertTable( ph, i, "Data" );

    freeTable( ph );
}*/

