/*
 * ============================================================================
 * FILE: 13_library_management.cpp
 * ============================================================================
 * LIBRARY MANAGEMENT SYSTEM - Real-world DSA Application
 *
 * EXAM FREQUENCY: Very High. Library management is one of the most commonly
 * asked DSA practical exam projects. Tests your understanding of MULTIPLE
 * data structures working together in a single application.
 *
 * DATA STRUCTURES USED:
 *   1. Singly Linked List  - Book catalog (dynamic collection of books)
 *   2. Binary Search Tree  - Member records (sorted by member ID for fast search)
 *   3. Queue               - Borrow/Return requests (FIFO processing)
 *   4. Hash Table          - ISBN lookup (O(1) book retrieval)
 *   5. Stack               - Transaction history (undo/last-in-first-out review)
 *
 * FUNCTIONALITIES:
 *   - Add/Remove/Search books (by title, author, ISBN)
 *   - Register members (BST for fast ID-based lookup)
 *   - Borrow book (queue-based request processing)
 *   - Return book
 *   - View transaction history (stack)
 *   - List all books/members
 * ============================================================================
 */
#include <iostream>
#include <string>
#include <queue>        // For borrow/return request queue
#include <stack>        // For transaction history
#include <vector>
#include <unordered_map> // For hash table (ISBN -> Book mapping)
#include <iomanip>      // For formatted output
#include <ctime>        // For date/time in transactions
using namespace std;

// ============================================================================
// SECTION 1: DATA STRUCTURES USED BY THE SYSTEM
// ============================================================================

// -------------------------------------------------------------------
// Book structure - Core entity of the library system
// Each book has a unique ISBN, title, author, and availability status.
// -------------------------------------------------------------------
struct Book {
    string isbn;        // Unique identifier (International Standard Book Number)
    string title;       // Book title
    string author;      // Author name
    bool isAvailable;   // true = on shelf, false = borrowed out
    string borrowerId;  // Member ID who borrowed it (empty if available)

    // Constructor with default values
    Book(string i, string t, string a)
        : isbn(i), title(t), author(a), isAvailable(true), borrowerId("") {}
};

// -------------------------------------------------------------------
// Member structure - Represents a library member
// Each member has a unique ID, name, and list of borrowed ISBNs.
// -------------------------------------------------------------------
struct Member {
    string memberId;        // Unique identifier (e.g., "M001")
    string name;            // Member's full name
    vector<string> borrowedBooks;  // ISBNs of currently borrowed books
    int maxBooks;           // Maximum books a member can borrow at once

    Member(string id, string n)
        : memberId(id), name(n), maxBooks(5) {}
};

// -------------------------------------------------------------------
// Transaction record - Logs every borrow/return action
// Stored in a stack for easy review of recent activity.
// -------------------------------------------------------------------
struct Transaction {
    string transactionId;   // Unique transaction ID (auto-generated)
    string memberId;        // Who performed the action
    string isbn;            // Which book
    string action;          // "BORROW" or "RETURN"
    string date;            // Date of transaction

    Transaction(string tid, string mid, string i, string act)
        : transactionId(tid), memberId(mid), isbn(i), action(act) {
        // Generate current date string
        time_t now = time(nullptr);
        date = ctime(&now);
        // Remove trailing newline from ctime
        if (!date.empty() && date[date.length()-1] == '\n')
            date.pop_back();
    }
};

// ============================================================================
// SECTION 2: LINKED LIST FOR BOOK CATALOG
//
// Why Linked List? The book catalog needs dynamic memory allocation
// (we don't know how many books beforehand). Insertion/deletion at
// any position is O(n) but we get dynamic sizing. For a real library
// with thousands of books, a balanced BST or hash table would be better.
// Here we use linked list for the sequential catalog listing and
// a hash table (unordered_map) for O(1) ISBN lookups.
// ============================================================================

struct BookNode {
    Book book;
    BookNode* next;

    BookNode(Book b) : book(b), next(nullptr) {}
};

class BookCatalog {
private:
    BookNode* head;                // Head of linked list
    unordered_map<string, BookNode*> isbnMap;  // Hash: ISBN -> Node pointer

public:
    BookCatalog() : head(nullptr) {}

