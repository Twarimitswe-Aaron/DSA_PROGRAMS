/*
 * ============================================================================
 * FILE: 27_gym_fitness.cpp
 * ============================================================================
 * GYM & FITNESS MANAGEMENT SYSTEM - Real-world DSA Application
 *
 * EXAM FREQUENCY: Medium. Gym management systems test BST (member records),
 * queues (class waitlists), linked lists (workout logs), stacks (payment
 * history), and hash tables (equipment/facility lookup).
 *
 * DATA STRUCTURES USED:
 *   1. Binary Search Tree (BST) - Member records by member ID
 *      - O(log n) lookup for check-in, billing, profile management
 *   2. Queue (FIFO) - Class/program waitlist
 *      - Members wait in arrival order for popular fitness classes
 *   3. Singly Linked List - Workout/exercise log per member
 *      - Dynamic list of workout sessions with exercises, sets, reps
 *   4. Stack - Payment/billing history (LIFO)
 *      - Most recent payment viewed first; undo last payment
 *   5. Hash Table (unordered_map) - Equipment/facility lookup
 *      - O(1) check if equipment is available or in use
 *
 * FUNCTIONALITIES:
 *   - Member registration and management (BST)
 *   - Class scheduling with waitlist (queue - FIFO)
 *   - Workout logging (linked list per member)
 *   - Payment processing with history (stack)
 *   - Equipment availability check (hash table)
 *   - Display members sorted by ID (BST in-order)
 *   - Membership renewal tracking
 * ============================================================================
 */
#include <iostream>
#include <string>
#include <queue>
#include <stack>
#include <vector>
#include <unordered_map>
#include <iomanip>
#include <ctime>
#include <sstream>
using namespace std;

// ============================================================================
// SECTION 1: ENTITY STRUCTURES
// ============================================================================

// -------------------------------------------------------------------
// WORKOUT LOG (Linked List Node)
// Each workout session is a node in a singly linked list.
// -------------------------------------------------------------------
struct WorkoutNode {
    string date;            // Date of workout
    string exercise;        // Exercise name
    int sets;               // Number of sets
    int reps;               // Repetitions per set
    double weight;           // Weight used (kg/lbs)
    WorkoutNode* next;      // Next workout entry

    WorkoutNode(string d, string e, int s, int r, double w)
        : date(d), exercise(e), sets(s), reps(r), weight(w), next(nullptr) {}
};

class WorkoutLog {
private:
    WorkoutNode* head;
    int count;

public:
    WorkoutLog() : head(nullptr), count(0) {}
    ~WorkoutLog() {
        WorkoutNode* current = head;
        while (current) {
            WorkoutNode* temp = current;
            current = current->next;
            delete temp;
        }
    }

    // Insert at head (most recent first). O(1).
    void addEntry(string exercise, int sets, int reps, double weight) {
        time_t now = time(0);
        string dt = ctime(&now);
        if (!dt.empty() && dt.back() == '\n') dt.pop_back();

        WorkoutNode* node = new WorkoutNode(dt, exercise, sets, reps, weight);
        node->next = head;
        head = node;
        count++;
    }

    void display() {
        if (!head) {
            cout << "    No workout entries.\n";
            return;
        }
        cout << "    " << left << setw(25) << "Date"
             << setw(20) << "Exercise"
             << setw(6) << "Sets"
             << setw(6) << "Reps"
             << "Weight\n";
        cout << "    " << string(65, '-') << "\n";
        WorkoutNode* current = head;
        while (current) {
            cout << "    " << left << setw(25) << current->date
                 << setw(20) << current->exercise
                 << setw(6) << current->sets
                 << setw(6) << current->reps
                 << current->weight << " kg\n";
            current = current->next;
        }
    }

    int getEntryCount() { return count; }
};

