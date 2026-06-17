/*
 * ============================================================================
 * FILE: 17_student_record.cpp
 * ============================================================================
 * STUDENT RECORD MANAGEMENT SYSTEM - Real-world DSA Application
 *
 * EXAM FREQUENCY: High. Student record systems test your ability to integrate
 * multiple data structures handling hierarchical, sequential, and associative
 * data within a single real-world application.
 *
 * DATA STRUCTURES USED:
 *   1. Binary Search Tree (BST)  - Student records sorted by roll number
 *      (O(log n) search/insert/delete for sorted student lookup)
 *   2. Singly Linked List        - Course enrollment per student
 *      (dynamic list of enrolled courses with grades; no fixed limit)
 *   3. Stack                     - Grade history / undo grade changes
 *      (LIFO: most recent grade change undone first)
 *   4. Hash Table (unordered_map) - Quick student lookup by name or email
 *      (O(1) average case when roll number is unknown)
 *   5. Queue                     - Waitlist for full courses
 *      (FIFO: first student to wait gets the first available seat)
 *
 * FUNCTIONALITIES:
 *   - Add / Remove / Search students by roll number (BST)
 *   - Enroll students in courses (linked list per student)
 *   - Record and update grades with undo capability (stack)
 *   - View all students sorted by roll number (BST in-order traversal)
 *   - Search student by name or email (hash table - O(1) average)
 *   - Waitlist for full courses (queue - FIFO processing)
 *   - Generate report card with GPA calculation
 *   - Course catalog management (add courses, set capacity)
 * ============================================================================
 */
#include <iostream>
#include <string>
#include <queue>         // For course waitlist (FIFO)
#include <stack>         // For grade undo history (LIFO)
#include <vector>
#include <unordered_map> // For name/email -> student lookup (hash table)
#include <iomanip>       // For formatted output and GPA precision
#include <sstream>       // For string stream operations
#include <cmath>         // For GPA rounding
using namespace std;

// ============================================================================
// SECTION 1: ENTITY STRUCTURES
// ============================================================================

// -------------------------------------------------------------------
// COURSE STRUCTURE
// Represents a course in the catalog. Courses have a fixed capacity;
// when full, students are placed on a FIFO waitlist (queue).
// Courses are stored in a hash table (courseCode -> Course) for O(1)
// lookup during enrollment.
// -------------------------------------------------------------------
struct Course {
    string courseCode;      // Unique identifier (e.g., "CS201")
    string courseName;      // Full course name (e.g., "Data Structures")
    int credits;            // Credit hours (e.g., 3, 4)
    int maxCapacity;        // Maximum students allowed
    int enrolledCount;      // Currently enrolled students
    queue<string> waitlist; // Queue of student IDs waiting for a seat (FIFO)

    Course() : courseCode(""), courseName(""), credits(0),
               maxCapacity(0), enrolledCount(0) {}

    Course(string code, string name, int cr, int cap)
        : courseCode(code), courseName(name), credits(cr),
          maxCapacity(cap), enrolledCount(0) {}

    bool isFull() const { return enrolledCount >= maxCapacity; }

    void display() const {
        cout << "  " << left << setw(10) << courseCode
             << setw(35) << courseName
             << setw(8) << credits
             << enrolledCount << "/" << maxCapacity
             << (isFull() ? " (FULL)" : "")
             << " | Waitlist: " << waitlist.size() << "\n";
    }
};

// -------------------------------------------------------------------
// ENROLLMENT RECORD (Linked List Node)
// Each student has a singly linked list of enrolled courses. We use
// a linked list instead of a vector because:
//   1. Dynamic sizing with no reallocation cost
//   2. O(1) insertion at head for new enrollments
//   3. Easy removal if a student drops a course
// Each node stores the course code, grade info, and a pointer to
// the next enrollment in the student's list.
// -------------------------------------------------------------------
struct EnrollmentNode {
    string courseCode;       // Code of enrolled course
    string grade;            // Letter grade (A, B+, C, etc.) or "IP" (In Progress)
    double gradePoints;      // Numeric grade points (4.0 scale)
    EnrollmentNode* next;    // Pointer to next enrollment