    ~BookCatalog() {
        BookNode* current = head;
        while (current) {
            BookNode* temp = current;
            current = current->next;
            delete temp;
        }
    }

    // -------------------------------------------------------------------
    // ADD BOOK: Insert at end of linked list + map in hash table. O(1) avg.
    // -------------------------------------------------------------------
    void addBook(string isbn, string title, string author) {
        // Check if ISBN already exists (using hash table)
        if (isbnMap.find(isbn) != isbnMap.end()) {
            cout << "  [ERROR] Book with ISBN " << isbn << " already exists!\n";
            return;
        }
        BookNode* newNode = new BookNode(Book(isbn, title, author));

        if (!head) {
            head = newNode;     // First book in catalog
        } else {
            BookNode* temp = head;
            while (temp->next) temp = temp->next;
            temp->next = newNode;  // Append to end
        }
        isbnMap[isbn] = newNode;  // O(1) insertion in hash table
        cout << "  [OK] Book '" << title << "' added successfully.\n";
    }

    // -------------------------------------------------------------------
    // SEARCH BY ISBN: O(1) using hash table (unordered_map).
    // Returns pointer to Book or nullptr if not found.
    // -------------------------------------------------------------------
    Book* searchByISBN(string isbn) {
        if (isbnMap.find(isbn) != isbnMap.end()) {
            return &(isbnMap[isbn]->book);
        }
        return nullptr;
    }

    // -------------------------------------------------------------------
    // SEARCH BY TITLE: O(n) - must scan the linked list.
    // Returns vector of all matching books (partial match).
    // -------------------------------------------------------------------
    vector<Book*> searchByTitle(string title) {
        vector<Book*> results;
        BookNode* temp = head;
        while (temp) {
            // Case-insensitive substring search (simplified: case-sensitive here)
            if (temp->book.title.find(title) != string::npos) {
                results.push_back(&temp->book);
            }
            temp = temp->next;
        }
        return results;
    }

    // -------------------------------------------------------------------
    // REMOVE BOOK: O(1) hash lookup + O(n) linked list deletion.
    // -------------------------------------------------------------------
    bool removeBook(string isbn) {
        if (isbnMap.find(isbn) == isbnMap.end()) {
            cout << "  [ERROR] Book with ISBN " << isbn << " not found.\n";
            return false;
        }
        // Remove from linked list
        BookNode* toDelete = isbnMap[isbn];
        if (head == toDelete) {
            head = head->next;  // Removing head
        } else {
            BookNode* temp = head;
            while (temp && temp->next != toDelete) temp = temp->next;
            if (temp) temp->next = toDelete->next;
        }
        isbnMap.erase(isbn);  // Remove from hash table
        cout << "  [OK] Book '" << toDelete->book.title << "' removed.\n";
        delete toDelete;
        return true;
    }

    // -------------------------------------------------------------------
    // UPDATE AVAILABILITY: Called when books are borrowed/returned.
    // -------------------------------------------------------------------
    void setAvailability(string isbn, bool available, string memberId = "") {
        Book* book = searchByISBN(isbn);
        if (book) {
            book->isAvailable = available;
            book->borrowerId = available ? "" : memberId;
        }
    }

    // -------------------------------------------------------------------
    // DISPLAY ALL BOOKS: Traverse linked list. O(n).
    // -------------------------------------------------------------------
    void displayAllBooks() {
        if (!head) {
            cout << "  [INFO] No books in catalog.\n";
            return;
        }
        cout << "  " << left << setw(15) << "ISBN"
             << setw(35) << "Title"
             << setw(25) << "Author"
             << setw(12) << "Status"
             << "Borrower\n";
        cout << "  " << string(100, '-') << "\n";

        BookNode* temp = head;
        while (temp) {
            cout << "  " << left << setw(15) << temp->book.isbn
                 << setw(35) << temp->book.title
                 << setw(25) << temp->book.author
                 << setw(12) << (temp->book.isAvailable ? "Available" : "Borrowed")
                 << (temp->book.isAvailable ? "" : temp->book.borrowerId)
                 << "\n";
            temp = temp->next;
        }
    }

