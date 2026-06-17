/*
 * ============================================================================
 * FILE: 15_banking_system.cpp
 * ============================================================================
 * BANKING MANAGEMENT SYSTEM - Real-world DSA Application
 *
 * EXAM FREQUENCY: High. Banking systems are classic DSA practical exam
 * projects that test multiple data structure concepts in one application.
 *
 * DATA STRUCTURES USED:
 *   1. Hash Table (unordered_map) - Customer accounts (O(1) lookup by account#)
 *      - Each account stores personal info, balance, transaction history
 *   2. Queue (FIFO) - Customer service / teller queue
 *      - Customers waiting for service are processed in arrival order
 *   3. Linked List (Doubly) - Transaction history per account
 *      - Each transaction links to next/prev for easy forward/backward review
 *   4. Stack - Undo operations / recent activity
 *      - Last N transactions can be "undone" (LIFO)
 *   5. Priority Queue - Loan/credit applications
 *      - Higher loan amounts or premium customers get priority processing
 *
 * REAL-WORLD SCENARIO:
 *   - Customers open accounts, deposit, withdraw, transfer money
 *   - Teller queue: customers wait in FIFO order for service
 *   - High-value transactions get priority processing
 *   - Transaction history is doubly linked for easy forward/back navigation
 *   - Recent transactions can be undone (stack behavior)
 * ============================================================================
 */
#include <iostream>
#include <string>
#include <queue>          // For teller queue and priority queue
#include <stack>          // For undo operations
#include <vector>
#include <unordered_map>  // For account lookup
#include <iomanip>        // For formatted output
#include <ctime>          // For timestamps
#include <sstream>        // For string formatting
using namespace std;

// ============================================================================
// SECTION 1: ENTITY STRUCTURES
// ============================================================================

// -------------------------------------------------------------------
// TRANSACTION STRUCTURE
// Records every banking transaction for audit/review.
// Each account maintains a doubly linked list of these records.
// -------------------------------------------------------------------
struct Transaction {
    string txnId;           // Unique transaction ID
    string type;            // DEPOSIT, WITHDRAWAL, TRANSFER_SENT, TRANSFER_RCVD
    double amount;          // Transaction amount
    double balanceBefore;   // Balance before this transaction
    double balanceAfter;    // Balance after this transaction
    string date;            // Timestamp
    string description;     // Optional description

    Transaction(string tid, string t, double amt, double balBefore,
                double balAfter, string desc = "")
        : txnId(tid), type(t), amount(amt), balanceBefore(balBefore),
          balanceAfter(balAfter), description(desc) {
        time_t now = time(nullptr);
        date = ctime(&now);
        if (!date.empty() && date.back() == '\n') date.pop_back();
    }
};

// -------------------------------------------------------------------
// DOUBLY LINKED LIST NODE for transaction history
//
// Why Doubly Linked List?
//   - Allows forward AND backward traversal of transactions
//   - Easy to add new transactions at the end (tail)
//   - O(1) insertion at tail and O(1) access to most recent
// ============================================================================

struct TxnNode {
    Transaction data;
    TxnNode* prev;  // Pointer to PREVIOUS transaction (older)
    TxnNode* next;  // Pointer to NEXT transaction (newer)

    TxnNode(Transaction t) : data(t), prev(nullptr), next(nullptr) {}
};

class TransactionList {
private:
    TxnNode* head;  // Oldest transaction (front)
    TxnNode* tail;  // Most recent transaction (back)
    int count;

public:
    TransactionList() : head(nullptr), tail(nullptr), count(0) {}

    ~TransactionList() {
        TxnNode* current = head;
        while (current) {
            TxnNode* temp = current;
            current = current->next;
            delete temp;
        }
    }

    // -------------------------------------------------------------------
    // ADD TRANSACTION at the end of the list. O(1) using tail pointer.
    // -------------------------------------------------------------------
    void addTransaction(Transaction t) {
        TxnNode* newNode = new TxnNode(t);
        if (!head) {
            head = tail = newNode;  // First transaction
        } else {
            tail->next = newNode;     // Link current tail -> new node
            newNode->prev = tail;     // New node -> current tail
            tail = newNode;           // Update tail to new node
        }
        count++;
    }

