/***********************************************************/
/*Kishan Patel											   */
/*260376121       										   */
/*Mini Assingment 3					                       */
/***********************************************************/
#include <stdio.h>
#include <stdlib.h>

int noProcess;
int noResources;
int i;
int *resourceVector;
int *available;
int **max;
int **need;
int **allocated;

initialize()
{
	//Number of processes and resources.
	noProcess = (rand() % 6) + 5; //noProc in the range 5-10
	noResources = 4;
	
	//Initialize to appropriate size.
	resourceVector = (int *)malloc(sizeof(int)*noResources);
	available = (int *)malloc(sizeof(int)*noResources);
	max = (int **)malloc(sizeof(int *)*noResources);
	need = (int **)malloc(sizeof(int *)*noResources);
	allocated = (int **)malloc(sizeof(int *)*noResources);
	for(i=0;i<noProcess;i++)
	{
		max[i] = (int *)malloc(sizeof(int)*noProcess);
		need[i] = (int *)malloc(sizeof(int)*noProcess);
		allocated[i] = (int *)malloc(sizeof(int)*noProcess);
	}
	
	//Initialse the arrays to the appropriate values.
	int noP = 0, noR = 0;
	int sumOfMaxResources = 0;
	for(noP=0;noP<noProcess;noP++)
	{
		for(noR=0;noR<noResources;noR++)
		{
			max[noP][noR] = (rand() % 8) + 5;
			need[noP][noR] = max[noP][noR];
		}
	}
	
	for(noR=0;noR<noResources;noR++)
	{
		sumOfMaxResources = 0;
		for(noP=0;noP<noProcess;noP++)
		{
			sumOfMaxResources += max[noP][noR];
		}
		resourceVector[noR] = 0.6*sumOfMaxResources;
		available[noR] = resourceVector[noR];
	}
}

void generateRequest()
{
}

void verifyRequest()
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