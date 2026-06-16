// 14. Check if an array is sorted recursively
#include <iostream>
using namespace std;

// Recursive function to check if array is sorted in ascending order
// Base case: if array has 0 or 1 element, it is sorted
// Recursive step: check if last two elements are sorted, then check the rest
bool isSorted(int arr[], int n) {
    if (n <= 1) {
        return true; // Base case
    }
    if (arr[n - 1] < arr[n - 2]) {
        return false; // Found an inversion
    }
    return isSorted(arr, n - 1); // Recursive step
}

int main() {
    int arr1[] = {1, 2, 3, 4, 5};
    int arr2[] = {1, 3, 2, 4, 5};
    
    cout << "Array 1 is sorted: " << (isSorted(arr1, 5) ? "Yes" : "No") << endl;
    cout << "Array 2 is sorted: " << (isSorted(arr2, 5) ? "Yes" : "No") << endl;
    
    return 0;
}