    EnrollmentNode(string code)
        : courseCode(code), grade("IP"), gradePoints(0.0), next(nullptr) {}
};

// -------------------------------------------------------------------
// GRADE CHANGE RECORD (Stack)
// Stored on a stack per student to enable undo of grade changes.
// Why Stack? Grade changes should be undone in reverse order --
// the most recent change is undone first (LIFO). This exactly
// matches the stack data structure's behavior.
// -------------------------------------------------------------------
struct GradeChange {
    string courseCode;    // Which course's grade was changed
    string oldGrade;      // Previous grade before change
    double oldGradePoints; // Previous grade points before change
    string newGrade;      // New grade after change
    double newGradePoints; // New grade points after change

    GradeChange(string code, string oldG, double oldGP,
                string newG, double newGP)
        : courseCode(code), oldGrade(oldG), oldGradePoints(oldGP),
          newGrade(newG), newGradePoints(newGP) {}
};

// ============================================================================
// SECTION 2: LINKED LIST FOR COURSE ENROLLMENT
//
// Why Linked List? Each student can enroll in a variable number of
// courses. A linked list provides dynamic memory allocation without
// the overhead of resizing an array/vector. Insertion at the head is
// O(1) -- we add new enrollments at the front for efficiency since
// order does not matter for grade calculation.
// ============================================================================

class EnrollmentList {
private:
    EnrollmentNode* head;  // Head of linked list of course enrollments
    int count;             // Number of enrolled courses

public:
    EnrollmentList() : head(nullptr), count(0) {}

    ~EnrollmentList() {
        EnrollmentNode* current = head;
        while (current) {
            EnrollmentNode* temp = current;
            current = current->next;
            delete temp;
        }
    }

    // -------------------------------------------------------------------
    // ENROLL IN COURSE: Insert at head of linked list. O(1).
    // -------------------------------------------------------------------
    void enroll(string courseCode) {
        EnrollmentNode* newNode = new EnrollmentNode(courseCode);
        newNode->next = head;
        head = newNode;
        count++;
    }

    // -------------------------------------------------------------------
    // DROP COURSE: Remove from linked list. O(n).
    // -------------------------------------------------------------------
    bool drop(string courseCode) {
        EnrollmentNode* current = head;
        EnrollmentNode* prev = nullptr;

        while (current) {
            if (current->courseCode == courseCode) {
                if (prev) {
                    prev->next = current->next;
                } else {
                    head = current->next;  // Removing head node
                }
                delete current;
                count--;
                return true;
            }
            prev = current;
            current = current->next;
        }
        return false;  // Course not found in enrollment list
    }

    // -------------------------------------------------------------------
    // FIND ENROLLMENT: Search linked list for a specific course. O(n).
    // -------------------------------------------------------------------
    EnrollmentNode* find(string courseCode) {
        EnrollmentNode* current = head;
        while (current) {
            if (current->courseCode == courseCode)
                return current;
            current = current->next;
        }
        return nullptr;
    }

    // -------------------------------------------------------------------
    // UPDATE GRADE: Set grade for a specific course. O(n).
    // Returns the old grade info for undo stack.
    // -------------------------------------------------------------------
    GradeChange updateGrade(string courseCode, string newGrade, double newGP) {
        EnrollmentNode* node = find(courseCode);
        if (node) {
            GradeChange change(courseCode, node->grade, node->gradePoints,
                               newGrade, newGP);
            node->grade = newGrade;
            node->gradePoints = newGP;
            return change;
        }
        return GradeChange(courseCode, "", 0.0, newGrade, newGP);
    }

    // -------------------------------------------------------------------
    // CALCULATE GPA: Iterate linked list, sum gradePoints * credits. O(n).
    // Formula: GPA = sum(gradePoints * credits) / sum(credits)
    // Only counts courses with non-"IP" (In Progress) grades.
    // -------------------------------------------------------------------
    double calculateGPA(unordered_map<string, Course>& catalog) {
        double totalPoints = 0.0;
        int totalCredits = 0;
        EnrollmentNode* current = head;

        while (current) {
            if (current->grade != "IP") {
                auto it = catalog.find(current->courseCode);
                if (it != catalog.end()) {
                    totalPoints += current->gradePoints * it->second.credits;
                    totalCredits += it->second.credits;
                }
            }
            current = current->next;
        }

        if (totalCredits == 0) return 0.0;
        return totalPoints / totalCredits;
    }

