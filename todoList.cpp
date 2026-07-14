#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

namespace Style {
const string reset = "\033[0m";
const string bold = "\033[1m";
const string dim = "\033[2m";
const string cyan = "\033[36m";
const string blue = "\033[34m";
const string green = "\033[32m";
const string yellow = "\033[33m";
const string red = "\033[31m";
const string magenta = "\033[35m";
} // namespace Style

struct Task {
    int id;
    string title;
    string category;
    int priority;
    bool completed;
};

const string dataFile = "todo_tasks.txt";
vector<Task> tasks;

void clearScreen() {
    cout << "\033[2J\033[H";
}

void line(char character = '-') {
    cout << Style::blue << string(70, character) << Style::reset << '\n';
}

string trim(const string &value) {
    const size_t start = value.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    const size_t end = value.find_last_not_of(" \t\r\n");
    return value.substr(start, end - start + 1);
}

string safeField(string value) {
    replace(value.begin(), value.end(), '|', '/');
    return value;
}

string lower(string value) {
    transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(tolower(ch));
    });
    return value;
}

string priorityName(int priority) {
    if (priority == 3) return Style::red + "HIGH" + Style::reset;
    if (priority == 2) return Style::yellow + "MEDIUM" + Style::reset;
    return Style::green + "LOW" + Style::reset;
}

string shortText(const string &value, size_t width) {
    if (value.size() <= width) return value;
    return value.substr(0, width - 3) + "...";
}

void saveTasks() {
    ofstream file(dataFile);
    for (const Task &task : tasks) {
        file << task.id << '|' << task.completed << '|' << task.priority << '|'
             << safeField(task.category) << '|' << safeField(task.title) << '\n';
    }
}

void loadTasks() {
    ifstream file(dataFile);
    string row;

    while (getline(file, row)) {
        string id, completed, priority, category, title;
        stringstream stream(row);
        if (getline(stream, id, '|') && getline(stream, completed, '|') &&
            getline(stream, priority, '|') && getline(stream, category, '|') &&
            getline(stream, title)) {
            try {
                tasks.push_back({stoi(id), title, category, stoi(priority), stoi(completed) != 0});
            } catch (const exception &) {
                // Skip a malformed saved task instead of stopping the program.
            }
        }
    }
}

int nextId() {
    int largest = 0;
    for (const Task &task : tasks) largest = max(largest, task.id);
    return largest + 1;
}

int readNumber(const string &prompt, int minimum, int maximum) {
    while (true) {
        cout << prompt;
        string input;
        getline(cin, input);
        stringstream stream(trim(input));
        int value;
        char extra;

        if (stream >> value && !(stream >> extra) && value >= minimum && value <= maximum) {
            return value;
        }
        cout << Style::red << "  Please enter a number from " << minimum << " to " << maximum << ".\n" << Style::reset;
    }
}

void waitForEnter() {
    cout << Style::dim << "\nPress Enter to return to the dashboard..." << Style::reset;
    string input;
    getline(cin, input);
}

void header() {
    cout << Style::cyan << Style::bold;
    cout << "\n  ████████╗ █████╗ ███████╗██╗  ██╗███████╗██╗      ██████╗ \n";
    cout << "  ╚══██╔══╝██╔══██╗██╔════╝██║ ██╔╝██╔════╝██║     ██╔═══██╗\n";
    cout << "     ██║   ███████║███████╗█████╔╝ █████╗  ██║     ██║   ██║\n";
    cout << "     ██║   ██╔══██║╚════██║██╔═██╗ ██╔══╝  ██║     ██║   ██║\n";
    cout << "     ██║   ██║  ██║███████║██║  ██╗██║     ███████╗╚██████╔╝\n";
    cout << "     ╚═╝   ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝╚═╝     ╚══════╝ ╚═════╝ \n";
    cout << Style::reset << "  " << Style::dim << "A focused command-center for your daily work" << Style::reset << "\n";
    line('=');
}

void dashboard() {
    int done = 0;
    int high = 0;
    for (const Task &task : tasks) {
        if (task.completed) ++done;
        if (!task.completed && task.priority == 3) ++high;
    }

    cout << "  " << Style::bold << "TODAY'S SNAPSHOT" << Style::reset << "\n\n";
    cout << "  " << Style::cyan << "Total" << Style::reset << ": " << tasks.size()
         << "    " << Style::green << "Completed" << Style::reset << ": " << done
         << "    " << Style::yellow << "Pending" << Style::reset << ": " << tasks.size() - done
         << "    " << Style::red << "High priority" << Style::reset << ": " << high << "\n";
    line();
}

void printTasks(const vector<Task> &list, const string &title) {
    clearScreen();
    header();
    cout << "  " << Style::bold << title << Style::reset << "\n\n";

    if (list.empty()) {
        cout << Style::dim << "  No tasks found. Your list is clear.\n" << Style::reset;
        waitForEnter();
        return;
    }

    cout << "  " << left << setw(5) << "ID" << setw(12) << "STATUS" << setw(10) << "PRIORITY"
         << setw(16) << "CATEGORY" << "TASK\n";
    line();
    for (const Task &task : list) {
        const string status = task.completed ? Style::green + "DONE" + Style::reset
                                             : Style::yellow + "PENDING" + Style::reset;
        cout << "  " << left << setw(5) << task.id << setw(21) << status << setw(19)
             << priorityName(task.priority) << setw(16) << shortText(task.category, 14)
             << shortText(task.title, 30) << '\n';
    }
    line();
    waitForEnter();
}