    // -------------------------------------------------------------------
    // REMOVE LAST TRANSACTION (for undo). O(1) using tail pointer.
    // Returns the removed transaction or empty if none.
    // -------------------------------------------------------------------
    Transaction removeLast() {
        if (!tail) return Transaction("", "", 0, 0, 0);

        TxnNode* toRemove = tail;
        Transaction t = toRemove->data;

        if (head == tail) {
            // Only one node
            head = tail = nullptr;
        } else {
            tail = tail->prev;   // Move tail backward
            tail->next = nullptr; // New tail has no next
        }
        delete toRemove;
        count--;
        return t;
    }

    // -------------------------------------------------------------------
    // DISPLAY TRANSACTIONS in reverse order (most recent first). O(n).
    // -------------------------------------------------------------------
    void displayRecent(int n = 10) const {
        if (!tail) {
            cout << "    No transactions.\n";
            return;
        }
        TxnNode* current = tail;
        int shown = 0;
        cout << "    " << left << setw(12) << "Type"
             << setw(12) << "Amount"
             << setw(12) << "Before"
             << setw(12) << "After"
             << "Date\n";
        cout << "    " << string(70, '-') << "\n";
        while (current && shown < n) {
            cout << "    " << left << setw(12) << current->data.type
                 << setw(12) << fixed << setprecision(2) << current->data.amount
                 << setw(12) << current->data.balanceBefore
                 << setw(12) << current->data.balanceAfter
                 << current->data.date.substr(0, 19) << "\n";
            current = current->prev;
            shown++;
        }
    }

    // -------------------------------------------------------------------
    // DISPLAY transaction history in forward order (oldest first). O(n).
    // -------------------------------------------------------------------
    void displayAll() const {
        if (!head) {
            cout << "    No transactions.\n";
            return;
        }
        TxnNode* current = head;
        cout << "    " << left << setw(12) << "Type"
             << setw(12) << "Amount"
             << setw(12) << "Before"
             << setw(12) << "After"
             << "Date\n";
        cout << "    " << string(70, '-') << "\n";
        while (current) {
            cout << "    " << left << setw(12) << current->data.type
                 << setw(12) << fixed << setprecision(2) << current->data.amount
                 << setw(12) << current->data.balanceBefore
                 << setw(12) << current->data.balanceAfter
                 << current->data.date.substr(0, 19) << "\n";
            current = current->next;
        }
    }

    bool isEmpty() const { return head == nullptr; }
    TxnNode* getTail() { return tail; }
    int getCount() { return count; }
};

// -------------------------------------------------------------------
// ACCOUNT STRUCTURE
// Each account has basic info and a doubly linked list of transactions.
// Accounts are stored in a HASH TABLE for O(1) lookup by account number.
// -------------------------------------------------------------------
struct Account {
    string accountNo;        // Unique account number (e.g., "ACC001")
    string holderName;       // Account holder's name
    string pin;              // Simple security PIN (4-digit string)
    double balance;          // Current balance
    bool isActive;           // Account status
    TransactionList history; // Doubly linked list of transactions
    int nextTxnId;           // For generating unique transaction IDs

    Account() : accountNo(""), holderName(""), pin(""),
                balance(0.0), isActive(true), nextTxnId(1) {}

    Account(string acc, string name, string p, double initialDep = 0.0)
        : accountNo(acc), holderName(name), pin(p),
          balance(initialDep), isActive(true), nextTxnId(1) {
        if (initialDep > 0) {
            addTransaction("DEPOSIT", initialDep, "Initial deposit");
        }
    }

    // -------------------------------------------------------------------
    // ADD TRANSACTION: Creates Transaction object and adds to linked list.
    // Updates balance automatically.
    // Returns the auto-generated transaction ID.
    // -------------------------------------------------------------------
    string addTransaction(string type, double amount, string desc = "") {
        string txnId = accountNo + "-" + to_string(nextTxnId++);
        double balBefore = balance;

        if (type == "DEPOSIT") {
            balance += amount;
        } else if (type == "WITHDRAWAL" || type == "TRANSFER_SENT") {
            balance -= amount;
        } // TRANSFER_RCVD: balance already updated via DEPOSIT

        double balAfter = balance;
        history.addTransaction(Transaction(txnId, type, amount,
                                           balBefore, balAfter, desc));
        return txnId;
    }

