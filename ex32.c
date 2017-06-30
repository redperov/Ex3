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

/**
 * function name: PrintResult.
 * The input: game result.
 * The output: void.
 * The function operation: prints the game result message.
*/
void WritePid(int file, pid_t pid);

/**
 * function name: SIGUSR1Handler.
 * The input: signum.
 * The output: void.
 * The function operation: handles SIGUSR1 signal.
*/
void SIGUSR1Handler(int signum);

/**
 * function name: InitializeBoard.
 * The input: game board, shared memory data.
 * The output: void.
 * The function operation: Initializes the board.
*/
void InitializeBoard(char board[], char *data);

/**
 * function name: ReadUserInput.
 * The input: game board, pointer to x, pointer to y.
 * The output: user input status.
 * The function operation: prints the game result message.
*/
int ReadUserInput(char board[], int *x, int *y);

/**
 * function name: IsValidCell.
 * The input: user input, pointer to x, pointer to y.
 * The output: is the cell valid.
 * The function operation: checks if the cell is valid.
*/
int IsValidCell(char input[], int *x, int *y);

/**
 * function name: IsValidMove.
 * The input: directions, x, y, game board.
 * The output: is the move valid.
 * The function operation: checks if the move is valid.
*/
int IsValidMove(int directions[], int x, int y, char board[]);

/**
 * function name: WriteMessage.
 * The input: message to print.
 * The output: void.
 * The function operation: prints the given message.
*/
void WriteMessage(char *message);

/**
 * function name: WriteCell.
 * The input: cell data.
 * The output: void.
 * The function operation: prints the cell's data.
*/
void WriteCell(char num);

/**
 * function name: PrintBoard.
 * The input: game board.
 * The output: void.
 * The function operation: prints the board to the screen.
*/
void PrintBoard(char board[]);

/**
 * function name: HandleResult.
 * The input: result, x, y, game board, shared memory data.
 * The output: void.
 * The function operation: handles the user input response.
*/
void HandleResult(int result, int x, int y, char board[], char *data);

/**
 * function name: MoveUp.
 * The input: grid, x, y, my symbol, opponent symbol.
 * The output: number of moves.
 * The function operation: checks how many moves are possible at the given
 * direction.
*/
int MoveUp(char *grid, int x, int y, char mySymbol, char opponentSymbol);

/**
 * function name: MoveUp.
 * The input: grid, x, y, my symbol, opponent symbol.
 * The output: number of moves.
 * The function operation: checks how many moves are possible upwards.
*/
int MoveDown(char *grid, int x, int y, char mySymbol, char opponentSymbol);

/**
 * function name: MoveLeft.
 * The input: grid, x, y, my symbol, opponent symbol.
 * The output: number of moves.
 * The function operation: checks how many moves are possible to the left.
*/
int MoveLeft(char *grid, int x, int y, char mySymbol, char opponentSymbol);

/**
 * function name: MoveRight.
 * The input: grid, x, y, my symbol, opponent symbol.
 * The output: number of moves.
 * The function operation: checks how many moves are possible to the right.
*/
int MoveRight(char *grid, int x, int y, char mySymbol, char opponentSymbol);

/**
 * function name: MoveUpperRight.
 * The input: grid, x, y, my symbol, opponent symbol.
 * The output: number of moves.
 * The function operation: checks how many moves are possible to the upper
 * right side.
*/
int MoveUpperRight(char *grid, int x, int y, char mySymbol,
                   char opponentSymbol);

/**
 * function name: MoveUpperLeft.
 * The input: grid, x, y, my symbol, opponent symbol.
 * The output: number of moves.
 * The function operation: checks how many moves are possible to the upper
 * left side.
*/
int MoveUpperLeft(char *grid, int x, int y, char mySymbol, char opponentSymbol);

/**
 * function name: MoveLowerRight.
 * The input: grid, x, y, my symbol, opponent symbol.
 * The output: number of moves.
 * The function operation: checks how many moves are possible to te lower
 * right side.
*/
int MoveLowerRight(char *grid, int x, int y, char mySymbol,
                   char opponentSymbol);

