#include "graph.h"

int getIndex( Graph* g, graphType p );
void updatePathsArray( Graph* g );
void ssspBellmanFord( Graph *g, int source );

/* createGraph
 * input: an int representing the maximum number of vertices needed in your graph
 * output: a pointer to a Graph (this is malloc-ed so must be freed eventually)
 *
 * Creates a new empty Graph and returns a pointer to it.  The maximum number of vertices is set to capacity.
 */
Graph* createGraph( int capacity )
{
    int i,j;
    Graph* g = (Graph*)malloc(sizeof(Graph));
    g->vertexArray = (Vertex*)malloc(sizeof(Vertex)*capacity);
    g->vertexTable = createTableType( capacity, FIBONACCI_HASH, false );

    g->adjacencyMatrix = (int**)malloc(sizeof(int*)*capacity);
    g->pathsArray = (int**)malloc(sizeof(int*)*capacity);
    g->distance = (int**)malloc(sizeof(int*)*capacity);
    g->edgeData = (edgeType**)malloc(sizeof(edgeType*)*capacity);

    for( i=0; i<capacity; i++ ){
        g->adjacencyMatrix[i] = (int*)malloc(sizeof(int)*capacity);
        g->pathsArray[i] = (int*)malloc(sizeof(int)*capacity);
        g->distance[i] = (int*)malloc(sizeof(int)*capacity);
        g->edgeData[i] = (edgeType*)malloc(sizeof(edgeType)*capacity);
    }

    for( i=0; i<capacity; i++ ){
        for( j=0; j<capacity; j++ ){
            g->adjacencyMatrix[i][j] = -1;
            g->pathsArray[i][j] = -1;
            g->distance[i][j] = -1;
            g->edgeData[i][j] = NULL;
        }
    }

    g->numVertices = 0;
    g->capacity = capacity;
    g->changed = true;
    return g;
}

/* freeGraph and freeAdjList
 * input: a pointer to a Graph
 * output: none
 *
 * frees the given Graph and all of it's Vertex elements
 */
void freeGraph( Graph* g )
{
    int i;

    if( g==NULL ){
        printf("ERROR - freeGraph - Attempt to free NULL Graph pointer\n");
        exit(-1);
    }

    for( i=0; i<g->capacity; i++ ){
        free(g->adjacencyMatrix[i]);
        free(g->pathsArray[i]);
        free(g->distance[i]);
        free(g->edgeData[i]);
    }

    free( g->adjacencyMatrix );
    free( g->pathsArray );
    free( g->distance );
    free( g->edgeData );

    /* Free vertex table */
    freeTable( g->vertexTable );

    /* Free vertex array */
    free( g->vertexArray );
    free( g );
}

/* addVertex
 * input: a pointer to a Graph, a graphType p
 * output: none
 *
 * adds a new vertex to the graph with its data set to p
 */
void addVertex( Graph* g, graphType p ){
    int i = getIndex( g, p );
    if( g->numVertices==g->capacity ){
        printf("ERROR - addVertex - Attempt to add more vertices than the capacity of the graph\n");
        exit(-1);
    }

    /* Check if vertex already in graph */
    if( i!=-1 ){
        printf("WARNING - addVertex - Attempt to add same graphType to graph multiple times\n");
        return;
    }

    g->vertexArray[g->numVertices].data = p;
    g->vertexArray[g->numVertices].index = g->numVertices;
    g->vertexArray[g->numVertices].visited = false;

    insertTable( g->vertexTable, getKey( p ), &g->vertexArray[g->numVertices] );

    g->vertexArray[g->numVertices].currentSuccessorIndex = -1;
    g->vertexArray[g->numVertices].currentPredecessorIndex = -1;

    g->numVertices++;
    g->changed = true;
}

/* isVertex
 * input: a pointer to a Graph, a graphType p
 * output: a bool
 *
 * returns true if the given graphType is contained in the graph
 */
bool isVertex( Graph* g, graphType p ){
    return getIndex( g, p )!=-1;
}