// -------------------------------------------------------------------
// MEMBER STRUCTURE
// Stored in BST keyed by member ID for O(log n) access.
// Each member has a linked list of workouts and a stack of payments.
// -------------------------------------------------------------------
struct Member {
    int memberId;              // Unique member ID
    string name;               // Full name
    string phone;              // Contact number
    string membershipType;     // Basic, Premium, VIP
    string joinDate;           // Date joined
    string expiryDate;         // Membership expiry
    WorkoutLog workouts;       // Linked list of workout sessions
    stack<string> paymentHistory;  // Stack of payment records

    Member() : memberId(0) {}
    Member(int id, string n, string p, string type, string expiry)
        : memberId(id), name(n), phone(p),
          membershipType(type), expiryDate(expiry) {
        time_t now = time(0);
        joinDate = ctime(&now);
        if (!joinDate.empty() && joinDate.back() == '\n')
            joinDate.pop_back();
    }
};

// -------------------------------------------------------------------
// FITNESS CLASS STRUCTURE
// Each class has a queue for waitlist (FIFO).
// -------------------------------------------------------------------
struct FitnessClass {
    string className;           // Yoga, Zumba, CrossFit, etc.
    string instructor;          // Instructor name
    int maxCapacity;            // Max participants
    int enrolledCount;          // Currently enrolled
    string timeSlot;            // Class time
    queue<int> waitlist;        // Queue of member IDs waiting (FIFO)

    FitnessClass() : maxCapacity(0), enrolledCount(0) {}
    FitnessClass(string name, string inst, int cap, string time)
        : className(name), instructor(inst),
          maxCapacity(cap), enrolledCount(0), timeSlot(time) {}

    bool isFull() { return enrolledCount >= maxCapacity; }
};

// ============================================================================
// SECTION 2: BST FOR MEMBER RECORDS
//
// Why BST? Members are searched by ID and displayed sorted.
// BST gives O(log n) CRUD and O(n) in-order sorted traversal.
// ============================================================================

struct MemberNode {
    Member member;
    MemberNode* left;
    MemberNode* right;

    MemberNode(Member m) : member(m), left(nullptr), right(nullptr) {}
};

class MemberBST {
private:
    MemberNode* root;

    MemberNode* insert(MemberNode* node, Member m) {
        if (!node) return new MemberNode(m);
        if (m.memberId < node->member.memberId)
            node->left = insert(node->left, m);
        else if (m.memberId > node->member.memberId)
            node->right = insert(node->right, m);
        return node;
    }

    MemberNode* search(MemberNode* node, int id) {
        if (!node || node->member.memberId == id) return node;
        if (id < node->member.memberId)
            return search(node->left, id);
        return search(node->right, id);
    }

    void inorderDisplay(MemberNode* node) {
        if (!node) return;
        inorderDisplay(node->left);
        cout << "  " << left << setw(8) << node->member.memberId
             << setw(25) << node->member.name
             << setw(15) << node->member.membershipType
             << setw(12) << node->member.expiryDate
             << node->member.workouts.getEntryCount() << " workouts\n";
        inorderDisplay(node->right);
    }

