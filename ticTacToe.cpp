#include <iostream>
#include <vector>
#include <string>
using namespace std;

const string RESET="\033[0m";
const string RED="\033[31m";
const string BLUE="\033[34m";
const string GREEN="\033[32m";
const string YELLOW="\033[33m";
const string CYAN="\033[36m";

vector<char> board(10);

void initBoard()
{
    for(int i=1;i<=9;i++)
        board[i]='0'+i;
}

void clearScreen()
{
    cout<<"\033[2J\033[1;1H";
}

void printTitle()
{
    cout<<CYAN;
    cout<<"=====================================\n";
    cout<<"         TIC TAC TOE\n";
    cout<<"=====================================\n";
    cout<<RESET;
}

string cell(int i)
{
    if(board[i]=='X')
        return RED+"X"+RESET;
    if(board[i]=='O')
        return BLUE+"O"+RESET;
    return string(1,board[i]);
}

void drawBoard()
{
    cout<<"\n";
    cout<<" "<<cell(1)<<" │ "<<cell(2)<<" │ "<<cell(3)<<"\n";
    cout<<"───┼───┼───\n";
    cout<<" "<<cell(4)<<" │ "<<cell(5)<<" │ "<<cell(6)<<"\n";
    cout<<"───┼───┼───\n";
    cout<<" "<<cell(7)<<" │ "<<cell(8)<<" │ "<<cell(9)<<"\n";
    cout<<"\n";
}

bool validMove(int pos)
{
    if(pos<1||pos>9)
        return false;

    if(board[pos]=='X'||board[pos]=='O')
        return false;

    return true;
}

void placeMove(int pos,char symbol)
{
    board[pos]=symbol;
}

bool checkWin(char s)
{
    int win[8][3]={
        {1,2,3},
        {4,5,6},
        {7,8,9},
        {1,4,7},
        {2,5,8},
        {3,6,9},
        {1,5,9},
        {3,5,7}
    };

    for(int i=0;i<8;i++)
    {
        if(board[win[i][0]]==s &&
           board[win[i][1]]==s &&
           board[win[i][2]]==s)
            return true;
    }

    return false;
}

bool boardFull()
{
    for(int i=1;i<=9;i++)
        if(board[i]!='X'&&board[i]!='O')
            return false;

    return true;
}
struct Player
{
    string name;
    char symbol;
    int score;
};

void printScore(const Player &p1,const Player &p2,int draws)
{
    cout<<GREEN;
    cout<<"=========================================\n";
    cout<<" SCOREBOARD\n";
    cout<<"=========================================\n";
    cout<<RESET;

    cout<<RED<<p1.name<<RESET<<" ("<<p1.symbol<<") : "<<p1.score<<"\n";
    cout<<BLUE<<p2.name<<RESET<<" ("<<p2.symbol<<") : "<<p2.score<<"\n";
    cout<<YELLOW<<"Draws"<<RESET<<" : "<<draws<<"\n";

    cout<<"=========================================\n\n";
}

void waitForEnter()
{
    cout<<"\nPress Enter to continue...";
    cin.ignore(10000,'\n');
    cin.get();
}

int getMove(Player &current)
{
    int pos;

    while(true)
    {
        cout<<current.name<<" ("<<current.symbol<<")";
        cout<<" enter position (1-9): ";

        cin>>pos;

        if(cin.fail())
        {
            cin.clear();
            cin.ignore(10000,'\n');

            cout<<YELLOW;
            cout<<"Invalid input.\n";
            cout<<RESET;

            continue;
        }

        if(!validMove(pos))
        {
            cout<<YELLOW;
            cout<<"Invalid move.\n";
            cout<<RESET;
            continue;
        }

        return pos;
    }
}

void announceWinner(Player &winner)
{
    cout<<"\n";

    if(winner.symbol=='X')
        cout<<RED;
    else
        cout<<BLUE;

    cout<<"#########################################\n";
    cout<<" Winner : "<<winner.name<<"\n";
    cout<<"#########################################\n";

    cout<<RESET;
}

void announceDraw()
{
    cout<<YELLOW;
    cout<<"\n=========================================\n";
    cout<<"               DRAW GAME\n";
    cout<<"=========================================\n";
    cout<<RESET;
}

bool playAgain()
{
    char ch;

    while(true)
    {
        cout<<"\nPlay Again? (Y/N): ";
        cin>>ch;

        if(ch=='Y'||ch=='y')
            return true;

        if(ch=='N'||ch=='n')
            return false;

        cout<<"Enter only Y or N.\n";
    }
}

void intro()
{
    clearScreen();

    printTitle();

    cout<<"Rules:\n";
    cout<<"1. Two players.\n";
    cout<<"2. Enter positions from 1 to 9.\n";
    cout<<"3. First to make three in a row wins.\n\n";
}

void resetBoard()
{
    initBoard();
}

Player currentPlayer(Player &p1,Player &p2,bool turn)
{
    if(turn)
        return p1;

    return p2;
}
void gameLoop(Player &p1, Player &p2, int &draws)
{
    bool turn = true;
    resetBoard();

    while (true)
    {
        clearScreen();

        printTitle();
        printScore(p1, p2, draws);
        drawBoard();

        Player current = currentPlayer(p1, p2, turn);

        int move = getMove(current);

        placeMove(move, current.symbol);

        if (checkWin(current.symbol))
        {
            clearScreen();

            printTitle();
            printScore(p1, p2, draws);
            drawBoard();

            announceWinner(current);

            if (current.symbol == p1.symbol)
                p1.score++;
            else
                p2.score++;

            break;
        }

        if (boardFull())
        {
            clearScreen();

            printTitle();
            printScore(p1, p2, draws);
            drawBoard();

            announceDraw();

            draws++;

            break;
        }

        turn = !turn;
    }
}

void getPlayers(Player &p1, Player &p2)
{
    cout << "Enter Player 1 Name : ";
    getline(cin, p1.name);

    if (p1.name.empty())
        getline(cin, p1.name);

    cout << "Enter Player 2 Name : ";
    getline(cin, p2.name);

    p1.symbol = 'X';
    p2.symbol = 'O';

    p1.score = 0;
    p2.score = 0;
}

void goodbye()
{
    clearScreen();

    cout << GREEN;
    cout << "=========================================\n";
    cout << "      THANKS FOR PLAYING TIC TAC TOE\n";
    cout << "=========================================\n";
    cout << RESET;
}

int main()
{
    Player player1;
    Player player2;

    int draws = 0;

    intro();

    getPlayers(player1, player2);

    while (true)
    {
        gameLoop(player1, player2, draws);

        if (!playAgain())
            break;
    }

    goodbye();

    return 0;
}
