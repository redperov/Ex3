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
#include <memory.h>

//TODO minimize size
#define SHM_SIZE 4096

/**
 * function name: ReadPid.
 * The input: pid, file descriptor.
 * The output: void.
 * The function operation: Reads a pid from a file.
*/
void ReadPid(pid_t *pid, int file);

/**
 * function name: NotifyGameStart.
 * The input: pid.
 * The output: void.
 * The function operation: Notifies a process to play.
*/
void NotifyGameStart(pid_t pid);

/**
 * function name: ReleaseMemory.
 * The input: shmid.
 * The output: void.
 * The function operation: releases shared memory.
*/
void ReleaseMemory(int shmid);

/**
 * function name: WriteMessage.
 * The input: message to print.
 * The output: void.
 * The function operation: prints the message to the screen.
*/
void WriteMessage(char *message);

/**
 * function name: PrintResult.
 * The input: game result.
 * The output: void.
 * The function operation: prints the game result message.
*/
void PrintResult(char result);

int main(){

    //Variable declarations.
    key_t key;
    int shmid;
    char *data;
    int file;
    pid_t pid1;
    pid_t pid2;
    int unlinkResult;
    int stop;

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
    file = open("fifo_clientTOserver", O_RDWR);

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

    //Notify first process.
    NotifyGameStart(pid1);
    stop = 0;

    while(!stop){

        //Check if first process wrote to memory.
        if(strcmp(data, "") != 0){

            stop = 1;
        }
        else{

            sleep(1);
        }
    }

    //Notify second process.
    NotifyGameStart(pid2);

    stop = 0;

    while(!stop){

        //Check if game is over
        if(data[0] == 'B' || data[0] == 'W' || data[0] == 'T'){
            stop = 1;
        }
        else{
            sleep(1);
        }
    }

    WriteMessage("GAME OVER\n");

    //Print game result message.
    PrintResult(data[0]);

    //Release shared memory.
    ReleaseMemory(shmid);
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

        perror("Error: memory release failed.\n");
        exit(1);
    }
}

void WriteMessage(char *message) {

    int writeResult;

    writeResult = write(1, message, strlen(message));

    //Check if write succeeded.
    if (writeResult < 0) {

        perror("Error: write failed.\n");
        exit(1);
    }
}

void PrintResult(char result){

    switch(result){

        case 'B':
            WriteMessage("Winning player: Black\n");
            break;

        case 'W':
            WriteMessage("Winning player: White\n");
            break;

        case 'T':
            WriteMessage("No winning player\n");
            break;

        default:
            break;
    }
}