    // -------------------------------------------------------------------
    // UNDO LAST TRANSACTION: Removes from linked list, reverses balance.
    // Returns true if successful.
    // -------------------------------------------------------------------
    bool undoLastTransaction() {
        if (history.isEmpty()) {
            cout << "    No transactions to undo.\n";
            return false;
        }
        Transaction last = history.removeLast();
        // Reverse the balance change
        if (last.type == "DEPOSIT" || last.type == "TRANSFER_RCVD") {
            balance = last.balanceBefore;  // Reverse deposit
        } else if (last.type == "WITHDRAWAL" || last.type == "TRANSFER_SENT") {
            balance = last.balanceBefore;  // Reverse withdrawal
        }
        cout << "    Undone: " << last.type << " of $" << last.amount << "\n";
        return true;
    }

    void displaySummary() const {
        cout << "  Account: " << accountNo
             << " | Holder: " << holderName
             << " | Balance: $" << fixed << setprecision(2) << balance
             << " | Status: " << (isActive ? "Active" : "Frozen")
             << " | Txns: " << history.getCount() << "\n";
    }
};

// ============================================================================
// SECTION 2: CUSTOMER SERVICE QUEUE (FIFO)
//
// In a bank, customers wait in line for a teller.
// This queue processes them in FIRST-COME-FIRST-SERVED order.
// ============================================================================

class TellerQueue {
private:
    queue<string> customerQueue;  // Queue of account numbers
    int tellerCount;              // Number of active tellers
    int customersServed;          // Total customers served

public:
    TellerQueue(int tellers = 3) : tellerCount(tellers), customersServed(0) {}

    // -------------------------------------------------------------------
    // ADD CUSTOMER TO QUEUE: Enqueue account number. O(1).
    // -------------------------------------------------------------------
    void addCustomer(string accountNo) {
        customerQueue.push(accountNo);
        cout << "  [QUEUE] Customer " << accountNo << " added to teller queue.\n";
        cout << "  [QUEUE] Position: " << customerQueue.size() << " people ahead.\n";
    }

    // -------------------------------------------------------------------
    // CALL NEXT CUSTOMER: Dequeue. O(1).
    // Returns account number or empty string if queue is empty.
    // -------------------------------------------------------------------
    string callNext() {
        if (customerQueue.empty()) {
            cout << "  [QUEUE] No customers waiting.\n";
            return "";
        }
        string accountNo = customerQueue.front();
        customerQueue.pop();
        customersServed++;
        cout << "  [TELLER] Calling customer " << accountNo
             << " (Served today: " << customersServed << ")\n";
        return accountNo;
    }

    void displayQueue() {
        if (customerQueue.empty()) {
            cout << "  [QUEUE] Teller queue is empty.\n";
            return;
        }
        queue<string> temp = customerQueue;
        cout << "  [QUEUE] Customers waiting (" << customerQueue.size() << "):\n";
        int pos = 1;
        while (!temp.empty()) {
            cout << "    " << pos++ << ". " << temp.front() << "\n";
            temp.pop();
        }
    }
};

// ============================================================================
// SECTION 3: LOAN APPLICATION PRIORITY QUEUE
//
// High-value customers or premium accounts get loan priority.
// Priority = loanAmount * 0.7 + premiumWeight * 0.3
// ============================================================================

struct LoanApplication {
    string accountNo;
    double loanAmount;
    double interestRate;
    string purpose;       // Reason for loan
    double priorityScore; // Higher score = higher priority

    LoanApplication(string acc, double amt, double rate, string purposeStr)
        : accountNo(acc), loanAmount(amt), interestRate(rate),
          purpose(purposeStr) {
        // Priority: larger loans get slightly higher priority
        // (In a real bank, this would be based on credit score, relationship, etc.)
        priorityScore = loanAmount * 1.0;
    }

    // For MAX-HEAP: higher priorityScore = higher priority
    bool operator<(const LoanApplication& other) const {
        return priorityScore < other.priorityScore;
    }
};

// ============================================================================
// SECTION 4: MAIN BANKING SYSTEM
// ============================================================================

class BankingSystem {
private:
    unordered_map<string, Account> accounts;  // Hash: accountNo -> Account
    TellerQueue tellerQueue;                   // FIFO queue for teller service
    priority_queue<LoanApplication> loanQueue; // Max-heap for loan processing
    stack<string> activityLog;                 // Stack: recent activities
    int nextAccountNo;                         // For auto-generating account numbers

public:
    BankingSystem() : nextAccountNo(1001) {
        cout << "  Banking System initialized.\n";
    }

    // ==================================================================
    // ACCOUNT MANAGEMENT (Hash Table: O(1) average)
    // ==================================================================

