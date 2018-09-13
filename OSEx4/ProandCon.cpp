#include <iostream>
#include <string>
#include <fstream>
#include <semaphore.h>
#include <sys/types.h>
#include <pthread.h>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
using std::cin;
using std::cout;
using std::endl;
using std::fstream;
using std::ios;
using std::string;


#define BUFFER_SIZE 5
typedef int buffer_item;
//线程序号，P/C，操作的开始时间(延迟)，持续时间，生产的产品号(仅生产者有)


buffer_item buffer[BUFFER_SIZE];
sem_t * empty;
sem_t * full;
sem_t * mutex;
int nowProduce = 0, nowConsume = 0;

struct sto
{
	int pid;
	char role;
	int startTime;
	int lastTime;
	int num;
};




void *produce(void *arg)
{   
	struct sto *tempsto = (struct sto*)arg;

	while(true){
	sem_wait(empty);
	sleep(tempsto->startTime);
	sem_wait(mutex);

	buffer[nowProduce] = tempsto->num;
    cout << "Producer No." << tempsto->pid 
         << " produces " << "product No." << tempsto->num << endl;
    nowProduce = (nowProduce + 1) % BUFFER_SIZE;

	sleep(tempsto->lastTime);

	sem_post(mutex);
	sem_post(full);

	pthread_exit(0);
	}
}



void *consume(void *arg)
{
    struct sto *tempsto = (struct sto*)arg;

    while(true){
	sem_wait(full);
	sleep(tempsto->startTime);
	sem_wait(mutex);

    cout << "Comsumer No." << tempsto->pid 
         << " consumes " << "product No." << buffer[nowConsume] << endl;
    buffer[nowConsume] = 0;
	nowConsume = (nowConsume + 1) % BUFFER_SIZE;
	sleep(tempsto->lastTime);

	sem_post(mutex);
	sem_post(empty);

	pthread_exit(0);
	}
}


int main(int argc, char const *argv[])
{
	int total = atoi(argv[1]);

	//create an array to store all people
	struct sto allpeople[total];

	//create pthread
	pthread_t myPth[total];

	sem_unlink("mutex");
	sem_unlink("empty");
	sem_unlink("full");

	mutex = sem_open("mutex", O_CREAT, 0, 1);
	full = sem_open("full", O_CREAT, 0, 0);
	empty = sem_open("empty", O_CREAT, 0, BUFFER_SIZE);

	//initialize the buffer
	for (int i = 0; i < BUFFER_SIZE; i++)
	{
		buffer[i] = 0;
	}

	fstream in_stream("test.txt", ios::in|ios::out);
	string temp;
	int counter = 0;

	while(!in_stream.eof()){
		getline(in_stream, temp);
		if (temp[2]=='C'){
			allpeople[counter].pid = atoi(&temp[0]);
			allpeople[counter].role = temp[2];
			allpeople[counter].startTime = atoi(&temp[4]);
			allpeople[counter].lastTime = atoi(&temp[6]);
			allpeople[counter].num = 0;
		}else if(temp[2]=='P'){
			allpeople[counter].pid = atoi(&temp[0]);
			allpeople[counter].role = temp[2];
			allpeople[counter].startTime = atoi(&temp[4]);
			allpeople[counter].lastTime = atoi(&temp[6]);
			allpeople[counter].num = atoi(&temp[8]);
		}
		counter++;
	}

	for (int i = 0; i < total; i++)
	{
		if (allpeople[i].role=='P'){
			//cout << "create a producer : " << allpeople[i].pid << endl;
			pthread_create(&myPth[i], NULL, produce, &allpeople[i]);

		}else if (allpeople[i].role=='C'){
			//cout << "create a consumer : " << allpeople[i].pid << endl;
			pthread_create(&myPth[i], NULL, consume, &allpeople[i]);
		}
	}

	for (int i = 0; i < total; i++)
	{
		//cout << allpeople[i].num << endl;
		pthread_join(myPth[i], NULL);
	}


	sem_destroy(mutex);
   	sem_destroy(empty);
   	sem_destroy(full);
	return 0;
}

