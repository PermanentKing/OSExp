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

#define MAX_RAND 1000


int data = 0;
int read_count = 0, write_count = 0;// 记录读者的数量和写者的数量
// writeAccess:对全局变量write_count实现互斥
// readAccess：对全局变量read_count实现互斥
// mutexR：对阻塞read这一过程实现互斥
// mutexW：当有写者在写文件或者等待时，读者阻塞在mutexW上
sem_t writeAccess, readAccess, mutexR, mutexW;

struct command
{
    int pid;// 线程号
    char type;// 线程角色（R：读者；W：写者）
    int startTime;// 操作开始的时间
    int lastTime;// 操作的持续时间
};


// 写操作函数
void write() {
    int rd = rand() % MAX_RAND;
    cout << "Write data " << rd << "." << endl;
    data = rd;
}

// 读操作函数
void read() {
    cout << "Read data " << data << "." << endl;
}


void *writer(void *param) {
    struct command* c = (struct command*)param;
    while (true) {
        sleep(c->startTime);
        cout << "Writer pthread " << c->pid << " requests to write." << endl;
        sem_wait(&writeAccess);

        write_count++;
        if (write_count == 1) 
            sem_wait(&mutexR);
        sem_post(&writeAccess);

        sem_wait(&mutexW);
        cout << "Writer pthread " << c->pid << " begins to write." << endl;
        write();

        sleep(c->lastTime);
        cout << "Writer pthread " << c->pid << " stops writing." << endl;
        sem_post(&mutexW);

        sem_wait(&writeAccess);
        write_count--;
        if (write_count == 0) 
            sem_post(&mutexR);
        sem_post(&writeAccess);

        pthread_exit(0);
    }
}



void *reader(void *param) {
    struct command* c = (struct command*)param;
    while (true) {
        sleep(c->startTime);
        cout << "Reader pthread " << c->pid << " requests to read." << endl;
        sem_wait(&mutexR);
        sem_wait(&readAccess);

        read_count++;
        if (read_count == 1) 
            sem_wait(&mutexW);
        sem_post(&readAccess);
        sem_post(&mutexR);

        cout << "Reader pthread " << c->pid << " begins to read " << endl;
        read();

        sleep(c->lastTime);
        cout << "Reader pthread " << c->pid << " stops reading." << endl;

        sem_wait(&readAccess);
        read_count--;
        if (read_count == 0) 
            sem_post(&mutexW);
        sem_post(&readAccess);

        pthread_exit(0);
    }
}


int main(int argc, char const *argv[])
{
	int number_person = atoi(argv[1]);

	sem_unlink("writeAccess");
	sem_unlink("readAccess");
	sem_unlink("mutexR");
	sem_unlink("mutexW");

	sem_open("writeAccess", O_CREAT, 0, 1);
	sem_open("readAccess", O_CREAT, 0, 1);
	sem_open("mutexR", O_CREAT, 0, 1);
	sem_open("mutexW", O_CREAT, 0, 1);

	struct command information[number_person];
	pthread_t pid[number_person];


	fstream in_stream("test2.txt", ios::in|ios::out);
	string temp;
	int counter = 0;

	while(!in_stream.eof()){
		getline(in_stream, temp);
		information[counter].pid = atoi(&temp[0]);
		information[counter].type = temp[2];
		information[counter].startTime = atoi(&temp[4]);
		information[counter].lastTime = atoi(&temp[6]);
		counter++;
	}


	for (int i = 0; i < number_person; i++) {
	    if (information[i].type == 'R') {
	        cout << "Create a reader pthread-No." << information[i].pid << " pthread." << endl;
	        pthread_create(&pid[i], NULL, reader, &information[i]);
	    }

	    if (information[i].type == 'W') {
	        pthread_create(&pid[i], NULL, writer, &information[i]);
	        cout << "Create a writer pthread-No." << information[i].pid << " pthread." << endl;
	    }
	}

	for (int i = 0; i < number_person; i++) {
	    pthread_join(pid[i], NULL);
	}

	sem_destroy(&writeAccess);
	sem_destroy(&readAccess);
	sem_destroy(&mutexW);
	sem_destroy(&mutexR);

	return 0;
}