/* getVertexByIndex
 * input: a pointer to a Graph, an int, a pointer to a graphType
 * output: a bool
 *
 * finds index-th vertex added to the graph and inserts into next.  It returns true if this search was successful and false otherwise.
 */
bool getVertexByIndex( Graph* g, int index, graphType* next ){
    if( index>=g->numVertices )
        return false;
    *next = g->vertexArray[index].data;
    return true;
}

/* getIndex
 * input: a pointer to a Graph, a graphType
 * output: a bool
 *
 * finds index-th vertex added to the graph and inserts into next.  It returns true if this search was successful and false otherwise.
 */
int getIndex( Graph* g, graphType p ){
    Vertex* v = (Vertex *)searchTable( g->vertexTable, getKey(p) );
    if( v==NULL )
        return -1;
    return v->index;
}

/* setEdge
 * input: a pointer to a Graph, two graphType variables, a int
 * output: none
 *
 * Sets the specified edge in the graph to the int value
 */
void setEdge( Graph* g, graphType p1, graphType p2, int value )
{
    int i, j;

    /* Add vertices if they're not already in graph */
    i = getIndex( g, p1 );
    if( i==-1 ){
        addVertex( g, p1 );
        i = getIndex( g, p1 );
    }
    
    j = getIndex( g, p2 );
    if( j==-1 ){
        addVertex( g, p2 );
        j = getIndex( g, p2 );
    }
    g->changed = true;
    g->adjacencyMatrix[i][j] = value;
}

/* getEdge
 * input: a pointer to a Graph, two graphType variables
 * output: a int
 *
 * Returns the int value for the specified edge is in the graph or -1 if the edge has not been added
 */
int getEdge( Graph* g, graphType p1, graphType p2 )
{
    int i, j;

    /* Add vertices if they're not already in graph */
    i = getIndex( g, p1 );
    if( i==-1 ){
        addVertex( g, p1 );
        i = getIndex( g, p1 );
    }
    
    j = getIndex( g, p2 );
    if( j==-1 ){
        addVertex( g, p2 );
        j = getIndex( g, p2 );
    }

    return g->adjacencyMatrix[i][j];
}

/* getSuccessor
 * input: a pointer to a Graph, a graphType 'start', a pointer to a graphType 'pnext'
 * output: a bool and the next successor of start is stored at 'pnext'
 *
 * Finds the next node reachable from 'start'.  It stores the destination of this edge into location pointed to by 'pnext'.
 * If a successor exists it returns true and false otherwise.
 */
bool getSuccessor( Graph* g, graphType start, graphType* pnext )
{
    int i = getIndex( g, start );
    int j;


    /* Add vertices if they're not already in graph */
    if( i==-1 ){
        addVertex( g, start );
        i = getIndex( g, start );
    }
    else if( pnext==NULL ){
        printf("ERROR - getSuccessor - Attempt to call getSuccessor with NULL pnext\n");
        exit(-1);
    }

    Vertex* v = &g->vertexArray[i];

    j = v->currentSuccessorIndex;
    for( j=j+1; j<g->numVertices; j++ ){
        if( g->adjacencyMatrix[i][j]!=-1){
            *pnext = g->vertexArray[j].data;
            v->currentSuccessorIndex = j;
            return true;
        }
    }
    v->currentSuccessorIndex = -1;
    return false;
}

/* getPredecessor
 * input: a pointer to a Graph, a graphType 'start', a pointer to a graphType 'pprev'
 * output: a bool and the next node with an edge going to start is stored at 'pprev'
 *
 * Finds the next node which has an edge going to 'start'.  It stores the destination of this edge into location pointed to by 'pprev'.
 * If a predecessor exists it returns true and false otherwise.
 */