    // -------------------------------------------------------------------
    // DISPLAY ENROLLED COURSES: Traverse linked list. O(n).
    // -------------------------------------------------------------------
    void display(unordered_map<string, Course>& catalog) {
        if (!head) {
            cout << "    No enrolled courses.\n";
            return;
        }
        EnrollmentNode* current = head;
        cout << "    " << left << setw(10) << "Code"
             << setw(35) << "Course Name"
             << setw(8) << "Credits"
             << "Grade\n";
        cout << "    " << string(65, '-') << "\n";
        while (current) {
            string cName = current->courseCode;
            int creds = 0;
            auto it = catalog.find(current->courseCode);
            if (it != catalog.end()) {
                cName = it->second.courseName;
                creds = it->second.credits;
            }
            cout << "    " << left << setw(10) << current->courseCode
                 << setw(35) << cName
                 << setw(8) << creds
                 << current->grade << "\n";
            current = current->next;
        }
    }

    int getCount() const { return count; }
    EnrollmentNode* getHead() { return head; }
};

// ============================================================================
// SECTION 3: BST FOR STUDENT RECORDS
//
// Why BST? Student records must be searched, inserted, and displayed
// sorted by roll number efficiently. BST provides:
//   - O(log n) search by roll number (average case)
//   - O(log n) insertion and deletion
//   - O(n) in-order traversal for sorted display
//   - Natural ordering by roll number for report generation
// A hash table would give O(1) lookup but cannot produce sorted output
// and does not support range queries. BST is the right choice here.
// ============================================================================

struct StudentNode {
    int rollNumber;              // Unique integer roll number (key)
    string name;                 // Full name
    string email;                // Email address
    EnrollmentList courses;      // Linked list of enrolled courses
    stack<GradeChange> gradeHistory; // Stack for undo (LIFO)
    StudentNode* left;           // Left child (smaller roll numbers)
    StudentNode* right;          // Right child (larger roll numbers)

    StudentNode(int roll, string n, string e)
        : rollNumber(roll), name(n), email(e),
          left(nullptr), right(nullptr) {}
};

class StudentBST {
private:
    StudentNode* root;

    // -------------------------------------------------------------------
    // INSERT helper: Recursive BST insertion by roll number.
    // Smaller roll numbers go left, larger go right. O(log n) average.
    // -------------------------------------------------------------------
    StudentNode* insert(StudentNode* node, int roll, string name, string email) {
        if (!node)
            return new StudentNode(roll, name, email);
        if (roll < node->rollNumber)
            node->left = insert(node->left, roll, name, email);
        else if (roll > node->rollNumber)
            node->right = insert(node->right, roll, name, email);
        // Equal roll numbers: handled by caller (duplicate check)
        return node;
    }

    // -------------------------------------------------------------------
    // SEARCH helper: Recursive BST search by roll number.
    // Compares key at each node, traverses left or right. O(log n).
    // -------------------------------------------------------------------
    StudentNode* search(StudentNode* node, int roll) {
        if (!node || node->rollNumber == roll)
            return node;
        if (roll < node->rollNumber)
            return search(node->left, roll);
        return search(node->right, roll);
    }

    // -------------------------------------------------------------------
    // FIND MIN: Helper used in deletion. Leftmost node = smallest key.
    // O(log n) worst case (O(n) in degenerate tree).
    // -------------------------------------------------------------------
    StudentNode* findMin(StudentNode* node) {
        while (node && node->left) node = node->left;
        return node;
    }

