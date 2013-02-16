#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

//Count variabes and flags
static int noItemsToProduce = 10; //Number of items produced by a single thread.
static int totalItemsToProduce = 100; //Number of items produced by 10 threads.
static int noItemsConsumed = 0; //Number of items removed by consumer.
static int done = 0; //Flag used to know when to terminate.
static int queueEmpty = 0;
void *res; //Stores the result from the thread.

//Queue Variables
int *queueArray;
int front = 0;
int back = -1;

void enqueue(int item)
{
	if(back < totalItemsToProduce-1)
	{
		int index = ++back;
		queueArray[index] = item;
	}
}

void dequeue()
{
	if(front<=back)
	{
		front++;
		noItemsConsumed++;
	}
	else
	{
		queueEmpty = 1;
		front=0;
		back=-1;
	}
}

static void *producer(void *arg)
{
	int i;
	int id = strtol(arg,NULL,0);
//	printf("The total items to produce = %d.\n",noItemsToProduce);
	for(i=0;i<noItemsToProduce;i++)
	{
		enqueue(id);
		//printf("I'm in thread %d enqueuing.\n",id);
		usleep(100);
	}
}

static void *consumer(void *arg)
{
	while(!done || !queueEmpty)
	{
	//	printf("Dequeuing item with id = %d at pos = %d.\n",queueArray[front],front);
		dequeue();
		//usleep(10000);
	}
}

int main(int argc,char *argv[])
{
	//The user needs to specify the number of ints each of the 10
	//producers will produce.
	if (argc!=2)
	{
		printf("Usage: %s #-items-for-each-producer\n",argv[0]);
		return -1;
	}
	else
	{
		noItemsToProduce = strtol(argv[1],NULL,0);
		totalItemsToProduce = 10*noItemsToProduce;
		queueArray = (int *)malloc(sizeof(int)*totalItemsToProduce);
	}
	
	//Declarations.
	pthread_t p1;
	pthread_t p2;
	pthread_t p3;
	pthread_t p4;
	pthread_t p5;
	pthread_t p6;
	pthread_t p7;
	pthread_t p8;
	pthread_t p9;
	pthread_t p10;
	pthread_t c;
	
	//Start producer and consumer threads.
	pthread_create(&p1,NULL,producer,"1");
	pthread_create(&p2,NULL,producer,"2");
	pthread_create(&p3,NULL,producer,"3");
	pthread_create(&p4,NULL,producer,"4");
	pthread_create(&p5,NULL,producer,"5");
	pthread_create(&p6,NULL,producer,"6");
	pthread_create(&p7,NULL,producer,"7");
	pthread_create(&p8,NULL,producer,"8");
	pthread_create(&p9,NULL,producer,"9");
	pthread_create(&p10,NULL,producer,"10");
	pthread_create(&c, NULL, consumer,"");
	
	//Wait until all of the producers finish producing.
	pthread_join(p1,&res);
	pthread_join(p2,&res); 
	pthread_join(p3,&res); 
	pthread_join(p4,&res); 
	pthread_join(p5,&res); 
	pthread_join(p6,&res); 
	pthread_join(p7,&res); 
	pthread_join(p8,&res); 
	pthread_join(p9,&res); 
	pthread_join(p10,&res); 
	
	//We're done producing so let the consumer know.
	done = 1;
	pthread_join(c,&res); 	
	printf("Total items produced = %d.\n",10*noItemsToProduce);
	printf("Total items consumed = %d.\n",noItemsConsumed);
	
	return 0;
}