bool getPredecessor( Graph* g, graphType start, graphType* pprev ){
    int i;
    int j = getIndex( g, start );


    /* Add vertices if they're not already in graph */
    if( j==-1 ){
        addVertex( g, start );
        j = getIndex( g, start );
    }
    else if( pprev==NULL ){
        printf("ERROR - getPredecessor - Attempt to call getSuccessor with NULL pprev\n");
        exit(-1);
    }

    Vertex* v = &g->vertexArray[j];

    i = v->currentPredecessorIndex;
    for( i=i+1; i<g->numVertices; i++ ){
        if( g->adjacencyMatrix[i][j]!=-1 ){
            *pprev = g->vertexArray[i].data;
            v->currentPredecessorIndex= i;
            return true;
        }
    }
    v->currentPredecessorIndex = -1;
    return false;
}

/* setVisited
 * input: a pointer to a Graph, a graphType, a bool value
 * output: none
 *
 * Sets the visited status of the vertex with the given graphType to 'value'
 */
void setVisited( Graph* g, graphType p, bool value ){
    int i = getIndex( g, p );

    /* Add vertices if they're not already in graph */
    if( i==-1 ){
        printf("WARNING - setVisited - Attempt to set visited for a graphType not in graph \n");
        addVertex( g, p );
        i = getIndex( g, p );
    }

    g->vertexArray[i].visited = value;
}

/* getVisited
 * input: a pointer to a Graph, a graphType
 * output: none
 *
 * Gets the visited status of the vertex with the given graphType
 */
bool getVisited( Graph* g, graphType p ){
    int i = getIndex( g, p );

    /* Add vertices if they're not already in graph */
    if( i==-1 ){
        printf("WARNING - getVisited - Attempt to get visited for a graphType not in graph \n");
        addVertex( g, p );
        i = getIndex( g, p );
    }

    return g->vertexArray[i].visited;
}

/* setDistance
 * input: a pointer to a Graph, two graphTypes, an int
 * output: none
 *
 * Sets the distance from the vertex 'from' to the vertex 'to' equal to 'dist'.
 */
void setDistance( Graph* g, graphType from, graphType to, int dist ){
    int i = getIndex( g, from );
    int j = getIndex( g, to );

    /* Add vertices if they're not already in graph */
    if( i==-1 ){
        printf("WARNING - getVisited - Attempt to set distance for a graphType not in graph \n");
        addVertex( g, from );
        i = getIndex( g, from );
    }
    if( j==-1 ){
        printf("WARNING - getVisited - Attempt to set distance for a graphType not in graph \n");
        addVertex( g, to );
        j = getIndex( g, to );
    }

    g->changed = true;

    g->distance[i][j] = dist;
}

/* getDistance
 * input: a pointer to a Graph, two graphTypes
 * output: an int
 *
 * Returns the distance from the vertex 'from' to the vertex 'to'.
 */
int getDistance( Graph* g, graphType from, graphType to ){
    int i = getIndex( g, from );
    int j = getIndex( g, to );

    /* Add vertices if they're not already in graph */
    if( i==-1 ){
        printf("WARNING - getDistance - Attempt to get distance for a graphType not in graph \n");
        addVertex( g, from );
        i = getIndex( g, from );
    }
    if( j==-1 ){
        printf("WARNING - getDistance - Attempt to get distance for a graphType not in graph \n");
        addVertex( g, to );
        j = getIndex( g, to );
    }

    return g->distance[i][j];
}


/* setEdgeData
 * input: a pointer to a Graph, two graphType variables, an edgeType
 * output: none
 *
 * Sets the specified edgeData in the graph to the edgeType value
 */
void setEdgeData( Graph* g, graphType from, graphType to, edgeType edgeData )
{
    int i = getIndex( g, from );
    int j = getIndex( g, to );

    if( i==-1 || j==-1){
        printf("ERROR - Attempt to set invalid edge\n");
        exit(-1);
    }

    g->edgeData[i][j] = edgeData;
}

/* getEdgeData
 * input: a pointer to a Graph, two graphType variables
 * output: an edgeType
 *
 * Finds and returns the specified edgeData in the graph
 */
