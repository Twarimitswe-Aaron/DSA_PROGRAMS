// 13. Recursive binary search in a sorted array
#include <iostream>
using namespace std;

// Recursive function for binary search
// Base case 1: if left > right, element is not found
// Base case 2: if middle element matches, return middle index
// Recursive step: search in left or right half
int binarySearch(int arr[], int left, int right, int target) {
    if (left > right) {
        return -1; // Base case 1
    }
    
    int mid = left + (right - left) / 2;
    
    if (arr[mid] == target) {
        return mid; // Base case 2
    }
    
    if (arr[mid] > target) {
        // Search left half (Recursive step)
        return binarySearch(arr, left, mid - 1, target);
    } else {
        // Search right half (Recursive step)
        return binarySearch(arr, mid + 1, right, target);
    }
}

int main() {
    int arr[] = {2, 4, 6, 8, 10, 12, 14};
    int n = sizeof(arr) / sizeof(arr[0]);
    int target = 10;
    
    int index = binarySearch(arr, 0, n - 1, target);
    if (index != -1) {
        cout << "Element " << target << " found at index " << index << endl;
    } else {
        cout << "Element " << target << " not found." << endl;
    }
    return 0;
}
