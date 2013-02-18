/***********************************************************/
/*Kishan Patel											   */
/*260376121       										   */
/*Mini Assingment 3					                       */
/***********************************************************/
#include <stdio.h>
#include <stdlib.h>

#define REQ_NOT_MADE 	1
#define	REQ_PENDING 	2
#define	REQ_REJECTED	3
int noProcess;
int noResources;
int i;
int *availabilityVector;
int **reqVector;
int **needVector;
int **holdVector;
int *workVector;
int *finishVector;
int *processState;
int terminate = 0;

/*
Vectors needed:
REQ - N*M
AVAIL - M
NEED - N*M
HOLD - N*M
WORK - M
FINISH - N
*/
initialize()
{
	//Number of processes and resources.
	noProcess = (rand() % 6) + 5; //noProc in the range 5-10
	noResources = 4;
	
	//Initialize to appropriate size.
	availabilityVector = (int *)malloc(sizeof(int)*noResources);
	reqVector = (int **)malloc(sizeof(int *)*noResources);
	needVector = (int **)malloc(sizeof(int *)*noResources);
	holdVector = (int **)malloc(sizeof(int *)*noResources);
	workVector = (int *)malloc(sizeof(int)*noResources);
	finishVector = (int *)malloc(sizeof(int)*noProcess);
	for(i=0;i<noProcess;i++)
	{
		reqVector[i] = (int *)malloc(sizeof(int)*noProcess);
		needVector[i] = (int *)malloc(sizeof(int)*noProcess);
		holdVector[i] = (int *)malloc(sizeof(int)*noProcess);
	}
	processState = (int *)malloc(sizeof(int)*noProcess);
	
	//Initialize the state of each process. Intially, none of the processes have
	//made any requests
	for(i=0;i<noProcess;i++)
	{
		processState[i] = REQ_NOT_MADE;
	}
	
	//Initialse the arrays to the appropriate values.
	int noP = 0, noR = 0;
	int sumOfMaxResources = 0;
	for(noP=0;noP<noProcess;noP++)
	{
		for(noR=0;noR<noResources;noR++)
		{
			needVector[noP][noR] = (rand() % 8) + 5;
		}
	}
	
	for(noR=0;noR<noResources;noR++)
	{
		sumOfMaxResources = 0;
		for(noP=0;noP<noProcess;noP++)
		{
			sumOfMaxResources += needVector[noP][noR];
		}
		availabilityVector[noR] = 0.6*sumOfMaxResources;
	}
}

void generateRequest()
{

}

void verifyRequest()
{
	//Check if the requests of all the processes have been rejected. If 
	//so, we print the statistics and end the program.
	terminate = 1;
	for(i=0;i<noProcess;i++)
	{
		if(processState[i]!=REQ_REJECTED)
		{
			terminate = 0;
			break;
		}
	}
	if(terminate)
		return;
		
	//Pre-requisites before checking if system will be in safe state after
	//granting resources.
	
	
}

int isSafe()
{

}

void updateSystemState()
{
}

void printStatistics()
{
}

int main(int argc,char *argv[])
{
	initialize();
}