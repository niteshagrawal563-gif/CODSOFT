#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <chrono>
#include <map>
#include <termios.h>
#include <unistd.h>
#include <cmath>

using namespace std;

// ==========================================
// COLOR STYLES (ANSI)
// ==========================================
namespace Style {
    const string reset   = "\033[0m";
    const string bold    = "\033[1m";
    const string dim     = "\033[2m";
    const string italic  = "\033[3m";
    const string line    = "\033[4m";
    
    const string red     = "\033[31m";
    const string green   = "\033[32m";
    const string yellow  = "\033[33m";
    const string blue    = "\033[34m";
    const string magenta = "\033[35m";
    const string cyan    = "\033[36m";
    const string white   = "\033[37m";
    
    const string bg_blue = "\033[44m";
    const string bg_dark = "\033[40m";
}

// ==========================================
// DATA STRUCTURES
// ==========================================
struct Book {
    string isbn;
    string title;
    string author;
    string category;
    string publisher;
    int year;
    string shelfLocation;
    int totalCopies;
    int availableCopies;
};

struct Member {
    string id;
    string password;
    string name;
    string email;
    string phone;
    double outstandingFines;
    string role; // "Admin" or "Member"
};

struct Transaction {
    int id;
    string isbn;
    string memberId;
    time_t checkoutDate;
    time_t dueDate;
    time_t returnDate; // 0 if not returned yet
    double fineAmount;
    bool isReturned;
};

struct Reservation {
    int id;
    string isbn;
    string memberId;
    time_t reservationDate;
    bool isActive;
};

struct AuditLog {
    string timestamp;
    string category;
    string user;
    string message;
};

// ==========================================
// SYSTEM CLOCK & SIMULATION ENGINE
// ==========================================
long long virtualTimeOffset = 0; // offset in seconds

time_t getSystemTime() {
    return time(nullptr) + virtualTimeOffset;
}

string formatDate(time_t rawTime) {
    if (rawTime == 0) return "N/A";
    struct tm *timeinfo = localtime(&rawTime);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", timeinfo);
    return string(buffer);
}

string formatSimulatedClockIndicator() {
    time_t cur = getSystemTime();
    string ind = " [🕒 System Time: " + formatDate(cur);
    if (virtualTimeOffset > 0) {
        int days = virtualTimeOffset / (24 * 3600);
        ind += " (Simulated +" + to_string(days) + " Days) ]";
        return Style::bold + Style::yellow + ind + Style::reset;
    }
    ind += "]";
    return Style::bold + Style::cyan + ind + Style::reset;
}

