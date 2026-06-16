/*
 * Algorithm: Selection Sort
 * Description: Divides the input list into two parts: a sorted sublist and an unsorted sublist. It repeatedly selects the smallest element from the unsorted sublist and moves it to the sorted sublist.
 * 
 * Time Complexity:
 *  - Best Case: O(n^2)
 *  - Average Case: O(n^2)
 *  - Worst Case: O(n^2)
 * 
 * Space (Memory) Complexity: O(1) (In-place sorting)
 */

#include <iostream>
using namespace std;

void selectionSort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        // Find the minimum element in unsorted array
        int min_idx = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j] < arr[min_idx])
                min_idx = j;
        }

        // Swap the found minimum element with the first element
        if (min_idx != i) {
            int temp = arr[min_idx];
            arr[min_idx] = arr[i];
            arr[i] = temp;
        }
    }
}

void printArray(int arr[], int size) {
    for (int i = 0; i < size; i++)
        cout << arr[i] << " ";
    cout << endl;
}

int main() {
    int arr[] = {64, 25, 12, 22, 11};
    int n = sizeof(arr) / sizeof(arr[0]);
    cout << "Unsorted array: ";
    printArray(arr, n);
    
    selectionSort(arr, n);
    
    cout << "Sorted array (Selection Sort): ";
    printArray(arr, n);
    return 0;
}
