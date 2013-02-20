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
	int iteration = 1;
	initialize();//Just sets the number of processes and resources.
	do{
		if(allRequestsSatisfied)
		{
			if(iteration > 1)
			{
				printf("Solved Problem. Will generate new one.\n");
				printStatistics();
				printf("\n\n");
				iteration = 0;
			}
			generateRandomRequest();//If the request are satisfied, we generate a new random request.
			//generatePassingExample();	//For testing purposes. Comment out the other two.
			//generateFailingExample();	//For testing purposes. Comment out the other two.
			allRequestsSatisfied = 0;
		}
		status = processRequest();
		printf("Iteration %d. Results.\n",iteration);
		printStatistics();
		printf("\n\n");
		iteration++;
	}while(status != UNSAFE_STATE);
	printf("Unsolved Problem. Program will terminate.\n");
}

void initialize()
{
	//Number of processes and resources.
	noProcess = (rand() % 4) ;//+ 5; //noProc in the range 5-10
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
			reqVector[noP][noR] = (needVector[noP][noR]%2)+1;
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
		availabilityVector[noR] = 0.7*sumOfMaxResources;
	}
	printf("Original Problem\n");
	printStatistics();
	printf("\n\n");
}

int processRequest()
{	
	//Attempt to statisfy the resource request made by a particular process. If no 
	//such safe state exist, we terminate the algorithm.
	int reqAttempted = 0;
	int i,j;
	for(i=0;i<noProcess;i++)
	{
		//This is a provisional allocation. If allocating resources in this manner
		//will result in an unsafe state, we have to revert back. Thus, I'm storing
		//the current value in temporary variables.
		if(reqVector[i][0] <= availabilityVector[0]
				&&reqVector[i][1] <= availabilityVector[1]
					&&reqVector[i][2] <= availabilityVector[2]
						&&reqVector[i][3] <= availabilityVector[3]){
			
			//Resource 1
			int oldAvail1 = availabilityVector[0];
			int oldHold1 = holdVector[i][0];
			int oldNeed1 = needVector[i][0];
			availabilityVector[0] -= reqVector[i][0];
			holdVector[i][0] += reqVector[i][0];
			needVector[i][0] -= (reqVector[i][0]>needVector[i][0])?0:reqVector[i][0];
			//reqVector[i][0] = needVector[i][0] - holdVector[i][0];
			
			//Resource 2
			int oldAvail2 = availabilityVector[1];
			int oldHold2 = holdVector[i][1];
			int oldNeed2 = needVector[i][1];
			availabilityVector[1] -= reqVector[i][1];
			holdVector[i][1] += reqVector[i][1];
			needVector[i][1] -= (reqVector[i][1]>needVector[i][1])?0:reqVector[i][1];
		//	reqVector[i][1] = needVector[i][1] - holdVector[i][1];

			
			//Resource 3
			int oldAvail3 = availabilityVector[2];
			int oldHold3 = holdVector[i][2];
			int oldNeed3 = needVector[i][2];
			availabilityVector[2] -= reqVector[i][2];
			holdVector[i][2] += reqVector[i][2];
			needVector[i][2] -= (reqVector[i][2]>needVector[i][2])?0:reqVector[i][2];
			//reqVector[i][2] = needVector[i][2] - holdVector[i][2];

			
			//Resource 4
			int oldAvail4 = availabilityVector[3];
			int oldHold4 = holdVector[i][3];
			int oldNeed4 = needVector[i][3];
			availabilityVector[3] -= reqVector[i][3];
			holdVector[i][3] += reqVector[i][3];
			needVector[i][3] -= (reqVector[i][3]>needVector[i][3])?0:reqVector[i][3];
			//reqVector[i][3] = needVector[i][3] - holdVector[i][3];


			if(isSafe() == SAFE_STATE)
				{

					if(needVector[i][0] == 0 && needVector[i][0] == 0 
							&& needVector[i][0] == 0 && needVector[i][0] == 0)
					{
						//Resource 1
						availabilityVector[0] = holdVector[i][0];
						holdVector[i][0] = 0;
						needVector[i][0] = 0;
						reqVector[i][0] = 0;
						
						//Resource 2
						availabilityVector[1] = holdVector[i][1];
						holdVector[i][1] = 0;
						needVector[i][1] = 0;
						reqVector[i][1] = 0;
						
						//Resource 3
						availabilityVector[2] = holdVector[i][2];
						holdVector[i][2] = 0;
						needVector[i][2] = 0;
						reqVector[i][2] = 0;
						
						//Resource 4
						availabilityVector[3] = holdVector[i][3];
						holdVector[i][3] = 0;
						needVector[i][3] = 0;
						reqVector[i][3] = 0;
					}
					
					return SAFE_STATE; //We have fulfilled this request so now we start over.
				}
				else
				{
					//Just ignore this request and process the other ones. Reset the value first.
					reqAttempted = 1;
					//Resource 1
					availabilityVector[0] = oldAvail1;
					holdVector[i][0] = oldHold1;
					needVector[i][0] = oldNeed1;
					
					//Resource 2
					availabilityVector[1] = oldAvail2;
					holdVector[i][1] = oldHold2;
					needVector[i][1] = oldNeed2;
					
					//Resource 3
					availabilityVector[2] = oldAvail3;
					holdVector[i][2] = oldHold3;
					needVector[i][2] = oldNeed3;
					
					//Resource 4
					availabilityVector[3] = oldAvail4;
					holdVector[i][3] = oldHold4;
					needVector[i][3] = oldNeed4;
				}	
		}
		else
		{
			continue;
		}
	}		
	if(reqAttempted)
	{	//This is the case where there are still pending requests that cannot
		//be fullfilled.
		return UNSAFE_STATE;
	}
	else
	{
		//All of the requests of the proceses have been satisfied so we terminate.
		allRequestsSatisfied = 1;
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
	for(i=0;i<noProcess;i++)
	{
		if(needVector[i][0] <= workVector[0] && needVector[i][1] <= workVector[1]
					&& needVector[i][2] <= workVector[2] && needVector[i][3] <= workVector[3]
						&& finishVector[i] == 0)
		{
				finishVector[i] = 1;
				processFinish++;
				
				//Resource 1
				workVector[0] = workVector[0] +  holdVector[i][0];

				//Resource 2
				workVector[1] = workVector[1] +  holdVector[i][1];
				
				//Resource 3
				workVector[2] = workVector[2] +  holdVector[i][2];
				
				//Resource 4
				workVector[3] = workVector[3] +  holdVector[i][3];
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
	reqVector[0][0] = 5;
	reqVector[1][0] = 4;
	reqVector[2][0] = 2;
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
	reqVector[0][0] = 5;
	reqVector[1][0] = 5;
	reqVector[2][0] = 2;
	holdVector[0][0] = 2;
	holdVector[1][0] = 2;
	holdVector[2][0] = 1;
	needVector[0][0] = 3;
	needVector[1][0] = 3;
	needVector[1][0] = 1;
	availabilityVector[0] = 2;
	workVector[0] = 2;
}