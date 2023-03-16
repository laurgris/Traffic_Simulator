#ifndef _trafficSimulator_h
#define _trafficSimulator_h
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "graph.h"
#include "queue.h"
#include "priorityQueue.h"
#include "event.h"
#include "road.h"

typedef struct TrafficData
{
    /* graph representing the road network */
	Graph* g;

    /* array of the roads of the graph in the sequence they were added to the graph */
	int roadNum;
	RoadData** roads;

    /* priority queue of events where the priority represents the time the event will occur */
	PriorityQueue* eventQueue;

    /* track the number of cars still in the simulator */
	int carsRemaining;

    /* track the longest number of time steps that any light takes to cycle around.  This is useful for detecting gridlock. */
	int longestLight;
	
	/*total count of time sim has run for*/
	int totalSteps;

}  TrafficData;

void printNames( );

void trafficSimulator( char* filename );

#endif
