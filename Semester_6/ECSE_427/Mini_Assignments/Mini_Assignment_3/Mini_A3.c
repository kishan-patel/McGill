/***********************************************************/
/*Kishan Patel											  										 */
/*260376121       										   									 */
/*Mini Assingment 3					                       				 */
/***********************************************************/
#include <stdio.h>
#include <stdlib.h>

#define SAFE_STATE		1
#define UNSAFE_STATE 	2
int noProcess;
int noResources;
int i,j;
int *availabilityVector;
int **reqVector;
int **needVector;
int **holdVector;
int *workVector;
int *finishVector;
int allRequestsSatisfied = 1;
int status;

/*
Vectors needed:
REQ - N*M
AVAIL - M
NEED - N*M
HOLD - N*M
WORK - M
FINISH - N
*/
void initialize();
void generateRequest();
int processRequest();
int isSafe();
void printStatistics();



int main(int argc,char *argv[])
{
	initialize();
	//printStatistics();
	do{
		if(allRequestsSatisfied)
		{
			generateRequest();
			allRequestsSatisfied = 0;
		}
		status = processRequest();	
	}while(status != UNSAFE_STATE);
	printStatistics();
}

void initialize()
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
	generateRequest();
}

void generateRequest()
{
	//Set the request vector and need vectors. The request vector will
	//not change throught the course of the program.
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

int processRequest()
{	
	//Attempt to statisfy the resource request made by a particular process. If no 
	//such safe state exist, we terminate the algorithm.
	int reqAttempted = 0;
	for(i=0;i<noProcess;i++)
	{
		for(j=0;j<noResources;j++)
		{
			if(needVector[i][j] <= availabilityVector[j] && needVector[i][j] != 0)
			{
				int oldAvail = availabilityVector[j];
				int oldHold = holdVector[i][j];
				int oldNeed = needVector[i][j];
				availabilityVector[j] -= needVector[i][j];
				holdVector[i][j] += needVector[i][j];
				needVector[i][j] -= needVector[i][j];
				if(isSafe(i,j) == SAFE_STATE)
				{
					//resourceGranted = 1;
					availabilityVector[j] += holdVector[i][j];
					holdVector[i][j] = 0;
					needVector[i][j] = 0;
					return SAFE_STATE; //We have fulfilled this request so now we start over.
				}
				else
				{
					//Just ignore this request and process the other ones.
					reqAttempted = 1;
					availabilityVector[j] = oldAvail;
					holdVector[i][j] = oldHold;
					needVector[i][j] = oldNeed;
				}
			}
		}
	}
	
	if(reqAttempted)
	{	//This is the case where there are still pending requests that cannot
		//be fullfilled.
		return UNSAFE_STATE;
	}else{
		//All of the requests of the proceses have been satisfied so we terminate.
		allRequestsSatisfied = 1;
		return SAFE_STATE;
	}
}

int isSafe()
{
	
	for(j=0;j<noResources;j++)
	{
		workVector[j] = availabilityVector[j];
	}
	for(i=0;i<noProcess;i++)
	{
		finishVector[i] = 0;
	}
	int processFinish = 0;
	for(i=0;i<noResources;i++)
	{
		for(j=0;j<noProcess;j++)
		{
			if(needVector[i][j] <= workVector[j] && finishVector[i] == 0)
			{
				finishVector[0] = 1;
				workVector[j] += holdVector[i][j];
				processFinish++;
			}
		}
	}
	if(processFinish == noProcess)
	{
		return SAFE_STATE;
	}
	else
	{
		return UNSAFE_STATE;
	}
}


void printStatistics()
{
	printf("Hold Matrix\n");
	for(i=0;i<noProcess;i++)
	{
		for(j=0;j<noResources;j++)
		{
			printf("%d\t",holdVector[i][j]);
		}
		printf("\n");
	}
	
	printf("\n");
	printf("Need Matrix\n");
	for(i=0;i<noProcess;i++)
	{
		for(j=0;j<noResources;j++)
		{
			printf("%d\t",needVector[i][j]);
		}
		printf("\n");
	}
}