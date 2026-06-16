// 5. Find maximum element in an array
#include <iostream>
using namespace std;

// Recursive function to find the maximum element in an array
// Base case: if only one element is left, return it
// Recursive step: return the maximum of the last element and the max of the remaining elements
int maxInArray(int arr[], int n) {
    if (n == 1) {
        return arr[0]; // Base case
    }
    return max(arr[n - 1], maxInArray(arr, n - 1)); // Recursive step
}

int main() {
    int arr[] = {1, 5, 3, 9, 2};
    int n = sizeof(arr) / sizeof(arr[0]);
    cout << "Maximum element in the array is: " << maxInArray(arr, n) << endl;
    return 0;
}
