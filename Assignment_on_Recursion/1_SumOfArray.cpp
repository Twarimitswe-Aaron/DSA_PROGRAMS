// 1. Sum of elements in an array
#include <iostream>
using namespace std;

// Recursive function to find the sum of elements in an array
// Base case: if n <= 0, sum is 0
// Recursive step: return the last element + sum of the rest of the array
int sumOfArray(int arr[], int n) {
    if (n <= 0) {
        return 0; // Base case
    }
    return arr[n - 1] + sumOfArray(arr, n - 1); // Recursive step
}

int main() {
    int arr[] = {1, 2, 3, 4, 5};
    int n = sizeof(arr) / sizeof(arr[0]);
    cout << "Sum of array elements is: " << sumOfArray(arr, n) << endl;
    return 0;
}
