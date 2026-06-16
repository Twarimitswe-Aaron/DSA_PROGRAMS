// Exercise 10: Array of pointers to structures
#include <iostream>
#include <string>
using namespace std;

// 1. Define Employee struct
struct Employee {
    int id;
    string name;
    double salary;
};

// 5. Implement displayEmployees
void displayEmployees(Employee** employees, int size) {
    for (int i = 0; i < size; i++) {
        // Iterate and display using -> operator
        cout << "ID: " << employees[i]->id 
             << ", Name: " << employees[i]->name 
             << ", Salary: $" << employees[i]->salary << endl;
    }
}

int main() {
    // 2. Create array of 5 Employee structures
    Employee empArray[5] = {
        {1, "Alice", 50000},
        {2, "Bob", 60000},
        {3, "Charlie", 55000},
        {4, "Diana", 70000},
        {5, "Eve", 65000}
    };
    
    // 3. Create array of 5 pointers to Employee
    Employee* ptrArray[5];
    
    // 4. Initialize pointers
    for (int i = 0; i < 5; i++) {
        ptrArray[i] = &empArray[i];
    }
    
    // Call the function
    cout << "Employee Details:" << endl;
    displayEmployees(ptrArray, 5);
    
    return 0;
}