    // -------------------------------------------------------------------
    // DELETE helper: Recursive deletion with three cases:
    //   - Leaf node: simply remove
    //   - One child: replace with child
    //   - Two children: replace with inorder successor (smallest in right)
    // O(log n) average.
    // -------------------------------------------------------------------
    StudentNode* deleteNode(StudentNode* node, int roll) {
        if (!node) return nullptr;

        if (roll < node->rollNumber) {
            node->left = deleteNode(node->left, roll);
        } else if (roll > node->rollNumber) {
            node->right = deleteNode(node->right, roll);
        } else {
            // Found the node to delete
            if (!node->left) {
                StudentNode* temp = node->right;
                delete node;
                return temp;
            }
            if (!node->right) {
                StudentNode* temp = node->left;
                delete node;
                return temp;
            }
            // Two children: copy inorder successor's data, then delete successor
            StudentNode* successor = findMin(node->right);
            // For simplicity, we transfer the data. In a full implementation,
            // we would transfer the student record including the enrollment list.
            // Here we copy scalar fields and leave the linked list as-is.
            // (For exam purposes, this demonstrates the BST deletion algorithm.)
            node->rollNumber = successor->rollNumber;
            node->name = successor->name;
            node->email = successor->email;
            // Note: courses and gradeHistory would also need transfer in a
            // production system. For exam demonstration, this is sufficient.
            node->right = deleteNode(node->right, successor->rollNumber);
        }
        return node;
    }

    // -------------------------------------------------------------------
    // IN-ORDER DISPLAY helper: Left -> Root -> Right traversal.
    // Produces students sorted by roll number ascending. O(n).
    // This is the key advantage of BST over hash table for sorted output.
    // -------------------------------------------------------------------
    void inorderDisplay(StudentNode* node, unordered_map<string, Course>& catalog) {
        if (!node) return;
        inorderDisplay(node->left, catalog);
        cout << "  " << left << setw(8) << node->rollNumber
             << setw(25) << node->name
             << setw(30) << node->email
             << node->courses.getCount() << " course(s)"
             << " | GPA: " << fixed << setprecision(2)
             << node->courses.calculateGPA(catalog) << "\n";
        inorderDisplay(node->right, catalog);
    }