// ==========================================
// STRING & INPUT UTILITIES
// ==========================================
string trim(const string &str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

string toLower(string str) {
    transform(str.begin(), str.end(), str.begin(), [](unsigned char c){ return tolower(c); });
    return str;
}

vector<string> split(const string &s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Masked password input (Unix termios)
string getMaskedPassword(const string &prompt) {
    cout << prompt;
    string password = "";
    char ch;
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    while (true) {
        ch = getchar();
        if (ch == '\n' || ch == '\r') {
            break;
        } else if (ch == 127 || ch == 8) { // Backspace
            if (!password.empty()) {
                password.pop_back();
                cout << "\b \b";
            }
        } else {
            password += ch;
            cout << "*";
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    cout << endl;
    return password;
}

int readInteger(const string &prompt, int minVal, int maxVal) {
    while (true) {
        cout << prompt;
        string input;
        getline(cin, input);
        stringstream ss(trim(input));
        int val;
        char extra;
        if (ss >> val && !(ss >> extra) && val >= minVal && val <= maxVal) {
            return val;
        }
        cout << Style::red << "Invalid input. Please enter a number between " << minVal << " and " << maxVal << "." << Style::reset << endl;
    }
}

double readDouble(const string &prompt, double minVal) {
    while (true) {
        cout << prompt;
        string input;
        getline(cin, input);
        stringstream ss(trim(input));
        double val;
        char extra;
        if (ss >> val && !(ss >> extra) && val >= minVal) {
            return val;
        }
        cout << Style::red << "Invalid input. Please enter a positive decimal number." << Style::reset << endl;
    }
}

string readNonEmptyString(const string &prompt) {
    while (true) {
        cout << prompt;
        string input;
        getline(cin, input);
        input = trim(input);
        if (!input.empty() && input.find('|') == string::npos) {
            return input;
        }
        if (input.empty()) {
            cout << Style::red << "Input cannot be empty." << Style::reset << endl;
        } else {
            cout << Style::red << "Input cannot contain the '|' character." << Style::reset << endl;
        }
    }
}

void clearScreen() {
    cout << "\033[2J\033[H";
}

void pressEnterToContinue() {
    cout << Style::dim << "\nPress Enter to continue..." << Style::reset;
    string dummy;
    getline(cin, dummy);
}

// ==========================================
// FILE PERSISTENCE & AUDIT LOGGING
// ==========================================
const string BOOK_FILE        = "library_books.txt";
const string MEMBER_FILE      = "library_members.txt";
const string TRANSACTION_FILE = "library_transactions.txt";
const string RESERVATION_FILE = "library_reservations.txt";
const string AUDIT_FILE       = "library_audit.log";

vector<Book> books;
vector<Member> members;
vector<Transaction> transactions;
vector<Reservation> reservations;
Member *currentLoggedInUser = nullptr;

void writeAuditLog(const string &category, const string &user, const string &message) {
    time_t cur = getSystemTime();
    string ts = formatDate(cur);
    ofstream logFile(AUDIT_FILE, ios::app);
    if (logFile.is_open()) {
        logFile << ts << "|" << category << "|" << user << "|" << message << "\n";
    }
}

void loadBooks() {
    books.clear();
    ifstream file(BOOK_FILE);
    if (!file.is_open()) return;
    string line;
    while (getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        vector<string> parts = split(line, '|');
        if (parts.size() >= 9) {
            try {
                Book b;
                b.isbn = parts[0];
                b.title = parts[1];
                b.author = parts[2];
                b.category = parts[3];
                b.publisher = parts[4];
                b.year = stoi(parts[5]);
                b.shelfLocation = parts[6];
                b.totalCopies = stoi(parts[7]);
                b.availableCopies = stoi(parts[8]);
                books.push_back(b);
            } catch(...) {}
        }
    }
}

void saveBooks() {
    ofstream file(BOOK_FILE);
    if (!file.is_open()) return;
    file << "# ISBN|Title|Author|Category|Publisher|Year|ShelfLocation|TotalCopies|AvailableCopies\n";
    for (const auto &b : books) {
        file << b.isbn << "|" << b.title << "|" << b.author << "|" << b.category << "|"
             << b.publisher << "|" << b.year << "|" << b.shelfLocation << "|"
             << b.totalCopies << "|" << b.availableCopies << "\n";
    }
}

void loadMembers() {
    members.clear();
    ifstream file(MEMBER_FILE);
    if (!file.is_open()) return;
    string line;
    while (getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        vector<string> parts = split(line, '|');
        if (parts.size() >= 7) {
            try {
                Member m;
                m.id = parts[0];
                m.password = parts[1];
                m.name = parts[2];
                m.email = parts[3];
                m.phone = parts[4];
                m.outstandingFines = stod(parts[5]);
                m.role = parts[6];
                members.push_back(m);
            } catch(...) {}
        }
    }
}

void saveMembers() {
    ofstream file(MEMBER_FILE);
    if (!file.is_open()) return;
    file << "# MemberID|Password|Name|Email|Phone|OutstandingFines|Role\n";
    for (const auto &m : members) {
        file << m.id << "|" << m.password << "|" << m.name << "|" << m.email << "|"
             << m.phone << "|" << fixed << setprecision(2) << m.outstandingFines << "|" << m.role << "\n";
    }
}

void loadTransactions() {
    transactions.clear();
    ifstream file(TRANSACTION_FILE);
    if (!file.is_open()) return;
    string line;
    while (getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        vector<string> parts = split(line, '|');
        if (parts.size() >= 8) {
            try {
                Transaction t;
                t.id = stoi(parts[0]);
                t.isbn = parts[1];
                t.memberId = parts[2];
                t.checkoutDate = stoll(parts[3]);
                t.dueDate = stoll(parts[4]);
                t.returnDate = stoll(parts[5]);
                t.fineAmount = stod(parts[6]);
                t.isReturned = (parts[7] == "1");
                transactions.push_back(t);
            } catch(...) {}
        }
    }
}

void saveTransactions() {
    ofstream file(TRANSACTION_FILE);
    if (!file.is_open()) return;
    file << "# TxID|ISBN|MemberID|CheckoutDate|DueDate|ReturnDate|FineAmount|IsReturned\n";
    for (const auto &t : transactions) {
        file << t.id << "|" << t.isbn << "|" << t.memberId << "|"
             << t.checkoutDate << "|" << t.dueDate << "|" << t.returnDate << "|"
             << fixed << setprecision(2) << t.fineAmount << "|" << (t.isReturned ? "1" : "0") << "\n";
    }
}

void loadReservations() {
    reservations.clear();
    ifstream file(RESERVATION_FILE);
    if (!file.is_open()) return;
    string line;
    while (getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        vector<string> parts = split(line, '|');
        if (parts.size() >= 5) {
            try {
                Reservation r;
                r.id = stoi(parts[0]);
                r.isbn = parts[1];
                r.memberId = parts[2];
                r.reservationDate = stoll(parts[3]);
                r.isActive = (parts[4] == "1");
                reservations.push_back(r);
            } catch(...) {}
        }
    }
}

void saveReservations() {
    ofstream file(RESERVATION_FILE);
    if (!file.is_open()) return;
    file << "# ResID|ISBN|MemberID|ReservationDate|IsActive\n";
    for (const auto &r : reservations) {
        file << r.id << "|" << r.isbn << "|" << r.memberId << "|"
             << r.reservationDate << "|" << (r.isActive ? "1" : "0") << "\n";
    }
}

void loadAllData() {
    loadBooks();
    loadMembers();
    loadTransactions();
    loadReservations();
}

void saveAllData() {
    saveBooks();
    saveMembers();
    saveTransactions();
    saveReservations();
}

// ==========================================
// CORE DOMAIN LOGIC
// ==========================================
Book* findBook(const string &isbn) {
    for (auto &b : books) {
        if (b.isbn == isbn) return &b;
    }
    return nullptr;
}

Member* findMember(const string &id) {
    for (auto &m : members) {
        if (m.id == id) return &m;
    }
    return nullptr;
}

double getFineRate() {
    return 0.50; // $0.50 per day
}

double calculateActiveFine(const Transaction &t) {
    if (t.isReturned) return t.fineAmount;
    time_t cur = getSystemTime();
    if (cur > t.dueDate) {
        double diffSecs = difftime(cur, t.dueDate);
        double days = ceil(diffSecs / (24 * 3600));
        if (days < 0) days = 0;
        return days * getFineRate();
    }
    return 0.0;
}

void updateOverdueFinesInternally() {
    for (auto &t : transactions) {
        if (!t.isReturned) {
            double fine = calculateActiveFine(t);
            // Update individual member balance if the calculated fine increased
            if (fine > 0.0) {
                Member *m = findMember(t.memberId);
                if (m) {
                    // Update fine for the transaction
                    t.fineAmount = fine;
                }
            }
        }
    }
    // Aggregate fines in members
    for (auto &m : members) {
        double totalFine = 0.0;
        for (const auto &t : transactions) {
            if (t.memberId == m.id && !t.isReturned) {
                totalFine += calculateActiveFine(t);
            }
        }
        // Let's add previously accumulated fines if unpaid (could be kept in database outstandingFines)
        // For simplicity: member outstanding fines = sum of active transaction fines + manual adjustments.
        // We will keep outstandingFines updated with active transaction fines.
        m.outstandingFines = totalFine;
    }
    saveAllData();
}

// ==========================================
// INTERACTIVE GRAPHICS & BORDERS
// ==========================================
void drawBorder(int width = 80, char borderChar = '=') {
    cout << Style::blue << string(width, borderChar) << Style::reset << "\n";
}

void drawDoubleBorder(int width = 80) {
    // Beautiful blocky border
    cout << Style::cyan << string(width, '=') << Style::reset << "\n";
}

void printBanner() {
    clearScreen();
  cout << Style::bold << Style::cyan;
cout << "╔════════════════════════════════════════════════════════════════════════════════════╗\n";
cout << "║                                                                                    ║\n";
cout << "║      ██╗     ██╗██████╗ ██████╗  █████╗ ██████╗ ██╗   ██╗    ██╗  ██╗██╗   ██╗██████╗      ║\n";
cout << "║      ██║     ██║██╔══██╗██╔══██╗██╔══██╗██╔══██╗╚██╗ ██╔╝    ██║  ██║██║   ██║██╔══██╗     ║\n";
cout << "║      ██║     ██║██████╔╝██████╔╝███████║██████╔╝ ╚████╔╝     ███████║██║   ██║██████╔╝     ║\n";
cout << "║      ██║     ██║██╔══██╗██╔══██╗██╔══██║██╔══██╗  ╚██╔╝      ██╔══██║██║   ██║██╔══██╗     ║\n";
cout << "║      ███████╗██║██████╔╝██║  ██║██║  ██║██║  ██║   ██║       ██║  ██║╚██████╔╝██████╔╝     ║\n";
cout << "║      ╚══════╝╚═╝╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝       ╚═╝  ╚═╝ ╚═════╝ ╚═════╝      ║\n";
cout << "║                                                                                    ║\n";
cout << "║                         COMMERCIAL LIBRARY MANAGEMENT SYSTEM                       ║\n";
cout << "║                  Secure • Smart • Reliable • Efficient                            ║\n";
cout << "║                                                                                    ║\n";
cout << "╚════════════════════════════════════════════════════════════════════════════════════╝\n";
cout << Style::reset;
    cout << formatSimulatedClockIndicator() << "\n\n";
}

void drawSectionHeader(const string &title) {
    int totalWidth = 80;
    int titleWidth = title.length() + 4;
    int padding = (totalWidth - titleWidth) / 2;
    
    drawBorder(totalWidth, '-');
    cout << string(padding, ' ') << "« " << Style::bold << Style::magenta << title << Style::reset << " »" << endl;
    drawBorder(totalWidth, '-');
}

// ==========================================
// PORTALS: AUTHENTICATION GATEWAY
// ==========================================
void registerMember() {
    printBanner();
    drawSectionHeader("REGISTER NEW MEMBERSHIP");
    
    string name = readNonEmptyString("Enter Full Name: ");
    string email = readNonEmptyString("Enter Email Address: ");
    string phone = readNonEmptyString("Enter Phone Number: ");
    
    // Generate new unique ID
    int largest = 1000;
    for (const auto &m : members) {
        try {
            int currentId = stoi(m.id);
            if (currentId > largest) largest = currentId;
        } catch(...) {}
    }
    string newId = to_string(largest + 1);
    
    string password = getMaskedPassword("Create secure password: ");
    while (trim(password).empty()) {
        cout << Style::red << "Password cannot be empty.\n" << Style::reset;
        password = getMaskedPassword("Create secure password: ");
    }
    
    Member newMember;
    newMember.id = newId;
    newMember.password = password;
    newMember.name = name;
    newMember.email = email;
    newMember.phone = phone;
    newMember.outstandingFines = 0.0;
    newMember.role = "Member";
    
    members.push_back(newMember);
    saveMembers();
    
    writeAuditLog("USER_REGISTRATION", "SYSTEM", "New member registered: ID " + newId + " (" + name + ")");
    
    cout << "\n" << Style::bold << Style::green << "✔ Registration Successful!" << Style::reset << "\n";
    cout << "Your Account ID is: " << Style::bold << Style::yellow << newId << Style::reset << "\n";
    cout << "Keep this ID safe. You will need it to login.\n";
    pressEnterToContinue();
}

bool loginPortal() {
    printBanner();
    drawSectionHeader("SECURE LOGIN ENTRYWAY");
    
    string id = readNonEmptyString("Enter User ID or Username: ");
    string password = getMaskedPassword("Enter Security Password: ");
    
    updateOverdueFinesInternally();
    
    for (auto &m : members) {
        if (m.id == id && m.password == password) {
            currentLoggedInUser = &m;
            writeAuditLog("USER_LOGIN", m.id, "Successfully logged in (" + m.role + ")");
            cout << "\n" << Style::bold << Style::green << "✔ Login Successful! Access Granted." << Style::reset << "\n";
            cout << "Welcome back, " << Style::bold << m.name << Style::reset << " [" << m.role << "]\n";
            pressEnterToContinue();
            return true;
        }
    }
    
    // Fallback support for admin/admin123 username
    if (id == "admin") {
        for (auto &m : members) {
            if (m.role == "Admin" && m.password == password) {
                currentLoggedInUser = &m;
                writeAuditLog("USER_LOGIN", m.id, "Successfully logged in as admin");
                cout << "\n" << Style::bold << Style::green << "✔ Login Successful! Access Granted." << Style::reset << "\n";
                cout << "Welcome back, " << Style::bold << m.name << Style::reset << " [" << m.role << "]\n";
                pressEnterToContinue();
                return true;
            }
        }
    }
    
    cout << "\n" << Style::bold << Style::red << "✘ Authentication Failed! Invalid Credentials." << Style::reset << "\n";
    writeAuditLog("LOGIN_FAILURE", "UNKNOWN", "Failed login attempt for ID: " + id);
    pressEnterToContinue();
    return false;
}

// ==========================================
// MEMBER PANEL FEATURES
// ==========================================
void viewCatalogPaginated(bool memberMode = true) {
    int pageSize = 5;
    int totalBooks = books.size();
    if (totalBooks == 0) {
        cout << Style::yellow << "\nThe library catalog is currently empty." << Style::reset << "\n";
        pressEnterToContinue();
        return;
    }
    
    int totalPages = ceil((double)totalBooks / pageSize);
    int currentPage = 1;
    
    while (true) {
        printBanner();
        drawSectionHeader("LIBRARY CATALOG (Page " + to_string(currentPage) + " of " + to_string(totalPages) + ")");
        
        // Print table headers
        cout << left << setw(15) << "ISBN" 
             << setw(25) << "Title" 
             << setw(20) << "Author" 
             << setw(12) << "Category" 
             << setw(8) << "Status" << "\n";
        drawBorder(80, '-');
        
        int startIdx = (currentPage - 1) * pageSize;
        int endIdx = min(startIdx + pageSize, totalBooks);
        
        for (int i = startIdx; i < endIdx; ++i) {
            const auto &b = books[i];
            string statusStr = to_string(b.availableCopies) + "/" + to_string(b.totalCopies) + " Avail";
            if (b.availableCopies == 0) {
                statusStr = Style::red + "BORROWED" + Style::reset;
            } else {
                statusStr = Style::green + statusStr + Style::reset;
            }
            
            // Limit title and author display length to avoid table breaking
            string dispTitle = b.title.length() > 23 ? b.title.substr(0, 20) + "..." : b.title;
            string dispAuthor = b.author.length() > 18 ? b.author.substr(0, 15) + "..." : b.author;
            string dispCategory = b.category.length() > 11 ? b.category.substr(0, 9) + "..." : b.category;
            
            cout << left << setw(15) << b.isbn 
                 << setw(25) << dispTitle 
                 << setw(20) << dispAuthor 
                 << setw(12) << dispCategory 
                 << setw(8) << statusStr << "\n";
        }
        
        drawBorder(80, '-');
        cout << "Commands: [N] Next Page | [P] Previous Page | [S] Search Book | [R] Reserve Book | [Q] Quit Catalog\n";
        cout << "Enter command: ";
        string cmd;
        getline(cin, cmd);
        cmd = toLower(trim(cmd));
        
        if (cmd == "n") {
            if (currentPage < totalPages) currentPage++;
            else {
                cout << Style::red << "Already on the last page.\n" << Style::reset;
                sleep(1);
            }
        } else if (cmd == "p") {
            if (currentPage > 1) currentPage--;
            else {
                cout << Style::red << "Already on the first page.\n" << Style::reset;
                sleep(1);
            }
        } else if (cmd == "q") {
            break;
        } else if (cmd == "s") {
            printBanner();
            drawSectionHeader("CATALOG SEARCH");
            string query = toLower(readNonEmptyString("Enter search query (ISBN, Title, Author, or Category): "));
            
            cout << "\nSearch Results:\n";
            cout << left << setw(15) << "ISBN" 
                 << setw(25) << "Title" 
                 << setw(20) << "Author" 
                 << setw(12) << "Category" 
                 << setw(8) << "Status" << "\n";
            drawBorder(80, '-');
            
            int resultsCount = 0;
            for (const auto &b : books) {
                if (toLower(b.isbn).find(query) != string::npos ||
                    toLower(b.title).find(query) != string::npos ||
                    toLower(b.author).find(query) != string::npos ||
                    toLower(b.category).find(query) != string::npos) {
                    
                    string statusStr = to_string(b.availableCopies) + "/" + to_string(b.totalCopies) + " Avail";
                    if (b.availableCopies == 0) {
                        statusStr = Style::red + "BORROWED" + Style::reset;
                    } else {
                        statusStr = Style::green + statusStr + Style::reset;
                    }
                    
                    string dispTitle = b.title.length() > 23 ? b.title.substr(0, 20) + "..." : b.title;
                    string dispAuthor = b.author.length() > 18 ? b.author.substr(0, 15) + "..." : b.author;
                    string dispCategory = b.category.length() > 11 ? b.category.substr(0, 9) + "..." : b.category;
                    
                    cout << left << setw(15) << b.isbn 
                         << setw(25) << dispTitle 
                         << setw(20) << dispAuthor 
                         << setw(12) << dispCategory 
                         << setw(8) << statusStr << "\n";
                    resultsCount++;
                }
            }
            if (resultsCount == 0) {
                cout << Style::yellow << "No books found matching your query.\n" << Style::reset;
            }
            drawBorder(80, '-');
            pressEnterToContinue();
        } else if (cmd == "r") {
            if (!memberMode) {
                cout << Style::red << "Reservations can only be requested from a Member account.\n" << Style::reset;
                pressEnterToContinue();
                continue;
            }
            string isbn = readNonEmptyString("Enter Book ISBN to reserve: ");
            Book *b = findBook(isbn);
            if (!b) {
                cout << Style::red << "Book with ISBN " << isbn << " not found.\n" << Style::reset;
                pressEnterToContinue();
                continue;
            }
            if (b->availableCopies > 0) {
                cout << Style::yellow << "This book is currently available. You can borrow it directly at the Circulation Desk!\n" << Style::reset;
                pressEnterToContinue();
                continue;
            }
            
            // Check if already reserved
            bool already = false;
            for (const auto &r : reservations) {
                if (r.isbn == isbn && r.memberId == currentLoggedInUser->id && r.isActive) {
                    already = true;
                    break;
                }
            }
            if (already) {
                cout << Style::red << "You have already reserved this book.\n" << Style::reset;
                pressEnterToContinue();
                continue;
            }
            
            // Create reservation queue item
            int nextResId = 1;
            if (!reservations.empty()) nextResId = reservations.back().id + 1;
            
            Reservation newRes;
            newRes.id = nextResId;
            newRes.isbn = isbn;
            newRes.memberId = currentLoggedInUser->id;
            newRes.reservationDate = getSystemTime();
            newRes.isActive = true;
            
            reservations.push_back(newRes);
            saveReservations();
            
            writeAuditLog("BOOK_RESERVATION", currentLoggedInUser->id, "Reserved book: ISBN " + isbn);
            cout << Style::bold << Style::green << "✔ Hold placed successfully! You will be notified when this book is returned.\n" << Style::reset;
            pressEnterToContinue();
        } else {
            cout << Style::red << "Unknown Catalog Command.\n" << Style::reset;
            sleep(1);
        }
    }
}

void viewActiveCheckouts(const string &memberId) {
    printBanner();
    drawSectionHeader("YOUR ACTIVE BORROWINGS & FINES");
    
    updateOverdueFinesInternally();
    
    cout << left << setw(8) << "TxID"
         << setw(35) << "Book Title"
         << setw(17) << "Due Date"
         << setw(10) << "Fine Due"
         << setw(10) << "Status" << "\n";
    drawBorder(80, '-');
    
    int count = 0;
    for (const auto &t : transactions) {
        if (t.memberId == memberId && !t.isReturned) {
            Book *b = findBook(t.isbn);
            string title = b ? b->title : "Unknown Book";
            if (title.length() > 32) title = title.substr(0, 29) + "...";
            
            double fine = calculateActiveFine(t);
            string statusStr = Style::green + "Borrowing" + Style::reset;
            if (getSystemTime() > t.dueDate) {
                statusStr = Style::bold + Style::red + "OVERDUE" + Style::reset;
            }
            
            cout << left << setw(8) << t.id
                 << setw(35) << title
                 << setw(17) << formatDate(t.dueDate).substr(0, 10)
                 << "$" << fixed << setprecision(2) << setw(9) << fine
                 << setw(10) << statusStr << "\n";
            count++;
        }
    }
    if (count == 0) {
        cout << Style::yellow << "You currently have no active borrowings.\n" << Style::reset;
    }
    
    drawBorder(80, '-');
    Member *m = findMember(memberId);
    if (m) {
        cout << Style::bold << "Total Aggregate Fine Balance: " << Style::red << "$" << m->outstandingFines << Style::reset << "\n";
    }
    pressEnterToContinue();
}

void payFinesMember() {
    printBanner();
    drawSectionHeader("PAY OUTSTANDING FINES");
    
    updateOverdueFinesInternally();
    
    if (currentLoggedInUser->outstandingFines <= 0.001) {
        cout << Style::green << "You have no outstanding fines! Thank you for maintaining a clean record.\n" << Style::reset;
        pressEnterToContinue();
        return;
    }
    
    cout << "Your Outstanding Fines: " << Style::bold << Style::red << "$" << currentLoggedInUser->outstandingFines << Style::reset << "\n\n";
    double amt = readDouble("Enter Amount to Pay ($): ", 0.01);
    
    if (amt > currentLoggedInUser->outstandingFines) {
        cout << Style::yellow << "Payment exceeds fine balance. Adjusting to exact fine amount: $" << currentLoggedInUser->outstandingFines << "\n" << Style::reset;
        amt = currentLoggedInUser->outstandingFines;
    }
    
    currentLoggedInUser->outstandingFines -= amt;
    
    // Deduct fine amount proportionally from the overdue transactions
    double remainingPayment = amt;
    for (auto &t : transactions) {
        if (t.memberId == currentLoggedInUser->id && !t.isReturned && t.fineAmount > 0.0) {
            if (remainingPayment >= t.fineAmount) {
                remainingPayment -= t.fineAmount;
                t.fineAmount = 0.0;
            } else {
                t.fineAmount -= remainingPayment;
                remainingPayment = 0.0;
                break;
            }
        }
    }
    
    saveAllData();
    writeAuditLog("FINE_PAYMENT", currentLoggedInUser->id, "Paid outstanding fine: $" + to_string(amt));
    
    cout << "\n" << Style::bold << Style::green << "✔ Payment of $" << fixed << setprecision(2) << amt << " recorded successfully!" << Style::reset << "\n";
    cout << "New Outstanding Fines Balance: " << Style::bold << Style::red << "$" << currentLoggedInUser->outstandingFines << Style::reset << "\n";
    pressEnterToContinue();
}

void viewMyReservations() {
    printBanner();
    drawSectionHeader("YOUR ACTIVE RESERVATION QUEUE");
    
    cout << left << setw(8) << "ResID"
         << setw(15) << "ISBN"
         << setw(35) << "Book Title"
         << setw(20) << "Hold Date" << "\n";
    drawBorder(80, '-');
    
    int count = 0;
    for (const auto &r : reservations) {
        if (r.memberId == currentLoggedInUser->id && r.isActive) {
            Book *b = findBook(r.isbn);
            string title = b ? b->title : "Unknown Title";
            if (title.length() > 32) title = title.substr(0, 29) + "...";
            
            cout << left << setw(8) << r.id
                 << setw(15) << r.isbn
                 << setw(35) << title
                 << setw(20) << formatDate(r.reservationDate).substr(0, 10) << "\n";
            count++;
        }
    }
    
    if (count == 0) {
        cout << Style::yellow << "You currently have no active holds.\n" << Style::reset;
    }
    drawBorder(80, '-');
    pressEnterToContinue();
}

void memberPanel() {
    while (true) {
        printBanner();
        drawSectionHeader("MEMBER CONTROL PORTAL");
        
        cout << "  " << Style::bold << "1" << Style::reset << " 🔍 Browse & Search Library Catalog\n";
        cout << "  " << Style::bold << "2" << Style::reset << " 📂 View Active Checkouts & Fine Tracker\n";
        cout << "  " << Style::bold << "3" << Style::reset << " ⏳ View My Hold Reservations\n";
        cout << "  " << Style::bold << "4" << Style::reset << " 💳 Pay Outstanding Fines\n";
        cout << "  " << Style::bold << "5" << Style::reset << " 🚪 Logout and Exit Portal\n\n";
        
        int choice = readInteger("Select Option (1-5): ", 1, 5);
        if (choice == 1) {
            viewCatalogPaginated(true);
        } else if (choice == 2) {
            viewActiveCheckouts(currentLoggedInUser->id);
        } else if (choice == 3) {
            viewMyReservations();
        } else if (choice == 4) {
            payFinesMember();
        } else {
            writeAuditLog("USER_LOGOUT", currentLoggedInUser->id, "User logged out");
            currentLoggedInUser = nullptr;
            cout << Style::yellow << "\nSecurely logging out of Member Portal..." << Style::reset << endl;
            sleep(1);
            break;
        }
    }
}

// ==========================================
// LIBRARIAN PANEL FEATURES (ADMIN)
// ==========================================
void viewSystemStatistics() {
    printBanner();
    drawSectionHeader("LIBRARY STATISTICS & INSIGHTS");
    
    updateOverdueFinesInternally();
    
    int totalBooksInSystem = 0;
    int totalBookCopies = 0;
    int totalActiveCheckouts = 0;
    double totalOutstandingFines = 0.0;
    
    totalBooksInSystem = books.size();
    for (const auto &b : books) {
        totalBookCopies += b.totalCopies;
    }
    for (const auto &t : transactions) {
        if (!t.isReturned) totalActiveCheckouts++;
    }
    for (const auto &m : members) {
        totalOutstandingFines += m.outstandingFines;
    }
    
    // Most popular book calculation
    map<string, int> popMap;
    for (const auto &t : transactions) {
        popMap[t.isbn]++;
    }
    string popularIsbn = "N/A";
    int popMax = 0;
    for (const auto &p : popMap) {
        if (p.second > popMax) {
            popMax = p.second;
            popularIsbn = p.first;
        }
    }
    Book *popularBook = findBook(popularIsbn);
    string popularTitle = popularBook ? popularBook->title : "None";
    
    cout << "  " << Style::bold << "GENERAL METRICS" << Style::reset << "\n";
    cout << "  --------------------------------------------------\n";
    cout << "  Total Unique Book Titles:        " << Style::cyan << totalBooksInSystem << Style::reset << "\n";
    cout << "  Total Book Inventory Copies:     " << Style::cyan << totalBookCopies << Style::reset << "\n";
    cout << "  Current Active Checkouts:        " << Style::bold << Style::yellow << totalActiveCheckouts << Style::reset << "\n";
    cout << "  Total Active Members Registered: " << members.size() - 1 << " (excluding admins)\n";
    cout << "  Total Fines Currently Unpaid:    " << Style::bold << Style::red << "$" << totalOutstandingFines << Style::reset << "\n";
    cout << "  Total Hold Reservations:         " << reservations.size() << "\n\n";
    
    cout << "  " << Style::bold << "POPULARITY INDEX" << Style::reset << "\n";
    cout << "  --------------------------------------------------\n";
    cout << "  Most Checked-Out Title:          " << Style::bold << Style::green << popularTitle << Style::reset 
         << " (" << popMax << " borrows)\n";
         
    drawBorder(80, '=');
    pressEnterToContinue();
}

void addNewBook() {
    printBanner();
    drawSectionHeader("ADD NEW BOOK TO CATALOG");
    
    string isbn = readNonEmptyString("Enter Book ISBN (10 or 13 digits): ");
    Book *existing = findBook(isbn);
    if (existing) {
        cout << Style::yellow << "\nISBN already exists in database. Updating copies count instead." << Style::reset << endl;
        int additionalCopies = readInteger("Enter additional copy count to add (1-100): ", 1, 100);
        existing->totalCopies += additionalCopies;
        existing->availableCopies += additionalCopies;
        saveBooks();
        writeAuditLog("INVENTORY_UPDATE", currentLoggedInUser->id, "Increased ISBN " + isbn + " stock by " + to_string(additionalCopies));
        cout << Style::bold << Style::green << "✔ Inventory updated. Total copies: " << existing->totalCopies << Style::reset << endl;
        pressEnterToContinue();
        return;
    }
    
    string title = readNonEmptyString("Enter Book Title: ");
    string author = readNonEmptyString("Enter Author Name: ");
    string category = readNonEmptyString("Enter Genre/Category (e.g. Science, Fiction): ");
    string publisher = readNonEmptyString("Enter Publisher: ");
    int year = readInteger("Enter Publication Year: ", 1000, 2027);
    string shelf = readNonEmptyString("Enter Shelf Location (e.g., Aisle 4, Row F): ");
    int copies = readInteger("Enter Total Inventory Copies (1-500): ", 1, 500);
    
    Book b;
    b.isbn = isbn;
    b.title = title;
    b.author = author;
    b.category = category;
    b.publisher = publisher;
    b.year = year;
    b.shelfLocation = shelf;
    b.totalCopies = copies;
    b.availableCopies = copies;
    
    books.push_back(b);
    saveBooks();
    
    writeAuditLog("ADD_BOOK", currentLoggedInUser->id, "Added new book: " + title + " (ISBN: " + isbn + ")");
    cout << "\n" << Style::bold << Style::green << "✔ Book added successfully to database." << Style::reset << endl;
    pressEnterToContinue();
}

void viewAllMembers() {
    printBanner();
    drawSectionHeader("REGISTERED LIBRARY MEMBERS");
    
    cout << left << setw(10) << "MemberID"
         << setw(20) << "Name"
         << setw(25) << "Email"
         << setw(13) << "Phone"
         << setw(10) << "Fine Balance" << "\n";
    drawBorder(80, '-');
    
    for (const auto &m : members) {
        if (m.role == "Admin") continue; // Hide admin accounts from simple catalog lists
        
        string nameDisp = m.name.length() > 18 ? m.name.substr(0, 15) + "..." : m.name;
        string emailDisp = m.email.length() > 23 ? m.email.substr(0, 20) + "..." : m.email;
        
        cout << left << setw(10) << m.id
             << setw(20) << nameDisp
             << setw(25) << emailDisp
             << setw(13) << m.phone
             << "$" << fixed << setprecision(2) << setw(9) << m.outstandingFines << "\n";
    }
    
    drawBorder(80, '-');
    pressEnterToContinue();
}

void checkOutBookDesk() {
    printBanner();
    drawSectionHeader("CIRCULATION DESK - BOOK CHECKOUT");
    
    string memberId = readNonEmptyString("Enter Member ID: ");
    Member *m = findMember(memberId);
    if (!m || m->role == "Admin") {
        cout << Style::red << "Error: Member ID not registered.\n" << Style::reset;
        pressEnterToContinue();
        return;
    }
    
    // Check if member has critical overdue fines preventing checkout
    if (m->outstandingFines > 10.00) {
        cout << Style::bold << Style::red << "Checkout Blocked! Member has outstanding unpaid fines of $" 
             << fixed << setprecision(2) << m->outstandingFines 
             << " (Limit is $10.00)." << Style::reset << endl;
        pressEnterToContinue();
        return;
    }
    
    string isbn = readNonEmptyString("Enter Book ISBN: ");
    Book *b = findBook(isbn);
    if (!b) {
        cout << Style::red << "Error: Book ISBN not registered in inventory catalog.\n" << Style::reset;
        pressEnterToContinue();
        return;
    }
    
    if (b->availableCopies <= 0) {
        cout << Style::yellow << "Book is currently unavailable. No copies are on the shelf.\n" << Style::reset;
        cout << "Would you like to place a Reservation Hold for this member? (Y/N): ";
        string ans;
        getline(cin, ans);
        if (toLower(trim(ans)) == "y") {
            int nextResId = 1;
            if (!reservations.empty()) nextResId = reservations.back().id + 1;
            
            Reservation newRes;
            newRes.id = nextResId;
            newRes.isbn = isbn;
            newRes.memberId = memberId;
            newRes.reservationDate = getSystemTime();
            newRes.isActive = true;
            
            reservations.push_back(newRes);
            saveReservations();
            
            writeAuditLog("BOOK_RESERVATION", currentLoggedInUser->id, "Librarian reserved ISBN " + isbn + " for member " + memberId);
            cout << Style::bold << Style::green << "✔ Hold placed successfully!\n" << Style::reset;
        }
        pressEnterToContinue();
        return;
    }
    
    // Proceed with checkout transaction
    int nextTxId = 10001;
    if (!transactions.empty()) {
        nextTxId = transactions.back().id + 1;
    }
    
    b->availableCopies--;
    
    Transaction t;
    t.id = nextTxId;
    t.isbn = isbn;
    t.memberId = memberId;
    t.checkoutDate = getSystemTime();
    t.dueDate = t.checkoutDate + (14 * 24 * 3600); // 14 days loan period
    t.returnDate = 0;
    t.fineAmount = 0.0;
    t.isReturned = false;
    
    transactions.push_back(t);
    
    // If checkout resolves an active reservation of this member, mark it inactive
    for (auto &r : reservations) {
        if (r.isbn == isbn && r.memberId == memberId && r.isActive) {
            r.isActive = false;
        }
    }
    
    saveAllData();
    writeAuditLog("BOOK_CHECKOUT", currentLoggedInUser->id, "Checked out book " + isbn + " to member " + memberId + " (Tx ID " + to_string(nextTxId) + ")");
    
    cout << "\n" << Style::bold << Style::green << "✔ Checkout Successful!" << Style::reset << "\n";
    cout << "Book Title:  " << b->title << "\n";
    cout << "Due Date:    " << formatDate(t.dueDate).substr(0, 10) << "\n";
    pressEnterToContinue();
}

void checkInBookDesk() {
    printBanner();
    drawSectionHeader("CIRCULATION DESK - BOOK RETURN");
    
    string isbn = readNonEmptyString("Enter Book ISBN to return: ");
    Book *b = findBook(isbn);
    if (!b) {
        cout << Style::red << "Error: Book not found.\n" << Style::reset;
        pressEnterToContinue();
        return;
    }
    
    string memberId = readNonEmptyString("Enter Borrower Member ID: ");
    
    Transaction *targetTx = nullptr;
    for (auto &t : transactions) {
        if (t.isbn == isbn && t.memberId == memberId && !t.isReturned) {
            targetTx = &t;
            break;
        }
    }
    
    if (!targetTx) {
        cout << Style::red << "Error: No matching active checkout transaction found for this book and member.\n" << Style::reset;
        pressEnterToContinue();
        return;
    }
    
    time_t curTime = getSystemTime();
    targetTx->returnDate = curTime;
    targetTx->isReturned = true;
    
    // Check if overdue
    double fine = calculateActiveFine(*targetTx);
    if (fine > 0.0) {
        targetTx->fineAmount = fine;
        Member *m = findMember(memberId);
        if (m) {
            m->outstandingFines += fine;
        }
        cout << Style::bold << Style::red << "⚠ Book returned LATE! Fine assessed: $" 
             << fixed << setprecision(2) << fine << Style::reset << endl;
    } else {
        cout << Style::bold << Style::green << "✔ Returned on time! No fines incurred.\n" << Style::reset;
    }
    
    // Handle reservation holds queue for the returned book
    bool holdTriggered = false;
    for (auto &r : reservations) {
        if (r.isbn == isbn && r.isActive) {
            Member *reservedMember = findMember(r.memberId);
            if (reservedMember) {
                cout << Style::bold << Style::yellow << "★ NOTICE: Book has active reservation hold for Member " 
                     << reservedMember->name << " (ID " << r.memberId << ").\n"
                     << "This book is held on the hold shelf and cannot be borrowed by others." << Style::reset << "\n";
                holdTriggered = true;
                break;
            }
        }
    }
    
    if (!holdTriggered) {
        // Increment inventory since there are no active holds
        b->availableCopies++;
    }
    
    saveAllData();
    writeAuditLog("BOOK_RETURN", currentLoggedInUser->id, "Returned book " + isbn + " from member " + memberId + " (Tx ID " + to_string(targetTx->id) + ")");
    
    cout << Style::bold << Style::green << "✔ Return transaction processed successfully." << Style::reset << "\n";
    pressEnterToContinue();
}

void viewOutstandingCheckouts() {
    printBanner();
    drawSectionHeader("ACTIVE OVERDUE CHECKOUTS LIST");
    
    updateOverdueFinesInternally();
    
    cout << left << setw(8) << "TxID"
         << setw(12) << "MemID"
         << setw(28) << "Book Title"
         << setw(16) << "Due Date"
         << setw(10) << "Fine"
         << setw(10) << "Overdue" << "\n";
    drawBorder(80, '-');
    
    int count = 0;
    for (const auto &t : transactions) {
        if (!t.isReturned) {
            Book *b = findBook(t.isbn);
            string title = b ? b->title : "Unknown Title";
            if (title.length() > 25) title = title.substr(0, 22) + "...";
            
            double fine = calculateActiveFine(t);
            bool isOverdue = getSystemTime() > t.dueDate;
            string overdueStr = isOverdue ? Style::red + "YES" + Style::reset : Style::green + "NO" + Style::reset;
            
            cout << left << setw(8) << t.id
                 << setw(12) << t.memberId
                 << setw(28) << title
                 << setw(16) << formatDate(t.dueDate).substr(0, 10)
                 << "$" << fixed << setprecision(2) << setw(9) << fine
                 << setw(10) << overdueStr << "\n";
            count++;
        }
    }
    if (count == 0) {
        cout << Style::yellow << "No active checkouts in the system database.\n" << Style::reset;
    }
    drawBorder(80, '-');
    pressEnterToContinue();
}

void payFinesDesk() {
    printBanner();
    drawSectionHeader("COLLECT FINE PAYMENT");
    
    string memberId = readNonEmptyString("Enter Member ID: ");
    Member *m = findMember(memberId);
    if (!m || m->role == "Admin") {
        cout << Style::red << "Error: Member ID not registered.\n" << Style::reset;
        pressEnterToContinue();
        return;
    }
    
    updateOverdueFinesInternally();
    
    if (m->outstandingFines <= 0.001) {
        cout << Style::green << "This member has no outstanding fines.\n" << Style::reset;
        pressEnterToContinue();
        return;
    }
    
    cout << "Member Name:      " << m->name << "\n";
    cout << "Current Balance:  " << Style::bold << Style::red << "$" << m->outstandingFines << Style::reset << "\n\n";
    
    double amt = readDouble("Enter Amount to Pay ($): ", 0.01);
    if (amt > m->outstandingFines) {
        cout << Style::yellow << "Payment exceeds fine. Adjusting to exact amount: $" << m->outstandingFines << "\n" << Style::reset;
        amt = m->outstandingFines;
    }
    
    m->outstandingFines -= amt;
    
    // Deduct fine amount proportionally from the overdue transactions
    double remainingPayment = amt;
    for (auto &t : transactions) {
        if (t.memberId == m->id && !t.isReturned && t.fineAmount > 0.0) {
            if (remainingPayment >= t.fineAmount) {
                remainingPayment -= t.fineAmount;
                t.fineAmount = 0.0;
            } else {
                t.fineAmount -= remainingPayment;
                remainingPayment = 0.0;
                break;
            }
        }
    }
    
    saveAllData();
    writeAuditLog("FINE_COLLECTION", currentLoggedInUser->id, "Collected $" + to_string(amt) + " fine from member " + memberId);
    
    cout << "\n" << Style::bold << Style::green << "✔ Payment recorded successfully!" << Style::reset << "\n";
    cout << "New Member Outstanding Balance: " << Style::bold << Style::red << "$" << m->outstandingFines << Style::reset << "\n";
    pressEnterToContinue();
}

void systemLogsPanel() {
    printBanner();
    drawSectionHeader("SYSTEM AUDIT TRAIL LOGS");
    
    ifstream logFile(AUDIT_FILE);
    if (!logFile.is_open()) {
        cout << Style::yellow << "Audit log file is empty or missing.\n" << Style::reset;
        pressEnterToContinue();
        return;
    }
    
    string line;
    vector<string> logsList;
    while (getline(logFile, line)) {
        if (!line.empty() && line[0] != '#') logsList.push_back(line);
    }
    
    if (logsList.empty()) {
        cout << Style::yellow << "No logs in system audit file.\n" << Style::reset;
        pressEnterToContinue();
        return;
    }
    
    // Print logs in reverse chronological order (newest first)
    reverse(logsList.begin(), logsList.end());
    
    int page = 1;
    int logPageSize = 10;
    int totalLogPages = ceil((double)logsList.size() / logPageSize);
    
    while (true) {
        printBanner();
        drawSectionHeader("AUDIT LOG (Page " + to_string(page) + " of " + to_string(totalLogPages) + ")");
        
        cout << left << setw(17) << "Timestamp"
             << setw(20) << "Category"
             << setw(12) << "User"
             << "Message\n";
        drawBorder(80, '-');
        
        int start = (page - 1) * logPageSize;
        int end = min(start + logPageSize, (int)logsList.size());
        
        for (int i = start; i < end; ++i) {
            vector<string> p = split(logsList[i], '|');
            if (p.size() >= 4) {
                string msgDisp = p[3].length() > 30 ? p[3].substr(0, 27) + "..." : p[3];
                cout << left << setw(17) << p[0].substr(0, 16)
                     << setw(20) << p[1]
                     << setw(12) << p[2]
                     << p[3] << "\n";
            }
        }
        drawBorder(80, '-');
        cout << "Commands: [N] Next Page | [P] Previous Page | [Q] Quit Logs\n";
        cout << "Enter selection: ";
        string sel;
        getline(cin, sel);
        sel = toLower(trim(sel));
        
        if (sel == "n") {
            if (page < totalLogPages) page++;
        } else if (sel == "p") {
            if (page > 1) page--;
        } else if (sel == "q") {
            break;
        }
    }
}

void timeMachineTravel() {
    printBanner();
    drawSectionHeader("VIRTUAL SYSTEM TIME MACHINE");
    
    cout << "Welcome to the system time machine! Use this to fast-forward system\n";
    cout << "dates to check late returns, overdue fine calculations, and active blocks.\n\n";
    cout << "Current Simulated Date: " << Style::bold << Style::green << formatDate(getSystemTime()) << Style::reset << "\n";
    
    int days = readInteger("Enter number of DAYS to fast-forward into the future (0 to cancel): ", 0, 365);
    if (days > 0) {
        virtualTimeOffset += days * 24 * 3600;
        updateOverdueFinesInternally();
        writeAuditLog("TIME_TRAVEL", currentLoggedInUser->id, "Simulated time fast-forwarded by " + to_string(days) + " days");
        cout << "\n" << Style::bold << Style::green << "✔ Success! The clock has leaped " << days << " days into the future!" << Style::reset << "\n";
        cout << "New System Time: " << Style::bold << Style::yellow << formatDate(getSystemTime()) << Style::reset << "\n";
    }
    pressEnterToContinue();
}

void exportCSVReports() {
    printBanner();
    drawSectionHeader("EXPORT DATABASE REPORTS TO CSV");
    
    cout << "This tool will compile and export data into standard commercial format CSVs\n";
    cout << "saved directly in this repository folder.\n\n";
    
    // Books report
    ofstream bCsv("books_report.csv");
    bCsv << "ISBN,Title,Author,Category,Publisher,Year,ShelfLocation,TotalCopies,AvailableCopies\n";
    for (const auto &b : books) {
        bCsv << "\"" << b.isbn << "\",\"" << b.title << "\",\"" << b.author << "\",\"" << b.category << "\",\""
             << b.publisher << "\"," << b.year << ",\"" << b.shelfLocation << "\"," << b.totalCopies << "," << b.availableCopies << "\n";
    }
    bCsv.close();
    
    // Members report
    ofstream mCsv("members_report.csv");
    mCsv << "MemberID,Name,Email,Phone,OutstandingFines,Role\n";
    for (const auto &m : members) {
        mCsv << "\"" << m.id << "\",\"" << m.name << "\",\"" << m.email << "\",\"" << m.phone << "\"," << m.outstandingFines << ",\"" << m.role << "\"\n";
    }
    mCsv.close();
    
    writeAuditLog("CSV_EXPORT", currentLoggedInUser->id, "Exported system databases to CSV reports");
    cout << Style::bold << Style::green << "✔ Reports Generated Successfully!\n" << Style::reset;
    cout << "1. [books_report.csv](file:///Users/niteshagrawal/codesoft/CODSOFT/books_report.csv)\n";
    cout << "2. [members_report.csv](file:///Users/niteshagrawal/codesoft/CODSOFT/members_report.csv)\n\n";
    pressEnterToContinue();
}

void librarianPanel() {
    while (true) {
        printBanner();
        drawSectionHeader("LIBRARIAN ADMINISTRATIVE DASHBOARD");
        
        cout << "  " << Style::bold << "1" << Style::reset << " 📊 View Library Statistics & Index\n";
        cout << "  " << Style::bold << "2" << Style::reset << " 📚 Add New Book / Increase Stock\n";
        cout << "  " << Style::bold << "3" << Style::reset << " 🔍 Browse/Search Catalog & Reserve\n";
        cout << "  " << Style::bold << "4" << Style::reset << " 👥 View Registered Members\n";
        cout << "  " << Style::bold << "5" << Style::reset << " 🔄 Desk: Process Book Checkout\n";
        cout << "  " << Style::bold << "6" << Style::reset << " 🔄 Desk: Process Book Return\n";
        cout << "  " << Style::bold << "7" << Style::reset << " 📂 View Active Outstanding Checkouts\n";
        cout << "  " << Style::bold << "8" << Style::reset << " 💰 Fines Desk: Collect Fine Payment\n";
        cout << "  " << Style::bold << "9" << Style::reset << " 📜 System Logs & Security Audit Trail\n";
        cout << "  " << Style::bold << "10" << Style::reset << " 🕒 Virtual Clock: Simulated Time Machine\n";
        cout << "  " << Style::bold << "11" << Style::reset << " ⚙️ Export Library Reports to CSV\n";
        cout << "  " << Style::bold << "12" << Style::reset << " 🚪 Logout Administrative Dashboard\n\n";
        
        int choice = readInteger("Select Administrative Option (1-12): ", 1, 12);
        if (choice == 1) viewSystemStatistics();
        else if (choice == 2) addNewBook();
        else if (choice == 3) viewCatalogPaginated(false);
        else if (choice == 4) viewAllMembers();
        else if (choice == 5) checkOutBookDesk();
        else if (choice == 6) checkInBookDesk();
        else if (choice == 7) viewOutstandingCheckouts();
        else if (choice == 8) payFinesDesk();
        else if (choice == 9) systemLogsPanel();
        else if (choice == 10) timeMachineTravel();
        else if (choice == 11) exportCSVReports();
        else {
            writeAuditLog("USER_LOGOUT", currentLoggedInUser->id, "Admin logged out");
            currentLoggedInUser = nullptr;
            cout << Style::yellow << "\nClosing administrative session..." << Style::reset << endl;
            sleep(1);
            break;
        }
    }
}

// ==========================================
// MAIN CONTROLLER
// ==========================================
int main() {
    loadAllData();
    updateOverdueFinesInternally();
    
    while (true) {
        printBanner();
        drawSectionHeader("AUTHENTICATION ENTRY GATEWAY");
        
        cout << "  " << Style::bold << "1" << Style::reset << " 🔑 Member & Admin Security Login\n";
        cout << "  " << Style::bold << "2" << Style::reset << " 📝 Register New Library Membership\n";
        cout << "  " << Style::bold << "3" << Style::reset << " 🚪 Shutdown Library Management Terminal\n\n";
        
        int choice = readInteger("Select option (1-3): ", 1, 3);
        
        if (choice == 1) {
            if (loginPortal()) {
                if (currentLoggedInUser->role == "Admin") {
                    librarianPanel();
                } else {
                    memberPanel();
                }
            }
        } else if (choice == 2) {
            registerMember();
        } else {
            cout << "\nSaving databases and closing file system...\n";
            saveAllData();
            cout << Style::green << Style::bold << "✔ Database Saved. Systems shutdown complete. Goodbye!\n\n" << Style::reset;
            break;
        }
    }
    return 0;
}
