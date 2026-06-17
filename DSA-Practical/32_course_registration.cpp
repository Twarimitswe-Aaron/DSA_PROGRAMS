/*
 * ============================================================================
 * FILE: 32_course_registration.cpp
 * ============================================================================
 * COURSE REGISTRATION SYSTEM - Real-world DSA Application
 *
 * EXAM FREQUENCY: High. Course registration is a commonly asked DSA practical
 * exam problem in university lab manuals. Tests queues (waitlist), BSTs
 * (course catalog by code), hash tables (student records), stacks
 * (registration history), and linked lists (enrolled courses per student).
 *
 * DATA STRUCTURES USED:
 *   1. Binary Search Tree - Course Catalog (sorted by code)
 *      - O(log n) course lookup + sorted in-order traversal
 *      - Displays courses in ascending code order
 *   2. Queue - Course Waitlist (FIFO)
 *      - Students join waitlist when course is full
 *      - Auto-enrolled when seat opens up
 *   3. Hash Table (unordered_map) - Student Records
 *      - O(1) student lookup by ID
 *      - Stores enrolled courses list
 *   4. Stack - Registration History / Undo (LIFO)
 *      - Most recent registration/deregistration first
 *      - Supports undo last action
 *   5. Linked List - Enrolled Courses per Student
 *      - Dynamic list of courses a student is registered in
 *      - O(1) add, O(n) drop
 *
 * FUNCTIONALITIES:
 *   - Add courses (BST by code)
 *   - Display courses sorted by code (BST in-order)
 *   - Register student in course (hash + linked list)
 *   - Drop course (linked list removal)
 *   - Waitlist when full (queue)
 *   - View registration history (stack)
 *   - Undo last registration (stack pop)
 *   - Search student by ID (hash O(1))
 * ============================================================================
 */
#include <iostream>
#include <string>
#include <unordered_map>
#include <queue>
#include <stack>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <vector>
using namespace std;

// ============================================================================
// SECTION 1: ENTITY STRUCTS
// ============================================================================
struct Course {
    string courseCode;     // e.g. "CS201"
    string title;
    string instructor;
    int credits;
    int capacity;
    int enrolled;

    Course() : credits(0), capacity(0), enrolled(0) {}
    Course(string code, string t, string i, int cr, int cap)
        : courseCode(code), title(t), instructor(i),
          credits(cr), capacity(cap), enrolled(0) {}

    bool isFull() { return enrolled >= capacity; }
    int available() { return capacity - enrolled; }
};

struct Student {
    string studentId;
    string name;
    string department;
    int semester;

    Student() : semester(0) {}
    Student(string id, string n, string dept, int sem)
        : studentId(id), name(n), department(dept), semester(sem) {}
};

// Enrolled course linked list node
struct EnrolledNode {
    string courseCode;
    string courseTitle;
    EnrolledNode* next;

    EnrolledNode(string code, string title)
        : courseCode(code), courseTitle(title), next(nullptr) {}
};

struct Registration {
    string regId;
    string studentId;
    string studentName;
    string courseCode;
    string courseTitle;
    string action;      // ENROLLED or DROPPED

    Registration() {}
    Registration(string rid, string sid, string sn, string cc, string ct, string a)
        : regId(rid), studentId(sid), studentName(sn),
          courseCode(cc), courseTitle(ct), action(a) {}
};

// Waitlist entry
struct WaitEntry {
    string studentId;
    string studentName;
    string courseCode;

    WaitEntry() {}
    WaitEntry(string sid, string sn, string cc)
        : studentId(sid), studentName(sn), courseCode(cc) {}
};

// ============================================================================
// SECTION 2: BST - COURSE CATALOG
//
// Why BST?
//   - Courses sorted by code for in-order display
//   - O(log n) search/insert for balanced tree
//   - Natural ordering for catalog listing
// ============================================================================
struct BSTNode {
    Course course;
    BSTNode* left;
    BSTNode* right;

    BSTNode(Course c) : course(c), left(nullptr), right(nullptr) {}
};

class CourseBST {
private:
    BSTNode* root;

    BSTNode* insert(BSTNode* node, Course c) {
        if (!node) return new BSTNode(c);
        if (c.courseCode < node->course.courseCode)
            node->left = insert(node->left, c);
        else
            node->right = insert(node->right, c);
        return node;
    }

    BSTNode* search(BSTNode* node, string code) {
        if (!node || node->course.courseCode == code) return node;
        if (code < node->course.courseCode)
            return search(node->left, code);
        return search(node->right, code);
    }