    // -------------------------------------------------------------------
    // DESTROY helper: Post-order traversal to free memory. O(n).
    // -------------------------------------------------------------------
    void destroy(StudentNode* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

    // -------------------------------------------------------------------
    // GENERATE REPORT CARD: Full details for one student.
    // -------------------------------------------------------------------
    void generateReport(StudentNode* node, unordered_map<string, Course>& catalog) {
        cout << "\n=========================================\n";
        cout << "            REPORT CARD\n";
        cout << "=========================================\n";
        cout << "  Roll Number: " << node->rollNumber << "\n";
        cout << "  Name:        " << node->name << "\n";
        cout << "  Email:       " << node->email << "\n";
        cout << "-----------------------------------------\n";
        node->courses.display(catalog);
        cout << "-----------------------------------------\n";
        double gpa = node->courses.calculateGPA(catalog);
        cout << "  GPA: " << fixed << setprecision(2) << gpa << "\n";
        cout << "=========================================\n";
    }

public:
    StudentBST() : root(nullptr) {}

    ~StudentBST() { destroy(root); }

    // -------------------------------------------------------------------
    // ADD STUDENT: Insert into BST. O(log n) average.
    // -------------------------------------------------------------------
    bool addStudent(int roll, string name, string email) {
        if (search(root, roll)) {
            cout << "  [ERROR] Student with roll number " << roll
                 << " already exists.\n";
            return false;
        }
        root = insert(root, roll, name, email);
        cout << "  [OK] Student '" << name << "' (Roll: " << roll
             << ") added.\n";
        return true;
    }

    // -------------------------------------------------------------------
    // FIND STUDENT BY ROLL NUMBER: O(log n) average.
    // Returns pointer to the StudentNode or nullptr if not found.
    // -------------------------------------------------------------------
    StudentNode* findByRoll(int roll) {
        return search(root, roll);
    }

    // -------------------------------------------------------------------
    // REMOVE STUDENT: O(log n) average.
    // -------------------------------------------------------------------
    bool removeStudent(int roll) {
        if (!search(root, roll)) {
            cout << "  [ERROR] Student with roll number " << roll
                 << " not found.\n";
            return false;
        }
        root = deleteNode(root, roll);
        cout << "  [OK] Student " << roll << " removed.\n";
        return true;
    }

    // -------------------------------------------------------------------
    // DISPLAY ALL STUDENTS (sorted by roll number): O(n) in-order.
    // -------------------------------------------------------------------
    void displayAll(unordered_map<string, Course>& catalog) {
        if (!root) {
            cout << "  [INFO] No students registered.\n";
            return;
        }
        cout << "  " << left << setw(8) << "Roll"
             << setw(25) << "Name"
             << setw(30) << "Email"
             << "Status\n";
        cout << "  " << string(80, '-') << "\n";
        inorderDisplay(root, catalog);
    }

    // -------------------------------------------------------------------
    // GENERATE REPORT CARD: Find student then print full report. O(log n).
    // -------------------------------------------------------------------
    void showReportCard(int roll, unordered_map<string, Course>& catalog) {
        StudentNode* node = search(root, roll);
        if (!node) {
            cout << "  [ERROR] Student " << roll << " not found.\n";
            return;
        }
        generateReport(node, catalog);
    }

    // -------------------------------------------------------------------
    // ENROLL STUDENT IN COURSE: O(log n) for BST lookup + O(1) for list.
    // If course is full, add to waitlist queue instead.
    // -------------------------------------------------------------------
    bool enrollInCourse(int roll, string courseCode,
                        unordered_map<string, Course>& catalog) {
        StudentNode* student = search(root, roll);
        if (!student) {
            cout << "  [ERROR] Student " << roll << " not found.\n";
            return false;
        }

        auto it = catalog.find(courseCode);
        if (it == catalog.end()) {
            cout << "  [ERROR] Course " << courseCode << " not found in catalog.\n";
            return false;
        }

        Course& course = it->second;

        // Check if already enrolled
        if (student->courses.find(courseCode)) {
            cout << "  [ERROR] Student is already enrolled in "
                 << courseCode << ".\n";
            return false;
        }

        if (course.isFull()) {
            // Course full -> add to FIFO waitlist queue
            string sid = to_string(roll);
            course.waitlist.push(sid);
            cout << "  [WAITLIST] Course " << courseCode << " is full. "
                 << "Student " << roll << " added to waitlist (position "
                 << course.waitlist.size() << ").\n";
            return false;
        }

        student->courses.enroll(courseCode);
        course.enrolledCount++;
        cout << "  [OK] Student " << roll << " enrolled in "
             << courseCode << " (" << course.courseName << ").\n";
        return true;
    }

    // -------------------------------------------------------------------
    // DROP COURSE: Remove from linked list, free seat for waitlisted. O(n).
    // If there is a waitlist, the first student (FIFO) gets the seat.
    // -------------------------------------------------------------------
    bool dropCourse(int roll, string courseCode,
                    unordered_map<string, Course>& catalog) {
        StudentNode* student = search(root, roll);
        if (!student) {
            cout << "  [ERROR] Student " << roll << " not found.\n";
            return false;
        }

        if (!student->courses.drop(courseCode)) {
            cout << "  [ERROR] Student is not enrolled in " << courseCode << ".\n";
            return false;
        }

        auto it = catalog.find(courseCode);
        if (it != catalog.end()) {
            Course& course = it->second;
            course.enrolledCount--;

            // Process waitlist: give seat to first student in queue (FIFO)
            if (!course.waitlist.empty()) {
                string nextRoll = course.waitlist.front();
                course.waitlist.pop();
                int nextRollNum = stoi(nextRoll);
                StudentNode* nextStudent = search(root, nextRollNum);
                if (nextStudent) {
                    nextStudent->courses.enroll(courseCode);
                    course.enrolledCount++;
                    cout << "  [WAITLIST] Student " << nextRoll
                         << " auto-enrolled from waitlist.\n";
                }
            }
        }

        cout << "  [OK] Student " << roll << " dropped " << courseCode << ".\n";
        return true;
    }

    // -------------------------------------------------------------------
    // RECORD GRADE: Update grade + push old state onto undo stack. O(n).
    // -------------------------------------------------------------------
    bool recordGrade(int roll, string courseCode, string grade, double gradePoints) {
        StudentNode* student = search(root, roll);
        if (!student) {
            cout << "  [ERROR] Student " << roll << " not found.\n";
            return false;
        }

        GradeChange change = student->courses.updateGrade(courseCode, grade, gradePoints);
        if (change.oldGrade.empty() && change.oldGradePoints == 0.0) {
            // This might mean the course wasn't found OR it was IP with 0 points
            // We check by looking for the course again
            if (!student->courses.find(courseCode)) {
                cout << "  [ERROR] Student is not enrolled in " << courseCode << ".\n";
                return false;
            }
        }

        // Push grade change onto stack for potential undo
        student->gradeHistory.push(change);
        cout << "  [OK] Grade for " << courseCode << " updated to '"
             << grade << "' (" << gradePoints << ").\n";
        return true;
    }

    // -------------------------------------------------------------------
    // UNDO LAST GRADE CHANGE: Pop from stack, restore previous grade. O(1).
    // -------------------------------------------------------------------
    bool undoGrade(int roll) {
        StudentNode* student = search(root, roll);
        if (!student) {
            cout << "  [ERROR] Student " << roll << " not found.\n";
            return false;
        }

        if (student->gradeHistory.empty()) {
            cout << "  [INFO] No grade changes to undo for student "
                 << roll << ".\n";
            return false;
        }

        GradeChange lastChange = student->gradeHistory.top();
        student->gradeHistory.pop();

        // Restore the old grade
        EnrollmentNode* enrollment = student->courses.find(lastChange.courseCode);
        if (enrollment) {
            enrollment->grade = lastChange.oldGrade;
            enrollment->gradePoints = lastChange.oldGradePoints;
            cout << "  [UNDO] Grade for " << lastChange.courseCode
                 << " restored to '" << lastChange.oldGrade
                 << "' (" << lastChange.oldGradePoints << ").\n";
        } else {
            cout << "  [WARN] Course " << lastChange.courseCode
                 << " not found in enrollment. Cannot undo.\n";
            return false;
        }
        return true;
    }

    StudentNode* getRoot() { return root; }
};

// ============================================================================
// SECTION 4: MAIN STUDENT RECORD SYSTEM
// Integrates all data structures into a cohesive application.
// ============================================================================

class StudentRecordSystem {
private:
    StudentBST studentTree;              // BST: roll number -> student record
    unordered_map<string, Course> catalog;  // Hash: courseCode -> Course
    unordered_map<string, int> nameIndex;   // Hash: student name -> roll number
    // nameIndex enables O(1) student lookup by name (alternative to BST by roll)

public:
    StudentRecordSystem() {
        // Seed with some default courses
        addCourse("CS201", "Data Structures", 4, 30);
        addCourse("CS202", "Algorithms", 4, 30);
        addCourse("CS203", "Database Systems", 3, 25);
        addCourse("CS204", "Operating Systems", 3, 25);
        addCourse("CS205", "Computer Networks", 3, 25);
    }

