// Exercise 5: Array of structures
#include <iostream>
#include <string>
using namespace std;

// 1. Define Student struct
struct Student {
    string name;
    int grades[5];
};

// 4. Function to print names of students with average grade > 90
void printTopStudents(Student students[], int size) {
    cout << "Students with average > 90:" << endl;
    for (int i = 0; i < size; i++) {
        int sum = 0;
        for (int j = 0; j < 5; j++) {
            sum += students[i].grades[j];
        }
        double avg = (double)sum / 5;
        if (avg > 90.0) {
            cout << students[i].name << " (Average: " << avg << ")" << endl;
        }
    }
}

int main() {
    // 2 & 3. Create array and populate data
    Student students[3] = {
        {"Alice", {95, 92, 88, 97, 90}},
        {"Bob", {70, 80, 85, 90, 75}},
        {"Charlie", {99, 98, 95, 92, 100}}
    };

    printTopStudents(students, 3);

    return 0;
}