    void destroy(MemberNode* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

public:
    MemberBST() : root(nullptr) {}
    ~MemberBST() { destroy(root); }

    bool addMember(Member m) {
        if (search(root, m.memberId)) {
            cout << "  [ERROR] Member ID " << m.memberId << " already exists.\n";
            return false;
        }
        root = insert(root, m);
        cout << "  [OK] Member '" << m.name << "' (ID: " << m.memberId
             << ") registered. Plan: " << m.membershipType << "\n";
        return true;
    }

    Member* findMember(int id) {
        MemberNode* node = search(root, id);
        if (node) return &node->member;
        return nullptr;
    }

    void displayAll() {
        if (!root) {
            cout << "  [INFO] No members registered.\n";
            return;
        }
        cout << "  " << left << setw(8) << "ID"
             << setw(25) << "Name"
             << setw(15) << "Plan"
             << setw(12) << "Expiry"
             << "Workouts\n";
        cout << "  " << string(70, '-') << "\n";
        inorderDisplay(root);
    }

    void displayByPlan(string plan) {
        cout << "  Members on " << plan << " plan:\n";
        // Iterative in-order traversal with filter
        stack<MemberNode*> stk;
        MemberNode* curr = root;
        bool found = false;
        while (curr || !stk.empty()) {
            while (curr) {
                stk.push(curr);
                curr = curr->left;
            }
            curr = stk.top();
            stk.pop();
            if (curr->member.membershipType == plan) {
                if (!found) {
                    cout << "  " << left << setw(8) << "ID"
                         << setw(25) << "Name" << "\n";
                    found = true;
                }
                cout << "  " << left << setw(8) << curr->member.memberId
                     << setw(25) << curr->member.name << "\n";
            }
            curr = curr->right;
        }
        if (!found) cout << "    No members found.\n";
    }
};

// ============================================================================
// SECTION 3: GYM SYSTEM
// ============================================================================

class GymSystem {
private:
    MemberBST members;                            // BST: memberId -> Member
    unordered_map<string, FitnessClass> classes;  // Hash: className -> Class
    unordered_map<string, bool> equipment;        // Hash: equipment name -> available
    int memberCounter;

public:
    GymSystem() : memberCounter(1000) {
        // Seed equipment (hash table - O(1) availability check)
        equipment["Treadmill 1"] = true;
        equipment["Treadmill 2"] = true;
        equipment["Bench Press"] = true;
        equipment["Squat Rack"] = true;
        equipment["Dumbbells (5-50)"] = true;
        equipment["Lat Pulldown"] = true;
        equipment["Leg Press"] = true;
        equipment["Stationary Bike"] = true;
        equipment["Rowing Machine"] = true;
        equipment["Cable Crossover"] = true;

        // Seed fitness classes (with queue waitlists)
        classes["Yoga"] = FitnessClass("Yoga", "Sarah Johnson", 20, "7:00 AM");
        classes["Zumba"] = FitnessClass("Zumba", "Mike Chen", 25, "8:00 AM");
        classes["CrossFit"] = FitnessClass("CrossFit", "Alex Rivera", 15, "9:00 AM");
        classes["Spin Class"] = FitnessClass("Spin Class", "Emma Wilson", 20, "10:00 AM");
        classes["Pilates"] = FitnessClass("Pilates", "Lisa Park", 15, "6:00 PM");
    }

    // -------------------------------------------------------------------
    // REGISTER MEMBER: Insert into BST. O(log n).
    // -------------------------------------------------------------------
    void registerMember(string name, string phone, string plan, string expiry) {
        members.addMember(Member(memberCounter++, name, phone, plan, expiry));
    }

    // -------------------------------------------------------------------
    // LOG WORKOUT: Add to member's workout linked list. O(log n) + O(1).
    // -------------------------------------------------------------------
    void logWorkout(int memberId, string exercise, int sets, int reps, double weight) {
        Member* m = members.findMember(memberId);
        if (!m) {
            cout << "  [ERROR] Member " << memberId << " not found.\n";
            return;
        }
        m->workouts.addEntry(exercise, sets, reps, weight);
        cout << "  [OK] Workout logged for " << m->name << ".\n";
    }

    // -------------------------------------------------------------------
    // VIEW WORKOUT HISTORY: Display linked list for a member. O(n).
    // -------------------------------------------------------------------
    void viewWorkouts(int memberId) {
        Member* m = members.findMember(memberId);
        if (!m) {
            cout << "  [ERROR] Member " << memberId << " not found.\n";
            return;
        }
        cout << "  Workout History for " << m->name
             << " (ID: " << m->memberId << "):\n";
        m->workouts.display();
    }

    // -------------------------------------------------------------------
    // PROCESS PAYMENT: Add to member's payment history stack. O(log n) + O(1).
    // -------------------------------------------------------------------
    void processPayment(int memberId, double amount, string method) {
        Member* m = members.findMember(memberId);
        if (!m) {
            cout << "  [ERROR] Member " << memberId << " not found.\n";
            return;
        }

        time_t now = time(0);
        string dt = ctime(&now);
        if (!dt.empty() && dt.back() == '\n') dt.pop_back();

        stringstream ss;
        ss << "$" << fixed << setprecision(2) << amount
           << " via " << method << " on " << dt;
        m->paymentHistory.push(ss.str());
        cout << "  [OK] Payment of $" << amount << " received from "
             << m->name << " (" << method << ").\n";
    }