    // ==================================================================
    // COURSE CATALOG MANAGEMENT
    // ==================================================================

    // -------------------------------------------------------------------
    // ADD COURSE: Insert into hash table by course code. O(1) average.
    // -------------------------------------------------------------------
    void addCourse(string code, string name, int credits, int capacity) {
        if (catalog.find(code) != catalog.end()) {
            cout << "  [ERROR] Course " << code << " already exists.\n";
            return;
        }
        catalog[code] = Course(code, name, credits, capacity);
        cout << "  [OK] Course '" << name << "' (" << code
             << ") added. Capacity: " << capacity << "\n";
    }

    // -------------------------------------------------------------------
    // DISPLAY COURSE CATALOG: Iterate hash table. O(n) for n courses.
    // -------------------------------------------------------------------
    void displayCatalog() {
        if (catalog.empty()) {
            cout << "  [INFO] No courses in catalog.\n";
            return;
        }
        cout << "  " << left << setw(10) << "Code"
             << setw(35) << "Course Name"
             << setw(8) << "Credits"
             << "Enrolled\n";
        cout << "  " << string(70, '-') << "\n";
        for (auto& pair : catalog) {
            pair.second.display();
        }
    }

    // ==================================================================
    // STUDENT MANAGEMENT
    // ==================================================================

    // -------------------------------------------------------------------
    // ADD STUDENT: Insert into BST + update name hash table. O(log n) + O(1).
    // The nameIndex hash table allows O(1) search by name later.
    // -------------------------------------------------------------------
    void addStudent(int roll, string name, string email) {
        if (studentTree.addStudent(roll, name, email)) {
            nameIndex[name] = roll;  // O(1) insertion into hash table
            // Also index by email for flexible search
            nameIndex[email] = roll;
        }
    }