    int getCount() { return isbnMap.size(); }
};

// ============================================================================
// SECTION 3: BST FOR MEMBER RECORDS
//
// Why BST? Members need to be searchable by ID quickly. BST gives O(log n)
// search, insertion, and deletion on average. We store members sorted by
// their memberId (string comparison).
// ============================================================================

struct MemberNode {
    Member member;
    MemberNode* left;
    MemberNode* right;

    MemberNode(Member m) : member(m), left(nullptr), right(nullptr) {}
};

class MemberTree {
private:
    MemberNode* root;

    // Helper: Insert member into BST recursively
    MemberNode* insert(MemberNode* node, Member m) {
        if (!node) return new MemberNode(m);
        if (m.memberId < node->member.memberId)
            node->left = insert(node->left, m);
        else if (m.memberId > node->member.memberId)
            node->right = insert(node->right, m);
        // If equal, member already exists (we'll check before calling)
        return node;
    }

    // Helper: Search member by ID recursively
    MemberNode* search(MemberNode* node, string memberId) {
        if (!node || node->member.memberId == memberId)
            return node;
        if (memberId < node->member.memberId)
            return search(node->left, memberId);
        return search(node->right, memberId);
    }

    // Helper: Find minimum node (used in deletion)
    MemberNode* findMin(MemberNode* node) {
        while (node && node->left) node = node->left;
        return node;
    }

    // Helper: Delete node recursively
    MemberNode* deleteNode(MemberNode* node, string memberId) {
        if (!node) return nullptr;
        if (memberId < node->member.memberId)
            node->left = deleteNode(node->left, memberId);
        else if (memberId > node->member.memberId)
            node->right = deleteNode(node->right, memberId);
        else {
            // Found the node to delete
            if (!node->left) {
                MemberNode* temp = node->right;
                delete node;
                return temp;
            }
            if (!node->right) {
                MemberNode* temp = node->left;
                delete node;
                return temp;
            }
            // Two children: find inorder successor
            MemberNode* successor = findMin(node->right);
            node->member = successor->member;
            node->right = deleteNode(node->right, successor->member.memberId);
        }
        return node;
    }

    // Helper: Inorder traversal to display all members
    void inorderDisplay(MemberNode* node) {
        if (!node) return;
        inorderDisplay(node->left);
        cout << "  " << left << setw(10) << node->member.memberId
             << setw(30) << node->member.name
             << "Books borrowed: " << node->member.borrowedBooks.size()
             << "/" << node->member.maxBooks << "\n";
        inorderDisplay(node->right);
    }

