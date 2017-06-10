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

void InitializeBoard(char board[][BOARD_SIZE], char *data);

int ReadUserInput(char board[][BOARD_SIZE], int *x, int *y);

int IsValidCell(char input[], int *x, int *y);

int IsValidMove(int directions[], int x, int y, char board[][BOARD_SIZE]);

void WriteMessage(char *message);

void WriteCell(char num);

void PrintBoard(char board[][BOARD_SIZE]);

void
HandleResult(int result, int x, int y, char board[][BOARD_SIZE], char *data);

int checkUp(char *gameArray, int x, int y, char mySymbol, char opSymbol);

int checkDown(char *gameArray, int x, int y, char mySymbol, char opSymbol);

int checkLeft(char *gameArray, int x, int y, char mySymbol, char opSymbol);

int checkRight(char *gameArray, int x, int y, char mySymbol, char opSymbol);

int checkUpRight(char *gameArray, int x, int y, char mySymbol, char opSymbol);

int checkUpLeft(char *gameArray, int x, int y, char mySymbol, char opSymbol);

int checkDownRight(char *gameArray, int x, int y, char mySymbol, char opSymbol);

int checkDownLeft(char *gameArray, int x, int y, char mySymbol, char opSymbol);

void UpdateBoard(int *directions, int x, int y, char color,
                 char board[][BOARD_SIZE]);

void FillDirections(int x, int y, char player, char opponent, int directions[],
                    char board[][BOARD_SIZE]);

void WriteMove(int x, int y, char *data);

void WaitForOpponent(char *data, char board[][BOARD_SIZE]);

int IsGameOver(char board[][BOARD_SIZE], char *data);

void PrintResult(char result);

//TODO make local
int  stop;
char myColor;
char opponentColor;

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
    char             board[BOARD_SIZE][BOARD_SIZE];
    int              x;
    int              y;

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

    //Close FIFO.
    resultValue = close(file);

    //Check if close succeeded.
    if (resultValue < 0) {

        perror("Error: close failed.\n");
        exit(1);
    }

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

    //Set players colors.
    if (strcmp(data, "") == 0) {

        myColor       = 'b';
        opponentColor = 'w';
    } else {

        myColor       = 'w';
        opponentColor = 'b';
    }

    InitializeBoard(board, data);
    //TODO delete if required at the start of the game
    PrintBoard(board);
    stop = 0;

    while (!stop) {

        WriteMessage("Please choose a square\n");

        resultValue = ReadUserInput(board, &x, &y);

        HandleResult(resultValue, x, y, board, data);

        if(IsGameOver(board, data)){

            stop = 1;
        }
    }

    //TODO detach if needed

    //Print game result message.
    PrintResult(data[0]);
}

void
HandleResult(int result, int x, int y, char board[][BOARD_SIZE], char *data) {

    switch (result) {

        //Legal move.
        case 0:
            WriteMessage("The board is:\n");
            PrintBoard(board);
            WriteMove(x, y, data);
            WaitForOpponent(data, board);
            break;

            //Illegal cell.
        case 1:

            WriteMessage("No such square\n");
            WriteMessage("Please choose another square\n");
            break;

            //Illegal move.
        case 2:
            WriteMessage("This square is invalid\n");
            WriteMessage("Please choose another square\n");
            break;

            //TODO handle the case of game over

        default:
            break;
    }
}

void WriteMove(int x, int y, char *data) {

    //Write color.
    data[0] = myColor;

    //Write column number.
    data[1] = x + '0';

    //Write row number.
    data[2] = y + '0';

    data[3] = '\0';
}

void WaitForOpponent(char *data, char board[][BOARD_SIZE]) {

    int isMoveMade = 0;
    int x;
    int y;
    int directions[8];

    while (!isMoveMade) {

        //Check if opponent made a move.
        if (data[0] != myColor) {

            isMoveMade = 1;
        } else {

            //TODO uncomment
           // WriteMessage("Waiting for the other player to make a move\n");
            sleep(1);
        }
    }

    x = data[1] - '0';
    y = data[2] - '0';

    //Fill opponents directions.
    FillDirections(x, y, opponentColor, myColor, directions, board);

    //Update board according to opponent move.
    UpdateBoard(directions, y, x, opponentColor, board);

    WriteMessage("The board is:\n");

    //Print board to screen.
    PrintBoard(board);
}