    // -------------------------------------------------------------------
    // CREATE ACCOUNT: Add to hash table. O(1) average.
    // -------------------------------------------------------------------
    void createAccount(string name, string pin, double initialDeposit = 0) {
        string accNo = "ACC" + to_string(nextAccountNo++);
        accounts[accNo] = Account(accNo, name, pin, initialDeposit);
        cout << "  [OK] Account created!\n";
        cout << "  Account Number: " << accNo << "\n";
        cout << "  Holder: " << name << "\n";
        cout << "  Initial Balance: $" << fixed << setprecision(2)
             << initialDeposit << "\n";
        activityLog.push("CREATE:" + accNo);
    }

    // -------------------------------------------------------------------
    // FIND ACCOUNT: O(1) average using hash table.
    // -------------------------------------------------------------------
    Account* findAccount(string accNo) {
        auto it = accounts.find(accNo);
        if (it != accounts.end() && it->second.isActive) {
            return &(it->second);
        }
        return nullptr;
    }

    // -------------------------------------------------------------------
    // AUTHENTICATE: Simple PIN check for security.
    // -------------------------------------------------------------------
    bool authenticate(Account* acc, string pin) {
        return acc->pin == pin;
    }

    // -------------------------------------------------------------------
    // DEPOSIT: Add money to account + record transaction. O(1).
    // -------------------------------------------------------------------
    void deposit(string accNo, double amount, string pin) {
        Account* acc = findAccount(accNo);
        if (!acc) { cout << "  [ERROR] Account not found.\n"; return; }
        if (!authenticate(acc, pin)) { cout << "  [ERROR] Wrong PIN.\n"; return; }
        if (amount <= 0) { cout << "  [ERROR] Invalid amount.\n"; return; }

        acc->addTransaction("DEPOSIT", amount, "Cash deposit");
        cout << "  [OK] $" << fixed << setprecision(2) << amount
             << " deposited. New balance: $" << acc->balance << "\n";
    }

    // -------------------------------------------------------------------
    // WITHDRAWAL: Deduct money + record transaction. O(1).
    // -------------------------------------------------------------------
    void withdraw(string accNo, double amount, string pin) {
        Account* acc = findAccount(accNo);
        if (!acc) { cout << "  [ERROR] Account not found.\n"; return; }
        if (!authenticate(acc, pin)) { cout << "  [ERROR] Wrong PIN.\n"; return; }
        if (amount <= 0) { cout << "  [ERROR] Invalid amount.\n"; return; }
        if (amount > acc->balance) {
            cout << "  [ERROR] Insufficient balance. Available: $"
                 << acc->balance << "\n";
            return;
        }

        acc->addTransaction("WITHDRAWAL", amount, "Cash withdrawal");
        cout << "  [OK] $" << fixed << setprecision(2) << amount
             << " withdrawn. New balance: $" << acc->balance << "\n";
    }

    // -------------------------------------------------------------------
    // TRANSFER: Between two accounts. O(1) each (hash table).
    // -------------------------------------------------------------------
    void transfer(string fromAcc, string toAcc, double amount, string pin) {
        Account* sender = findAccount(fromAcc);
        Account* receiver = findAccount(toAcc);
        if (!sender) { cout << "  [ERROR] Sender account not found.\n"; return; }
        if (!receiver) { cout << "  [ERROR] Receiver account not found.\n"; return; }
        if (!authenticate(sender, pin)) { cout << "  [ERROR] Wrong PIN.\n"; return; }
        if (amount <= 0) { cout << "  [ERROR] Invalid amount.\n"; return; }
        if (amount > sender->balance) {
            cout << "  [ERROR] Insufficient balance.\n"; return;
        }

        sender->addTransaction("TRANSFER_SENT", amount,
                               "Transfer to " + toAcc);
        // For receiver, we need to handle this directly since addTransaction
        // in the receiver's account will add to its balance via DEPOSIT type
        receiver->addTransaction("TRANSFER_RCVD", amount,
                                 "Transfer from " + fromAcc);
        // Adjust: addTransaction for TRANSFER_RCVD doesn't change balance
        // by default (since it's not DEPOSIT). We handle it manually.
        receiver->balance += amount;

        cout << "  [OK] $" << fixed << setprecision(2) << amount
             << " transferred from " << fromAcc << " to " << toAcc << "\n";
    }

    // ==================================================================
    // TELLER QUEUE OPERATIONS
    // ==================================================================

