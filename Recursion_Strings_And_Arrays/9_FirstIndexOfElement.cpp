/*
 * Topic: Recursion - Find First Index
 * Description: Recursively finds the first occurrence of an element in an array.
 */

#include <iostream>
using namespace std;

// Helper function to keep track of the current index
int firstIndexHelper(int arr[], int size, int element, int currentIndex) {
    // Base case: if we reach the end of the array and haven't found it
    if (currentIndex == size) {
        return -1;
    }
    
    // If we find the element, return the current index immediately
    if (arr[currentIndex] == element) {
        return currentIndex;
    }
    
    // Recursive step: look at the next element
    return firstIndexHelper(arr, size, element, currentIndex + 1);
}

int firstIndex(int arr[], int size, int element) {
    // Start searching from index 0
    return firstIndexHelper(arr, size, element, 0);
}

int main() {
    int arr[] = {5, 5, 6, 20, 5, 6};
    int size = sizeof(arr) / sizeof(arr[0]);
    int x = 5;
    
    cout << "Array: ";
    for(int i=0; i<size; i++) cout << arr[i] << " ";
    cout << "\n";
    
    cout << "First index of " << x << " is: " << firstIndex(arr, size, x) << "\n";
    
    return 0;
}