    // Helper: Clean up memory
    void destroy(MemberNode* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

public:
    MemberTree() : root(nullptr) {}

    ~MemberTree() { destroy(root); }

    // -------------------------------------------------------------------
    // REGISTER MEMBER: Insert into BST. O(log n) average.
    // -------------------------------------------------------------------
    void registerMember(string id, string name) {
        if (search(root, id)) {
            cout << "  [ERROR] Member ID " << id << " already exists!\n";
            return;
        }
        root = insert(root, Member(id, name));
        cout << "  [OK] Member '" << name << "' registered successfully.\n";
    }

    // -------------------------------------------------------------------
    // FIND MEMBER: O(log n) average. Returns pointer to Member.
    // -------------------------------------------------------------------
    Member* findMember(string memberId) {
        MemberNode* node = search(root, memberId);
        return node ? &(node->member) : nullptr;
    }

    // -------------------------------------------------------------------
    // REMOVE MEMBER: O(log n). Member must have no borrowed books.
    // -------------------------------------------------------------------
    bool removeMember(string memberId) {
        Member* m = findMember(memberId);
        if (!m) {
            cout << "  [ERROR] Member " << memberId << " not found.\n";
            return false;
        }
        if (!m->borrowedBooks.empty()) {
            cout << "  [ERROR] Member has " << m->borrowedBooks.size()
                 << " books to return first.\n";
            return false;
        }
        root = deleteNode(root, memberId);
        cout << "  [OK] Member " << memberId << " removed.\n";
        return true;
    }

    // -------------------------------------------------------------------
    // ADD BORROWED BOOK to member's record
    // -------------------------------------------------------------------
    bool addBorrowedBook(string memberId, string isbn) {
        Member* m = findMember(memberId);
        if (!m) return false;
        if ((int)m->borrowedBooks.size() >= m->maxBooks) {
            cout << "  [ERROR] Member has reached max borrowing limit ("
                 << m->maxBooks << ").\n";
            return false;
        }
        m->borrowedBooks.push_back(isbn);
        return true;
    }

    // -------------------------------------------------------------------
    // REMOVE BORROWED BOOK from member's record (on return)
    // -------------------------------------------------------------------
    bool removeBorrowedBook(string memberId, string isbn) {
        Member* m = findMember(memberId);
        if (!m) return false;
        for (auto it = m->borrowedBooks.begin(); it != m->borrowedBooks.end(); it++) {
            if (*it == isbn) {
                m->borrowedBooks.erase(it);
                return true;
            }
        }
        return false;
    }

    // -------------------------------------------------------------------
    // DISPLAY ALL MEMBERS: Inorder traversal. O(n).
    // -------------------------------------------------------------------
    void displayAllMembers() {
        if (!root) {
            cout << "  [INFO] No members registered.\n";
            return;
        }
        cout << "  " << left << setw(10) << "Member ID"
             << setw(30) << "Name"
             << "Status\n";
        cout << "  " << string(60, '-') << "\n";
        inorderDisplay(root);
    }
};

// ============================================================================
// SECTION 4: QUEUE FOR BORROW/RETURN REQUEST PROCESSING
//
// Why Queue? When multiple members request to borrow books, requests
// should be processed in FIFO (First-In-First-Out) order - fair for
// all members. The queue holds pending requests.
// ============================================================================

struct Request {
    string memberId;
    string isbn;
    string type;   // "BORROW" or "RETURN"

    Request(string m, string i, string t)
        : memberId(m), isbn(i), type(t) {}
};

class RequestQueue {
private:
    queue<Request> requests;  // STL queue (FIFO)
    int nextRequestId;        // For generating unique request IDs

public:
    RequestQueue() : nextRequestId(1001) {}

    // -------------------------------------------------------------------
    // ENQUEUE REQUEST: Add to back of queue. O(1).
    // -------------------------------------------------------------------
    void addRequest(string memberId, string isbn, string type) {
        requests.push(Request(memberId, isbn, type));
        cout << "  [OK] " << type << " request for " << isbn
             << " by " << memberId << " queued.\n";
    }

    // -------------------------------------------------------------------
    // PROCESS NEXT REQUEST: Dequeue and process. O(1).
    // Returns the request for the caller to process further.
    // -------------------------------------------------------------------
    Request processNext() {
        if (requests.empty()) {
            return Request("", "", "NONE");
        }
        Request req = requests.front();
        requests.pop();
        return req;
    }

    bool hasPending() { return !requests.empty(); }
    int pendingCount() { return requests.size(); }

    void displayPending() {
        if (requests.empty()) {
            cout << "  [INFO] No pending requests.\n";
            return;
        }
        queue<Request> temp = requests;
        cout << "  Pending requests (" << requests.size() << "):\n";
        while (!temp.empty()) {
            cout << "    " << temp.front().type << " - Book: "
                 << temp.front().isbn << " by " << temp.front().memberId << "\n";
            temp.pop();
        }
    }
};

// ============================================================================
// SECTION 5: STACK FOR TRANSACTION HISTORY
//
// Why Stack? Recent transactions should be reviewable in LIFO order
// (most recent first). Stack is perfect for this "undo/review" pattern.
// ============================================================================

class TransactionHistory {
private:
    stack<Transaction> history;  // STL stack (LIFO)
    int nextTxnId;

public:
    TransactionHistory() : nextTxnId(5001) {}

    // -------------------------------------------------------------------
    // ADD TRANSACTION: Push onto stack. O(1).
    // -------------------------------------------------------------------
    void addTransaction(string memberId, string isbn, string action) {
        string txnId = "TXN" + to_string(nextTxnId++);
        history.push(Transaction(txnId, memberId, isbn, action));
    }

