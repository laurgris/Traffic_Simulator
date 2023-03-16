#include "trafficSimulator.h"

/* Function declarations: */
TrafficData* readTrafficDataFromInputFile( char* name );

void updateLights(TrafficData* tData);
void addCars(RoadData* roadTemp, Event* tempEvent, TrafficData* tData, int* totalCars);
void printEvents(RoadData* roadTemp);
bool carIsLeaving(TrafficData* tData, RoadData* roadTemp, int* longestCar, double* averageTime);
bool moveCar(RoadData* nextRoad, RoadData* roadTemp);

/* printNames
 * input: none
 * output: none
 *
 * Prints names of the students who worked on this solution
 */
void printNames( )
{
    printf("This solution was completed by:\n");
    printf("Lauren Grissom\n");
    printf("Christian Walker\n");
}


/* trafficSimulator
 * input: char* name of file to read
 * output: N/A
 *
 * Read in the traffic data from the file whose name is stored in char* filename
 */
void trafficSimulator( char* filename )
{
    TrafficData* tData = readTrafficDataFromInputFile( filename );
	Event* tempEvent;
	RoadData* roadTemp;
	bool gridlocked = false, hasCarMoved = false;  		/*hasCarMoved tracks if any car has moved this time step*/
	int i, j, timeSinceLastMove = 0, longestCar = 0;	/*longestCar tracks the longest time a car has been in the sim for*/
	int totalCars = 0;					/*tracks the absolute total cars that entered the sim*/
	double averageTime = 0;
	tData->totalSteps = 0;
	tData->carsRemaining = 0;
	
	
    /* Loop until all events processed and either all cars reached destination or gridlock has occurred */
	do{
		updateLights(tData);
		
		/*check the event queue for events*/
		while(!isEmptyPQ(tData->eventQueue) && getNextPQ(tData->eventQueue)->time == tData->totalSteps){
			tempEvent = dequeuePQ(tData->eventQueue);
			
			/*if we must add a car*/
			if(tempEvent->type == ADD_CAR_EVENT){
				roadTemp = tData->roads[tempEvent->startingRoad];			/*fetch appropriate road*/
				
				addCars(roadTemp, tempEvent, tData, &totalCars);
			}
			/*if we must print the roads*/
			else{
				printf("\nCYCLE %d - PRINT_ROADS_EVENT - Current contents of the roads:\n", tData->totalSteps);
				
				for(i = 0; i < tData->roadNum; i++){
					roadTemp = tData->roads[i];
					
					printEvents(roadTemp);
				}
				printf("\n");
			}
			
			free(tempEvent);
		}
	

        /* First try to move cars through the next intersection */
		for(i = 0; i < tData->roadNum; i++){
			roadTemp = tData->roads[i];
			Car* car = roadTemp->carsArr[roadTemp->length-1];
			
			/*if there is an open spot in the next intersection and the light is green*/
			if(car != NULL && roadTemp->isGreen){
				int nextInter = -1;
				bool isNextInter = getNextOnShortestPath( tData->g, roadTemp->interTo, car->end, &nextInter );
				
				/*if the car is on the last road before its destination*/
				if(isNextInter && car->end == roadTemp->interTo){
					hasCarMoved = carIsLeaving(tData, roadTemp, &longestCar, &averageTime);
					
				}
				else{
					RoadData* nextRoad = getEdgeData(tData->g, roadTemp->interTo, nextInter);
					if(nextRoad->carsArr[0] == NULL){
						/*move car on end of roadTemp to beginning of nextRoad*/
						hasCarMoved = moveCar(nextRoad, roadTemp);
					}
				}
				
				/*if the next intersection is the car's destination*/
				if(car != NULL && nextInter == car->end){
					car->lastRoad = true;
				}
			}
		}
		
		/* Second move waiting cars onto the end of each road if possible */
		for(i = 0; i < tData->roadNum; i++){
			roadTemp = tData->roads[i];
			
			if(!isEmpty(roadTemp->carsWaiting) && roadTemp->carsArr[0] == NULL){
				roadTemp->carsArr[0] = dequeue(roadTemp->carsWaiting);
				roadTemp->carsArr[0]->movedThisStep = true;
				hasCarMoved = true;
			}
		}
		
        /* Third move cars forward on every road (only those that haven't moved yet this time step) */
		for(i = 0; i < tData->roadNum; i++){
			roadTemp = tData->roads[i];
			
			for(j = roadTemp->length - 2; j >= 0; j--){
				if(roadTemp->carsArr[j] != NULL && roadTemp->carsArr[j + 1] == NULL && !roadTemp->carsArr[j]->movedThisStep){
					roadTemp->carsArr[j+1] = roadTemp->carsArr[j];
					roadTemp->carsArr[j] = NULL;
					roadTemp->carsArr[j+1]->movedThisStep = true;
					hasCarMoved = true;
					j--;
				}
			}
		}
		
		/*reset all cars' "movedThisStep" */
		for(i = 0; i < tData->roadNum; i++){
			for(j = 0; j < tData->roads[i]->length; j++){
				if(tData->roads[i]->carsArr[j] != NULL){
					tData->roads[i]->carsArr[j]->movedThisStep = false;
				}
			}
		}
		
		/*if no cars in the sim have moved*/
		if(!hasCarMoved && tData->carsRemaining != 0){
			timeSinceLastMove++;
		}
		else{
			timeSinceLastMove = 0;
		}
		
		if(timeSinceLastMove >= tData->longestLight){
			gridlocked = true;
		}
		
		/*update more variables*/
		tData->totalSteps++;
		hasCarMoved = false;
		
	}while(!isEmptyPQ(tData->eventQueue) || (tData->carsRemaining != 0 && !gridlocked));
	
	if(!gridlocked){
		printf("\nAverage number of time steps to the reach their destination is %.2lf.\n", (double)(averageTime / totalCars));
		printf("Maximum number of time steps to the reach their destination is %d.\n", longestCar);
	}
	else{
		printf("CYCLE %d - Gridlock detected.\n", tData->totalSteps);
	}
	
	/*freeing event PQ*/
	while(!isEmptyPQ(tData->eventQueue)){
		/*free all events inside*/
		tempEvent = dequeuePQ(tData->eventQueue);
		while(!isEmpty(tempEvent->carsToAdd)){
			/*free cars to add Q*/
			free(dequeue(tempEvent->carsToAdd));
		}
		freeQueue(tempEvent->carsToAdd);
		
		free(tempEvent);
	}
	freePQ(tData->eventQueue);
	
	
	/*freeing tData->roads*/
	for(i = tData->roadNum - 1; i >= 0; i--){
		/*free cars array*/
		roadTemp = tData->roads[i];
		for(j = roadTemp->length-1; j >= 0; j--){
			/*free any cars inside*/
			if(roadTemp->carsArr[j] != NULL){
				free(roadTemp->carsArr[j]);
			}
		}
		free(roadTemp->carsArr);
		
		/*free waiting car Q*/
		while(!isEmpty(roadTemp->carsWaiting)){
			/*free any cars inside*/
			free(dequeue(roadTemp->carsWaiting));
		}
		freeQueue(roadTemp->carsWaiting);
		
		free(roadTemp);
	}
	
	free(tData->roads);
	
	/*free graph*/
	freeGraph(tData->g);
	
	/*free traffic data*/
	free(tData);
}

