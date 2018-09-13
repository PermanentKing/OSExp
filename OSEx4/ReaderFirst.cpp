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
int read_count = 0;// 记录读者的数量
sem_t *writer, *mutex;// 临界区对象writer和mutex分别用于阻塞读写操作和改变读者数量

struct command
{
    int pid;// 线程号
    char type;// 线程角色（R：读者；W：写者）
    int startTime;// 操作开始的时间
    int lastTime;// 操作的持续时间
};


void write() {
    int rd = rand() % MAX_RAND;
    cout << "Write data " << rd << endl;
    data = rd;
}

void read() {
    cout << "Read data " << data << endl;
}


void *Writer(void *param) {
    struct command* c = (struct command*)param;
    while (true) {
        sleep(c->startTime);
        cout << "Writer pthread " << c->pid << " requests to write." << endl;
        sem_wait(writer);

        cout << "Writer pthread " << c->pid << " begins to write." << endl;
        write();

        sleep(c->lastTime);
        cout << "Writer pthread " << c->pid << " stops writing." << endl;
        sem_post(writer);

        pthread_exit(0);
    }
}


void *reader(void *param) {
    struct command* c = (struct command*)param;
    while (true) {
        sleep(c->startTime);
        cout << "Reader pthread " << c->pid << " requests to read." << endl;
        sem_wait(mutex);

        read_count++;
        if (read_count == 1) {
            sem_wait(writer);
        }
        sem_post(mutex);

        cout << "Reader pthread " << c->pid << " begins to read." << endl;
        read();

        sleep(c->lastTime);
        cout << "Reader pthread " << c->pid << " stops reading." << endl;
        sem_wait(mutex);

        read_count--;
        if (read_count == 0) {
            sem_post(writer);
        }
        sem_post(mutex);

        pthread_exit(0);
    }
}




int main(int argc, char const *argv[])
{
	int number_person = atoi(argv[1]);
	
	sem_unlink("writer");
	sem_unlink("mutex");

	writer = sem_open("writer", O_CREAT, 0, 1);
	mutex = sem_open("mutex", O_CREAT, 0, 1);

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
        cout << "Create a reader pthread, it's the " << information[i].pid << " pthread." << endl;
        pthread_create(&pid[i], NULL, reader, &information[i]);
    }

    if (information[i].type == 'W') {
        pthread_create(&pid[i], NULL, Writer, &information[i]);
        cout << "Create a writer pthread, it's the " << information[i].pid << " pthread." << endl;
    }
	}

	for (int i = 0; i < number_person; i++) {
    	pthread_join(pid[i], NULL);
	}


	sem_destroy(writer);
	sem_destroy(mutex);


	
	return 0;
}