    void inorder(BSTNode* node) {
        if (!node) return;
        inorder(node->left);
        cout << "    " << left << setw(10) << node->course.courseCode
             << setw(35) << node->course.title
             << setw(15) << node->course.instructor
             << setw(4) << node->course.credits
             << node->course.enrolled << "/" << node->course.capacity << "\n";
        inorder(node->right);
    }

    void collect(BSTNode* node, vector<BSTNode*>& vec) {
        if (!node) return;
        collect(node->left, vec);
        vec.push_back(node);
        collect(node->right, vec);
    }

    void destroy(BSTNode* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

public:
    CourseBST() : root(nullptr) {}

    ~CourseBST() { destroy(root); }

    void insert(Course c) {
        root = insert(root, c);
    }

    Course* search(string code) {
        BSTNode* node = search(root, code);
        return node ? &node->course : nullptr;
    }

    void display() {
        if (!root) {
            cout << "    [No courses available]\n";
            return;
        }
        cout << "    Course Catalog (BST - Sorted by Code):\n";
        cout << "    " << left << setw(10) << "Code"
             << setw(35) << "Title"
             << setw(15) << "Instructor"
             << setw(4) << "Cred"
             << "Enrolled\n";
        cout << "    " << string(72, '-') << "\n";
        inorder(root);
    }

    vector<BSTNode*> getAllNodes() {
        vector<BSTNode*> vec;
        collect(root, vec);
        return vec;
    }
};

// ============================================================================
// SECTION 3: LINKED LIST - ENROLLED COURSES PER STUDENT
// ============================================================================
class EnrolledList {
private:
    EnrolledNode* head;
    int count;

public:
    EnrolledList() : head(nullptr), count(0) {}

    ~EnrolledList() {
        EnrolledNode* curr = head;
        while (curr) {
            EnrolledNode* temp = curr;
            curr = curr->next;
            delete temp;
        }
    }

    void add(string code, string title) {
        EnrolledNode* node = new EnrolledNode(code, title);
        node->next = head;
        head = node;
        count++;
    }

    bool remove(string code) {
        EnrolledNode* curr = head;
        EnrolledNode* prev = nullptr;
        while (curr) {
            if (curr->courseCode == code) {
                if (prev) prev->next = curr->next;
                else head = curr->next;
                delete curr;
                count--;
                return true;
            }
            prev = curr;
            curr = curr->next;
        }
        return false;
    }

    bool hasCourse(string code) {
        EnrolledNode* curr = head;
        while (curr) {
            if (curr->courseCode == code) return true;
            curr = curr->next;
        }
        return false;
    }

    void display() {
        if (!head) {
            cout << "      [No courses enrolled]\n";
            return;
        }
        EnrolledNode* curr = head;
        while (curr) {
            cout << "      - " << curr->courseCode << ": " << curr->courseTitle << "\n";
            curr = curr->next;
        }
    }

    int getCount() { return count; }
};

// ============================================================================
// SECTION 4: MAIN COURSE REGISTRATION SYSTEM
// ============================================================================
class RegistrationSystem {
private:
    CourseBST courseCatalog;                          // BST of courses
    unordered_map<string, Student> students;          // Hash: studentId -> Student
    unordered_map<string, EnrolledList> enrollments;  // Hash: studentId -> LL of courses
    stack<Registration> regHistory;                   // Stack: recent registrations
    queue<WaitEntry> waitlist;                        // Queue: waitlist FIFO
    int nextRegNum;

public:
    RegistrationSystem() : nextRegNum(5001) {
        seedData();
    }

    void seedData() {
        courseCatalog.insert(Course("CS201", "Data Structures", "Dr. Sharma", 4, 3));
        courseCatalog.insert(Course("CS301", "Algorithms", "Dr. Gupta", 4, 3));
        courseCatalog.insert(Course("CS401", "Operating Systems", "Dr. Patel", 4, 2));
        courseCatalog.insert(Course("MA201", "Linear Algebra", "Dr. Rao", 3, 3));
        courseCatalog.insert(Course("MA301", "Probability & Stats", "Dr. Singh", 3, 3));
        courseCatalog.insert(Course("EE201", "Digital Logic", "Dr. Verma", 3, 2));
        courseCatalog.insert(Course("PH201", "Physics I", "Dr. Joshi", 3, 3));
        courseCatalog.insert(Course("EN101", "Technical Writing", "Dr. Bose", 2, 3));
        courseCatalog.insert(Course("CS101", "Programming Basics", "Dr. Kumar", 4, 3));
        courseCatalog.insert(Course("CS501", "Computer Networks", "Dr. Das", 4, 2));

        students["S001"] = Student("S001", "Arun Kumar", "CSE", 3);
        students["S002"] = Student("S002", "Bhavna Reddy", "CSE", 3);
        students["S003"] = Student("S003", "Chirag Shah", "ECE", 5);
        students["S004"] = Student("S004", "Deepika Singh", "CSE", 3);
        students["S005"] = Student("S005", "Esha Patel", "EE", 5);
    }