void FillDirections(int x, int y, char player, char opponent, int directions[],
                    char board[][BOARD_SIZE]) {

    directions[0] = checkLeft(board, x, y, player, opponent);

    directions[1] = checkUpLeft(board, x, y, player, opponent);

    directions[2] = checkUp(board, x, y, player, opponent);

    directions[3] = checkUpRight(board, x, y, player, opponent);

    directions[4] = checkRight(board, x, y, player, opponent);

    directions[5] = checkDownRight(board, x, y, player, opponent);

    directions[6] = checkDown(board, x, y, player, opponent);

    directions[7] = checkDownLeft(board, x, y, player, opponent);
}

void PrintBoard(char board[][BOARD_SIZE]) {

    //Variable declarations.
    int i;
    int j;

    for (i = 0; i < BOARD_SIZE; ++i) {

        for (j = 0; j < BOARD_SIZE; ++j) {

            WriteCell(board[i][j]);
            WriteMessage(" ");
        }

        WriteMessage("\n");
    }

    WriteMessage("\n");
}

void WriteCell(char num) {

    int writeResult;

    if (num == 'b') {
        num = '2';

    } else if (num == 'w') {
        num = '1';
    }

    writeResult = write(1, &num, sizeof(char));

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

    return;

}

int ReadUserInput(char board[][BOARD_SIZE], int *x, int *y) {

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

        counter++;
    }

    //Check if the cell is valid.
    isValid = IsValidCell(input, x, y);

    if (!isValid) {

        //Invalid cell.
        return 1;
    }

    int directions[8];

    //Check if the move is valid.
    isValid = IsValidMove(directions, *x, *y, board);

    if (!isValid) {

        //Invalid move.
        return 2;
    }

    //Update board according to move.
    UpdateBoard(directions, *y, *x, myColor, board);

    return 0;

}

int IsValidCell(char input[], int *x, int *y) {

    //Check if the input is legal.
    if (input[0] != '[' || input[4] != ']' || input[2] != ',') {

        return 0;
    }

    *x = input[1] - '0';
    *y = input[3] - '0';

    //Check if the row and column are legal.
    if (*x < 0 || *x > BOARD_SIZE || *y < 0 || *y > BOARD_SIZE) {

        return 0;
    }

    return 1;
}

int IsValidMove(int directions[], int x, int y, char board[][BOARD_SIZE]) {

    int sum = 0;
    int i;

    directions[0] = checkLeft(board, x, y, myColor, opponentColor);

    directions[1] = checkUpLeft(board, x, y, myColor, opponentColor);

    directions[2] = checkUp(board, x, y, myColor, opponentColor);

    directions[3] = checkUpRight(board, x, y, myColor, opponentColor);

    directions[4] = checkRight(board, x, y, myColor, opponentColor);

    directions[5] = checkDownRight(board, x, y, myColor, opponentColor);

    directions[6] = checkDown(board, x, y, myColor, opponentColor);

    directions[7] = checkDownLeft(board, x, y, myColor, opponentColor);

    for (i = 0; i < 8 && sum == 0; ++i) {

        sum += directions[i];
    }

    //Check if there are possible moves.
    if (sum == 0) {

        return 0;
    }

    return 1;
}

