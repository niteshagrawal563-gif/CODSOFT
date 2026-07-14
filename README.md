# CodSoft C++ Programming Internship Projects

[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg?style=flat&logo=c%2B%2B)](https://en.cppreference.com/w/cpp/17)
[![Platform](https://img.shields.io/badge/Platform-macOS%20%7C%20Linux%20%7C%20Unix-lightgrey.svg?style=flat)](#requirements)
[![Environment](https://img.shields.io/badge/Console-ANSI%20Color-green.svg?style=flat)](#visual-aesthetics--terminal-styling)
[![Status](https://img.shields.io/badge/Status-Completed-success.svg?style=flat)](#projects)

A comprehensive suite of five standalone, console-based C++ applications developed during the **CodSoft C++ Programming Internship**. These applications demonstrate advanced software engineering practices, custom visual frameworks, complex data structures, and persistent storage management inside raw terminal interfaces.

---

## Table of Contents
- [Executive Project Overview](#executive-project-overview)
- [Project Showcase](#project-showcase)
  - [1. Number Guessing Game](#1-number-guessing-game)
  - [2. Simple Calculator](#2-simple-calculator)
  - [3. Tic-Tac-Toe Game](#3-tic-tac-toe-game)
  - [4. To-Do List Dashboard](#4-to-do-list-dashboard)
  - [5. Next-Gen Library Management System](#5-next-gen-library-management-system)
- [Visual Aesthetics & Terminal Styling](#visual-aesthetics--terminal-styling)
- [Software Engineering Patterns Applied](#software-engineering-patterns-applied)
- [Prerequisites & Build Instructions](#prerequisites--build-instructions)
- [Directory Layout](#directory-layout)
- [Author](#author)
- [Internship](#internship)
- [License](#license)

---

## Executive Project Overview

Each program is an independent tool compiled with a modern standard toolchain (`g++ -std=c++17`). Together, they showcase the journey from introductory procedural design to sophisticated, industry-grade commercial CLI software.

| Task ID | Application | Source File | Core Capabilities |
| :---: | :--- | :--- | :--- |
| **01** | **Number Guessing Game** | `numberGame.cpp` | Randomization, persistent local high scores, close-proximity hints. |
| **02** | **Simple Calculator** | `calculator.cpp` | Multi-functional arithmetic, trigonometric/power operations, division protection. |
| **03** | **Tic-Tac-Toe Game** | `ticTacToe.cpp` | Multi-round scoreboard tracking, dynamic grid coloring, victory pattern matching. |
| **04** | **To-Do List Dashboard** | `todoList.cpp` | Categorization, priorities, snapshot analytics, flat-file serialization. |
| **05** | **Library Management System** | `librarySystem.cpp` | Role-Based Access Control, time travel simulation, reservation queues, log auditing. |

---

## Project Showcase

### 1. Number Guessing Game
An interactive, feedback-driven game where the user attempts to guess a randomly chosen integer.
*   **Intelligent Proximity Hints**: Warns the player when they are within 5 units of the secret number ("You're very close! 🔥").
*   **High Score Tracking**: Automatically writes and reads the best score (fewest attempts) from a local `highscore.txt` database.
*   **Robust Input Parsing**: Handles malformed scanner queries (e.g. typing text instead of digits) to avoid terminal loop locks.

### 2. Simple Calculator
A menu-driven calculations utility optimized for basic and scientific arithmetic.
*   **Extensive Operator Library**: Addition, subtraction, multiplication, division, exponentiation (`pow`), square roots (`sqrt`), and real modulus (`fmod`).
*   **Zero-Division Guard**: Protects against program faults by intercepting divisions and moduli using `0` as a denominator.
*   **Continuous Calculation**: Menu loop structure remains active, enabling back-to-back mathematical operations.

### 3. Tic-Tac-Toe Game
A colorful, multi-round, local multiplayer board game.
*   **Interactive Scoreboard**: Retains wins, losses, and draw metrics dynamically across multiple games.
*   **ANSI Cell Coloring**: Custom cell renderer outputs distinct colored representations for Player X (Red) and Player O (Blue).
*   **Win Pattern Checker**: Evaluates board conditions against an index of all 8 possible winning linear combinations.

### 4. To-Do List Dashboard
A modern task-management workspace built around a live productivity dashboard.
*   **Live Metrics Panel**: Displays total, pending, completed, and critical-priority statistics.
*   **Advanced Metadata**: Tracks tasks by name, custom categories (e.g., Work, Personal, Study), and three priority levels (Low, Medium, High).
*   **Fuzzy Searching**: Fast substring search by task content or category category.
*   **Flat-File Serialization**: Saves tasks to `todo_tasks.txt` in a safe delimited format.

### 5. Next-Gen Library Management System
An industry-grade commercial-quality administrative and client portal.
*   **Role-Based Access Control (RBAC)**: Supports administrative Librarian accounts and student Member accounts with different terminal pathways.
*   **Security Shell**: Utilizes Unix `termios` configuration to capture masked passwords (`*` symbols) during authentication.
*   **Dynamic Time Travel Engine**: A simulated clock allowing administrators to fast-forward the system time by any number of days, enabling dynamic overdue fine assessments ($0.50/day) and borrow limitations to be verified immediately.
*   **Reservation Holds Queue**: Auto-blocks checked-out books when stock is 0, placing members in a reservation queue. The book is reserved for them upon check-in.
*   **Audit Logging**: Appends all database operations, logins, and overrides to `library_audit.log` with formatted timestamps.
*   **Data Exporter**: Compile and output databases directly to `books_report.csv` and `members_report.csv` standard formats.

---

## Visual Aesthetics & Terminal Styling

The projects utilize a unified, color-coded visual language using ANSI escape sequences. A custom styling namespace `Style` is used across applications to maintain high contrast and interface accessibility:

```cpp
namespace Style {
    const string reset   = "\033[0m";
    const string bold    = "\033[1m";
    const string dim     = "\033[2m";
    const string red     = "\033[31m";
    const string green   = "\033[32m";
    const string yellow  = "\033[33m";
    const string blue    = "\033[34m";
    const string cyan    = "\033[36m";
    const string magenta = "\033[35m";
}
```

UI borders are rendered using double-lined block borders (`╔`, `═`, `╗`, `║`, `╚`, `╝`) for an elegant, box-panel visual style.

---

## Software Engineering Patterns Applied

1.  **Robust Input Validation**: Avoids infinite loops by clearing input buffers (`cin.clear()` and `cin.ignore(10000, '\n')`) when user entries do not match expected types.
2.  **Delimited File Serialization**: Formats complex records into structured database flat files using the pipe operator (`|`). A custom parser splits fields safely and handles legacy/corrupt records gracefully.
3.  **Encapsulation**: Keeps business logic isolated from interface components, permitting easily editable menus and panels.

---

## Prerequisites & Build Instructions

### Prerequisites
*   A compiler supporting **C++17** or higher (e.g., `g++` 8.0+ or Clang).
*   A terminal with ANSI escape sequences enabled for color display.

### Compilation
Open a terminal in the root repository and compile any desired application:

```bash
# 1. Number Guessing Game
g++ -std=c++17 numberGame.cpp -o numberGame

# 2. Simple Calculator
g++ -std=c++17 calculator.cpp -o calculator

# 3. Tic-Tac-Toe Game
g++ -std=c++17 ticTacToe.cpp -o ticTacToe

# 4. To-Do List Dashboard
g++ -std=c++17 todoList.cpp -o todoList

# 5. Library Management System
g++ -std=c++17 librarySystem.cpp -o librarySystem
```

### Execution
Run the compiled binaries directly from the terminal:
```bash
./numberGame
./calculator
./ticTacToe
./todoList
./librarySystem
```

> **Note on Task 5 testing**:
> *   *Default Librarian Credentials*: Username: `admin` | Password: `admin123`
> *   *Default Member Credentials*: Username: `1001` | Password: `member123`

---

## Directory Layout

```text
CODSOFT/
├── numberGame.cpp            # Task 1: Number Guessing Game source
├── calculator.cpp            # Task 2: Simple Calculator application source
├── ticTacToe.cpp             # Task 3: Tic-Tac-Toe Game source
├── todoList.cpp              # Task 4: To-Do List Dashboard source
├── librarySystem.cpp         # Task 5: Next-Gen Library Management System source
├── highscore.txt             # Saved high-score tracker for Task 1
├── library_books.txt         # Inventory database for Library System (Task 5)
├── library_members.txt       # Users database for Library System (Task 5)
├── library_transactions.txt  # Lending database for Library System (Task 5)
├── library_reservations.txt  # Reservation holds database for Library System (Task 5)
├── library_audit.log         # Security audit log trail for Library System (Task 5)
├── .gitignore                # Rules for excluding compiled executables & logs
└── README.md                 # Project handbook & styling systems details
```

---

## Author

*   **Nitesh Agrawal**

---

## Internship

*   **CodSoft C++ Programming Internship**

---