    // -------------------------------------------------------------------
    // VIEW PAYMENT HISTORY: Display stack (most recent first). O(n).
    // -------------------------------------------------------------------
    void viewPayments(int memberId) {
        Member* m = members.findMember(memberId);
        if (!m) {
            cout << "  [ERROR] Member " << memberId << " not found.\n";
            return;
        }
        cout << "  Payment History for " << m->name << ":\n";
        if (m->paymentHistory.empty()) {
            cout << "    No payments recorded.\n";
            return;
        }
        stack<string> temp = m->paymentHistory;
        int i = 1;
        while (!temp.empty()) {
            cout << "  " << i++ << ". " << temp.top() << "\n";
            temp.pop();
        }
    }

    // -------------------------------------------------------------------
    // EQUIPMENT LOOKUP: O(1) hash table check.
    // -------------------------------------------------------------------
    void checkEquipment(string equipName) {
        auto it = equipment.find(equipName);
        if (it == equipment.end()) {
            cout << "  [INFO] Equipment '" << equipName << "' not found.\n";
            return;
        }
        cout << "  " << equipName << ": "
             << (it->second ? "AVAILABLE" : "IN USE") << "\n";
    }

    void toggleEquipment(string equipName) {
        auto it = equipment.find(equipName);
        if (it == equipment.end()) {
            cout << "  [ERROR] Equipment not found.\n";
            return;
        }
        it->second = !it->second;
        cout << "  [OK] " << equipName << " is now "
             << (it->second ? "AVAILABLE" : "IN USE") << "\n";
    }

    void displayEquipment() {
        cout << "  " << left << setw(30) << "Equipment"
             << "Status\n";
        cout << "  " << string(45, '-') << "\n";
        for (auto& pair : equipment) {
            cout << "  " << left << setw(30) << pair.first
                 << (pair.second ? "AVAILABLE" : "IN USE") << "\n";
        }
    }

    // -------------------------------------------------------------------
    // CLASS BOOKING WITH WAITLIST (Queue - FIFO)
    // -------------------------------------------------------------------
    void bookClass(int memberId, string className) {
        auto it = classes.find(className);
        if (it == classes.end()) {
            cout << "  [ERROR] Class '" << className << "' not found.\n";
            return;
        }
        Member* m = members.findMember(memberId);
        if (!m) {
            cout << "  [ERROR] Member " << memberId << " not found.\n";
            return;
        }

        FitnessClass& fc = it->second;
        if (fc.isFull()) {
            fc.waitlist.push(memberId);
            cout << "  [WAITLIST] " << className << " is full. "
                 << m->name << " added to waitlist (position "
                 << fc.waitlist.size() << ").\n";
        } else {
            fc.enrolledCount++;
            cout << "  [OK] " << m->name << " enrolled in "
                 << className << " (" << fc.timeSlot << ").\n";
        }
    }

    void showClasses() {
        cout << "  " << left << setw(20) << "Class"
             << setw(20) << "Instructor"
             << setw(12) << "Time"
             << setw(10) << "Enrolled"
             << "Status\n";
        cout << "  " << string(70, '-') << "\n";
        for (auto& pair : classes) {
            FitnessClass& fc = pair.second;
            cout << "  " << left << setw(20) << pair.first
                 << setw(20) << fc.instructor
                 << setw(12) << fc.timeSlot
                 << setw(10) << (to_string(fc.enrolledCount) + "/" + to_string(fc.maxCapacity))
                 << (fc.isFull() ? "FULL" : "Available")
                 << " | Wait: " << fc.waitlist.size() << "\n";
        }
    }

