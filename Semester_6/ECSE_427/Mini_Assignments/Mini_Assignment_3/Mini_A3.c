/***********************************************************/
/*Kishan Patel											  										 */
/*260376121       										   									 */
/*Mini Assingment 3					                       				 */
/***********************************************************/
#include <stdio.h>
#include <stdlib.h>

//Variable declarations.
#define SAFE_STATE		1
#define UNSAFE_STATE 	2
#define MAX_PROCESS 	12
#define MAX_RESOURCE	4
int noProcess;
int noResources;
int availabilityVector[MAX_RESOURCE];
int maxVector[MAX_PROCESS][MAX_RESOURCE];
int needVector[MAX_PROCESS][MAX_RESOURCE];
int holdVector[MAX_PROCESS][MAX_RESOURCE];
int reqVector[MAX_PROCESS][MAX_RESOURCE];
int workVector[MAX_RESOURCE];
int finishVector[MAX_PROCESS];
int allRequestsSatisfied = 1;
int status;
int terminated = 0;

//Function prototypes.
void initialize();
void generateRandomRequest();
int processRequest();
int isSafe();
void printStatistics();
void generatePassingExample();
void generateFailingExample();

int main(int argc,char *argv[])
{
	initialize();//Just sets the number of processes and resources.
	do{
		if(allRequestsSatisfied)
		{
			
			generateRandomRequest();//If the request are satisfied, we generate a new random request.
			//generatePassingExample();	//For testing purposes. Comment out the other two.
			//generateFailingExample();	//For testing purposes. Comment out the other two.
			allRequestsSatisfied = 0;
			printf("Requests created. Printing initial state.\n");
			printStatistics();
			printf("\n\n");
		}
		status = processRequest();
	}while(status != UNSAFE_STATE);
	printStatistics();
}

void initialize()
{
	//Number of processes and resources.
	noProcess = (rand() % 4) + 5; //noProc in the range 5-10
	noResources = 4;
}

void generateRandomRequest()
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
			//Hold is taken to be 0 and the need for 
			//a given process is a random number between 5-12.
			maxVector[noP][noR] = (rand() % 8) + 5;
			needVector[noP][noR] = maxVector[noP][noR];
			if (!terminated)
				reqVector[noP][noR] = rand() % needVector[noP][noR]+1;
			else
				reqVector[noP][noR] = rand() % needVector[noP][noR]+2;
		}
	}

	for(noR=0;noR<noResources;noR++)
	{
		sumOfMaxResources = 0;
		for(noP=0;noP<noProcess;noP++)
		{
			sumOfMaxResources += needVector[noP][noR];
		}
		//The availability of any given resource is 0.6*the total need for that
		//resources for all the processes participating.
		availabilityVector[noR] = 0.6*sumOfMaxResources;
	}
}

