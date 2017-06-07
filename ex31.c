/******************************************
* Student name: TODO add
* Student ID: TODO add
* Course Exercise Group: 05
* Exercise name: ex3
******************************************/
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#define SHM_SIZE 4096

void ReadPid(pid_t *pid, int file);

void NotifyGameStart(pid_t pid);

void ReleaseMemory(int shmid);

int main(){

    //Variable declarations.
    key_t key;
    int shmid;
    char *data;
    int file;
    pid_t pid1;
    pid_t pid2;
    int unlinkResult;

    //Create key.
    //TODO remove ../
    key = ftok("../ex31.c", 'k');

    //Check if ftok succeeded.
    if(key < 0){

        perror("Error: ftok failed.\n");
        exit(1);
    }

    //Create shared memory.
    shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);

    //Check if shmget succeeded.
    if(shmid < 0){

        perror("Error: shmget failed.\n");
        exit(1);
    }

    //Attach to shared memory.
    data = shmat(shmid, NULL, 0);

    //Check if shmat succeeded.
    if(data == (char*)(-1)){

        perror("Error: shmat failed.\n");
        exit(1);
    }

    //Create FIFO.
    file = mkfifo("fifo_clientTOserver", 0666);

    //Check if mkfifo succeeded.
    if(file < 0){

        perror("Error: mkfifo failed.\n");
    }

    //Open FIFO.
    file = open("fifo_clientTOserver", O_RDONLY);

    //Check if open succeeded.
    if(file < 0){

        perror("Error: open failed.\n");
    }

    //Read pids
    ReadPid(&pid1, file);
    ReadPid(&pid2, file);

    //Remove FIFO.
    unlinkResult = unlink("fifo_clientTOserver");

    //Check if unlink succeeded.
    if(unlinkResult < 0){

        perror("Error: unlink failed.\n");
        exit(1);
    }

    NotifyGameStart(pid1);

    //TODO just uncomment, ReleaseMemory(shmid);
}

void ReadPid(pid_t *pid, int file){

    int readResult;

    //Read pid.
    readResult = read(file, pid, sizeof(pid_t));

    //Check if read succeeded.
    if(readResult < 0){

        perror("Error: read failed.\n");
        exit(1);
    }
}

void NotifyGameStart(pid_t pid){

    int killResult;

    //Send signal.
    killResult = kill(pid, SIGUSR1);

    //Check kill succeeded.
    if(killResult < 0){

        perror("Error: kill failed.\n");
        exit(1);
    }
}

void ReleaseMemory(int shmid){

    int freeMemory;

    //Free shared memory.
    freeMemory = shmctl(shmid, IPC_RMID, NULL);

    //Check if shmctl succeeded.
    if(freeMemory < 0){

        perror("Error: free memory failed.\n");
        exit(1);
    }
}