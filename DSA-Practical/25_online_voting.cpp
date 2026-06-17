/*
 * ============================================================================
 * FILE: 25_online_voting.cpp
 * ============================================================================
 * ONLINE VOTING SYSTEM - Real-world DSA Application
 *
 * EXAM FREQUENCY: Medium-High. Voting systems are a common DSA practical
 * exam topic testing hash tables (voter registry), BST (candidate sort),
 * circular queues (recent voters), linked lists (vote tally), and stacks.
 *
 * DATA STRUCTURES USED:
 *   1. Binary Search Tree (BST) - Candidates sorted alphabetically
 *      - O(log n) search, sorted display for ballot papers
 *   2. Hash Table (unordered_map) - Voter registry by voter ID
 *      - O(1) verification to prevent duplicate voting
 *   3. Circular Queue - Recent voters (fixed-size, newest replaces oldest)
 *      - Maintains last N voters for audit trail
 *   4. Singly Linked List - Vote tally with candidate results
 *      - Dynamic list of candidates with running vote counts
 *   5. Stack - Vote undo / recount history (LIFO)
 *      - Most recent vote can be challenged/undone first
 *
 * FUNCTIONALITIES:
 *   - Admin panel with password for election management
 *   - Register voters (hash table for fast verification at polling)
 *   - Register candidates (BST for sorted ballot display)
 *   - Cast vote (verify voter, increment candidate count)
 *   - Display results with winner declaration
 *   - View recent voters (circular queue of last 10)
 *   - Undo last vote (stack - restore voter status)
 *   - Voter turnout statistics
 * ============================================================================
 */
#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <unordered_map>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <algorithm>
using namespace std;

const int RECENT_VOTERS_SIZE = 10;  // Circular queue capacity

// ============================================================================
// SECTION 1: ENTITY STRUCTURES
// ============================================================================

// -------------------------------------------------------------------
// VOTER STRUCTURE
// Stored in hash table by voter ID for O(1) verification at polling.
// -------------------------------------------------------------------
struct Voter {
    string voterId;     // Unique voter ID (e.g., "VOT001")
    string name;        // Full name
    int age;            // Must be >= 18
    string constituency; // Constituency/ward
    bool hasVoted;      // Flag to prevent double voting

    Voter() : age(0), hasVoted(false) {}
    Voter(string id, string n, int a, string consti)
        : voterId(id), name(n), age(a), constituency(consti), hasVoted(false) {}
};

// -------------------------------------------------------------------
// CANDIDATE NODE (Linked List for vote tally)
// Singly linked list since we only need forward traversal for results.
// -------------------------------------------------------------------
struct CandidateNode {
    string candidateId;     // Unique ID
    string name;            // Candidate name
    string party;           // Political party
    string constituency;    // Contesting constituency
    int voteCount;          // Running vote tally
    CandidateNode* next;    // Next candidate in list

    CandidateNode(string id, string n, string p, string c)
        : candidateId(id), name(n), party(p), constituency(c),
          voteCount(0), next(nullptr) {}
};

class CandidateList {
private:
    CandidateNode* head;
    int count;

public:
    CandidateList() : head(nullptr), count(0) {}

    ~CandidateList() {
        CandidateNode* current = head;
        while (current) {
            CandidateNode* temp = current;
            current = current->next;
            delete temp;
        }
    }

    // Insert at end (maintains order, O(n))
    void addCandidate(string id, string name, string party, string consti) {
        CandidateNode* newNode = new CandidateNode(id, name, party, consti);
        if (!head) {
            head = newNode;
        } else {
            CandidateNode* current = head;
            while (current->next) current = current->next;
            current->next = newNode;
        }
        count++;
    }

    // Find candidate by name (linear search O(n))
    CandidateNode* findByName(string name) {
        CandidateNode* current = head;
        while (current) {
            if (current->name == name) return current;
            current = current->next;
        }
        return nullptr;
    }

    // Find candidate by ID (linear search O(n))
    CandidateNode* findById(string id) {
        CandidateNode* current = head;
        while (current) {
            if (current->candidateId == id) return current;
            current = current->next;
        }
        return nullptr;
    }

    // Increment vote count O(n) to find, O(1) to update
    bool castVote(string candidateId) {
        CandidateNode* c = findById(candidateId);
        if (!c) return false;
        c->voteCount++;
        return true;
    }

    // Decrement vote count (for undo)
    bool undoVote(string candidateId) {
        CandidateNode* c = findById(candidateId);
        if (!c || c->voteCount <= 0) return false;
        c->voteCount--;
        return true;
    }

