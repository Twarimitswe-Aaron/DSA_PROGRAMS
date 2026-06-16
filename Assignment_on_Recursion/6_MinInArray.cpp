// 6. Find minimum element in an array
#include <iostream>
using namespace std;

// Recursive function to find the minimum element in an array
// Base case: if only one element is left, return it
// Recursive step: return the minimum of the last element and the min of the remaining elements
int minInArray(int arr[], int n) {
    if (n == 1) {
        return arr[0]; // Base case
    }
    return min(arr[n - 1], minInArray(arr, n - 1)); // Recursive step
}

int main() {
    int arr[] = {5, 2, 8, 1, 9};
    int n = sizeof(arr) / sizeof(arr[0]);
    cout << "Minimum element in the array is: " << minInArray(arr, n) << endl;
    return 0;
}
