#ifndef _car_h
#define _car_h
#include <stdbool.h>

typedef struct Car
{
	int start; /*road to start on*/
	int end;   /*intersection to end on*/
	
	int stepAdded;
	
	bool movedThisStep;	/*tracks if this car moved this time step*/
	bool lastRoad;		/*tracks if this car is on its last road before its destination*/

}  Car;

#endif