    void run() {
        cout << "\n=============================================\n";
        cout << "   GYM & FITNESS MANAGEMENT SYSTEM\n";
        cout << "   Data Structures: BST, Queue, Linked List,\n";
        cout << "   Stack, Hash Table\n";
        cout << "=============================================\n\n";

        int choice, memberId, sets, reps;
        string name, phone, plan, expiry, exercise, className, equipName, method;
        double weight, amount;

        do {
            cout << "\n------ MENU ------\n";
            cout << "  [MEMBERS - BST]\n";
            cout << "    1. Register Member\n";
            cout << "    2. Search Member\n";
            cout << "    3. Display All Members (BST In-Order)\n";
            cout << "    4. Display Members by Plan\n";
            cout << "  [WORKOUTS - Linked List]\n";
            cout << "    5. Log Workout\n";
            cout << "    6. View Workout History\n";
            cout << "  [PAYMENTS - Stack]\n";
            cout << "    7. Process Payment\n";
            cout << "    8. View Payment History (Most Recent First)\n";
            cout << "  [CLASSES - Queue]\n";
            cout << "    9. Show Fitness Classes\n";
            cout << "   10. Book a Class (Queue Waitlist)\n";
            cout << "  [EQUIPMENT - Hash Table]\n";
            cout << "   11. Display Equipment\n";
            cout << "   12. Check Equipment Availability\n";
            cout << "   13. Toggle Equipment Status\n";
            cout << "    0. Exit\n";
            cout << "Choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1:
                    cout << "  Name: "; getline(cin, name);
                    cout << "  Phone: "; getline(cin, phone);
                    cout << "  Plan (Basic/Premium/VIP): "; getline(cin, plan);
                    cout << "  Expiry Date (YYYY-MM-DD): "; getline(cin, expiry);
                    registerMember(name, phone, plan, expiry);
                    break;
                case 2:
                    cout << "  Member ID: "; cin >> memberId; cin.ignore();
                    {
                        Member* m = members.findMember(memberId);
                        if (m) {
                            cout << "  Found: " << m->name
                                 << " | Plan: " << m->membershipType
                                 << " | Phone: " << m->phone
                                 << " | Joined: " << m->joinDate << "\n";
                        } else {
                            cout << "  [ERROR] Member not found.\n";
                        }
                    }
                    break;
                case 3: members.displayAll(); break;
                case 4:
                    cout << "  Plan name: "; getline(cin, plan);
                    members.displayByPlan(plan);
                    break;
                case 5:
                    cout << "  Member ID: "; cin >> memberId; cin.ignore();
                    cout << "  Exercise: "; getline(cin, exercise);
                    cout << "  Sets: "; cin >> sets; cin.ignore();
                    cout << "  Reps: "; cin >> reps; cin.ignore();
                    cout << "  Weight (kg): "; cin >> weight; cin.ignore();
                    logWorkout(memberId, exercise, sets, reps, weight);
                    break;
                case 6:
                    cout << "  Member ID: "; cin >> memberId; cin.ignore();
                    viewWorkouts(memberId);
                    break;
                case 7:
                    cout << "  Member ID: "; cin >> memberId; cin.ignore();
                    cout << "  Amount ($): "; cin >> amount; cin.ignore();
                    cout << "  Method (Cash/Card/UPI): "; getline(cin, method);
                    processPayment(memberId, amount, method);
                    break;
                case 8:
                    cout << "  Member ID: "; cin >> memberId; cin.ignore();
                    viewPayments(memberId);
                    break;
                case 9: showClasses(); break;
                case 10:
                    cout << "  Member ID: "; cin >> memberId; cin.ignore();
                    cout << "  Class Name: "; getline(cin, className);
                    bookClass(memberId, className);
                    break;
                case 11: displayEquipment(); break;
                case 12:
                    cout << "  Equipment Name: "; getline(cin, equipName);
                    checkEquipment(equipName);
                    break;
                case 13:
                    cout << "  Equipment Name: "; getline(cin, equipName);
                    toggleEquipment(equipName);
                    break;
                case 0: cout << "  Exiting. Goodbye!\n"; break;
                default: cout << "  [ERROR] Invalid choice.\n";
            }
        } while (choice != 0);
    }
};

int main() {
    GymSystem system;
    system.run();
    return 0;
}