/**
 * function name: MoveLowerLeft.
 * The input: grid, x, y, my symbol, opponent symbol.
 * The output: number of moves.
 * The function operation: checks how many moves are possible to the lower
 * left side.
*/
int MoveLowerLeft(char *grid, int x, int y, char mySymbol, char opponentSymbol);

/**
 * function name: UpdateBoard.
 * The input: game result.
 * The output: void.
 * The function operation: prints the game result message.
*/
void UpdateBoard(int *directions, int x, int y, char color,
                 char board[]);

/**
 * function name: FillDirections.
 * The input: game result.
 * The output: void.
 * The function operation: prints the game result message.
*/
void FillDirections(int x, int y, char player, char opponent, int directions[],
                    char board[]);

/**
 * function name: WriteMove.
 * The input: game result.
 * The output: void.
 * The function operation: prints the game result message.
*/
void WriteMove(int x, int y, char *data);

/**
 * function name: WaitForOpponent.
 * The input: game result.
 * The output: void.
 * The function operation: prints the game result message.
*/
void WaitForOpponent(char *data, char board[]);

/**
 * function name: IsGameOver.
 * The input: game result.
 * The output: void.
 * The function operation: prints the game result message.
*/
int IsGameOver(char board[], char *data);

/**
 * function name: PrintResult.
 * The input: game result.
 * The output: void.
 * The function operation: prints the game result message.
*/
void PrintResult(char result);

/**
 * function name: CheckAllDirections.
 * The input: game board, x, y.
 * The output: number of valid directions..
 * The function operation: checks all the directions.
*/
int CheckAllDirections(char board[], int x, int y);

/**
 * function name: HasMove.
 * The input: game board, shared memory.
 * The output: does the player has.
 * The function operation: runs over the boad and checks for valid moves.
*/
int HasMove(char board[], char *data);

/**
 * function name: DetachMemory.
 * The input: shared memory.
 * The output: void.
 * The function operation: detaches shared memory.
*/
void DetachMemory(char *data);

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
    char             board[BOARD_SIZE * BOARD_SIZE];
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

        //Check if player has possible moves.
        if (!HasMove(board, data)) {

            stop = 1;
        } else {

            WriteMessage("Please choose a square\n");

            resultValue = ReadUserInput(board, &x, &y);

            HandleResult(resultValue, x, y, board, data);

            //Check if the game is over.
            if (IsGameOver(board, data)) {

                stop = 1;
            }
        }
    }

    //TODO detach if needed

    //Print game result message.
    PrintResult(data[0]);

    //Detach from shared memory.
    DetachMemory(data);
}

void DetachMemory(char *data) {

    int resultValue;

    resultValue = shmdt(data);

    //Check if shmdt succeeded.
    if (resultValue < 0) {

        perror("Error: shmdt failed.\n");
        exit(1);
    }
}

int HasMove(char board[], char *data) {

    int i;
    int j;
    int numOfValidMoves = 0;
    int playerCounter   = 0;
    int opponentCounter = 0;
    int freeCounter     = 0;

    for (i = 0; i < BOARD_SIZE; ++i) {

        for (j = 0; j < BOARD_SIZE; ++j) {

            if (board[i * BOARD_SIZE + j] == myColor) {

                playerCounter++;
            } else if (board[i * BOARD_SIZE + j] == opponentColor) {

                numOfValidMoves = CheckAllDirections(board, i, j);

                if (numOfValidMoves > 0) {
                    return 1;
                }

                opponentCounter++;
            } else {

                freeCounter++;
            }
        }
    }

    //Check if the player has any moves.
    if (numOfValidMoves == 0) {

        //Set the game result message.
        if (playerCounter > opponentCounter) {

            if (myColor == 'b') {

                data[0] = 'B';
            } else {

                data[0] = 'W';
            }
        } else if (opponentCounter > playerCounter) {

            if (opponentCounter == 'b') {

                data[0] = 'B';
            } else {

                data[0] = 'W';
            }
        } else {

            data[0] = 'T';
        }

        return 0;
    }

    return 1;
}

