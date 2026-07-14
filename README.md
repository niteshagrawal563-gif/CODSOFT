# CODSOFT C++ Projects

This repository contains three console-based C++ projects.

## Projects

### 1. Simple Calculator

**File:** `calculator.cpp`

A menu-driven calculator that supports addition, subtraction, multiplication, division, powers, square roots, and modulus. It validates numeric input and handles invalid operations such as division by zero.

### 2. Number Guessing Game

**File:** `numberGame.cpp`

The computer chooses a random number from 1 to 100. Players receive high/low hints and try to guess it in the fewest attempts. The best score is stored in `highscore.txt`.

### 3. Tic-Tac-Toe

**File:** `ticTacToe.cpp`

A two-player terminal Tic-Tac-Toe game. It checks valid moves, detects wins and draws, keeps a scoreboard, and lets players play multiple rounds.

## Requirements

- A C++ compiler, such as `g++`
- A terminal that supports ANSI colours (recommended)

## Build and Run

Compile and run each project separately from the repository folder:

```bash
# Calculator
g++ -std=c++17 calculator.cpp -o calculator
./calculator

# Number Guessing Game
g++ -std=c++17 numberGame.cpp -o numberGame
./numberGame

# Tic-Tac-Toe
g++ -std=c++17 ticTacToe.cpp -o ticTacToe
./ticTacToe
```

## Files

| File | Purpose |
| --- | --- |
| `calculator.cpp` | Calculator source code |
| `numberGame.cpp` | Number Guessing Game source code |
| `ticTacToe.cpp` | Tic-Tac-Toe source code |
| `highscore.txt` | Saved best score for the Number Guessing Game |

