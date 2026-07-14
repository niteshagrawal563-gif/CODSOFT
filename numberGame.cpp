#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>

using namespace std;

int loadHighScore() {
    ifstream file("highscore.txt");

    int score;
    if (file >> score)
        return score;

    return 999999;
}

void saveHighScore(int score) {
    ofstream file("highscore.txt");
    file << score;
}

void playGame() {
    int secret = rand() % 500 + 1;
    int guess;
    int attempts = 0;

    int highScore = loadHighScore();

    cout << "\n=====================================\n";
    cout << "      NUMBER GUESSING GAME\n";
    cout << "=====================================\n";
    cout << "I have chosen a number between 1 and 500.\n";
    cout << "Try to guess it!\n\n";

    while (true) {
        cout << "Enter your guess: ";
        cin >> guess;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input! Enter a number.\n\n";
            continue;
        }

        attempts++;

        if (guess < secret) {
            cout << "Too Low!\n";

            if (secret - guess <= 5)
                cout << "Hint: You're very close! 🔥\n";

            cout << endl;
        }
        else if (guess > secret) {
            cout << "Too High!\n";

            if (guess - secret <= 5)
                cout << "Hint: You're very close! 🔥\n";

            cout << endl;
        }
        else {
            cout << "\nCongratulations! You guessed it!\n";
            cout << "Attempts: " << attempts << endl;

            if (attempts < highScore) {
                cout << "New High Score!\n";
                saveHighScore(attempts);
            }
            else if (highScore != 999999) {
                cout << "Current High Score: " << highScore << endl;
            }

            break;
        }
    }
}

void instructions() {
    cout << "\n========== Instructions ==========\n";
    cout << "1. Computer selects a random number.\n";
    cout << "2. Guess the number.\n";
    cout << "3. Too High / Too Low hints are shown.\n";
    cout << "4. Try to finish in minimum attempts.\n";
    cout << "==================================\n\n";
}

void showHighScore() {
    int score = loadHighScore();

    cout << "\n========== High Score ==========\n";

    if (score == 999999)
        cout << "No high score yet.\n";
    else
        cout << "Best Attempts: " << score << endl;

    cout << "===============================\n\n";
}

int main() {

    int choice;
    srand(static_cast<unsigned int>(time(0)));

    while (true) {

        cout << "\n=========== MENU ===========\n";
        cout << "1. Play Game\n";
        cout << "2. High Score\n";
        cout << "3. Instructions\n";
        cout << "4. Exit\n";
        cout << "============================\n";

        cout << "Enter choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input! Please enter a number from 1 to 4.\n";
            continue;
        }

        switch (choice) {

        case 1:
            playGame();
            break;

        case 2:
            showHighScore();
            break;

        case 3:
            instructions();
            break;

        case 4:
            cout << "\nThanks for playing!\n";
            return 0;

        default:
            cout << "Invalid Choice!\n";
        }
    }

    return 0;
}