    // Display all candidates with vote counts
    void displayResults() {
        if (!head) {
            cout << "  [INFO] No candidates registered.\n";
            return;
        }
        cout << "  " << left << setw(12) << "ID"
             << setw(25) << "Name"
             << setw(20) << "Party"
             << setw(20) << "Constituency"
             << "Votes\n";
        cout << "  " << string(85, '-') << "\n";

        CandidateNode* current = head;
        CandidateNode* winner = head;
        int maxVotes = 0;

        while (current) {
            cout << "  " << left << setw(12) << current->candidateId
                 << setw(25) << current->name
                 << setw(20) << current->party
                 << setw(20) << current->constituency
                 << current->voteCount << "\n";
            if (current->voteCount > maxVotes) {
                maxVotes = current->voteCount;
                winner = current;
            }
            current = current->next;
        }
        cout << "  " << string(85, '-') << "\n";
        if (winner && maxVotes > 0) {
            cout << "  WINNER: " << winner->name << " (" << winner->party
                 << ") - " << maxVotes << " votes\n";
        }
    }

    int getCount() { return count; }
    CandidateNode* getHead() { return head; }
};

// ============================================================================
// SECTION 2: BST FOR CANDIDATE REGISTRATION (alphabetical order)
//
// Why BST in addition to linked list? The linked list stores running
// results, but we also need candidates sorted alphabetically for the
// ballot paper display. BST provides natural in-order alphabetical sort.
// ============================================================================

struct CandidateBSTNode {
    string name;                // Candidate name (BST key)
    string candidateId;         // ID
    string party;               // Party affiliation
    string constituency;        // Constituency
    CandidateBSTNode* left;
    CandidateBSTNode* right;

    CandidateBSTNode(string n, string id, string p, string c)
        : name(n), candidateId(id), party(p), constituency(c),
          left(nullptr), right(nullptr) {}
};

class CandidateBST {
private:
    CandidateBSTNode* root;

    CandidateBSTNode* insert(CandidateBSTNode* node, string name,
                             string id, string party, string consti) {
        if (!node)
            return new CandidateBSTNode(name, id, party, consti);
        if (name < node->name)
            node->left = insert(node->left, name, id, party, consti);
        else if (name > node->name)
            node->right = insert(node->right, name, id, party, consti);
        return node;
    }

    CandidateBSTNode* search(CandidateBSTNode* node, string name) {
        if (!node || node->name == name) return node;
        if (name < node->name) return search(node->left, name);
        return search(node->right, name);
    }

    void inorderDisplay(CandidateBSTNode* node) {
        if (!node) return;
        inorderDisplay(node->left);
        cout << "    " << left << setw(12) << node->candidateId
             << setw(25) << node->name
             << setw(20) << node->party
             << node->constituency << "\n";
        inorderDisplay(node->right);
    }

    void destroy(CandidateBSTNode* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

public:
    CandidateBST() : root(nullptr) {}
    ~CandidateBST() { destroy(root); }

    bool addCandidate(string name, string id, string party, string consti) {
        if (search(root, name)) {
            cout << "  [ERROR] Candidate '" << name << "' already registered.\n";
            return false;
        }
        root = insert(root, name, id, party, consti);
        return true;
    }

    void displayBallot() {
        if (!root) {
            cout << "  [INFO] No candidates available.\n";
            return;
        }
        cout << "  BALLOT PAPER (sorted alphabetically):\n";
        cout << "    " << left << setw(12) << "ID"
             << setw(25) << "Name"
             << setw(20) << "Party"
             << "Constituency\n";
        cout << "    " << string(75, '-') << "\n";
        inorderDisplay(root);
    }
};

// ============================================================================
// SECTION 3: CIRCULAR QUEUE FOR RECENT VOTERS
//
// Why Circular Queue? We track the last N voters for audit purposes.
// When full, the newest voter replaces the oldest. This is exactly
// the "circular buffer" pattern -- fixed size, O(1) insert, no resizing.
// -------------------------------------------------------------------

class RecentVotersQueue {
private:
    string voters[RECENT_VOTERS_SIZE];
    int front;
    int rear;
    int currentSize;

public:
    RecentVotersQueue() : front(0), rear(-1), currentSize(0) {}

    // -------------------------------------------------------------------
    // ADD VOTER: Add to rear. If full, overwrite oldest (front). O(1).
    // -------------------------------------------------------------------
    void addVoter(string voterId) {
        if (currentSize < RECENT_VOTERS_SIZE) {
            rear = (rear + 1) % RECENT_VOTERS_SIZE;
            voters[rear] = voterId;
            currentSize++;
        } else {
            // Queue full: overwrite oldest (circular behavior)
            rear = (rear + 1) % RECENT_VOTERS_SIZE;
            front = (front + 1) % RECENT_VOTERS_SIZE;
            voters[rear] = voterId;
        }
    }