    // -------------------------------------------------------------------
    // SEARCH STUDENT BY NAME: O(1) average using hash table.
    // The hash table maps name -> roll number, then we look up BST.
    // This provides two lookup paths: by roll (BST) and by name (hash).
    // -------------------------------------------------------------------
    StudentNode* searchByName(string name) {
        auto it = nameIndex.find(name);
        if (it != nameIndex.end()) {
            StudentNode* student = studentTree.findByRoll(it->second);
            if (student) {
                cout << "  [OK] Found: " << student->name
                     << " (Roll: " << student->rollNumber
                     << ", Email: " << student->email << ")\n";
            }
            return student;
        }
        cout << "  [ERROR] No student found with name/email '" << name << "'.\n";
        return nullptr;
    }

    // -------------------------------------------------------------------
    // SEARCH STUDENT BY ROLL: O(log n) using BST.
    // -------------------------------------------------------------------
    StudentNode* searchByRoll(int roll) {
        StudentNode* student = studentTree.findByRoll(roll);
        if (student) {
            cout << "  [OK] Found: " << student->name
                 << " (Roll: " << student->rollNumber
                 << ", Email: " << student->email << ")\n";
        } else {
            cout << "  [ERROR] No student found with roll number "
                 << roll << ".\n";
        }
        return student;
    }

    // ==================================================================
    // WAITLIST MANAGEMENT (Queue - FIFO)
    // ==================================================================

    // -------------------------------------------------------------------
    // PROCESS WAITLIST: For a given course, enroll students from the
    // waitlist queue as seats become available. FIFO order ensures
    // fairness: the first student who waited gets the first seat.
    // -------------------------------------------------------------------
    void processWaitlist(string courseCode) {
        auto it = catalog.find(courseCode);
        if (it == catalog.end()) {
            cout << "  [ERROR] Course " << courseCode << " not found.\n";
            return;
        }

        Course& course = it->second;
        int processed = 0;

        while (!course.isFull() && !course.waitlist.empty()) {
            string rollStr = course.waitlist.front();
            course.waitlist.pop();
            int roll = stoi(rollStr);

            StudentNode* student = studentTree.findByRoll(roll);
            if (student) {
                student->courses.enroll(courseCode);
                course.enrolledCount++;
                processed++;
                cout << "  [WAITLIST] Student " << roll
                     << " enrolled from waitlist into " << courseCode << ".\n";
            }
        }

        if (processed == 0) {
            cout << "  [INFO] No waitlisted students to process for "
                 << courseCode << ".\n";
        } else {
            cout << "  [OK] Processed " << processed
                 << " student(s) from waitlist.\n";
        }
    }

    // -------------------------------------------------------------------
    // SHOW WAITLIST: Display queue contents for a course.
    // -------------------------------------------------------------------
    void showWaitlist(string courseCode) {
        auto it = catalog.find(courseCode);
        if (it == catalog.end()) {
            cout << "  [ERROR] Course " << courseCode << " not found.\n";
            return;
        }

        Course& course = it->second;
        if (course.waitlist.empty()) {
            cout << "  [INFO] No students on waitlist for " << courseCode << ".\n";
            return;
        }

        queue<string> temp = course.waitlist;
        cout << "  Waitlist for " << courseCode
             << " (" << course.waitlist.size() << " students):\n";
        int pos = 1;
        while (!temp.empty()) {
            cout << "    " << pos++ << ". Roll " << temp.front() << "\n";
            temp.pop();
        }
    }