    // -------------------------------------------------------------------
    // VIEW RECENT TRANSACTIONS: Pop and display. O(n) for n transactions.
    // Displayed most-recent-first (LIFO).
    // -------------------------------------------------------------------
    void viewRecent(int count = 10) {
        if (history.empty()) {
            cout << "  [INFO] No transactions yet.\n";
            return;
        }
        // Create a copy of stack to not destroy the original
        stack<Transaction> temp = history;
        int shown = 0;

        cout << "  " << left << setw(15) << "Txn ID"
             << setw(10) << "Action"
             << setw(10) << "Member"
             << setw(15) << "ISBN"
             << "Date\n";
        cout << "  " << string(90, '-') << "\n";

        while (!temp.empty() && shown < count) {
            Transaction& t = temp.top();
            cout << "  " << left << setw(15) << t.transactionId
                 << setw(10) << t.action
                 << setw(10) << t.memberId
                 << setw(15) << t.isbn
                 << t.date << "\n";
            temp.pop();
            shown++;
        }
    }
};

// ============================================================================
// SECTION 6: MAIN LIBRARY SYSTEM - Integrates all data structures
// ============================================================================

class LibraryManagementSystem {
private:
    BookCatalog catalog;           // Linked list + Hash Table for books
    MemberTree members;            // BST for members
    RequestQueue requestQueue;     // Queue for processing requests
    TransactionHistory transactions; // Stack for transaction logs

    // -------------------------------------------------------------------
    // Process all pending requests in the queue (FIFO order)
    // -------------------------------------------------------------------
    void processRequests() {
        while (requestQueue.hasPending()) {
            Request req = requestQueue.processNext();
            if (req.type == "NONE") break;

            if (req.type == "BORROW") {
                handleBorrow(req.memberId, req.isbn, false);
            } else if (req.type == "RETURN") {
                handleReturn(req.memberId, req.isbn, false);
            }
        }
    }

    // -------------------------------------------------------------------
    // Handle book borrow: Check availability, update status, log.
    // -------------------------------------------------------------------
    bool handleBorrow(string memberId, string isbn, bool queueIfUnavailable) {
        Book* book = catalog.searchByISBN(isbn);
        Member* member = members.findMember(memberId);

        if (!book) {
            cout << "  [ERROR] Book " << isbn << " not found.\n";
            return false;
        }
        if (!member) {
            cout << "  [ERROR] Member " << memberId << " not found.\n";
            return false;
        }

        if (!book->isAvailable) {
            if (queueIfUnavailable) {
                requestQueue.addRequest(memberId, isbn, "BORROW");
                cout << "  [INFO] Book is currently borrowed. Request queued.\n";
            } else {
                cout << "  [INFO] Book is already borrowed by "
                     << book->borrowerId << ".\n";
            }
            return false;
        }

        if (!members.addBorrowedBook(memberId, isbn)) return false;

        catalog.setAvailability(isbn, false, memberId);
        transactions.addTransaction(memberId, isbn, "BORROW");
        cout << "  [OK] '" << book->title << "' borrowed by "
             << member->name << ".\n";
        return true;
    }

