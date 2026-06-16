// Exercise 6: Dynamic array with pointers
#include <iostream>
using namespace std;

// 1. Function to create dynamic array
int* createDynamicArray(int size) {
    return new int[size];
}

int main() {
    int size;
    cout << "Enter the size of the array: ";
    cin >> size;

    // 2. Call function and fill array
    int* dynArr = createDynamicArray(size);

    cout << "Enter " << size << " integers:" << endl;
    for (int i = 0; i < size; i++) {
        cin >> *(dynArr + i);
    }

    cout << "Array elements: ";
    for (int i = 0; i < size; i++) {
        cout << *(dynArr + i) << " ";
    }
    cout << endl;

    // 3. Deallocate memory
    delete[] dynArr;
    cout << "Memory deallocated." << endl;

    return 0;
}
