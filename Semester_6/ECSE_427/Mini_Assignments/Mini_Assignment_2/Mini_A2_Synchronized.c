/***********************************************************/
/*Kishan Patel																						 */
/*260376121       																				 */
/*Mini Assingment 2 - Without Synchronization							 */
/***********************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

//Count variabes and flags
static int noItemsToProduce = 10; //Number of items produced by a single thread.
static int totalItemsToProduce = 100; //Number of items produced by 10 threads.
static int noItemsConsumed = 0; //Number of items removed by consumer.
static int done = 0; //Flag used to know when to terminate.
static int queueEmpty = 0; //Flag used to tell when queue is empty.
void *res; //Stores the result from the thread.

//Queue Variables
static int *queueArray;
int front = 0;
int back = 0;

//Mutex and Conditions variables used for synchronization.
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void enqueue(int item)
{
	//Add to the queue. It's a infinte buffer so we don't care about overflow.
	pthread_mutex_lock(&mutex);
	queueArray[back] = item;
	back++;
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);

}

void dequeue()		
{
	if(front<back)
	{
		if(queueArray[front]>0)
		{
			//If an element in the array is 0 then a race condition occured while
			//the producer was writing it so we don't consider it.
			noItemsConsumed++;
		}
		
		//Incrementing the front of the queue means we have deleted the previous
		//item in the queue.
		front++;
	}
	else{
		//If front>=back we do nothing so it is equivalent to 
		//a busy wait which is inefficient. 
		//This will be fixed in the synchronization example.
		if(noItemsConsumed<totalItemsToProduce)
			pthread_cond_wait(&cond,&mutex2);
	}
}

static void *producer(void *arg)
{
	int i;
	int id = strtol(arg,NULL,0);
	for(i=0;i<noItemsToProduce;i++)
	{
		//Add an item to the queue. The item will be the id of the given 
		//thread.
		enqueue(id);
	}
}

static void *consumer(void *arg)
{
	while(!done || front<totalItemsToProduce)
	{
		//While all the producers haven't finished producing or if there
		//are still elements present in the queue, the consumer removes them.
		//pthread_cond_wait(&cond,&mutex);
		dequeue();
	}
}

int main(int argc,char *argv[])
{

	if (argc!=2)
	{
		//The user needs to specify the number of ints each of the 10
		//producers will produce.
		printf("Usage: %s #-items-for-each-producer\n",argv[0]);
		return -1;
	}
	else
	{
		//Calculate the total number of items to produce and initialize the 
		//size of the queue accordingly.
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

	//Print results
	printf("Total items produced = %d.\n",totalItemsToProduce);
	printf("Total items consumed = %d.\n",noItemsConsumed);
	
	return 0;
}