/* readTrafficDataFromInputFile
 * input: char* filename of file to read
 * output: TrafficData* which stores the data of this road network
 *
 * Create a new TrafficData variable and read in the traffic data from the file whose name is stored in char* filename
 */
TrafficData* readTrafficDataFromInputFile( char* filename )
{
    /* open the file */
    FILE *pFile = fopen( filename, "r" );
	TrafficData* data = (TrafficData*)malloc(sizeof(TrafficData));
	int vertNum, carCommands, events, i, j, k;
	int totalRoads = 0;
    
	data->totalSteps = 0;
	data->longestLight = 0;
	
	fscanf( pFile, "%d %d", &vertNum, &data->roadNum );
	
	data->g = createGraph(vertNum);
	data->longestLight = 0;
	data->roads = (RoadData**)malloc(sizeof(RoadData*) * data->roadNum);
	
	/*loop to create all intersections*/
	for(i = 0; i < vertNum; i++){
		int incomingRoads;
		fscanf( pFile, "%d", &incomingRoads );
		
		/*loop to create all incoming roads to one intersection*/
		for(j = 0; j < incomingRoads; j++){
			int from, length, greenOn, greenOff, reset;
			fscanf( pFile, "%d %d	    %d %d %d", &from, &length, &greenOn, &greenOff, &reset);
			
			/*update the longest light if necessary*/
			if(reset > data->longestLight){
				data->longestLight = reset;
			}
			
			/*initialize all road data*/
			setEdge( data->g, from, i, length);
			RoadData* roadData = (RoadData*)malloc(sizeof(RoadData));
			roadData->roadOrder = totalRoads;
			roadData->length = length;
			roadData->isGreen = false;
			roadData->greenOn = greenOn;
			roadData->greenOff = greenOff;
			roadData->lightReset = reset;
			roadData->interFrom = from;
			roadData->interTo = i;
			roadData->carsArr = (Car**)malloc(sizeof(Car*) * length);
			
			/*set every spot on the road to NULL*/
			for(k = 0; k < roadData->length; k++){
				roadData->carsArr[k] = NULL;
			}
			
			roadData->carsWaiting = createQueue();
			
			data->roads[totalRoads] = roadData;
			setEdgeData(data->g, from, i, roadData);
			
			totalRoads++;
		}
	}
	
	fscanf( pFile, "%d", &carCommands );
	
	data->eventQueue = createPQ();
	
	/*loop to create all "add car" events*/
	for(i = 0; i < carCommands; i++){
		int fromRoad, toRoad, time, cars, dest;
		Event* event = (Event*)malloc(sizeof(Event));
		RoadData* roadTemp;
		
		event->type = ADD_CAR_EVENT;
		event->carsToAdd = createQueue();
		
		fscanf( pFile, "%d %d %d", &fromRoad, &toRoad, &time );
		
		roadTemp = getEdgeData( data->g, fromRoad, toRoad );
		event->startingRoad = roadTemp->roadOrder;
		event->time = time;
		
		fscanf( pFile, "%d", &cars );
		event->numCars = cars;
		
		/*loop to create queue of cars associated with one event*/
		for(j = 0; j < cars; j++){
			Car* car = (Car*)malloc(sizeof(Car));
			fscanf( pFile, "%d", &dest );
			car->start = roadTemp->roadOrder;
			car->end = dest;
			car->stepAdded = time;
			car->movedThisStep = false;
			car->lastRoad = false;
			
			enqueue(event->carsToAdd, car);
		}
		
		enqueueByPriority(data->eventQueue, event, time);
	}
	
	fscanf( pFile, "%d", &events );
	
	/*loop to create all "print" events*/
	for(i = 0; i < events; i++){
		int time;
		fscanf( pFile, "%d", &time );
		Event* event = (Event*)malloc(sizeof(Event));
		
		event->type = PRINT_ROADS_EVENT;
		event->time = time;
		
		enqueueByPriority(data->eventQueue, event, time);
	}
	
    fclose( pFile );

    return data;
}