int CheckAllDirections(char board[], int x, int y) {

    int numOfValidDirections = 0;
    int directions[8];

    //Check if left move is valid.
    if (x > 0) {

        numOfValidDirections += IsValidMove(directions, y, x - 1, board);
    }

    //Check if upper-left move is valid.
    if (x > 0 && y > 0) {

        numOfValidDirections += IsValidMove(directions, y - 1, x - 1, board);
    }

    //Check if up move is valid.
    if (y > 0) {

        numOfValidDirections += IsValidMove(directions, y - 1, x, board);
    }

    //Check if upper-right move is legal.
    if (x < BOARD_SIZE - 1 && y > 0) {

        numOfValidDirections += IsValidMove(directions, y - 1, x + 1, board);
    }

    //Check if right move is valid.
    if (x < BOARD_SIZE - 1) {

        numOfValidDirections += IsValidMove(directions, y, x + 1, board);
    }

    //Check if lower-right move is valid.
    if (x < BOARD_SIZE - 1 && y < BOARD_SIZE - 1) {

        numOfValidDirections += IsValidMove(directions, y + 1, x + 1, board);
    }

    //Check if down move is valid.
    if (y < BOARD_SIZE - 1) {

        numOfValidDirections += IsValidMove(directions, y + 1, x, board);
    }

    //Check if lower-left move is valid.
    if (x > 0 && y < BOARD_SIZE - 1) {

        numOfValidDirections += IsValidMove(directions, y + 1, x - 1, board);
    }

    return numOfValidDirections;
}