    void joinTellerQueue(string accNo) {
        if (!findAccount(accNo)) {
            cout << "  [ERROR] Account not found.\n"; return;
        }
        tellerQueue.addCustomer(accNo);
        activityLog.push("QUEUE_JOIN:" + accNo);
    }

    void serveNextCustomer() {
        string accNo = tellerQueue.callNext();
        if (!accNo.empty()) {
            activityLog.push("SERVE:" + accNo);
        }
    }

    // ==================================================================
    // LOAN OPERATIONS (Priority Queue)
    // ==================================================================

    void applyForLoan(string accNo, double amount, double rate, string purpose) {
        if (!findAccount(accNo)) {
            cout << "  [ERROR] Account not found.\n"; return;
        }
        loanQueue.push(LoanApplication(accNo, amount, rate, purpose));
        cout << "  [LOAN] Application for $" << fixed << setprecision(2)
             << amount << " submitted.\n";
        cout << "  [LOAN] Priority score calculated. "
             << "Your application is queued for review.\n";
    }

    void processNextLoan() {
        if (loanQueue.empty()) {
            cout << "  [LOAN] No pending loan applications.\n";
            return;
        }
        LoanApplication app = loanQueue.top();
        loanQueue.pop();
        cout << "  [LOAN] Processing application from " << app.accountNo << "\n";
        cout << "  [LOAN] Amount: $" << fixed << setprecision(2)
             << app.loanAmount << "\n";
        cout << "  [LOAN] Purpose: " << app.purpose << "\n";
        cout << "  [LOAN] Status: APPROVED (priority score: "
             << app.priorityScore << ")\n";

        // Credit the loan amount to the account
        Account* acc = findAccount(app.accountNo);
        if (acc) {
            acc->addTransaction("DEPOSIT", app.loanAmount, "Loan approved");
            cout << "  [LOAN] $" << app.loanAmount << " credited to "
                 << app.accountNo << "\n";
        }
    }

    // ==================================================================
    // UNDO / HISTORY OPERATIONS
    // ==================================================================

    void undoLastTransaction(string accNo, string pin) {
        Account* acc = findAccount(accNo);
        if (!acc) { cout << "  [ERROR] Account not found.\n"; return; }
        if (!authenticate(acc, pin)) { cout << "  [ERROR] Wrong PIN.\n"; return; }
        acc->undoLastTransaction();
    }

    void showMinistatement(string accNo, string pin) {
        Account* acc = findAccount(accNo);
        if (!acc) { cout << "  [ERROR] Account not found.\n"; return; }
        if (!authenticate(acc, pin)) { cout << "  [ERROR] Wrong PIN.\n"; return; }

        cout << "\n  ---- MINI STATEMENT ----\n";
        acc->displaySummary();
        cout << "  Recent Transactions:\n";
        acc->history.displayRecent(5);
        cout << "  -------------------------\n";
    }

    void showFullHistory(string accNo, string pin) {
        Account* acc = findAccount(accNo);
        if (!acc) { cout << "  [ERROR] Account not found.\n"; return; }
        if (!authenticate(acc, pin)) { cout << "  [ERROR] Wrong PIN.\n"; return; }

        cout << "\n  ---- FULL TRANSACTION HISTORY ----\n";
        acc->displaySummary();
        acc->history.displayAll();
        cout << "  -----------------------------------\n";
    }

    // ==================================================================
    // ADMIN / DISPLAY FUNCTIONS
    // ==================================================================

    void showAllAccounts() {
        if (accounts.empty()) {
            cout << "  [INFO] No accounts in system.\n";
            return;
        }
        cout << "  " << left << setw(12) << "Account"
             << setw(25) << "Holder"
             << setw(15) << "Balance"
             << setw(10) << "Status"
             << "Transactions\n";
        cout << "  " << string(70, '-') << "\n";
        for (auto& [accNo, acc] : accounts) {
            cout << "  " << left << setw(12) << accNo
                 << setw(25) << acc.holderName
                 << setw(15) << fixed << setprecision(2) << acc.balance
                 << setw(10) << (acc.isActive ? "Active" : "Frozen")
                 << acc.history.getCount() << "\n";
        }
    }

    void showRecentActivity() {
        if (activityLog.empty()) {
            cout << "  [INFO] No recent activity.\n";
            return;
        }
        stack<string> temp = activityLog;
        cout << "  Recent activity (most recent first):\n";
        int count = 0;
        while (!temp.empty() && count < 10) {
            cout << "    " << temp.top() << "\n";
            temp.pop();
            count++;
        }
    }

