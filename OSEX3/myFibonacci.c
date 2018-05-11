#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#define MAX_SIZE 10

int *fibo;
void *runner(void *param);

int main(int argc, char const *argv[])
{
	pthread_t tid;
	pthread_attr_t attr;

	fibo = (int*)malloc(sizeof(int) * MAX_SIZE);
	fibo[0] = 0;
	fibo[1] = 1;

	if (argc!=2)
	{
		fprintf(stderr, "usage: a.out <integer value>\n");
		return -1;
	}
	if (atoi(argv[1]) < 0)
	{
		fprintf(stderr, "%d must be >= 0\n", atoi(argv[1]));
		return -1;
	}

	pthread_attr_init(&attr);

	pthread_create(&tid, &attr, runner, argv[1]);

	pthread_join(tid, NULL);

	printf("the fibonacci is \n");

	for (int i = 0; i < atoi(argv[1]); i++)
	{
		if (i!=0)
		{
			printf(", ");
		}
		printf("%d", fibo[i]);
		
	}
	printf("\n");

	return 0;
}


void *runner(void *param)
{
	int i, upper = atoi(param);

	for (int i = 2; i <= upper; i++)
	{
		fibo[i] = fibo[i-2] + fibo[i-1];
	}

	pthread_exit(0);
}