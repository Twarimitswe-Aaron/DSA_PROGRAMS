// Group 9: Secure Data Management - Student Record System
#include <iostream>
#include <string>
using namespace std;

struct Student {
    int regNumber;
    string name;
    string course;
    bool isActive;
    
    Student() : regNumber(-1), isActive(false) {}
    Student(int reg, string n, string c) : regNumber(reg), name(n), course(c), isActive(true) {}
};

const int TABLE_SIZE = 101;

class StudentDatabase {
private:
    Student table[TABLE_SIZE];

    int hashFunction(int regNumber) {
        return regNumber % TABLE_SIZE;
    }

public:
    void addStudent(int reg, string name, string course) {
        int index = hashFunction(reg);
        int start = index;
        while (table[index].isActive) {
            if (table[index].regNumber == reg) {
                cout << "Student with Reg Number " << reg << " already exists!\n";
                return;
            }
            index = (index + 1) % TABLE_SIZE;
            if (index == start) {
                cout << "Database Full!\n";
                return;
            }
        }
        table[index] = Student(reg, name, course);
        cout << "Added Student: " << name << "\n";
    }

    void modifyStudent(int reg, string newCourse) {
        int index = hashFunction(reg);
        int start = index;
        while (table[index].regNumber != -1) {
            if (table[index].regNumber == reg && table[index].isActive) {
                table[index].course = newCourse;
                cout << "Modified Reg " << reg << " to Course: " << newCourse << "\n";
                return;
            }
            index = (index + 1) % TABLE_SIZE;
            if (index == start) break;
        }
        cout << "Student not found.\n";
    }

    void deleteStudent(int reg) {
        int index = hashFunction(reg);
        int start = index;
        while (table[index].regNumber != -1) {
            if (table[index].regNumber == reg && table[index].isActive) {
                table[index].isActive = false; // Lazy deletion
                cout << "Deleted Student Reg: " << reg << "\n";
                return;
            }
            index = (index + 1) % TABLE_SIZE;
            if (index == start) break;
        }
        cout << "Student not found.\n";
    }

    void getStudent(int reg) {
        int index = hashFunction(reg);
        int start = index;
        while (table[index].regNumber != -1) {
            if (table[index].regNumber == reg && table[index].isActive) {
                cout << "Found -> Reg: " << table[index].regNumber 
                     << " | Name: " << table[index].name 
                     << " | Course: " << table[index].course << "\n";
                return;
            }
            index = (index + 1) % TABLE_SIZE;
            if (index == start) break;
        }
        cout << "Student not found.\n";
    }
};

int main() {
    StudentDatabase db;
    db.addStudent(1001, "Alice", "Computer Science");
    db.addStudent(1002, "Bob", "Information Technology");
    
    db.getStudent(1001);
    db.modifyStudent(1001, "Software Engineering");
    db.getStudent(1001);
    
    db.deleteStudent(1002);
    db.getStudent(1002); // Should not find
    
    return 0;
}