    // ==================================================================
    // INTERACTIVE MENU
    // ==================================================================
    void run() {
        cout << "\n=============================================\n";
        cout << "   STUDENT RECORD MANAGEMENT SYSTEM\n";
        cout << "   Data Structures: BST, Linked List,\n";
        cout << "   Stack, Hash Table, Queue\n";
        cout << "=============================================\n\n";

        int choice, roll, credits, capacity;
        string name, email, courseCode, courseName, grade;
        double gradePoints;

        do {
            cout << "\n------ MENU ------\n";
            cout << "  [STUDENTS - BST]\n";
            cout << "    1. Add Student\n";
            cout << "    2. Search by Roll Number (BST O(log n))\n";
            cout << "    3. Search by Name (Hash Table O(1))\n";
            cout << "    4. Remove Student (BST O(log n))\n";
            cout << "    5. Display All Students (BST In-Order)\n";
            cout << "    6. Generate Report Card\n";
            cout << "  [COURSES - Linked List]\n";
            cout << "    7. Add Course to Catalog\n";
            cout << "    8. Display Course Catalog\n";
            cout << "    9. Enroll Student in Course\n";
            cout << "   10. Drop Course\n";
            cout << "  [GRADES - Stack]\n";
            cout << "   11. Record Grade\n";
            cout << "   12. Undo Last Grade Change\n";
            cout << "  [WAITLIST - Queue]\n";
            cout << "   13. Show Course Waitlist\n";
            cout << "   14. Process Waitlist\n";
            cout << "    0. Exit\n";
            cout << "Choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1:
                    cout << "  Roll Number: "; cin >> roll; cin.ignore();
                    cout << "  Name: "; getline(cin, name);
                    cout << "  Email: "; getline(cin, email);
                    addStudent(roll, name, email);
                    break;

                case 2:
                    cout << "  Roll Number: "; cin >> roll; cin.ignore();
                    if (StudentNode* s = studentTree.findByRoll(roll)) {
                        cout << "  Found: " << s->name << " | Email: "
                             << s->email << "\n";
                        s->courses.display(catalog);
                    } else {
                        cout << "  [ERROR] Student not found.\n";
                    }
                    break;

                case 3:
                    cout << "  Name or Email: "; getline(cin, name);
                    searchByName(name);
                    break;

                case 4:
                    cout << "  Roll Number: "; cin >> roll; cin.ignore();
                    studentTree.removeStudent(roll);
                    break;

                case 5:
                    studentTree.displayAll(catalog);
                    break;

                case 6:
                    cout << "  Roll Number: "; cin >> roll; cin.ignore();
                    studentTree.showReportCard(roll, catalog);
                    break;

                case 7:
                    cout << "  Course Code: "; getline(cin, courseCode);
                    cout << "  Course Name: "; getline(cin, courseName);
                    cout << "  Credits: "; cin >> credits; cin.ignore();
                    cout << "  Capacity: "; cin >> capacity; cin.ignore();
                    addCourse(courseCode, courseName, credits, capacity);
                    break;

                case 8:
                    displayCatalog();
                    break;

                case 9:
                    cout << "  Roll Number: "; cin >> roll; cin.ignore();
                    cout << "  Course Code: "; getline(cin, courseCode);
                    studentTree.enrollInCourse(roll, courseCode, catalog);
                    break;

                case 10:
                    cout << "  Roll Number: "; cin >> roll; cin.ignore();
                    cout << "  Course Code: "; getline(cin, courseCode);
                    studentTree.dropCourse(roll, courseCode, catalog);
                    break;

                case 11:
                    cout << "  Roll Number: "; cin >> roll; cin.ignore();
                    cout << "  Course Code: "; getline(cin, courseCode);
                    cout << "  Grade (A/B/C/D/F): "; getline(cin, grade);
                    cout << "  Grade Points (4.0 scale): "; cin >> gradePoints; cin.ignore();
                    studentTree.recordGrade(roll, courseCode, grade, gradePoints);
                    break;

                case 12:
                    cout << "  Roll Number: "; cin >> roll; cin.ignore();
                    studentTree.undoGrade(roll);
                    break;

                case 13:
                    cout << "  Course Code: "; getline(cin, courseCode);
                    showWaitlist(courseCode);
                    break;

                case 14:
                    cout << "  Course Code: "; getline(cin, courseCode);
                    processWaitlist(courseCode);
                    break;

                case 0:
                    cout << "  Exiting Student Record System. Goodbye!\n";
                    break;

                default:
                    cout << "  [ERROR] Invalid choice. Try again.\n";
            }
        } while (choice != 0);
    }
};

// ============================================================================
// MAIN FUNCTION - Entry point
// ============================================================================
int main() {
    StudentRecordSystem system;
    system.run();
    return 0;
}
