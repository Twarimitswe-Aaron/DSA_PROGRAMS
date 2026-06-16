/*
 * Algorithm: Quick Sort
 * Description: A divide-and-conquer algorithm that selects a 'pivot' element and partitions the other elements into two sub-arrays, according to whether they are less than or greater than the pivot.
 * 
 * Time Complexity:
 *  - Best Case: O(n log n)
 *  - Average Case: O(n log n)
 *  - Worst Case: O(n^2) (When the array is already sorted and we pick the worst pivot, but usually avoided with randomized pivots)
 * 
 * Space (Memory) Complexity: O(log n) (Recursive call stack space)
 */

#include <iostream>
using namespace std;

// A utility function to swap two elements
void swap(int* a, int* b) {
    int t = *a;
    *a = *b;
    *b = t;
}

// This function takes last element as pivot, places the pivot element at its
// correct position in sorted array, and places all smaller to left of pivot
// and all greater elements to right of pivot
int partition(int arr[], int low, int high) {
    int pivot = arr[high]; // pivot
    int i = (low - 1); // Index of smaller element

    for (int j = low; j <= high - 1; j++) {
        // If current element is smaller than the pivot
        if (arr[j] < pivot) {
            i++; // increment index of smaller element
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

// The main function that implements QuickSort
void quickSort(int arr[], int low, int high) {
    if (low < high) {
        // pi is partitioning index, arr[p] is now at right place
        int pi = partition(arr, low, high);

        // Separately sort elements before partition and after partition
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

void printArray(int arr[], int size) {
    for (int i = 0; i < size; i++)
        cout << arr[i] << " ";
    cout << endl;
}

int main() {
    int arr[] = {10, 7, 8, 9, 1, 5};
    int n = sizeof(arr) / sizeof(arr[0]);
    cout << "Unsorted array: ";
    printArray(arr, n);
    
    quickSort(arr, 0, n - 1);
    
    cout << "Sorted array (Quick Sort): ";
    printArray(arr, n);
    return 0;
}
