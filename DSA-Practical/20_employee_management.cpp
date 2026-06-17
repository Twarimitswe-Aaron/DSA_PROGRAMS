/*
 * ============================================================================
 * FILE: 20_employee_management.cpp
 * ============================================================================
 * EMPLOYEE MANAGEMENT SYSTEM - Real-world DSA Application
 *
 * EXAM FREQUENCY: Very High. HR/employee management is a commonly asked DSA
 * practical exam project testing integration of MULTIPLE data structures.
 *
 * DATA STRUCTURES USED:
 *   1. Binary Search Tree (BST)    - Employee records (sorted by ID, O(log n))
 *   2. Queue                       - Attendance tracking (FIFO check-in/out)
 *   3. Singly Linked List           - Payroll history per employee
 *   4. Stack                       - Organisational hierarchy (CEO chain)
 *   5. Hash Table (unordered_map)  - O(1) lookup by name and department
 *
 * FUNCTIONALITIES:
 *   - Hire / Fire / Promote employees (BST insert/delete/update)
 *   - Mark attendance (queue-based check-in and check-out)
 *   - Process payroll (linked list of salary, bonus, deduction records)
 *   - View org hierarchy (stack - manager chain from employee up to CEO)
 *   - Search by ID (BST O(log n)), by name (hash table O(1) avg)
 *   - Display all employees sorted by ID (BST in-order traversal)
 *   - Department-wise employee listing
 *   - Salary analysis: averages, top earners, department comparisons
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <queue>          // For attendance tracking (FIFO)
#include <stack>          // For org hierarchy (LIFO)
#include <vector>
#include <unordered_map>  // Hash table: name -> Employee, department -> list
#include <iomanip>        // For setw, setprecision formatted output
#include <ctime>          // For timestamps on attendance records
#include <algorithm>      // For min utility
using namespace std;

// ============================================================================
// SECTION 1: CORE DATA STRUCTURES
// ============================================================================

// -------------------------------------------------------------------
// Employee - Central entity of the HR system. Stores personal info,
// job details, rating, and manager reference for hierarchy building.
// Payroll history is stored in a linked list in the BST wrapping node.
// -------------------------------------------------------------------
struct Employee {
    string empId;           // Unique identifier (e.g., "E001")
    string name;            // Full legal name
    string department;      // Department (e.g., "Engineering", "Sales")
    string position;        // Job title (e.g., "Junior Developer", "Manager")
    double salary;          // Current annual salary
    string hireDate;        // Date of joining (YYYY-MM-DD)
    double rating;          // Performance rating (0.0 to 5.0)
    string managerId;       // This person's manager ID (empty if CEO)

    Employee() : empId(""), name(""), department(""), position(""),
                 salary(0.0), hireDate(""), rating(0.0), managerId("") {}

    Employee(string id, string n, string dept, string pos,
             double sal, string date, double rate, string mgrId)
        : empId(id), name(n), department(dept), position(pos),
          salary(sal), hireDate(date), rating(rate), managerId(mgrId) {}
};

// -------------------------------------------------------------------
// PayrollNode - One transaction in an employee's payment history.
// Nodes link via 'next' to form a singly linked list per employee.
// -------------------------------------------------------------------
struct PayrollNode {
    double amount;          // Payment amount
    string date;            // Transaction date (YYYY-MM-DD)
    string type;            // "SALARY", "BONUS", or "DEDUCTION"
    PayrollNode* next;      // Pointer to next payment record

    PayrollNode(double amt, string d, string t)
        : amount(amt), date(d), type(t), next(nullptr) {}
};

// ============================================================================
// SECTION 2: SINGLY LINKED LIST FOR PAYROLL
//
// Why Linked List? Payment history is dynamic - number of payments is
// unknown ahead of time. Linked list grows on demand with O(1) tail
// insertion and uses exactly as much memory as needed per employee.
// ============================================================================

class PayrollList {
private:
    PayrollNode* head;
    PayrollNode* tail;       // Enables O(1) append

public:
    PayrollList() : head(nullptr), tail(nullptr) {}

    ~PayrollList() {
        PayrollNode* current = head;
        while (current) {
            PayrollNode* temp = current;
            current = current->next;
            delete temp;
        }
    }

    // -------------------------------------------------------------------
    // ADD PAYMENT: O(1) append via tail pointer.
    // -------------------------------------------------------------------
    void addPayment(double amount, string date, string type) {
        PayrollNode* newNode = new PayrollNode(amount, date, type);
        if (!head) { head = newNode; tail = newNode; }
        else        { tail->next = newNode; tail = newNode; }
    }

    // -------------------------------------------------------------------
    // CALCULATE TOTAL: Sum all payments. O(n) traversal.
    // -------------------------------------------------------------------
    double calculateTotal() const {
        double total = 0.0;
        for (PayrollNode* t = head; t; t = t->next) total += t->amount;
        return total;
    }

    // -------------------------------------------------------------------
    // DISPLAY ALL: Print formatted payment history table. O(n).
    // -------------------------------------------------------------------
    void displayAll() const {
        if (!head) { cout << "      [INFO] No payment records.\n"; return; }
        cout << "      " << left << setw(12) << "Amount"
             << setw(20) << "Date" << "Type\n";
        cout << "      " << string(50, '-') << "\n";
        for (PayrollNode* t = head; t; t = t->next)
            cout << "      $" << left << setw(10) << fixed << setprecision(2)
                 << t->amount << setw(20) << t->date << t->type << "\n";
        cout << "      " << string(50, '-') << "\n";
        cout << "      Total Paid: $" << fixed << setprecision(2)
             << calculateTotal() << "\n";
    }
};

// ============================================================================
// SECTION 3: BST FOR EMPLOYEE RECORDS
//
// Why BST? Fast O(log n) search, insertion, deletion by employee ID.
// In-order traversal outputs records sorted by ID automatically.
// Each BST node (EmpNode) also owns a PayrollList for that employee.
// ============================================================================

struct EmpNode {
    Employee employee;
    PayrollList payroll;     // Linked list of salary payments for this employee
    EmpNode* left;
    EmpNode* right;

    EmpNode(Employee e) : employee(e), left(nullptr), right(nullptr) {}
};

class EmployeeTree {
private:
    EmpNode* root;
    int nodeCount;

    // ---- Recursive BST helpers ----

    EmpNode* insert(EmpNode* node, Employee e) {
        if (!node) return new EmpNode(e);
        if (e.empId < node->employee.empId)
            node->left = insert(node->left, e);
        else if (e.empId > node->employee.empId)
            node->right = insert(node->right, e);
        return node;
    }

    EmpNode* search(EmpNode* node, string empId) const {
        if (!node || node->employee.empId == empId) return node;
        if (empId < node->employee.empId)
            return search(node->left, empId);
        return search(node->right, empId);
    }

    EmpNode* findMin(EmpNode* node) const {
        while (node && node->left) node = node->left;
        return node;
    }

    // -------------------------------------------------------------------
    // DELETE NODE: Leaf (no children) -> delete. One child -> replace
    // with child. Two children -> replace data with inorder successor
    // (smallest in right subtree), then delete successor.
    // -------------------------------------------------------------------
    EmpNode* deleteNode(EmpNode* node, string empId) {
        if (!node) return nullptr;
        if (empId < node->employee.empId)
            node->left = deleteNode(node->left, empId);
        else if (empId > node->employee.empId)
            node->right = deleteNode(node->right, empId);
        else {
            if (!node->left) {
                EmpNode* temp = node->right; delete node; return temp;
            }
            if (!node->right) {
                EmpNode* temp = node->left; delete node; return temp;
            }
            EmpNode* successor = findMin(node->right);
            node->employee = successor->employee;
            node->right = deleteNode(node->right, successor->employee.empId);
        }
        return node;
    }

    // ---- Traversal helpers ----

    void inorderDisplay(EmpNode* node) const {
        if (!node) return;
        inorderDisplay(node->left);
        cout << "  " << left << setw(8) << node->employee.empId
             << setw(22) << node->employee.name
             << setw(15) << node->employee.department
             << setw(20) << node->employee.position
             << "$" << setw(9) << fixed << setprecision(2) << node->employee.salary
             << setw(5) << node->employee.rating << "\n";
        inorderDisplay(node->right);
    }

    void deptDisplay(EmpNode* node, const string& dept) const {
        if (!node) return;
        deptDisplay(node->left, dept);
        if (node->employee.department == dept) {
            cout << "  " << left << setw(8) << node->employee.empId
                 << setw(22) << node->employee.name
                 << setw(20) << node->employee.position
                 << "$" << setw(9) << fixed << setprecision(2) << node->employee.salary
                 << setw(5) << node->employee.rating << "\n";
        }
        deptDisplay(node->right, dept);
    }

    void collectAll(EmpNode* node, vector<Employee*>& vec) {
        if (!node) return;
        collectAll(node->left, vec);
        vec.push_back(&node->employee);
        collectAll(node->right, vec);
    }

    void destroy(EmpNode* node) {
        if (!node) return;
        destroy(node->left); destroy(node->right);
        delete node;
    }

public:
    EmployeeTree() : root(nullptr), nodeCount(0) {}
    ~EmployeeTree() { destroy(root); }

    // -------------------------------------------------------------------
    // HIRE: Insert into BST after checking for duplicate ID. O(log n) avg.
    // -------------------------------------------------------------------
    bool hireEmployee(Employee e) {
        if (search(root, e.empId)) {
            cout << "  [ERROR] Employee ID " << e.empId << " already exists!\n";
            return false;
        }
        root = insert(root, e);
        nodeCount++;
        cout << "  [OK] '" << e.name << "' hired as " << e.position
             << " in " << e.department << " (ID: " << e.empId << ").\n";
        return true;
    }

    // -------------------------------------------------------------------
    // FIRE: Delete from BST. O(log n) avg.
    // -------------------------------------------------------------------
    bool fireEmployee(string empId) {
        if (!search(root, empId)) {
            cout << "  [ERROR] Employee " << empId << " not found.\n";
            return false;
        }
        root = deleteNode(root, empId);
        nodeCount--;
        cout << "  [OK] Employee " << empId << " removed.\n";
        return true;
    }

    Employee* findEmployee(string empId) {
        EmpNode* node = search(root, empId);
        return node ? &(node->employee) : nullptr;
    }

    PayrollList* getPayroll(string empId) {
        EmpNode* node = search(root, empId);
        return node ? &(node->payroll) : nullptr;
    }

    // -------------------------------------------------------------------
    // PROMOTE: Update position and salary in-place. O(log n).
    // -------------------------------------------------------------------
    void promoteEmployee(string empId, string newPosition, double newSalary) {
        Employee* emp = findEmployee(empId);
        if (!emp) { cout << "  [ERROR] Employee " << empId << " not found.\n"; return; }
        cout << "  [OK] " << emp->name << " promoted from '" << emp->position
             << "' to '" << newPosition << "'";
        emp->position = newPosition;
        emp->salary = newSalary;
        cout << " (New salary: $" << fixed << setprecision(2) << newSalary << ").\n";
    }

    // -------------------------------------------------------------------
    // DISPLAY ALL: In-order traversal prints records sorted by ID. O(n).
    // -------------------------------------------------------------------
    void displayAllEmployees() const {
        if (!root) { cout << "  [INFO] No employees in the system.\n"; return; }
        cout << "  " << left << setw(8) << "ID" << setw(22) << "Name"
             << setw(15) << "Department" << setw(20) << "Position"
             << setw(10) << "Salary" << "Rating\n";
        cout << "  " << string(85, '-') << "\n";
        inorderDisplay(root);
        cout << "  " << string(85, '-') << "\n";
        cout << "  Total employees: " << nodeCount << "\n";
    }

    // -------------------------------------------------------------------
    // DISPLAY BY DEPARTMENT: Filtered inorder, prints only matching dept.
    // -------------------------------------------------------------------
    void displayByDepartment(const string& dept) const {
        cout << "  Employees in " << dept << ":\n";
        cout << "  " << left << setw(8) << "ID" << setw(22) << "Name"
             << setw(20) << "Position" << setw(10) << "Salary" << "Rating\n";
        cout << "  " << string(70, '-') << "\n";
        deptDisplay(root, dept);
    }

    vector<Employee*> getAllEmployees() {
        vector<Employee*> all;
        collectAll(root, all);
        return all;
    }

    // -------------------------------------------------------------------
    // GET DEPARTMENTS: Scan all employees, return unique dept names. O(n).
    // -------------------------------------------------------------------
    vector<string> getDepartments() {
        vector<Employee*> all = getAllEmployees();
        vector<string> depts;
        for (size_t i = 0; i < all.size(); i++) {
            string d = all[i]->department;
            bool found = false;
            for (size_t j = 0; j < depts.size(); j++)
                if (depts[j] == d) { found = true; break; }
            if (!found) depts.push_back(d);
        }
        return depts;
    }

    int getCount() const { return nodeCount; }
};

// ============================================================================
// SECTION 4: QUEUE FOR ATTENDANCE TRACKING
//
// Why Queue? Attendance events (check-in/out) occur chronologically and
// must be processed in FIFO order. The queue preserves the exact event
// sequence, critical for accurate time records.
// ============================================================================

class AttendanceTracker {
private:
    // pair stores: (empId, "CHECK_IN/OUT @ timestamp")
    queue<pair<string, string> > records;

    // -------------------------------------------------------------------
    // Get formatted current timestamp from system clock.
    // -------------------------------------------------------------------
    string getCurrentTimestamp() {
        time_t now = time(nullptr);
        char buf[80];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
        return string(buf);
    }

public:
    // -------------------------------------------------------------------
    // CHECK IN: Record employee arrival. O(1) queue push.
    // -------------------------------------------------------------------
    void checkIn(string empId) {
        string ts = getCurrentTimestamp();
        records.push(make_pair(empId, "CHECK_IN @ " + ts));
        cout << "  [OK] " << empId << " CHECKED IN at " << ts << ".\n";
    }

    // -------------------------------------------------------------------
    // CHECK OUT: Record employee departure. O(1) queue push.
    // -------------------------------------------------------------------
    void checkOut(string empId) {
        string ts = getCurrentTimestamp();
        records.push(make_pair(empId, "CHECK_OUT @ " + ts));
        cout << "  [OK] " << empId << " CHECKED OUT at " << ts << ".\n";
    }

    // -------------------------------------------------------------------
    // VIEW ALL: Display all events (oldest first). Queue copy O(n).
    // -------------------------------------------------------------------
    void viewAllRecords() {
        if (records.empty()) {
            cout << "  [INFO] No attendance records.\n"; return;
        }
        queue<pair<string, string> > temp = records;
        cout << "  " << left << setw(12) << "Employee" << "Event\n";
        cout << "  " << string(55, '-') << "\n";
        while (!temp.empty()) {
            cout << "  " << left << setw(12) << temp.front().first
                 << temp.front().second << "\n";
            temp.pop();
        }
    }
};

// ============================================================================
// SECTION 5: STACK FOR ORGANISATIONAL HIERARCHY
//
// Why Stack? The reporting chain from employee to CEO is LIFO: walk up
// via managerId references, pushing each level onto a stack. Popping
// displays CEO-first -> employee-last (correct org-chart ordering).
// ============================================================================

class OrgHierarchy {
private:
    stack<string> chain;  // Holds employee IDs from CEO down to employee

public:
    // -------------------------------------------------------------------
    // DISPLAY CHAIN: Starting from emp, follow managerId upward, push IDs
    // onto stack, then pop and display CEO-first. O(h) where h = chain height.
    // -------------------------------------------------------------------
    void displayChain(Employee* emp, EmployeeTree& tree) {
        if (!emp) { cout << "  [ERROR] Employee not found.\n"; return; }
        while (!chain.empty()) chain.pop();

        // Walk up: employee -> manager -> director -> ... -> CEO
        string currentId = emp->empId;
        while (currentId != "") {
            chain.push(currentId);
            Employee* current = tree.findEmployee(currentId);
            if (!current || current->managerId == "" || current->managerId == currentId)
                break;
            currentId = current->managerId;
        }

        // Pop and display: CEO first (level 0), employee last
        cout << "  Organisational Hierarchy:\n";
        cout << "  " << string(40, '-') << "\n";
        int level = 0;
        while (!chain.empty()) {
            string id = chain.top(); chain.pop();
            Employee* e = tree.findEmployee(id);
            if (e) {
                cout << "  " << string(level * 4, ' ');
                if (level == 0) cout << "[CEO] ";
                else            cout << "|-- ";
                cout << e->name << " (" << e->position << ")\n";
            }
            level++;
        }
    }
};

// ============================================================================
// SECTION 6: MAIN EMPLOYEE MANAGEMENT SYSTEM
//
// Integrates all five data structures:
//   BST        -> Employee records sorted by ID
//   Queue      -> Attendance check-in/check-out
//   LinkedList -> Payroll history per employee
//   Stack      -> Org hierarchy chain
//   Hash Table -> nameIndex (name lookup), deptIndex (dept listing)
//
// Hash indexes are rebuilt from BST after modifications (O(n)).
// ============================================================================

class EmployeeManagementSystem {
private:
    EmployeeTree empTree;
    AttendanceTracker attendance;
    OrgHierarchy hierarchy;

    // Hash: employee name -> Employee* (O(1) avg lookup)
    unordered_map<string, Employee*> nameIndex;
    // Hash: department -> vector of employees in that dept
    unordered_map<string, vector<Employee*> > deptIndex;

    // -------------------------------------------------------------------
    // Rebuild hash indexes from BST after hire/fire/promote. O(n).
    // -------------------------------------------------------------------
    void rebuildIndexes() {
        nameIndex.clear();
        deptIndex.clear();
        vector<Employee*> all = empTree.getAllEmployees();
        for (size_t i = 0; i < all.size(); i++) {
            nameIndex[all[i]->name] = all[i];
            deptIndex[all[i]->department].push_back(all[i]);
        }
    }

public:
    EmployeeManagementSystem() {}

    // ========== HIRE / FIRE / PROMOTE ==========

    void hireEmployee() {
        string id, name, dept, pos, date, mgrId;
        double salary, rating;
        cout << "  Employee ID: ";        getline(cin, id);
        cout << "  Full Name: ";           getline(cin, name);
        cout << "  Department: ";          getline(cin, dept);
        cout << "  Position: ";            getline(cin, pos);
        cout << "  Salary: $";             cin >> salary;    cin.ignore();
        cout << "  Hire Date (YYYY-MM-DD): "; getline(cin, date);
        cout << "  Rating (0.0-5.0): ";    cin >> rating;    cin.ignore();
        cout << "  Manager ID (blank if CEO): "; getline(cin, mgrId);
        if (empTree.hireEmployee(
                Employee(id, name, dept, pos, salary, date, rating, mgrId)))
            rebuildIndexes();
    }

    void fireEmployee() {
        string id;
        cout << "  Employee ID to terminate: "; getline(cin, id);
        if (empTree.fireEmployee(id)) rebuildIndexes();
    }

    void promoteEmployee() {
        string id, newPos;
        double newSalary;
        cout << "  Employee ID: ";  getline(cin, id);
        cout << "  New Position: "; getline(cin, newPos);
        cout << "  New Salary: $";  cin >> newSalary; cin.ignore();
        empTree.promoteEmployee(id, newPos, newSalary);
        rebuildIndexes();
    }

    // ========== SEARCH ==========

    void searchById() {
        string id;
        cout << "  Employee ID: "; getline(cin, id);
        Employee* e = empTree.findEmployee(id);
        if (!e) { cout << "  [ERROR] No employee found.\n"; return; }
        cout << "  " << left << setw(18) << "ID:"       << e->empId << "\n"
             << "  " << setw(18) << "Name:"             << e->name << "\n"
             << "  " << setw(18) << "Department:"       << e->department << "\n"
             << "  " << setw(18) << "Position:"         << e->position << "\n"
             << "  " << setw(18) << "Salary:"           << "$" << fixed << setprecision(2) << e->salary << "\n"
             << "  " << setw(18) << "Hire Date:"        << e->hireDate << "\n"
             << "  " << setw(18) << "Rating:"           << e->rating << "\n"
             << "  " << setw(18) << "Manager:"
             << (e->managerId == "" ? "None (CEO)" : e->managerId) << "\n";
    }

    // -------------------------------------------------------------------
    // SEARCH BY NAME: O(1) average via hash table (unordered_map).
    // -------------------------------------------------------------------
    void searchByName() {
        string name;
        cout << "  Employee Name: "; getline(cin, name);
        unordered_map<string, Employee*>::iterator it = nameIndex.find(name);
        if (it != nameIndex.end()) {
            Employee* e = it->second;
            cout << "  Found: " << e->empId << " | " << e->name
                 << " | " << e->department << " | " << e->position
                 << " | $" << fixed << setprecision(2) << e->salary << "\n";
        } else {
            cout << "  [ERROR] No employee found with name '" << name << "'.\n";
        }
    }

    // ========== ATTENDANCE ==========

    void markAttendance() {
        string id;
        cout << "  Employee ID: "; getline(cin, id);
        if (!empTree.findEmployee(id)) {
            cout << "  [ERROR] Employee " << id << " not found.\n"; return;
        }
        cout << "  1. Check In  2. Check Out\n  Choice: ";
        int choice; cin >> choice; cin.ignore();
        if (choice == 1)      attendance.checkIn(id);
        else if (choice == 2) attendance.checkOut(id);
        else                  cout << "  [ERROR] Invalid choice.\n";
    }

    void viewAttendance() { attendance.viewAllRecords(); }

    // ========== PAYROLL ==========

    void processPayroll() {
        string id, date;
        double amount;
        cout << "  Employee ID: "; getline(cin, id);
        PayrollList* pl = empTree.getPayroll(id);
        if (!pl) { cout << "  [ERROR] Employee " << id << " not found.\n"; return; }
        cout << "  1. Salary  2. Bonus  3. Deduction\n  Choice: ";
        int type; cin >> type; cin.ignore();
        cout << "  Amount: $";  cin >> amount;  cin.ignore();
        cout << "  Date (YYYY-MM-DD): "; getline(cin, date);
        string typeStr = (type == 2) ? "BONUS" : (type == 3) ? "DEDUCTION" : "SALARY";
        pl->addPayment(amount, date, typeStr);
        cout << "  [OK] $" << fixed << setprecision(2) << amount
             << " " << typeStr << " recorded.\n";
    }

    void viewPayroll() {
        string id;
        cout << "  Employee ID: "; getline(cin, id);
        Employee* e = empTree.findEmployee(id);
        PayrollList* pl = empTree.getPayroll(id);
        if (!e || !pl) { cout << "  [ERROR] Employee " << id << " not found.\n"; return; }
        cout << "  Payroll History: " << e->name << " (" << id << ")\n";
        cout << "  " << string(55, '=') << "\n";
        pl->displayAll();
    }

    // ========== ORG HIERARCHY ==========

    void viewHierarchy() {
        string id;
        cout << "  Employee ID: "; getline(cin, id);
        hierarchy.displayChain(empTree.findEmployee(id), empTree);
    }

    // ========== DEPARTMENT LISTING ==========

    void listByDepartment() {
        vector<string> depts = empTree.getDepartments();
        if (depts.empty()) { cout << "  [INFO] No departments found.\n"; return; }
        cout << "  Departments:\n";
        for (size_t i = 0; i < depts.size(); i++)
            cout << "    " << (i + 1) << ". " << depts[i] << "\n";
        cout << "  Select (1-" << depts.size() << "): ";
        int choice; cin >> choice; cin.ignore();
        if (choice >= 1 && choice <= (int)depts.size())
            empTree.displayByDepartment(depts[choice - 1]);
        else
            cout << "  [ERROR] Invalid selection.\n";
    }

    // ========== SALARY ANALYSIS ==========

    // -------------------------------------------------------------------
    // SALARY ANALYSIS: Overall stats, dept breakdown, top 3 earners
    // and top 3 performers. Uses bubble sort on pointer copies.
    // -------------------------------------------------------------------
    void salaryAnalysis() {
        vector<Employee*> all = empTree.getAllEmployees();
        if (all.empty()) { cout << "  [INFO] No employees to analyse.\n"; return; }

        double totalSal = 0.0, maxSal = 0.0;
        string maxName, maxDept;
        for (size_t i = 0; i < all.size(); i++) {
            totalSal += all[i]->salary;
            if (all[i]->salary > maxSal) {
                maxSal = all[i]->salary;
                maxName = all[i]->name;
                maxDept = all[i]->department;
            }
        }
        cout << "\n  ====== SALARY ANALYSIS ======\n\n";
        cout << "  " << left << setw(35) << "Total Employees:" << all.size() << "\n";
        cout << "  " << setw(35) << "Total Salary Pool:"
             << "$" << fixed << setprecision(2) << totalSal << "\n";
        cout << "  " << setw(35) << "Average Salary:"
             << "$" << (totalSal / all.size()) << "\n";
        cout << "  " << setw(35) << "Highest Earner:"
             << maxName << " ($" << maxSal << ", " << maxDept << ")\n";

        // Department-wise breakdown
        cout << "\n  --- Department-wise Breakdown ---\n";
        vector<string> depts = empTree.getDepartments();
        for (size_t i = 0; i < depts.size(); i++) {
            double dTotal = 0.0; int dCount = 0;
            double dMax = 0.0; string dTop;
            for (size_t j = 0; j < all.size(); j++) {
                if (all[j]->department == depts[i]) {
                    dTotal += all[j]->salary; dCount++;
                    if (all[j]->salary > dMax) { dMax = all[j]->salary; dTop = all[j]->name; }
                }
            }
            cout << "  " << left << setw(22) << depts[i]
                 << "Cnt: " << dCount << " | Avg: $" << (dCount ? dTotal / dCount : 0.0)
                 << " | Top: " << dTop << " ($" << dMax << ")\n";
        }

        // Top 3 by salary (bubble sort on a pointer vector copy)
        cout << "\n  --- Top 3 Highest Paid ---\n";
        vector<Employee*> sorted = all;
        for (size_t i = 0; i < sorted.size(); i++)
            for (size_t j = 0; j < sorted.size() - 1 - i; j++)
                if (sorted[j]->salary < sorted[j + 1]->salary)
                    swap(sorted[j], sorted[j + 1]);
        int topN = min(3, (int)sorted.size());
        for (int i = 0; i < topN; i++)
            cout << "  " << (i + 1) << ". " << left
                 << setw(22) << sorted[i]->name
                 << setw(15) << sorted[i]->department
                 << "$" << sorted[i]->salary
                 << " (Rating: " << sorted[i]->rating << ")\n";

        // Top 3 by performance rating
        cout << "\n  --- Top 3 by Performance Rating ---\n";
        sorted = all;
        for (size_t i = 0; i < sorted.size(); i++)
            for (size_t j = 0; j < sorted.size() - 1 - i; j++)
                if (sorted[j]->rating < sorted[j + 1]->rating)
                    swap(sorted[j], sorted[j + 1]);
        topN = min(3, (int)sorted.size());
        for (int i = 0; i < topN; i++)
            cout << "  " << (i + 1) << ". " << left
                 << setw(22) << sorted[i]->name
                 << setw(15) << sorted[i]->department
                 << "Rating: " << sorted[i]->rating
                 << " | $" << sorted[i]->salary << "\n";
        cout << "\n  ====== END ANALYSIS ======\n";
    }

    // ========== DISPLAY ==========

    void displayAll() { empTree.displayAllEmployees(); }

    // ========== MAIN MENU LOOP ==========

    // -------------------------------------------------------------------
    // RUN: Interactive menu-driven console interface. Loops until 0.
    // -------------------------------------------------------------------
    void run() {
        cout << "\n=============================================\n";
        cout << "   EMPLOYEE MANAGEMENT SYSTEM\n";
        cout << "   BST | Queue | LinkedList | Stack | HashTable\n";
        cout << "=============================================\n";

        int choice;
        do {
            cout << "\n---------------------- MENU ----------------------\n";
            cout << "  1. Hire Employee        8.  Process Payroll\n";
            cout << "  2. Fire Employee        9.  View Payroll History\n";
            cout << "  3. Promote Employee    10.  View Org Hierarchy\n";
            cout << "  4. Search by ID        11.  List by Department\n";
            cout << "  5. Search by Name      12.  Salary Analysis\n";
            cout << "  6. Mark Attendance     13.  Display All\n";
            cout << "  7. View Attendance      0.  Exit\n";
            cout << "-----------------------------------------------\n";
            cout << "Choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1:  hireEmployee();     break;
                case 2:  fireEmployee();     break;
                case 3:  promoteEmployee();  break;
                case 4:  searchById();       break;
                case 5:  searchByName();     break;
                case 6:  markAttendance();   break;
                case 7:  viewAttendance();   break;
                case 8:  processPayroll();   break;
                case 9:  viewPayroll();      break;
                case 10: viewHierarchy();    break;
                case 11: listByDepartment(); break;
                case 12: salaryAnalysis();   break;
                case 13: displayAll();       break;
                case 0:  cout << "  Exiting system. Goodbye!\n"; break;
                default: cout << "  [ERROR] Invalid choice.\n";
            }
        } while (choice != 0);
    }
};

// ============================================================================
// MAIN FUNCTION - Entry point
// ============================================================================
int main() {
    EmployeeManagementSystem system;
    system.run();
    return 0;
}
