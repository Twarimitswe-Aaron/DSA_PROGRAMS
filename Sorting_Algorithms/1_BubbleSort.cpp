/*
 * Algorithm: Bubble Sort
 * Description: Repeatedly steps through the list, compares adjacent elements and swaps them if they are in the wrong order.
 * 
 * Time Complexity:
 *  - Best Case: O(n) (When the array is already sorted, with optimized version)
 *  - Average Case: O(n^2)
 *  - Worst Case: O(n^2) (When the array is reverse sorted)
 * 
 * Space (Memory) Complexity: O(1) (In-place sorting)
 */

#include <iostream>
using namespace std;

void bubbleSort(int arr[], int n) {
    bool swapped;
    for (int i = 0; i < n - 1; i++) {
        swapped = false;
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                // Swap arr[j] and arr[j+1]
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                swapped = true;
            }
        }
        // If no two elements were swapped by inner loop, then break
        if (!swapped)
            break;
    }
}

void printArray(int arr[], int size) {
    for (int i = 0; i < size; i++)
        cout << arr[i] << " ";
    cout << endl;
}

int main() {
    int arr[] = {64, 34, 25, 12, 22, 11, 90};
    int n = sizeof(arr) / sizeof(arr[0]);
    cout << "Unsorted array: ";
    printArray(arr, n);
    
    bubbleSort(arr, n);
    
    cout << "Sorted array (Bubble Sort): ";
    printArray(arr, n);
    return 0;
}