void UpdateBoard(int *directions, int x, int y, char color,
                 char board[][BOARD_SIZE]) {

    int i;
    int j;

    board[x][y] = color;

    //Fill to left side.
    for (i = 1; i <= directions[0]; ++i) {

        board[x][y - i] = color;
    }

    //Fill to upper-left side.
    for (i = 1; i <= directions[1]; ++i) {
        for (j = 1; j <= directions[1]; ++j) {

            board[x - i][y - j] = color;
        }
    }

    //Fill to up side.
    for (i = 1; i <= directions[2]; ++i) {

        board[x - i][y] = color;
    }

    //Fill to upper-right side.
    for (i = 1; i <= directions[3]; ++i) {
        for (j = 1; j <= directions[3]; ++j) {

            board[x - i][y + j] = color;
        }
    }

    //Fill to right side.
    for (i = 1; i <= directions[4]; ++i) {

        board[x][y + i] = color;
    }

    //Fill to lower-right side.
    for (i = 1; i <= directions[5]; ++i) {
        for (j = 1; j <= directions[5]; ++j) {

            board[x + i][y + j] = color;
        }
    }

    //Fill to down side.
    for (i = 1; i <= directions[6]; ++i) {

        board[x + i][y] = color;
    }

    //Fill to lower-left side.
    for (i = 1; i <= directions[7]; ++i) {
        for (j = 1; j <= directions[7]; ++j) {

            board[x + i][y - j] = color;
        }
    }
}

void InitializeBoard(char board[][BOARD_SIZE], char *data) {

    int i;
    int j;

    //Fill with empty cells.
    for (i = 0; i < BOARD_SIZE; ++i) {
        for (j = 0; j < BOARD_SIZE; ++j) {

            board[i][j] = '0';
        }
    }

    //Set black cells.
    board[3][3] = 'b';
    board[4][4] = 'b';

    //Set white cells.
    board[3][4] = 'w';
    board[4][3] = 'w';

    if (myColor == 'w') {

        int x;
        int y;
        int directions[8];

        x = data[1] - '0';
        y = data[2] - '0';

        //Fill opponents directions.
        FillDirections(x, y, opponentColor, myColor, directions, board);

        //Update board according to opponent move.
        UpdateBoard(directions, y, x, opponentColor, board);
    }
}