edgeType getEdgeData( Graph* g, graphType from, graphType to )
{
    int i = getIndex( g, from );
    int j = getIndex( g, to );

    if( i==-1 || j==-1){
        printf("ERROR - Attempt to get invalid edge\n");
        exit(-1);
    }

    return g->edgeData[i][j];
}

/* getNextOnShortestPath
 * input: a pointer to a Graph, a graphType 'to', a graphType 'from', a pointer to a graphType 'pnext'
 * output: a bool
 *
 * Returns false if no path exists.
 * Otherwise, returns true and fills pnext with next graph element to visit on a shortest path from 'from' to 'to'.
 */
bool getNextOnShortestPath( Graph* g, graphType from, graphType to, graphType *pnext ){
    int i = getIndex( g, from );
    int j = getIndex( g, to );

    if( i==-1 || j==-1){
        printf("ERROR - Invalid vertex\n");
        exit(-1);
    }

    if( g->changed==true ) /* update the distances if the graph has changed */
        updatePathsArray( g );

    if( g->pathsArray[i][j]<0 ) /* no path exists */
        return false;

    return getVertexByIndex( g, g->pathsArray[i][j], pnext );
}

/* shortestPath
 * input: a pointer to a Graph, a graphType 'to', a graphType 'from'
 * output: a int
 *
 * Returns the length of the shortest path from 'from' to 'to'.
 * Returns -1 if path does not exist
 */
int shortestPathDistance( Graph* g, graphType from, graphType to ){
    int i = getIndex( g, from );
    int j = getIndex( g, to );

    if( i==-1 || j==-1){
        printf("ERROR - Invalid vertex\n");
        exit(-1);
    }

    if( g->changed==true ) /* update the distances if the graph has changed*/
        updatePathsArray( g );

    return g->distance[i][j];
}

/* updatePathsArray
 * INTERNAL USE ONLY
 */
/* updatePathsArray
 * input: a pointer to a Graph
 * output: none
 *
 * Updates the pathsArray using a variant of the Floyd-Warshall algorithm
 */
void updatePathsArray( Graph* g ){
    int i, j, k;

    for( i=0; i<g->capacity; i++ )
        for( j=0; j<g->capacity; j++ )
            g->distance[i][j] = g->adjacencyMatrix[i][j];

    for( i=0; i<g->capacity; i++ )
        g->distance[i][i] = 0;

    for( i=0; i<g->capacity; i++ )
        for( j=0; j<g->capacity; j++ )
            if( g->adjacencyMatrix[i][j]>0 )
                g->pathsArray[i][j] = j;
            else
                g->pathsArray[i][j] = -1;

    for( i=0; i<g->capacity; i++ )
        g->pathsArray[i][i] = i;

    /* Floyd-Warshall algorithm */
    for( k=0; k<g->capacity; k++ )
        for( i=0; i<g->capacity; i++ )
            for( j=0; j<g->capacity; j++ )
                if( g->distance[i][k]>=0 && g->distance[k][j]>=0 && (g->distance[i][j]<0 || g->distance[i][k]+g->distance[k][j]<g->distance[i][j]) ){
                    g->distance[i][j]=g->distance[i][k]+g->distance[k][j];
                    g->pathsArray[i][j]=g->pathsArray[i][k]; /* update the stored shortest path */
                }

    g->changed=false;
}

/* ssspBellmanFord (currently unused)
 * INTERNAL USE ONLY
 */
void ssspBellmanFord( Graph *g, int source ){
    int i,j,k;

    for( k=0; k<g->capacity; k++ )
        for( i=0; i<g->capacity; i++ )
            for( j=0; j<g->capacity; j++ )
                if( g->adjacencyMatrix[i][j]>0 && g->distance[source][i]>0 && (g->distance[source][j]<=0 || g->distance[source][i]+g->adjacencyMatrix[i][j]<g->distance[source][j] )){
                    g->distance[source][j]=g->distance[source][i]+g->adjacencyMatrix[i][j];
                    g->pathsArray[ g->pathsArray[source][i] ][j] = i;
                }
}
