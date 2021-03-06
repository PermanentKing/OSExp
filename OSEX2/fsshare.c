#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define PERM S_IRUSR|S_IWUSR    //(见注1)

int main(int argc, char **argv)
{
    key_t shmid;
    char   *p_addr, *c_addr;
    pid_t pid;
    if(argc != 2) {
        fprintf(stderr, "Usage:%s\n\a", argv[0]);
        exit(1);
    }
    if( (shmid = shmget(IPC_PRIVATE, 1024, PERM)) == -1 )   { //(见注1)
        fprintf(stderr, "Create Share Memory Error:%s\n\a", strerror(errno));
        exit(1);
    }
    pid = fork();
    if(pid > 0) {
        p_addr = shmat(shmid, 0, 0);
        memset(p_addr, '\0', 1024);
        strncpy(p_addr, argv[1], 1024);
        wait(NULL);
        exit(0);
    }
    else if (pid == 0){
        sleep(1);
        c_addr = shmat(shmid, 0, 0);
        printf("Client get %s\n", c_addr);
        exit(0);
    }
}

