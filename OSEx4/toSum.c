#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

#define THREAD_NUMBER 10

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int sum = 0;

void* inc(void *arg)
{
	int i =(*(int*)arg);
	pthread_mutex_lock(&mutex);
	sum = sum + i;
	pthread_mutex_unlock(&mutex);
	return NULL;
}

int main(int argc, char *argv[])
{
	pthread_t pt[THREAD_NUMBER];
	int i;
	int arg[THREAD_NUMBER];
	for (int i = 0; i < THREAD_NUMBER; i++)
	{
		arg[i] = i;
		if (pthread_create(&pt[i], NULL, inc, (void*)&arg[i])!=0)
		{
			printf("pthread_create error\n");
			exit(1);
		}
	}
	for (int i = 0; i < THREAD_NUMBER; i++)
	{
		if (pthread_join(pt[i], NULL)!=0)
		{
			printf("pthread_join error\n");
			exit(1);
		}
	}
	printf("sum is %d\n", sum);
	pthread_mutex_destroy(&mutex);
	return 0;
}



