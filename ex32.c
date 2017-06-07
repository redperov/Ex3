/******************************************
* Student name: TODO add
* Student ID: TODO add
* Course Exercise Group: 05
* Exercise name: ex3
******************************************/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/shm.h>
#include <string.h>

#define SHM_SIZE 4096
#define BOARD_SIZE 8

void WritePid(int file, pid_t pid);

void SIGUSR1Handler(int signum);

void InitializeBoard(int **board);

int ReadUserInput(int board[][BOARD_SIZE], int *x, int *y);

int IsValidCell(char input[], int *x, int *y);

int IsValidMove(int x, int y, int board[][BOARD_SIZE]);

void WriteMessage(char *message);

void WriteCell(int num);

void PrintBoard(int board[][BOARD_SIZE]);

void HandleResult(int result, int board[][BOARD_SIZE]);

int main() {

    //Variable declarations.
    key_t            key;
    int              shmid;
    pid_t            pid;
    int              file;
    char             *data;
    struct sigaction usr_action;
    sigset_t         block_mask;
    int              resultValue;
    int              board[BOARD_SIZE][BOARD_SIZE];
    int              x;
    int              y;
    int              stop;

    //Set block mask.
    resultValue = sigfillset(&block_mask);

    //Check if sigfillset worked.
    if (resultValue < 0) {

        perror("Error: sigfillset failed.\n");
        exit(1);
    }

    //Set signal handling
    usr_action.sa_handler = SIGUSR1Handler;
    usr_action.sa_mask    = block_mask;
    usr_action.sa_flags   = 0;

    //Set sigaction for SIGUSR1
    resultValue = sigaction(SIGUSR1, &usr_action, NULL);

    //Check if sigaction worked.
    if (resultValue < 0) {

        perror("Error: sigaction failed.\n");
    }

    //Set process pid.
    pid = getpid();

    //Open FIFO.
    file = open("fifo_clientTOserver", O_WRONLY);

    //Check if open succeeded.
    if (file < 0) {

        perror("Error: open failed.\n");
        exit(1);
    }

    //Write pid to FIFO.
    WritePid(file, pid);

    //Wait for signal from server.
    pause();

    //Create key.
    key = ftok("../ex31.c", 'k');

    //Check if ftok succeeded.
    if (key < 0) {

        perror("Error: ftok failed.\n");
        exit(1);
    }

    //Get shared memory.
    shmid = shmget(key, SHM_SIZE, 0666);

    //Check if shmget succeeded.
    if (shmid < 0) {

        perror("Error: shmget failed.\n");
        exit(1);
    }

    //Attach to shared memory.
    data = shmat(shmid, NULL, 0);

    //Check if shmat succeeded.
    if (data == (char *) -1) {

        perror("Error: shmat failed.\n");
        exit(1);
    }

    InitializeBoard(board);

    stop = 0;

    while (!stop) {

        WriteMessage("Please choose a square\n");

        resultValue = ReadUserInput(board, &x, &y);

        HandleResult(resultValue, board);
    }


}

void HandleResult(int result, int board[][BOARD_SIZE]) {

    switch (result) {

        //Legal move.
        case 0:
            WriteMessage("The board is:\n");
            PrintBoard(board);
            WriteMessage("Waiting for the other player to make a move\n");
            //TODO Write to memory
            break;

            //Illegal cell.
        case 1:

            WriteMessage("No such square\n");
            WriteMessage("Please choose another square\n");
            break;

            //Illegal move.
        case 2:
            WriteMessage("This square is invalid");
            WriteMessage("Please choose another square");
            break;

        default:
            break;
    }
}

void PrintBoard(int board[][BOARD_SIZE]) {

    //Variable declarations.
    int i;
    int j;

    for (i = 0; i < BOARD_SIZE; ++i) {

        for (j = 0; j < BOARD_SIZE; ++j) {

            WriteCell(board[i][j]);
        }
    }
}

void WriteCell(int num) {

    int writeResult;

    writeResult = write(1, &num, sizeof(int));

    //Check if write succeeded.
    if (writeResult < 0) {

        perror("Error: write failed.\n");
        exit(1);
    }
}

void WritePid(int file, pid_t pid) {

    int writeResult;

    //Write to FIFO.
    writeResult = write(file, &pid, sizeof(pid_t));

    //Check if write succeeded.
    if (writeResult < 0) {

        perror("Error: write failed.\n");
        exit(1);
    }
}

void SIGUSR1Handler(int signum) {


}

int ReadUserInput(int board[][BOARD_SIZE], int *x, int *y) {

    //Variables declarations.
    int  readResult;
    int  counter = 0;
    char c;
    char input[5];
    int  isDone  = 0;
    int  isValid;

    while (!isDone) {

        readResult = read(0, &c, 1);

        //Check if read succeeded.
        if (readResult < 0) {

            perror("Error: read failed.\n");
            exit(1);
        }

        //Check if in the limit of the input.
        if (counter < 5) {

            input[counter] = c;
        }

        if (c == '\n') {

            isDone = 1;
        }
    }

    //Check if the cell is valid.
    isValid = IsValidCell(input, x, y);

    if (!isValid) {

        //Invalid cell.
        return 1;
    }

    //Check if the move is valid.
    isValid = IsValidMove(*x, *y, board);

    if (!isValid) {

        //Invalid move.
        return 2;
    }


}

int IsValidCell(char input[], int *x, int *y) {

    //Check if the input is legal.
    if (input[0] != '[' || input[4] != ']' || input[2] != ',') {

        return 0;
    }

    *x = atoi(input[1]);
    *y = atoi(input[2]);

    //Check if the row and column are legal.
    if (*x < 0 || *x > BOARD_SIZE || *y < 0 || *y > BOARD_SIZE) {

        return 0;
    }

    return 1;
}

int IsValidMove(int x, int y, int board[][BOARD_SIZE]) {

}

void InitializeBoard(int **board) {

    int i;
    int j;

    //Fill with empty cells.
    for (i = 0; i < BOARD_SIZE; ++i) {
        for (j = 0; j < BOARD_SIZE; ++j) {

            board[i][j] = 0;
        }
    }

    //Set black cells.
    board[3][3] = 2;
    board[4][4] = 2;

    //Set white cells.
    board[3][4] = 1;
    board[4][3] = 1;
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