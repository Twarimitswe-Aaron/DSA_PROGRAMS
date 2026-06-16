// 12. Recursive linear search in an array
#include <iostream>
using namespace std;

// Recursive function for linear search
// Base case 1: if n is 0, element is not found
// Base case 2: if last element matches, return its index
// Recursive step: search in the rest of the array
int linearSearch(int arr[], int n, int target) {
    if (n == 0) {
        return -1; // Base case 1
    }
    if (arr[n - 1] == target) {
        return n - 1; // Base case 2
    }
    return linearSearch(arr, n - 1, target); // Recursive step
}

int main() {
    int arr[] = {10, 20, 30, 40, 50};
    int n = sizeof(arr) / sizeof(arr[0]);
    int target = 30;
    
    int index = linearSearch(arr, n, target);
    if (index != -1) {
        cout << "Element " << target << " found at index " << index << endl;
    } else {
        cout << "Element " << target << " not found." << endl;
    }
    return 0;
}
