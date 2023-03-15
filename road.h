#ifndef _road_h
#define _road_h
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "car.h"
#include "queue.h"

typedef struct RoadData
{
    /* TODO - Add data associated with road.  Some suggested data: */
	
	/*the order of the roads added*/
	int roadOrder;

	int length;

    /* information used to record/update whether the light at the end of this road is green or red */
	bool isGreen;
	int greenOn;
	int greenOff;
	int lightReset;

    /* intersections this road starts from and moves to */
	int interFrom;
	int interTo;

	Car** carsArr;
    
	Queue* carsWaiting;

}  RoadData;

void printCar();

#endif