int IsGameOver(char board[][BOARD_SIZE], char *data) {

    int blackCounter = 0;
    int whiteCounter = 0;
    int freeCounter  = 0;
    int i;
    int j;
    int result = 0;

    for (i = 0; i < BOARD_SIZE; ++i) {

        for (j = 0; j < BOARD_SIZE; ++j) {

            if (board[i][j] == 'b') {

                blackCounter++;
            } else if (board[i][j] == 'w') {

                whiteCounter++;
            } else {

                freeCounter++;
            }
        }
    }

    //Check if board is full or there are no more moves.
    if(freeCounter == 0){

        if(blackCounter > whiteCounter){

            data[0] = 'B';
        }
        else if(whiteCounter > blackCounter){


            data[0] = 'W';
        } else{

            data[0] = 'T';
        }

        result = 1;
    }
    else if(blackCounter == 0){

        data[0] = 'W';
        result = 1;
    }
    else if(whiteCounter == 0){

        data[0] = 'B';
        result = 1;
    }

    return result;
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

/*******************************************************************************
* function name : checkUp                                                      *
* input : the game board as char array, the column and row number and two char *
*         which represent my and opponent symbols.                             *
* output : the number of move possible up.                                     *
* explanation : the function check how many moves the player can move up.      *
*******************************************************************************/
int checkUp(char *gameArray, int x, int y, char mySymbol, char opSymbol) {
    //index of the next move of this direction.
    int index = (y - 1) * BOARD_SIZE + x;
    int count = 0;
    if ((y % BOARD_SIZE == 0) || (y % BOARD_SIZE == 1)) {
        return 0;
    }
    //check how many opponent symbols there is in this direction.
    while ((index > BOARD_SIZE) && (gameArray[index] == opSymbol)) {
        count++;
        index -= BOARD_SIZE;
    }
    //return how many opponent symbols.
    if (gameArray[index] == mySymbol) {
        return count;
    } else {
        return 0;
    }
}

/*******************************************************************************
* function name : checkDown                                                    *
* input : the game board as char array, the column and row number and two char *
*         which represent my and opponent symbols.                             *
* output : the number of move possible down.                                   *
* explanation : the function check how many moves the player can move down.    *
*******************************************************************************/
int checkDown(char *gameArray, int x, int y, char mySymbol, char opSymbol) {
    //index of the next move of this direction.
    int index = (y + 1) * BOARD_SIZE + x;
    int count = 0;
    if ((y % BOARD_SIZE == BOARD_SIZE) || (y % BOARD_SIZE == BOARD_SIZE - 1)) {
        return 0;
    }
    int gameSize = BOARD_SIZE * BOARD_SIZE;
    //check how many opponent symbols there is in this direction.
    while ((index < gameSize) && (gameArray[index] == opSymbol)) {
        count++;
        index += BOARD_SIZE;
    }
    //return how many opponent symbols.
    if (gameArray[index] == mySymbol) {
        return count;
    } else {
        return 0;
    }
}

/*******************************************************************************
* function name : checkRight                                                   *
* input : the game board as char array, the column and row number and two char *
*         which represent my and opponent symbols.                             *
* output : the number of move possible right.                                  *
* explanation : the function check how many moves the player can move to the   *
*               right.                                                         *
*******************************************************************************/
int checkRight(char *gameArray, int x, int y, char mySymbol, char opSymbol) {
    //index of the next move of this direction.
    int index = y * BOARD_SIZE + x + 1;
    int count = 0;
    if ((x % BOARD_SIZE == BOARD_SIZE - 1) ||
        (x % BOARD_SIZE == BOARD_SIZE - 2)) {
        return 0;
    }
    //check how many opponent symbols there is in this direction.
    while ((index % BOARD_SIZE != BOARD_SIZE - 1) &&
           (gameArray[index] == opSymbol)) {
        count++;
        index++;
    }
    //return how many opponent symbols.
    if (gameArray[index] == mySymbol) {
        return count;
    } else {
        return 0;
    }
}

/*******************************************************************************
* function name : checkLeft                                                    *
* input : the game board as char array, the column and row number and two char *
*         which represent my and opponent symbols.                             *
* output : the number of move possible left.                                   *
* explanation : the function check how many moves the player can move to the   *
*               left.                                                          *
*******************************************************************************/
int checkLeft(char *gameArray, int x, int y, char mySymbol, char opSymbol) {
    //index of the next move of this direction.
    int index = y * BOARD_SIZE + x - 1;
    int count = 0;
    if ((x % BOARD_SIZE == 0) || (x % BOARD_SIZE == 1)) {
        return 0;
    }

    //check how many opponent symbols there is in this direction.
    while ((index % BOARD_SIZE != 0) && (gameArray[index] == opSymbol)) {
        count++;
        index--;
    }
    //return how many opponent symbols.
    if (gameArray[index] == mySymbol) {
        return count;
    } else {
        return 0;
    }
}

/*******************************************************************************
* function name : checkUpRight                                                 *
* input : the game board as char array, the column and row number and two char *
*         which represent my and opponent symbols.                             *
* output : the number of move possible diagonal up-right.                      *
* explanation : the function check how many moves the player can move to       *
*               diagonal up-right.                                             *
*******************************************************************************/
int checkUpRight(char *gameArray, int x, int y, char mySymbol, char opSymbol) {
    //index of the next move of this direction.
    int index = (y - 1) * BOARD_SIZE + x + 1;
    int count = 0;
    if ((y % BOARD_SIZE == 0) || (y % BOARD_SIZE == 1) ||
        (x % BOARD_SIZE == BOARD_SIZE - 1) ||
        (x % BOARD_SIZE == BOARD_SIZE - 2)) {
        return 0;
    }
    //check how many opponent symbols there is in this direction.
    while ((index % BOARD_SIZE != BOARD_SIZE - 1) && (index > BOARD_SIZE) &&
           (gameArray[index] == opSymbol)) {
        count++;
        index -= BOARD_SIZE;
        index++;
    }
    //return how many opponent symbols.
    if (gameArray[index] == mySymbol) {
        return count;
    } else {
        return 0;
    }
}

/*******************************************************************************
* function name : checkUpLeft                                                  *
* input : the game board as char array, the column and row number and two char *
*         which represent my and opponent symbols.                             *
* output : the number of move possible diagonal up-left.                       *
* explanation : the function check how many moves the player can move to       *
*               diagonal up-left.                                              *
*******************************************************************************/
int checkUpLeft(char *gameArray, int x, int y, char mySymbol, char opSymbol) {
    //index of the next move of this direction.
    int index = (y - 1) * BOARD_SIZE + x - 1;
    int count = 0;
    if ((x % BOARD_SIZE == 0) || (x % BOARD_SIZE == 1) ||
        (y % BOARD_SIZE == 0) ||
        (y % BOARD_SIZE == 1)) {
        return 0;
    }
    //check how many opponent symbols there is in this direction.
    while ((index % BOARD_SIZE != 0) && (index > BOARD_SIZE) &&
           (gameArray[index] == opSymbol)) {
        count++;
        index -= BOARD_SIZE;
        index--;
    }
    //return how many opponent symbols.
    if (gameArray[index] == mySymbol) {
        return count;
    } else {
        return 0;
    }
}

/*******************************************************************************
* function name : checkDownRight                                               *
* input : the game board as char array, the column and row number and two char *
*         which represent my and opponent symbols.                             *
* output : the number of move possible diagonal down-right.                    *
* explanation : the function check how many moves the player can move to       *
*               diagonal down-right.                                           *
*******************************************************************************/
int
checkDownRight(char *gameArray, int x, int y, char mySymbol, char opSymbol) {
    //index of the next move of this direction.
    int index = (y + 1) * BOARD_SIZE + x + 1;
    int count = 0;
    if ((x % BOARD_SIZE == BOARD_SIZE - 1) ||
        (x % BOARD_SIZE == BOARD_SIZE - 2) ||
        (y % BOARD_SIZE == BOARD_SIZE) || (y % BOARD_SIZE == BOARD_SIZE - 1) ||
        (gameArray[index] == mySymbol)) {
        return 0;
    }
    int gameSize = BOARD_SIZE * BOARD_SIZE;
    //check how many opponent symbols there is in this direction.
    while ((index % BOARD_SIZE != BOARD_SIZE - 1) && (index < gameSize) &&
           (gameArray[index] == opSymbol)) {
        count++;
        index += BOARD_SIZE;
        index++;
    }
    //return how many opponent symbols.
    if (gameArray[index] == mySymbol) {
        return count;
    } else {
        return 0;
    }
}

/*******************************************************************************
* function name : checkDownLeft                                                *
* input : the game board as char array, the column and row number and two char *
*         which represent my and opponent symbols.                             *
* output : the number of move possible diagonal down-left.                     *
* explanation : the function check how many moves the player can move to       *
*               diagonal down-left.                                            *
*******************************************************************************/
int checkDownLeft(char *gameArray, int x, int y, char mySymbol, char opSymbol) {
    //index of the next move of this direction.
    int index = (y + 1) * BOARD_SIZE + x - 1;
    int count = 0;
    if ((x % BOARD_SIZE == 0) || (x % BOARD_SIZE == 1) ||
        (y % BOARD_SIZE == BOARD_SIZE) ||
        (y % BOARD_SIZE == BOARD_SIZE - 1) || (gameArray[index] == mySymbol)) {
        return 0;
    }
    int gameSize = BOARD_SIZE * BOARD_SIZE;
    //check how many opponent symbols there is in this direction.
    while ((index % BOARD_SIZE != 0) && (index < gameSize) &&
           (gameArray[index] == opSymbol)) {
        count++;
        index += BOARD_SIZE;
        index--;
    }
    //return how many opponent symbols.
    if (gameArray[index] == mySymbol) {
        return count;
    } else {
        return 0;
    }
}