    // ==================================================================
    // INTERACTIVE MENU
    // ==================================================================
    void run() {
        cout << "\n=============================================\n";
        cout << "   BANKING MANAGEMENT SYSTEM\n";
        cout << "   Data Structures: Hash Table, Queue,\n";
        cout << "   Doubly Linked List, Stack, Priority Queue\n";
        cout << "=============================================\n\n";

        int choice;
        string accNo, toAcc, name, pin, purpose;
        double amount, rate;

        do {
            cout << "\n------ MENU ------\n";
            cout << "  [ACCOUNTS]\n";
            cout << "    1. Create Account\n";
            cout << "    2. Deposit\n";
            cout << "    3. Withdraw\n";
            cout << "    4. Transfer\n";
            cout << "    5. Mini Statement\n";
            cout << "    6. Full History\n";
            cout << "    7. Undo Last Transaction\n";
            cout << "  [TELLER QUEUE]\n";
            cout << "    8. Join Teller Queue\n";
            cout << "    9. Serve Next Customer\n";
            cout << "  [LOANS - Priority Queue]\n";
            cout << "   10. Apply for Loan\n";
            cout << "   11. Process Next Loan\n";
            cout << "  [ADMIN]\n";
            cout << "   12. All Accounts\n";
            cout << "   13. Recent Activity\n";
            cout << "   14. Teller Queue Status\n";
            cout << "    0. Exit\n";
            cout << "Choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1:
                    cout << "  Name: "; getline(cin, name);
                    cout << "  PIN (4 digits): "; getline(cin, pin);
                    cout << "  Initial Deposit: $"; cin >> amount; cin.ignore();
                    createAccount(name, pin, amount);
                    break;

                case 2:
                    cout << "  Account: "; getline(cin, accNo);
                    cout << "  Amount: $"; cin >> amount; cin.ignore();
                    cout << "  PIN: "; getline(cin, pin);
                    deposit(accNo, amount, pin);
                    break;

                case 3:
                    cout << "  Account: "; getline(cin, accNo);
                    cout << "  Amount: $"; cin >> amount; cin.ignore();
                    cout << "  PIN: "; getline(cin, pin);
                    withdraw(accNo, amount, pin);
                    break;

                case 4:
                    cout << "  From Account: "; getline(cin, accNo);
                    cout << "  To Account: "; getline(cin, toAcc);
                    cout << "  Amount: $"; cin >> amount; cin.ignore();
                    cout << "  PIN: "; getline(cin, pin);
                    transfer(accNo, toAcc, amount, pin);
                    break;

                case 5:
                    cout << "  Account: "; getline(cin, accNo);
                    cout << "  PIN: "; getline(cin, pin);
                    showMinistatement(accNo, pin);
                    break;

                case 6:
                    cout << "  Account: "; getline(cin, accNo);
                    cout << "  PIN: "; getline(cin, pin);
                    showFullHistory(accNo, pin);
                    break;

                case 7:
                    cout << "  Account: "; getline(cin, accNo);
                    cout << "  PIN: "; getline(cin, pin);
                    undoLastTransaction(accNo, pin);
                    break;

                case 8:
                    cout << "  Account: "; getline(cin, accNo);
                    joinTellerQueue(accNo);
                    break;

                case 9:
                    serveNextCustomer();
                    break;

                case 10:
                    cout << "  Account: "; getline(cin, accNo);
                    cout << "  Loan Amount: $"; cin >> amount; cin.ignore();
                    cout << "  Interest Rate (%): "; cin >> rate; cin.ignore();
                    cout << "  Purpose: "; getline(cin, purpose);
                    applyForLoan(accNo, amount, rate, purpose);
                    break;

                case 11:
                    processNextLoan();
                    break;

                case 12:
                    showAllAccounts();
                    break;

                case 13:
                    showRecentActivity();
                    break;

                case 14:
                    tellerQueue.displayQueue();
                    break;

                case 0:
                    cout << "  Thank you for using the Banking System.\n";
                    break;

                default:
                    cout << "  Invalid choice.\n";
            }
        } while (choice != 0);
    }
};

// ============================================================================
// MAIN FUNCTION
// ============================================================================
int main() {
    BankingSystem bank;
    bank.run();
    return 0;
}
