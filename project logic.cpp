#include <iostream>
#include <cstdlib>
#include <ctime>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#endif

using namespace std;

// ================= SNAKE GAME =================

bool gameOver;
const int width = 20;
const int height = 20;

int x, y, fruitX, fruitY, score;
int tailX[100], tailY[100];
int nTail;

enum Direction
{
    STOP = 0,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

Direction dir;

#ifdef _WIN32
void delay(int ms)
{
    Sleep(ms);
}
#else
void delay(int ms)
{
    usleep(ms * 1000);
}

int _kbhit()
{
    termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

int _getch()
{
    struct termios oldt, newt;
    int ch;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    newt.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return ch;
}
#endif

void Setup()
{
    gameOver = false;
    dir = STOP;

    x = width / 2;
    y = height / 2;

    fruitX = rand() % width;
    fruitY = rand() % height;

    score = 0;
    nTail = 0;
}

void Draw()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    for (int i = 0; i < width + 2; i++)
        cout << "#";

    cout << endl;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (j == 0)
                cout << "#";

            if (i == y && j == x)
                cout << "O";
            else if (i == fruitY && j == fruitX)
                cout << "F";
            else
            {
                bool print = false;

                for (int k = 0; k < nTail; k++)
                {
                    if (tailX[k] == j && tailY[k] == i)
                    {
                        cout << "o";
                        print = true;
                    }
                }

                if (!print)
                    cout << " ";
            }

            if (j == width - 1)
                cout << "#";
        }

        cout << endl;
    }

    for (int i = 0; i < width + 2; i++)
        cout << "#";

    cout << endl;

    cout << "Score: " << score << endl;
    cout << "Controls: W A S D" << endl;
}

void Input()
{
    if (_kbhit())
    {
        switch (_getch())
        {
        case 'a':
        case 'A':
            dir = LEFT;
            break;

        case 'd':
        case 'D':
            dir = RIGHT;
            break;

        case 'w':
        case 'W':
            dir = UP;
            break;

        case 's':
        case 'S':
            dir = DOWN;
            break;

        case 'x':
        case 'X':
            gameOver = true;
            break;
        }
    }
}

void Logic()
{
    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;

    tailX[0] = x;
    tailY[0] = y;

    for (int i = 1; i < nTail; i++)
    {
        prev2X = tailX[i];
        prev2Y = tailY[i];

        tailX[i] = prevX;
        tailY[i] = prevY;

        prevX = prev2X;
        prevY = prev2Y;
    }

    switch (dir)
    {
    case LEFT:
        x--;
        break;

    case RIGHT:
        x++;
        break;

    case UP:
        y--;
        break;

    case DOWN:
        y++;
        break;

    default:
        break;
    }

    // Wall collision
    if (x >= width || x < 0 || y >= height || y < 0)
        gameOver = true;

    // Tail collision
    for (int i = 0; i < nTail; i++)
    {
        if (tailX[i] == x && tailY[i] == y)
            gameOver = true;
    }

    // Fruit eaten
    if (x == fruitX && y == fruitY)
    {
        score += 10;

        fruitX = rand() % width;
        fruitY = rand() % height;

        nTail++;
    }
}

// ================= TIC TAC TOE =================

char board[3][3] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'}};

void displayBoard()
{
    cout << endl;

    cout << " " << board[0][0] << " | " << board[0][1] << " | " << board[0][2] << endl;
    cout << "---|---|---" << endl;
    cout << " " << board[1][0] << " | " << board[1][1] << " | " << board[1][2] << endl;
    cout << "---|---|---" << endl;
    cout << " " << board[2][0] << " | " << board[2][1] << " | " << board[2][2] << endl;

    cout << endl;
}

bool checkWin(char player)
{
    for (int i = 0; i < 3; i++)
    {
        if (board[i][0] == player &&
            board[i][1] == player &&
            board[i][2] == player)
            return true;

        if (board[0][i] == player &&
            board[1][i] == player &&
            board[2][i] == player)
            return true;
    }

    if (board[0][0] == player &&
        board[1][1] == player &&
        board[2][2] == player)
        return true;

    if (board[0][2] == player &&
        board[1][1] == player &&
        board[2][0] == player)
        return true;

    return false;
}

bool checkDraw()
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (board[i][j] != 'X' && board[i][j] != 'O')
                return false;
        }
    }

    return true;
}

void ticTacToe()
{
    int choice;
    char player = 'X';

    while (true)
    {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif

        cout << "===== TIC TAC TOE =====" << endl;

        displayBoard();

        cout << "Player " << player << ", enter position (1-9): ";
        cin >> choice;

        if (choice < 1 || choice > 9)
        {
            cout << "Invalid position!" << endl;
            delay(1000);
            continue;
        }

        int row = (choice - 1) / 3;
        int col = (choice - 1) % 3;

        if (board[row][col] == 'X' || board[row][col] == 'O')
        {
            cout << "Position already taken!" << endl;
            delay(1000);
            continue;
        }

        board[row][col] = player;

        if (checkWin(player))
        {
#ifdef _WIN32
            system("cls");
#else
            system("clear");
#endif
            displayBoard();

            cout << "Player " << player << " Wins!" << endl;
            break;
        }

        if (checkDraw())
        {
#ifdef _WIN32
            system("cls");
#else
            system("clear");
#endif
            displayBoard();

            cout << "Game Draw!" << endl;
            break;
        }

        if (player == 'X')
            player = 'O';
        else
            player = 'X';
    }
}

// ================= MAIN =================

int main()
{
    srand(time(0));

    int choice;

    cout << "=========================" << endl;
    cout << "      MINI GAME PROJECT" << endl;
    cout << "=========================" << endl;
    cout << "1. Snake Game" << endl;
    cout << "2. Tic Tac Toe" << endl;
    cout << "Enter your choice: ";

    cin >> choice;

    if (choice == 1)
    {
        Setup();

        while (!gameOver)
        {
            Draw();
            Input();
            Logic();
            delay(100);
        }

        cout << endl;
        cout << "Game Over!" << endl;
        cout << "Final Score: " << score << endl;
    }
    else if (choice == 2)
    {
        ticTacToe();
    }
    else
    {
        cout << "Invalid choice!" << endl;
    }

    return 0;
}