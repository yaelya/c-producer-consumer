#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>

#include <unistd.h>
#define maxbuf 6   // max produce count.

pthread_mutex_t the_mutex;
sem_t  full, empty;
int buffer[maxbuf];
static int count = 0;
pthread_t pro[3], con[4];

void init()
{
	sem_init(&empty, 0, 6);
	sem_init(&full, 0, 0);
}


int create()
{
	int item = rand() % 1000001;
	//printf("create item %d\n", item);
	return item;
}


void add(int item)
{
	buffer[count] = item;
	printf("add item %d\n", item);
	count++;
}


int remove_item()
{
	int index = buffer[count];
	count--;
	return index;
}


void* producer()
{
	int i, item;
	for (i = 1; i <= 40; i++)
	{
		printf("      ---     \n");
		item = create();
		sem_wait(&empty);
		pthread_mutex_lock(&the_mutex); /* to protect the buffer */
		printf("produce:get lock\n");	
		add(item);

		pthread_mutex_unlock(&the_mutex);
		sem_post(&full);
	}
	pthread_exit(0);	
}


void* consumer()
{
	int j;
	for (j = 1; j <= 30; j++)
	{
		sem_wait(&full);
		pthread_mutex_lock(&the_mutex); /* to protect the buffer */
		printf("consume:get lock\n");
		
		remove_item();
		pthread_mutex_unlock(&the_mutex); 
		sem_post(&empty);
	}
	pthread_exit(0);
}


void send_fun()
{
	int i;
	for (i = 0; i < 3 ; ++i)
	{
     	printf("produce:wake-- \n") ;
		pthread_create(&pro[i], NULL, producer, NULL);
	}
     
	for (i = 0; i < 4 ; ++i)
	{
     	printf("consumer:wake-- \n") ;
		pthread_create(&con[i], NULL, consumer, NULL);
	}
}


void join_threads()
{
	int i;
	for (i = 0; i < 3; ++i)
	{
		pthread_join(pro[i], NULL);
	}
	
	for (i = 0; i < 4; ++i)
	{
		pthread_join(con[i], NULL);
	
	}
	
	pthread_mutex_destroy(&the_mutex);
	sem_destroy(&full);
	sem_destroy(&empty);
}


int main()
{
	printf("---  MAIN  ---\n");
	
	init();
	
	send_fun();
	join_threads();
	
	return 0;
}

