/*
 * Topic: Recursion - Find Last Index
 * Description: Recursively finds the last occurrence of an element in an array.
 */

#include <iostream>
using namespace std;

// Helper function to pass the current index (size - 1)
int lastIndexHelper(int arr[], int size, int element, int currentIndex) {
    // Base case: if we have checked all elements down to 0 and haven't found it
    if (currentIndex < 0) {
        return -1;
    }
    
    // If we find the element, return the current index immediately since we are searching backwards
    if (arr[currentIndex] == element) {
        return currentIndex;
    }
    
    // Recursive step: look at the previous element
    return lastIndexHelper(arr, size, element, currentIndex - 1);
}

int lastIndex(int arr[], int size, int element) {
    // Start searching from the very last element (size - 1)
    return lastIndexHelper(arr, size, element, size - 1);
}

int main() {
    int arr[] = {5, 5, 6, 20, 5, 6};
    int size = sizeof(arr) / sizeof(arr[0]);
    int x = 5;
    
    cout << "Array: ";
    for(int i=0; i<size; i++) cout << arr[i] << " ";
    cout << "\n";
    
    cout << "Last index of " << x << " is: " << lastIndex(arr, size, x) << "\n";
    
    return 0;
}