    // -------------------------------------------------------------------
    // REGISTER: BST lookup O(log n), hash O(1), linked list O(1)
    // -------------------------------------------------------------------
    void registerCourse(string studentId, string courseCode) {
        auto sit = students.find(studentId);
        if (sit == students.end()) {
            cout << "  [ERROR] Student not found.\n";
            return;
        }

        Course* c = courseCatalog.search(courseCode);
        if (!c) {
            cout << "  [ERROR] Course not found.\n";
            return;
        }

        // Check already enrolled
        if (enrollments[studentId].hasCourse(courseCode)) {
            cout << "  [INFO] Already enrolled in " << courseCode << ".\n";
            return;
        }

        if (c->isFull()) {
            // Add to waitlist instead
            waitlist.push(WaitEntry(studentId, sit->second.name, courseCode));
            cout << "  [WAITLIST] " << c->courseCode << " is full. "
                 << sit->second.name << " added to waitlist.\n";
            return;
        }

        // Enroll
        c->enrolled++;
        enrollments[studentId].add(courseCode, c->title);

        string regId = "REG" + to_string(nextRegNum++);
        regHistory.push(Registration(regId, studentId, sit->second.name,
                                      courseCode, c->title, "ENROLLED"));

        cout << "  [ENROLLED] " << sit->second.name
             << " -> " << courseCode << ": " << c->title << "\n";
    }

    // -------------------------------------------------------------------
    // DROP: Linked list removal, BST update, history push
    // -------------------------------------------------------------------
    void dropCourse(string studentId, string courseCode) {
        auto sit = students.find(studentId);
        if (sit == students.end()) {
            cout << "  [ERROR] Student not found.\n";
            return;
        }

        if (!enrollments[studentId].hasCourse(courseCode)) {
            cout << "  [ERROR] Not enrolled in " << courseCode << ".\n";
            return;
        }

        Course* c = courseCatalog.search(courseCode);
        if (c) c->enrolled--;

        enrollments[studentId].remove(courseCode);

        string regId = "REG" + to_string(nextRegNum++);
        regHistory.push(Registration(regId, studentId, sit->second.name,
                                      courseCode, c ? c->title : "", "DROPPED"));

        cout << "  [DROPPED] " << sit->second.name << " dropped " << courseCode << "\n";

        // Process waitlist (FIFO)
        processWaitlist(courseCode);
    }

    // -------------------------------------------------------------------
    // PROCESS WAITLIST: Queue - FIFO when seat opens
    // -------------------------------------------------------------------
    void processWaitlist(string courseCode) {
        if (waitlist.empty()) return;

        queue<WaitEntry> temp;
        bool processed = false;
        while (!waitlist.empty()) {
            WaitEntry w = waitlist.front();
            waitlist.pop();
            if (!processed && w.courseCode == courseCode) {
                Course* c = courseCatalog.search(courseCode);
                if (c && !c->isFull()) {
                    c->enrolled++;
                    enrollments[w.studentId].add(courseCode, c->title);
                    cout << "  [WAITLIST->ENROLLED] " << w.studentName
                         << " enrolled in " << courseCode << "\n";
                    processed = true;
                } else {
                    temp.push(w);
                }
            } else {
                temp.push(w);
            }
        }
        while (!temp.empty()) {
            waitlist.push(temp.front());
            temp.pop();
        }
    }

    // -------------------------------------------------------------------
    // UNDO LAST: Stack pop
    // -------------------------------------------------------------------
    void undoLast() {
        if (regHistory.empty()) {
            cout << "  [INFO] No actions to undo.\n";
            return;
        }

        Registration last = regHistory.top();
        regHistory.pop();

        if (last.action == "ENROLLED") {
            // Reverse: drop the course
            enrollments[last.studentId].remove(last.courseCode);
            Course* c = courseCatalog.search(last.courseCode);
            if (c) c->enrolled--;
            cout << "  [UNDO] Un-enrolled " << last.studentName
                 << " from " << last.courseCode << "\n";
        } else {
            // Reverse: re-enroll
            Course* c = courseCatalog.search(last.courseCode);
            if (c && !c->isFull()) {
                c->enrolled++;
                enrollments[last.studentId].add(last.courseCode, last.courseTitle);
                cout << "  [UNDO] Re-enrolled " << last.studentName
                     << " in " << last.courseCode << "\n";
            } else {
                cout << "  [UNDO FAILED] " << last.courseCode << " is full.\n";
            }
        }
    }