/*maybe create helper functions to print out cycle movements?*/

/*updates all traffic lights*/
void updateLights(TrafficData* tData){
	int i;
	for(i = 0; i < tData->roadNum; i++){
		int resetTime = tData->roads[i]->lightReset;
		int lightGreen = tData->roads[i]->greenOn;
		int lightRed = tData->roads[i]->greenOff;
		
		if(tData->totalSteps % resetTime == lightGreen){
			tData->roads[i]->isGreen = true;
		}
		else if (tData->totalSteps % resetTime == lightRed){
			tData->roads[i]->isGreen = false;
		}
	}
}

/*adds cars from an event to an intersection's queue*/
void addCars(RoadData* roadTemp, Event* tempEvent, TrafficData* tData, int* totalCars){
	mergeQueues( roadTemp->carsWaiting, tempEvent->carsToAdd );
	
	tData->carsRemaining += tempEvent->numCars;					/*update num of cars that have entered sim*/
	*totalCars += tempEvent->numCars;
	
	freeQueue(tempEvent->carsToAdd);
	
	printf("CYCLE %d - ADD_CAR_EVENT - Cars enqueued on road from %d to %d\n", tData->totalSteps, roadTemp->interFrom, roadTemp->interTo);
}

/*prints all info for a "print" event*/
void printEvents(RoadData* roadTemp){
	int i;
	printf("Cars on the road from %d to %d:\n", roadTemp->interFrom, roadTemp->interTo);
	
	for(i = roadTemp->length - 1; i >= 0; i--){
		if(roadTemp->carsArr[i] == NULL){
			printf("- ");
		}
		else{
			printf("%d ", roadTemp->carsArr[i]->end);
		}
	}
	
	if(roadTemp->isGreen){
		printf("(GREEN light)\n");
	}
	else{
		printf("(RED light)\n");
	}
}

/*performs the actions to remove a car from the sim if it gets to its destination*/
bool carIsLeaving(TrafficData* tData, RoadData* roadTemp, int* longestCar, double* averageTime){
	Car* car = roadTemp->carsArr[roadTemp->length-1];
	int timeDiff = tData->totalSteps - car->stepAdded + 1;
	tData->carsRemaining--;
	
	if(timeDiff > *longestCar){
		*longestCar = timeDiff;
	}
	
	*averageTime += timeDiff;
	
	printf("CYCLE %d - Car successfully traveled from %d to %d in %d time steps.\n", tData->totalSteps, tData->roads[car->start]->interFrom, car->end, timeDiff);
	roadTemp->carsArr[roadTemp->length-1] = NULL;
	free(car);
	
	return true;
}

/*moves a car from one road to another*/
bool moveCar(RoadData* nextRoad, RoadData* roadTemp){
	
	nextRoad->carsArr[0] = roadTemp->carsArr[roadTemp->length-1];
	roadTemp->carsArr[roadTemp->length-1] = NULL;
	nextRoad->carsArr[0]->movedThisStep = true;
	
	return true;
}