void
HandleResult(int result, int x, int y, char board[], char *data) {

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

void WaitForOpponent(char *data, char board[]) {

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

    //Check if game is over.
    if (data[0] != myColor && data[0] != opponentColor) {
        return;
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
                    char board[]) {

    directions[0] = MoveLeft(board, x, y, player, opponent);

    directions[1] = MoveUpperLeft(board, x, y, player, opponent);

    directions[2] = MoveUp(board, x, y, player, opponent);

    directions[3] = MoveUpperRight(board, x, y, player, opponent);

    directions[4] = MoveRight(board, x, y, player, opponent);

    directions[5] = MoveLowerRight(board, x, y, player, opponent);

    directions[6] = MoveDown(board, x, y, player, opponent);

    directions[7] = MoveLowerLeft(board, x, y, player, opponent);
}

void PrintBoard(char board[]) {

    //Variable declarations.
    int i;
    int j;

    for (i = 0; i < BOARD_SIZE; ++i) {

        for (j = 0; j < BOARD_SIZE; ++j) {

            WriteCell(board[i * BOARD_SIZE + j]);
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

int ReadUserInput(char board[], int *x, int *y) {

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

int IsValidMove(int directions[], int x, int y, char board[]) {

    int sum = 0;
    int i;

    directions[0] = MoveLeft(board, x, y, myColor, opponentColor);

    directions[1] = MoveUpperLeft(board, x, y, myColor, opponentColor);

    directions[2] = MoveUp(board, x, y, myColor, opponentColor);

    directions[3] = MoveUpperRight(board, x, y, myColor, opponentColor);

    directions[4] = MoveRight(board, x, y, myColor, opponentColor);

    directions[5] = MoveLowerRight(board, x, y, myColor, opponentColor);

    directions[6] = MoveDown(board, x, y, myColor, opponentColor);

    directions[7] = MoveLowerLeft(board, x, y, myColor, opponentColor);

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
                 char board[]) {

    int i;

    board[x * BOARD_SIZE + y] = color;

    //Fill to left side.
    for (i = 1; i <= directions[0]; ++i) {

        board[x * BOARD_SIZE + y - i] = color;
    }

    //Fill to upper-left side.
    for (i = 1; i <= directions[1]; ++i) {

        board[(x - i) * BOARD_SIZE + (y - i)] = color;
    }

    //Fill to up side.
    for (i = 1; i <= directions[2]; ++i) {

        board[(x - i) * BOARD_SIZE + y] = color;
    }

    //Fill to upper-right side.
    for (i = 1; i <= directions[3]; ++i) {

        board[(x - i) * BOARD_SIZE + (y + i)] = color;
    }

    //Fill to right side.
    for (i = 1; i <= directions[4]; ++i) {

        board[x * BOARD_SIZE + (y + i)] = color;
    }

    //Fill to lower-right side.
    for (i = 1; i <= directions[5]; ++i) {

        board[(x + i) * BOARD_SIZE + (y + i)] = color;
    }

    //Fill to down side.
    for (i = 1; i <= directions[6]; ++i) {

        board[(x + i) * BOARD_SIZE + y] = color;
    }

    //Fill to lower-left side.
    for (i = 1; i <= directions[7]; ++i) {

        board[(x + i) * BOARD_SIZE + (y - i)] = color;
    }
}

void InitializeBoard(char board[], char *data) {

    int i;
    int j;

    //Fill with empty cells.
    for (i = 0; i < BOARD_SIZE; ++i) {
        for (j = 0; j < BOARD_SIZE; ++j) {

            board[i * BOARD_SIZE + j] = '0';
        }
    }

    //Set black cells.
    board[3 * BOARD_SIZE + 3] = 'b';
    board[4 * BOARD_SIZE + 4] = 'b';

    //Set white cells.
    board[3 * BOARD_SIZE + 4] = 'w';
    board[4 * BOARD_SIZE + 3] = 'w';

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

int IsGameOver(char board[], char *data) {

    int blackCounter = 0;
    int whiteCounter = 0;
    int freeCounter  = 0;
    int i;
    int j;
    int result       = 0;

    for (i = 0; i < BOARD_SIZE; ++i) {

        for (j = 0; j < BOARD_SIZE; ++j) {

            if (board[i * BOARD_SIZE + j] == 'b') {

                blackCounter++;
            } else if (board[i * BOARD_SIZE + j] == 'w') {

                whiteCounter++;
            } else {

                freeCounter++;
            }
        }
    }

    //Check if board is full or there are no more moves.
    if (freeCounter == 0) {

        if (blackCounter > whiteCounter) {

            data[0] = 'B';
        } else if (whiteCounter > blackCounter) {


            data[0] = 'W';
        } else {

            data[0] = 'T';
        }

        result = 1;
    } else if (blackCounter == 0) {

        data[0] = 'W';
        result = 1;
    } else if (whiteCounter == 0) {

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

void PrintResult(char result) {

    switch (result) {

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

int MoveUp(char *grid, int x, int y, char mySymbol, char opponentSymbol) {

    //The next position.
    int position = (y - 1) * BOARD_SIZE + x;
    int counter  = 0;

    if ((y % BOARD_SIZE == 0) || (y % BOARD_SIZE == 1)) {
        return 0;
    }

    //Check number of opponent symbols.
    while ((position > BOARD_SIZE) && (grid[position] == opponentSymbol)) {

        counter++;
        position -= BOARD_SIZE;
    }

    //return number of opponent symbols.
    if (grid[position] == mySymbol) {

        return counter;
    } else {

        return 0;
    }
}

int MoveDown(char *grid, int x, int y, char mySymbol, char opponentSymbol) {

    //The next position.
    int position = (y + 1) * BOARD_SIZE + x;
    int counter  = 0;

    if ((y % BOARD_SIZE == BOARD_SIZE) || (y % BOARD_SIZE == BOARD_SIZE - 1)) {

        return 0;
    }

    int size = BOARD_SIZE * BOARD_SIZE;

    //Check number of opponent symbols.
    while ((position < size) && (grid[position] == opponentSymbol)) {

        counter++;
        position += BOARD_SIZE;
    }

    //return number of opponent symbols.
    if (grid[position] == mySymbol) {

        return counter;
    } else {

        return 0;
    }
}

int MoveRight(char *grid, int x, int y, char mySymbol, char opponentSymbol) {

    //The next position.
    int position = y * BOARD_SIZE + x + 1;
    int counter  = 0;

    if ((x % BOARD_SIZE == BOARD_SIZE - 1) ||
        (x % BOARD_SIZE == BOARD_SIZE - 2)) {

        return 0;
    }

    //Check number of opponent symbols.
    while ((position % BOARD_SIZE != BOARD_SIZE - 1) &&
           (grid[position] == opponentSymbol)) {

        counter++;
        position++;
    }

    //return number of opponent symbols.
    if (grid[position] == mySymbol) {

        return counter;
    } else {

        return 0;
    }
}

int MoveLeft(char *grid, int x, int y, char mySymbol, char opponentSymbol) {

    //The next position.
    int position = y * BOARD_SIZE + x - 1;
    int counter  = 0;

    if ((x % BOARD_SIZE == 0) || (x % BOARD_SIZE == 1)) {
        return 0;
    }

    //Check number of opponent symbols.
    while ((position % BOARD_SIZE != 0) && (grid[position] == opponentSymbol)) {

        counter++;
        position--;
    }

    //return number of opponent symbols.
    if (grid[position] == mySymbol) {

        return counter;
    } else {

        return 0;
    }
}

int
MoveUpperRight(char *grid, int x, int y, char mySymbol, char opponentSymbol) {
    //The next position.
    int position = (y - 1) * BOARD_SIZE + x + 1;
    int counter  = 0;

    if ((y % BOARD_SIZE == 0) || (y % BOARD_SIZE == 1) ||
        (x % BOARD_SIZE == BOARD_SIZE - 1) ||
        (x % BOARD_SIZE == BOARD_SIZE - 2)) {

        return 0;
    }

    //Check number of opponent symbols.
    while ((position % BOARD_SIZE != BOARD_SIZE - 1) &&
           (position > BOARD_SIZE) &&
           (grid[position] == opponentSymbol)) {

        counter++;
        position -= BOARD_SIZE;
        position++;
    }
    //return number of opponent symbols.
    if (grid[position] == mySymbol) {

        return counter;
    } else {

        return 0;
    }
}

int
MoveUpperLeft(char *grid, int x, int y, char mySymbol, char opponentSymbol) {

    //The next position.
    int position = (y - 1) * BOARD_SIZE + x - 1;
    int counter  = 0;

    if ((x % BOARD_SIZE == 0) || (x % BOARD_SIZE == 1) ||
        (y % BOARD_SIZE == 0) ||
        (y % BOARD_SIZE == 1)) {

        return 0;
    }

    //Check number of opponent symbols.
    while ((position % BOARD_SIZE != 0) && (position > BOARD_SIZE) &&
           (grid[position] == opponentSymbol)) {
        counter++;
        position -= BOARD_SIZE;
        position--;
    }

    //return number of opponent symbols.
    if (grid[position] == mySymbol) {

        return counter;
    } else {

        return 0;
    }
}

int
MoveLowerRight(char *grid, int x, int y, char mySymbol, char opponentSymbol) {

    //The next position.
    int position = (y + 1) * BOARD_SIZE + x + 1;
    int counter  = 0;

    if ((x % BOARD_SIZE == BOARD_SIZE - 1) ||
        (x % BOARD_SIZE == BOARD_SIZE - 2) ||
        (y % BOARD_SIZE == BOARD_SIZE) || (y % BOARD_SIZE == BOARD_SIZE - 1) ||
        (grid[position] == mySymbol)) {

        return 0;
    }

    int size = BOARD_SIZE * BOARD_SIZE;

    //Check number of opponent symbols.
    while ((position % BOARD_SIZE != BOARD_SIZE - 1) && (position < size) &&
           (grid[position] == opponentSymbol)) {

        counter++;
        position += BOARD_SIZE;
        position++;
    }

    //return number of opponent symbols.
    if (grid[position] == mySymbol) {

        return counter;
    } else {

        return 0;
    }
}

int
MoveLowerLeft(char *grid, int x, int y, char mySymbol, char opponentSymbol) {

    //The next position.
    int position = (y + 1) * BOARD_SIZE + x - 1;
    int counter  = 0;

    if ((x % BOARD_SIZE == 0) || (x % BOARD_SIZE == 1) ||
        (y % BOARD_SIZE == BOARD_SIZE) ||
        (y % BOARD_SIZE == BOARD_SIZE - 1) || (grid[position] == mySymbol)) {

        return 0;
    }

    int size = BOARD_SIZE * BOARD_SIZE;

    //Check number of opponent symbols.
    while ((position % BOARD_SIZE != 0) && (position < size) &&
           (grid[position] == opponentSymbol)) {

        counter++;
        position += BOARD_SIZE;
        position--;
    }

    //return number of opponent symbols.
    if (grid[position] == mySymbol) {

        return counter;
    } else {

        return 0;
    }
}