// Exercise 2: Pointer and array traversal
#include <iostream>
using namespace std;

// Function to print array using pointer arithmetic
void printArray(int* arr, int size) {
    for (int i = 0; i < size; i++) {
        // Equivalent to arr[i]
        cout << *(arr + i) << " ";
    }
    cout << endl;
}

int main() {
    int arr[] = {10, 20, 30, 40, 50};
    int size = sizeof(arr) / sizeof(arr[0]);
    
    cout << "Array elements: ";
    printArray(arr, size);
    
    return 0;
}