    // -------------------------------------------------------------------
    // Handle book return: Update status, remove from member, log.
    // -------------------------------------------------------------------
    bool handleReturn(string memberId, string isbn, bool queueIfNotBorrowed) {
        Book* book = catalog.searchByISBN(isbn);
        if (!book) {
            cout << "  [ERROR] Book " << isbn << " not found.\n";
            return false;
        }

        if (book->isAvailable) {
            cout << "  [INFO] Book was not borrowed.\n";
            return false;
        }

        // Check if this member actually borrowed it
        if (book->borrowerId != memberId) {
            cout << "  [WARN] This book was borrowed by "
                 << book->borrowerId << ", not " << memberId << ".\n";
        }

        members.removeBorrowedBook(memberId, isbn);
        catalog.setAvailability(isbn, true);
        transactions.addTransaction(memberId, isbn, "RETURN");
        cout << "  [OK] '" << book->title << "' returned by "
             << memberId << ".\n";
        return true;
    }

public:
    // -------------------------------------------------------------------
    // RUN the interactive library management system
    // -------------------------------------------------------------------
    void run() {
        cout << "\n=============================================\n";
        cout << "   LIBRARY MANAGEMENT SYSTEM\n";
        cout << "=============================================\n\n";

        int choice;
        string isbn, title, author, memberId, name;
        vector<Book*> searchResults;

        do {
            cout << "\n------ MENU ------\n";
            cout << "  1. Add Book\n";
            cout << "  2. Search Book by ISBN\n";
            cout << "  3. Search Book by Title\n";
            cout << "  4. Remove Book\n";
            cout << "  5. Register Member\n";
            cout << "  6. Find Member\n";
            cout << "  7. Borrow Book\n";
            cout << "  8. Return Book\n";
            cout << "  9. Process Pending Requests\n";
            cout << " 10. Recent Transactions\n";
            cout << " 11. Display All Books\n";
            cout << " 12. Display All Members\n";
            cout << " 13. Show Pending Requests\n";
            cout << " 14. Remove Member\n";
            cout << "  0. Exit\n";
            cout << "Enter choice: ";
            cin >> choice;
            cin.ignore(); // Clear input buffer

            switch (choice) {
                case 1:
                    cout << "  ISBN: "; getline(cin, isbn);
                    cout << "  Title: "; getline(cin, title);
                    cout << "  Author: "; getline(cin, author);
                    catalog.addBook(isbn, title, author);
                    break;

                case 2:
                    cout << "  ISBN: "; getline(cin, isbn);
                    if (Book* b = catalog.searchByISBN(isbn)) {
                        cout << "  Found: " << b->title << " by " << b->author
                             << " [" << (b->isAvailable ? "Available" : "Borrowed by " + b->borrowerId)
                             << "]\n";
                    } else {
                        cout << "  [ERROR] Book not found.\n";
                    }
                    break;

                case 3:
                    cout << "  Title (or part): "; getline(cin, title);
                    searchResults = catalog.searchByTitle(title);
                    if (searchResults.empty()) {
                        cout << "  [INFO] No books found matching '" << title << "'.\n";
                    } else {
                        cout << "  Found " << searchResults.size() << " book(s):\n";
                        for (Book* b : searchResults) {
                            cout << "    " << b->isbn << " | " << b->title
                                 << " | " << b->author << "\n";
                        }
                    }
                    break;

                case 4:
                    cout << "  ISBN: "; getline(cin, isbn);
                    catalog.removeBook(isbn);
                    break;

                case 5:
                    cout << "  Member ID: "; getline(cin, memberId);
                    cout << "  Name: "; getline(cin, name);
                    members.registerMember(memberId, name);
                    break;

                case 6:
                    cout << "  Member ID: "; getline(cin, memberId);
                    if (Member* m = members.findMember(memberId)) {
                        cout << "  Found: " << m->name
                             << " (Borrowed: " << m->borrowedBooks.size()
                             << "/" << m->maxBooks << ")\n";
                    } else {
                        cout << "  [ERROR] Member not found.\n";
                    }
                    break;

                case 7:
                    cout << "  Member ID: "; getline(cin, memberId);
                    cout << "  ISBN: "; getline(cin, isbn);
                    handleBorrow(memberId, isbn, true);
                    break;

                case 8:
                    cout << "  Member ID: "; getline(cin, memberId);
                    cout << "  ISBN: "; getline(cin, isbn);
                    handleReturn(memberId, isbn, true);
                    break;

                case 9:
                    processRequests();
                    break;

                case 10:
                    transactions.viewRecent();
                    break;

                case 11:
                    catalog.displayAllBooks();
                    break;

                case 12:
                    members.displayAllMembers();
                    break;

                case 13:
                    requestQueue.displayPending();
                    break;

                case 14:
                    cout << "  Member ID: "; getline(cin, memberId);
                    members.removeMember(memberId);
                    break;

                case 0:
                    cout << "  Exiting system. Goodbye!\n";
                    break;

                default:
                    cout << "  [ERROR] Invalid choice.\n";
            }
        } while (choice != 0);
    }
};

// ============================================================================
// MAIN FUNCTION - Entry point
// ============================================================================
int main() {
    LibraryManagementSystem library;
    library.run();
    return 0;
}
