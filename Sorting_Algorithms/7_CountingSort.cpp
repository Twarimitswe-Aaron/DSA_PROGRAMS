/*
 * Algorithm: Counting Sort
 * Description: An integer sorting algorithm that operates by counting the number of objects that possess distinct key values, and calculating positions.
 * 
 * Time Complexity:
 *  - Best Case: O(n + k) (where n is the number of elements, k is the range of input)
 *  - Average Case: O(n + k)
 *  - Worst Case: O(n + k)
 * 
 * Space (Memory) Complexity: O(n + k) (requires a counting array and an output array)
 */

#include <iostream>
#include <vector>
using namespace std;

void countingSort(int arr[], int n) {
    // Find the maximum element to know the range
    int max = arr[0];
    for (int i = 1; i < n; i++) {
        if (arr[i] > max)
            max = arr[i];
    }

    // Create a count array to store count of individual elements and initialize count array as 0
    vector<int> count(max + 1, 0);

    // Store count of each character
    for (int i = 0; i < n; i++) {
        count[arr[i]]++;
    }

    // Change count[i] so that count[i] now contains actual position of this character in output array
    for (int i = 1; i <= max; i++) {
        count[i] += count[i - 1];
    }

    // Build the output character array
    vector<int> output(n);
    for (int i = n - 1; i >= 0; i--) {
        output[count[arr[i]] - 1] = arr[i];
        count[arr[i]]--;
    }

    // Copy the output array to arr, so that arr now contains sorted characters
    for (int i = 0; i < n; i++) {
        arr[i] = output[i];
    }
}

void printArray(int arr[], int size) {
    for (int i = 0; i < size; i++)
        cout << arr[i] << " ";
    cout << endl;
}

int main() {
    int arr[] = {4, 2, 2, 8, 3, 3, 1};
    int n = sizeof(arr) / sizeof(arr[0]);
    cout << "Unsorted array: ";
    printArray(arr, n);
    
    countingSort(arr, n);
    
    cout << "Sorted array (Counting Sort): ";
    printArray(arr, n);
    return 0;
}