    // -------------------------------------------------------------------
    // DISPLAY METHODS
    // -------------------------------------------------------------------
    void showCourses() {
        courseCatalog.display();
    }

    void showStudentCourses(string studentId) {
        auto sit = students.find(studentId);
        if (sit == students.end()) {
            cout << "  [ERROR] Student not found.\n";
            return;
        }
        cout << "  Student: " << sit->second.name
             << " (" << sit->second.studentId << ")\n";
        cout << "  Enrolled Courses (Linked List):\n";
        enrollments[studentId].display();
    }

    void showHistory() {
        if (regHistory.empty()) {
            cout << "  [INFO] No registration history.\n";
            return;
        }
        stack<Registration> temp = regHistory;
        cout << "  Registration History (Most Recent - Stack LIFO):\n";
        while (!temp.empty()) {
            Registration r = temp.top(); temp.pop();
            cout << "  " << r.regId << " | " << r.studentName
                 << " | " << r.courseCode << " | " << r.action << "\n";
        }
    }

    void showWaitlist() {
        if (waitlist.empty()) {
            cout << "  [INFO] Waitlist is empty.\n";
            return;
        }
        queue<WaitEntry> temp = waitlist;
        cout << "  Waitlist (Queue FIFO):\n";
        int i = 1;
        while (!temp.empty()) {
            WaitEntry w = temp.front(); temp.pop();
            cout << "  " << i++ << ". " << w.studentName
                 << " (" << w.studentId << ") wants " << w.courseCode << "\n";
        }
    }

    void showStudents() {
        cout << "  Student Records (Hash Table):\n";
        cout << "  " << left << setw(8) << "ID"
             << setw(20) << "Name"
             << setw(12) << "Dept"
             << "Semester\n";
        cout << "  " << string(45, '-') << "\n";
        for (auto& pair : students) {
            Student& s = pair.second;
            cout << "  " << left << setw(8) << s.studentId
                 << setw(20) << s.name
                 << setw(12) << s.department
                 << s.semester << "\n";
        }
    }

    void run() {
        cout << "\n=============================================\n";
        cout << "   COURSE REGISTRATION SYSTEM\n";
        cout << "   Data Structures: BST (Catalog),\n";
        cout << "   Queue (Waitlist), Hash Table (Students),\n";
        cout << "   Stack (History/Undo), Linked List (Courses)\n";
        cout << "=============================================\n\n";

        int choice;
        string studentId, courseCode;

        do {
            cout << "\n------ MENU ------\n";
            cout << "  [BST - Course Catalog]\n";
            cout << "    1. Show All Courses (Sorted by Code)\n";
            cout << "  [HASH TABLE - Students]\n";
            cout << "    2. Show All Students\n";
            cout << "    3. Show Student's Enrolled Courses (LL)\n";
            cout << "  [REGISTRATION]\n";
            cout << "    4. Register for Course (BST + Hash + LL)\n";
            cout << "    5. Drop Course (LL Remove + BST Update)\n";
            cout << "  [STACK - History/Undo]\n";
            cout << "    6. Registration History (Most Recent)\n";
            cout << "    7. Undo Last Action\n";
            cout << "  [QUEUE - Waitlist]\n";
            cout << "    8. Show Waitlist\n";
            cout << "    0. Exit\n";
            cout << "Choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1: showCourses(); break;
                case 2: showStudents(); break;
                case 3:
                    cout << "  Student ID: "; getline(cin, studentId);
                    showStudentCourses(studentId);
                    break;
                case 4:
                    cout << "  Student ID: "; getline(cin, studentId);
                    cout << "  Course Code: "; getline(cin, courseCode);
                    registerCourse(studentId, courseCode);
                    break;
                case 5:
                    cout << "  Student ID: "; getline(cin, studentId);
                    cout << "  Course Code: "; getline(cin, courseCode);
                    dropCourse(studentId, courseCode);
                    break;
                case 6: showHistory(); break;
                case 7: undoLast(); break;
                case 8: showWaitlist(); break;
                case 0: cout << "  Goodbye! Happy studying.\n"; break;
                default: cout << "  [ERROR] Invalid choice.\n";
            }
        } while (choice != 0);
    }
};

int main() {
    RegistrationSystem system;
    system.run();
    return 0;
}
