/*
 * Algorithm: Merge Sort
 * Description: A divide-and-conquer algorithm that divides the array into halves, recursively sorts them, and then merges the sorted halves.
 * 
 * Time Complexity:
 *  - Best Case: O(n log n)
 *  - Average Case: O(n log n)
 *  - Worst Case: O(n log n)
 * 
 * Space (Memory) Complexity: O(n) (Requires extra space for temporary arrays during merging)
 */

#include <iostream>
using namespace std;

// Merges two subarrays of arr[].
// First subarray is arr[left..mid]
// Second subarray is arr[mid+1..right]
void merge(int arr[], int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    // Create temp arrays
    int* L = new int[n1];
    int* R = new int[n2];

    // Copy data to temp arrays L[] and R[]
    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    // Merge the temp arrays back into arr[left..right]
    int i = 0; // Initial index of first subarray
    int j = 0; // Initial index of second subarray
    int k = left; // Initial index of merged subarray

    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    // Copy the remaining elements of L[], if there are any
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    // Copy the remaining elements of R[], if there are any
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
    
    // Free dynamic memory
    delete[] L;
    delete[] R;
}

// left is for left index and right is right index of the sub-array of arr to be sorted
void mergeSort(int arr[], int left, int right) {
    if (left >= right)
        return; // Returns recursively

    int mid = left + (right - left) / 2;
    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);
    merge(arr, left, mid, right);
}

void printArray(int arr[], int size) {
    for (int i = 0; i < size; i++)
        cout << arr[i] << " ";
    cout << endl;
}

int main() {
    int arr[] = {12, 11, 13, 5, 6, 7};
    int n = sizeof(arr) / sizeof(arr[0]);
    cout << "Unsorted array: ";
    printArray(arr, n);
    
    mergeSort(arr, 0, n - 1);
    
    cout << "Sorted array (Merge Sort): ";
    printArray(arr, n);
    return 0;
}
