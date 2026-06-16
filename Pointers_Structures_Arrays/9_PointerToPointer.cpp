// Exercise 9: Pointer to pointer to char
#include <iostream>
using namespace std;

int main() {
    // 1. Declare a char variable
    char myChar = 'A';
    
    // 2. Declare pointers
    char* ptr1;
    char** ptr2;
    
    // 3. Assign address of char
    ptr1 = &myChar;
    
    // 4. Assign address of pointer
    ptr2 = &ptr1;
    
    // 5. Print values
    cout << "Using the char variable itself: " << myChar << endl;
    cout << "Dereferencing the char pointer (*ptr1): " << *ptr1 << endl;
    cout << "Dereferencing the pointer to a pointer (**ptr2): " << **ptr2 << endl;
    
    return 0;
}