    // -------------------------------------------------------------------
    // DISPLAY: Show recent voters from oldest to newest. O(n).
    // -------------------------------------------------------------------
    void display() {
        if (currentSize == 0) {
            cout << "  [INFO] No recent voters.\n";
            return;
        }
        cout << "  Recent voters (last " << currentSize << "):\n";
        int idx = front;
        for (int i = 0; i < currentSize; i++) {
            cout << "    " << (i + 1) << ". " << voters[idx] << "\n";
            idx = (idx + 1) % RECENT_VOTERS_SIZE;
        }
    }
};

// ============================================================================
// SECTION 4: MAIN VOTING SYSTEM
// ============================================================================

class VotingSystem {
private:
    unordered_map<string, Voter> voterRegistry;  // Hash: voterId -> Voter
    CandidateBST candidateBST;                    // BST: sorted candidates
    CandidateList voteTally;                      // Linked list: results
    RecentVotersQueue recentVoters;               // Circular queue: audit
    stack<string> voteHistory;                    // Stack: undo (LIFO)
    bool electionActive;
    const string adminPassword = "admin123";

public:
    VotingSystem() : electionActive(true) {
        // Seed candidates
        addCandidate("C001", "Alice Johnson", "Freedom Party", "Constituency A");
        addCandidate("C002", "Bob Smith", "Unity Party", "Constituency A");
        addCandidate("C003", "Charlie Brown", "Progressive Alliance", "Constituency A");

        // Seed voters
        registerVoter("VOT001", "David Williams", 32, "Constituency A");
        registerVoter("VOT002", "Eve Davis", 45, "Constituency A");
        registerVoter("VOT003", "Frank Miller", 28, "Constituency A");
        registerVoter("VOT004", "Grace Wilson", 55, "Constituency A");
        registerVoter("VOT005", "Henry Moore", 19, "Constituency A");
    }

    // -------------------------------------------------------------------
    // REGISTER VOTER: Insert into hash table by voter ID. O(1) avg.
    // -------------------------------------------------------------------
    void registerVoter(string id, string name, int age, string consti) {
        if (voterRegistry.find(id) != voterRegistry.end()) {
            cout << "  [ERROR] Voter " << id << " already registered.\n";
            return;
        }
        if (age < 18) {
            cout << "  [ERROR] Voter must be at least 18 years old.\n";
            return;
        }
        voterRegistry[id] = Voter(id, name, age, consti);
        cout << "  [OK] Voter '" << name << "' (" << id << ") registered.\n";
    }

    // -------------------------------------------------------------------
    // REGISTER CANDIDATE: Add to BST (sorted) + linked list (tally). O(log n).
    // -------------------------------------------------------------------
    void addCandidate(string id, string name, string party, string consti) {
        if (candidateBST.addCandidate(name, id, party, consti)) {
            voteTally.addCandidate(id, name, party, consti);
        }
    }

    // -------------------------------------------------------------------
    // CAST VOTE: Verify voter (hash table) -> mark voted -> increment tally.
    // Push to stack for potential undo. O(1) avg + O(n) for tally update.
    // -------------------------------------------------------------------
    void castVote(string voterId, string candidateId) {
        if (!electionActive) {
            cout << "  [ERROR] Election is not active.\n";
            return;
        }

        // O(1) voter verification via hash table
        auto it = voterRegistry.find(voterId);
        if (it == voterRegistry.end()) {
            cout << "  [ERROR] Voter " << voterId << " not registered.\n";
            return;
        }

        Voter& voter = it->second;
        if (voter.hasVoted) {
            cout << "  [ERROR] Voter " << voterId
                 << " has already voted. Duplicate vote prevented!\n";
            return;
        }

        if (!voteTally.findById(candidateId)) {
            cout << "  [ERROR] Candidate " << candidateId << " not found.\n";
            return;
        }

        // Cast the vote
        voter.hasVoted = true;
        voteTally.castVote(candidateId);
        recentVoters.addVoter(voterId);
        voteHistory.push(voterId + "|" + candidateId);

        cout << "  [OK] Vote cast successfully by "
             << voter.name << " for candidate " << candidateId << ".\n";
    }

    // -------------------------------------------------------------------
    // UNDO LAST VOTE: Pop from stack, restore voter & candidate. O(1).
    // -------------------------------------------------------------------
    void undoLastVote() {
        if (voteHistory.empty()) {
            cout << "  [INFO] No votes to undo.\n";
            return;
        }

        string lastVote = voteHistory.top();
        voteHistory.pop();

        // Parse "voterId|candidateId"
        size_t delim = lastVote.find('|');
        string voterId = lastVote.substr(0, delim);
        string candidateId = lastVote.substr(delim + 1);

        // Restore voter status
        auto it = voterRegistry.find(voterId);
        if (it != voterRegistry.end()) {
            it->second.hasVoted = false;
        }

        // Decrement candidate count
        voteTally.undoVote(candidateId);

        cout << "  [UNDO] Vote by " << voterId
             << " for candidate " << candidateId << " reversed.\n";
    }

