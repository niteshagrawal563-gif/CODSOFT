# CODSOFT C++ Programming Internship Projects

This repository contains three console-based C++ projects completed as part of the CodSoft C++ Programming Internship.

Each project is independent and can be compiled and run from the terminal.

## Projects

| Task | Project | Source file | Description |
| --- | --- | --- | --- |
| 1 | Number Guessing Game | `numberGame.cpp` | Guess a randomly generated number from 1 to 500. |
| 2 | Simple Calculator | `calculator.cpp` | Perform arithmetic operations through a menu-driven interface. |
| 3 | Tic-Tac-Toe Game | `ticTacToe.cpp` | Play a two-player console version of Tic-Tac-Toe. |
| 4 | To-Do List | `todoList.cpp` | Organize tasks in a polished terminal dashboard. |

### 1. Number Guessing Game

The computer selects a random number between 1 and 500. The player keeps guessing until the correct number is found.

**Features**

- Too high and too low feedback
- A close-guess hint
- Input validation
- Attempt counter
- Persistent best score stored in `highscore.txt`

### 2. Simple Calculator

A menu-driven calculator that accepts numeric input and displays the result of the selected operation.

**Features**

- Addition, subtraction, multiplication, and division
- Power, square root, and modulus operations
- Invalid-input handling
- Protection against division or modulus by zero
- Colourful terminal output

### 3. Tic-Tac-Toe Game

A two-player terminal game using `X` and `O` on a 3 x 3 board.

**Features**

- Player-name input
- Valid-move checking
- Win and draw detection
- Scoreboard across multiple rounds
- Option to play again
- Colourful terminal board

### 4. To-Do List

A polished terminal task manager with a dashboard-style interface and local task persistence.

**Features**

- Create, view, search, and delete tasks
- Mark tasks as complete
- Low, medium, and high priority labels
- Task categories and a live productivity snapshot
- Saved task data between program runs
- Input validation and a colourful terminal UI

## Requirements

- A C++ compiler such as `g++`
- C++17 support
- A terminal with ANSI colour support is recommended

## Build and Run

Open a terminal in this repository folder and compile one project at a time.

```bash
# Number Guessing Game
g++ -std=c++17 numberGame.cpp -o numberGame
./numberGame

# Simple Calculator
g++ -std=c++17 calculator.cpp -o calculator
./calculator

# Tic-Tac-Toe Game
g++ -std=c++17 ticTacToe.cpp -o ticTacToe
./ticTacToe

# To-Do List
g++ -std=c++17 todoList.cpp -o todoList
./todoList
```

> Run `numberGame` from this repository folder so it can read and update `highscore.txt`.

## Repository Structure

```text
CODSOFT/
├── calculator.cpp      # Task 2: Simple Calculator
├── highscore.txt       # Saved best score for Task 1
├── numberGame.cpp      # Task 1: Number Guessing Game
├── ticTacToe.cpp       # Task 3: Tic-Tac-Toe Game
├── todoList.cpp        # Task 4: To-Do List
├── .gitignore          # Ignores compiled output files
└── README.md
```

## Concepts Used

- C++ functions and control flow
- Loops and conditional statements
- Input validation
- File handling
- Random-number generation
- Vectors, structures, and basic game logic
- Persistent file storage and task-management logic

## Author

Nitesh Agrawal

## Internship

CodSoft C++ Programming Internship
