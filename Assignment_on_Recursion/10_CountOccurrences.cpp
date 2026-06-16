// 10. Count occurrences of a given element in an array
#include <iostream>
using namespace std;

// Recursive function to count occurrences
// Base case: if n is 0, return 0
// Recursive step: check the last element, add 1 if it matches, then check the rest
int countOccurrences(int arr[], int n, int target) {
    if (n == 0) {
        return 0; // Base case
    }
    int count = (arr[n - 1] == target) ? 1 : 0;
    return count + countOccurrences(arr, n - 1, target); // Recursive step
}

int main() {
    int arr[] = {1, 2, 3, 2, 4, 2, 5};
    int n = sizeof(arr) / sizeof(arr[0]);
    int target = 2;
    cout << "The element " << target << " occurs " << countOccurrences(arr, n, target) << " times." << endl;
    return 0;
}