    // -------------------------------------------------------------------
    // DISPLAY VOTER TURNOUT STATISTICS
    // Count voters who have voted vs total registered. O(n).
    // -------------------------------------------------------------------
    void displayTurnout() {
        int total = voterRegistry.size();
        int voted = 0;
        unordered_map<string, int> constiTotal;
        unordered_map<string, int> constiVoted;

        for (auto& pair : voterRegistry) {
            Voter& v = pair.second;
            constiTotal[v.constituency]++;
            if (v.hasVoted) {
                voted++;
                constiVoted[v.constituency]++;
            }
        }

        cout << "  VOTER TURNOUT:\n";
        cout << "  Total registered: " << total << "\n";
        cout << "  Total voted:      " << voted << "\n";
        cout << "  Turnout:          " << fixed << setprecision(1)
             << (total > 0 ? (voted * 100.0 / total) : 0.0) << "%\n\n";
        cout << "  By Constituency:\n";
        cout << "  " << left << setw(25) << "Constituency"
             << setw(10) << "Total" << setw(10) << "Voted"
             << "Turnout\n";
        cout << "  " << string(55, '-') << "\n";
        for (auto& pair : constiTotal) {
            string consti = pair.first;
            int t = pair.second;
            int v = constiVoted[consti];
            cout << "  " << left << setw(25) << consti
                 << setw(10) << t << setw(10) << v
                 << fixed << setprecision(1)
                 << (t > 0 ? (v * 100.0 / t) : 0.0) << "%\n";
        }
    }

    // -------------------------------------------------------------------
    // ADMIN AUTHENTICATION
    // -------------------------------------------------------------------
    bool adminLogin() {
        string pwd;
        cout << "  Admin Password: ";
        cin >> pwd;
        cin.ignore();
        if (pwd != adminPassword) {
            cout << "  [ERROR] Incorrect password.\n";
            return false;
        }
        return true;
    }

    void run() {
        cout << "\n=============================================\n";
        cout << "   ONLINE VOTING SYSTEM\n";
        cout << "   Data Structures: BST, Hash Table,\n";
        cout << "   Circular Queue, Linked List, Stack\n";
        cout << "=============================================\n\n";

        int choice, age;
        string id, name, party, consti, voterId, candidateId, pnr;

        do {
            cout << "\n------ MENU ------\n";
            cout << "  [ADMIN - Password Protected]\n";
            cout << "    1. Register Voter (Hash Table)\n";
            cout << "    2. Register Candidate (BST)\n";
            cout << "    3. Display Ballot (BST In-Order)\n";
            cout << "    4. Display Results (Linked List)\n";
            cout << "    5. Display Voter Turnout\n";
            cout << "  [VOTER]\n";
            cout << "    6. Cast Vote\n";
            cout << "  [AUDIT - Circular Queue & Stack]\n";
            cout << "    7. View Recent Voters\n";
            cout << "    8. Undo Last Vote (Stack)\n";
            cout << "    0. Exit\n";
            cout << "Choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1:
                    if (!adminLogin()) break;
                    cout << "  Voter ID: "; getline(cin, id);
                    cout << "  Name: "; getline(cin, name);
                    cout << "  Age: "; cin >> age; cin.ignore();
                    cout << "  Constituency: "; getline(cin, consti);
                    registerVoter(id, name, age, consti);
                    break;
                case 2:
                    if (!adminLogin()) break;
                    cout << "  Candidate ID: "; getline(cin, id);
                    cout << "  Name: "; getline(cin, name);
                    cout << "  Party: "; getline(cin, party);
                    cout << "  Constituency: "; getline(cin, consti);
                    addCandidate(id, name, party, consti);
                    break;
                case 3: candidateBST.displayBallot(); break;
                case 4: voteTally.displayResults(); break;
                case 5: displayTurnout(); break;
                case 6:
                    cout << "  Voter ID: "; getline(cin, voterId);
                    cout << "  Candidate ID: "; getline(cin, candidateId);
                    castVote(voterId, candidateId);
                    break;
                case 7: recentVoters.display(); break;
                case 8: undoLastVote(); break;
                case 0: cout << "  Exiting. Goodbye!\n"; break;
                default: cout << "  [ERROR] Invalid choice.\n";
            }
        } while (choice != 0);
    }
};

int main() {
    VotingSystem system;
    system.run();
    return 0;
}