void addTask() {
    clearScreen();
    header();
    cout << "  " << Style::bold << "CREATE A NEW TASK" << Style::reset << "\n\n";

    string title;
    do {
        cout << "  Task title: ";
        getline(cin, title);
        title = trim(title);
        if (title.empty()) cout << Style::red << "  A task title is required.\n" << Style::reset;
    } while (title.empty());

    cout << "  Category (for example: Study, Personal, Work): ";
    string category;
    getline(cin, category);
    category = trim(category);
    if (category.empty()) category = "General";

    cout << "\n  Priority: 1. Low   2. Medium   3. High\n";
    int priority = readNumber("  Select priority: ", 1, 3);

    tasks.push_back({nextId(), title, category, priority, false});
    saveTasks();
    cout << "\n  " << Style::green << Style::bold << "✓ Task added successfully." << Style::reset << "\n";
    waitForEnter();
}

Task *findTask(int id) {
    for (Task &task : tasks) {
        if (task.id == id) return &task;
    }
    return nullptr;
}

void markComplete() {
    vector<Task> pending;
    for (const Task &task : tasks) if (!task.completed) pending.push_back(task);
    if (pending.empty()) {
        printTasks(pending, "COMPLETE A TASK");
        return;
    }

    printTasks(pending, "PENDING TASKS");
    clearScreen();
    header();
    int id = readNumber("  Enter the ID to mark complete: ", 1, nextId());
    Task *task = findTask(id);
    if (task == nullptr || task->completed) {
        cout << Style::red << "\n  That pending task was not found.\n" << Style::reset;
    } else {
        task->completed = true;
        saveTasks();
        cout << Style::green << "\n  ✓ \"" << task->title << "\" is complete. Great work!\n" << Style::reset;
    }
    waitForEnter();
}

void deleteTask() {
    if (tasks.empty()) {
        printTasks(tasks, "DELETE A TASK");
        return;
    }
    printTasks(tasks, "YOUR TASKS");
    clearScreen();
    header();
    int id = readNumber("  Enter the ID to delete: ", 1, nextId());
    auto iterator = find_if(tasks.begin(), tasks.end(), [id](const Task &task) { return task.id == id; });
    if (iterator == tasks.end()) {
        cout << Style::red << "\n  Task not found.\n" << Style::reset;
        waitForEnter();
        return;
    }

    cout << "  Delete \"" << iterator->title << "\"? (y/n): ";
    string confirmation;
    getline(cin, confirmation);
    if (lower(trim(confirmation)) == "y" || lower(trim(confirmation)) == "yes") {
        tasks.erase(iterator);
        saveTasks();
        cout << Style::green << "\n  ✓ Task deleted.\n" << Style::reset;
    } else {
        cout << Style::yellow << "\n  Delete cancelled.\n" << Style::reset;
    }
    waitForEnter();
}

void searchTasks() {
    clearScreen();
    header();
    cout << "  " << Style::bold << "SEARCH TASKS" << Style::reset << "\n\n  Search by task title or category: ";
    string query;
    getline(cin, query);
    query = lower(trim(query));

    vector<Task> results;
    for (const Task &task : tasks) {
        if (lower(task.title).find(query) != string::npos || lower(task.category).find(query) != string::npos) {
            results.push_back(task);
        }
    }
    printTasks(results, "SEARCH RESULTS");
}

int main() {
    loadTasks();

    while (true) {
        clearScreen();
        header();
        dashboard();
        cout << "  " << Style::bold << "1" << Style::reset << "  Create a task\n";
        cout << "  " << Style::bold << "2" << Style::reset << "  View all tasks\n";
        cout << "  " << Style::bold << "3" << Style::reset << "  View pending tasks\n";
        cout << "  " << Style::bold << "4" << Style::reset << "  Mark a task complete\n";
        cout << "  " << Style::bold << "5" << Style::reset << "  Delete a task\n";
        cout << "  " << Style::bold << "6" << Style::reset << "  Search tasks\n";
        cout << "  " << Style::bold << "7" << Style::reset << "  Exit\n\n";

        int choice = readNumber("  Select an option: ", 1, 7);
        if (choice == 1) addTask();
        else if (choice == 2) printTasks(tasks, "ALL TASKS");
        else if (choice == 3) {
            vector<Task> pending;
            for (const Task &task : tasks) if (!task.completed) pending.push_back(task);
            printTasks(pending, "PENDING TASKS");
        } else if (choice == 4) markComplete();
        else if (choice == 5) deleteTask();
        else if (choice == 6) searchTasks();
        else {
            clearScreen();
            header();
            cout << "  " << Style::green << Style::bold << "Your tasks are saved. See you next time!\n\n" << Style::reset;
            return 0;
        }
    }
}