int processRequest()
{	
	//Attempt to statisfy the resource request made by a particular process. If no 
	//such safe state exist, we terminate the algorithm.
	int stateChanged = 0;
	int i,j,r,done;
	for(i=0;i<noProcess;i++)
	{
		for(j=0;j<noResources;j++)
		{
			if(reqVector[i][j] > maxVector[i][j])
			{
				continue;
			}
			else if(reqVector[i][j] <= availabilityVector[j] && reqVector[i][j]!=0)
			{
				int oldAvail = availabilityVector[j];
				int oldHold = holdVector[i][j];
				int oldNeed = needVector[i][j];
				int oldReq = reqVector[i][j];
				availabilityVector[j] -= reqVector[i][j];
				holdVector[i][j] += reqVector[i][j];
				needVector[i][j] -= (reqVector[i][j]>needVector[i][j])?0:reqVector[i][j];
				reqVector[i][j] = (reqVector[i][j]>needVector[i][j])?needVector[i][j]:reqVector[i][j];
			
				if(isSafe() == SAFE_STATE)
				{
					printf("State changed. Printing updated state\n");
					printStatistics();
					printf("\n\n");
					stateChanged = 1;
					for(r=0,done=1;r<noResources&&done;r++)
					{
						if(needVector[i][j]!=0)
						{	
							done=0;
							break;
						}
					}
					if(done)
					{
						//Release resources
						for(r=0;r<noResources;r++)
						{
							availabilityVector[r] += holdVector[i][r];
							holdVector[i][r] = 0;
							needVector[i][r] = 0;
							reqVector[i][r] = 0;
						}
						printf("Resouce released. Pringin updated state.\n");
						printStatistics();
						printf("\n\n");
					}
					break;
				}
				else
				{
					//Reset the values, this assignment will not produce a safe state.
					availabilityVector[j] = oldAvail;
					holdVector[i][j] = oldHold;
					needVector[i][j] = oldNeed;
					reqVector[i][j] = oldReq;
				}
			}
			else
			{
				continue;
			}
		}
	}		
	if(!stateChanged)
	{	//This is the case where there are still pending requests that cannot
		//be fullfilled.
		return UNSAFE_STATE;
	}
	else
	{
		//All of the requests of the proceses have been satisfied so we terminate.
		//allRequestsSatisfied = 1;
		done=1;
		for(i=0;i<noProcess&&done;i++)
		{
			for(j=0;j<noResources&&done;j++)
			{
				if(needVector[i][j] != 0)
				{
					done = 0;
					break;
				}
			}
		}
		if(done)
		{
			allRequestsSatisfied = 1;
			terminated = 1;
		}
		return SAFE_STATE;
	}
	
}

int isSafe()
{
	int i,j;
	int processFinish = 0;
	for(j=0;j<noResources;j++)
	{
		workVector[j] = availabilityVector[j];
	}
	for(i=0;i<noProcess;i++)
	{
		finishVector[i] = 0;
	}
	
	findProcess:
		for(i=0;i<noProcess;i++)
		{
			for(j=0;j<noResources;j++)
			{
				if(reqVector[i][j]<=workVector[j] && finishVector[i]!=1)
				{
					finishVector[i] = 1;
					processFinish++;
					workVector[j] += workVector[j] + holdVector[i][j];
					goto findProcess;
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
	int i,j;
	printf("Orignal Request Matrix\n");
	for(i=0;i<noProcess;i++)
	{
		for(j=0;j<noResources;j++)
		{
			printf("%d\t",maxVector[i][j]);
		}
		printf("\n");
	}
	
	printf("\nHold Matrix\n");
	for(i=0;i<noProcess;i++)
	{
		for(j=0;j<noResources;j++)
		{
			printf("%d\t",holdVector[i][j]);
		}
		printf("\n");
	}
	
	printf("\nNeed Matrix\n");
	for(i=0;i<noProcess;i++)
	{
		for(j=0;j<noResources;j++)
		{
			printf("%d\t",needVector[i][j]);
		}
		printf("\n");
	}
	
	printf("\nAvailability Matrix\n");
	for(i=0;i<noResources;i++)
	{
		printf("%d\n",availabilityVector[i]);
	}
}

void generatePassingExample()
{
	//A passing example from the class notes.
	maxVector[0][0] = 5;
	maxVector[1][0] = 4;
	maxVector[2][0] = 2;
	reqVector[0][0] = 2;
	reqVector[1][0] = 3;
	reqVector[2][0] = 1;
	holdVector[0][0] = 3;
	holdVector[1][0] = 1;
	holdVector[2][0] = 1;
	needVector[0][0] = 2;
	needVector[1][0] = 3;
	needVector[2][0] = 1;
	availabilityVector[0] = 2;
	workVector[0] = 2;
}

void generateFailingExample()
{
	//A failing example from the class notes.
	maxVector[0][0] = 5;
	maxVector[1][0] = 5;
	maxVector[2][0] = 2;
	holdVector[0][0] = 2;
	holdVector[1][0] = 2;
	holdVector[2][0] = 1;
	needVector[0][0] = 3;
	needVector[1][0] = 3;
	needVector[2][0] = 1;
	reqVector[0][0] = 3;
	reqVector[1][0] = 3;
	reqVector[2][0] = 1;
	availabilityVector[0] = 1;
	workVector[0] = 1;
}