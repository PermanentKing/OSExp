#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#define MAX_SEQUENCE 10

typedef struct
{
	int fib_sequence[MAX_SEQUENCE];
	int sequence_size;
} shared_data;


int main(int argc, char const *argv[])
{
	if (argc!=2)
	{
		printf("You should only input one arguement\n");
		exit(0);
	}
	if (atoi(argv[1]) <= 0 || atoi(argv[1]) > 10) 
	{
		printf("You should input a arguement between 1 and 10\n");
		exit(0);
	}
	/* the identifier for the shared memory segment */
	int segment_id;
	/* a pointer to the shared memory segment */
	shared_data *shared_memory;
	/* the size (in bytes) of the shared memory segment */ 
	const int segment_size = sizeof(shared_data);
	/** allocate  a shared memory segment */
	segment_id = shmget(IPC_PRIVATE, segment_size, S_IRUSR | S_IWUSR);
	/** attach the shared memory segment */
	shared_memory = (shared_data *) shmat(segment_id, NULL, 0);
	//printf("shared memory segment %d attached at address %p\n", segment_id, shared_memory);
	shared_memory->sequence_size = atoi(argv[1]);
	int pid = 0;
	pid = fork();
	if (pid < 0)
	{
		printf("Error!\n");
		exit(0);
	}else if (pid == 0)
	{
		shared_memory->fib_sequence[0] = 0;
		shared_memory->fib_sequence[1] = 1;
		for (int i = 2; i < shared_memory->sequence_size; i++)
		{
			shared_memory->fib_sequence[i] = shared_memory->fib_sequence[i-1] + shared_memory->fib_sequence[i-2];
		}
	}else
	{
		wait(0);
		printf("Here is the result:\n");
		for (int i = 0; i < shared_memory->sequence_size;i++)
		{
			if (i==0)
			{
				printf("%d",  shared_memory->fib_sequence[i]);
			}else
			{
				printf(", %d", shared_memory->fib_sequence[i]);
			}
			
		}printf("\n");
	}



	/** write a message to the shared memory segment   */
	//sprintf(shared_memory, "Hi there!");
	/** now print out the string from shared memory */
	//printf("*%s*\n", shared_memory);
	/** now detach the shared memory segment */ 
	if ( shmdt(shared_memory) == -1) {
		fprintf(stderr, "Unable to detach\n");
	}
	/** now remove the shared memory segment */
	shmctl(segment_id, IPC_RMID, NULL);
	return 0;
}



	