/*
 * ============================================================================
 * FILE: 05_sorting.cpp
 * ============================================================================
 * SORTING ALGORITHMS - Essential for every DSA practical exam.
 *
 * EXAM FREQUENCY: Extremely high. Expect to implement at least 2-3 sorts.
 *
 * Algorithms Covered:
 *   1. Bubble Sort        O(n^2) - Simple but slow
 *   2. Selection Sort     O(n^2) - Minimum selection approach
 *   3. Insertion Sort     O(n^2) - Good for nearly sorted data
 *   4. Merge Sort         O(n log n) - Divide & Conquer, stable
 *   5. Quick Sort         O(n log n) avg - Partition-based, fastest avg
 *   6. Heap Sort          O(n log n) - Uses heap data structure
 *   7. Counting Sort      O(n+k) - Non-comparison, for integers
 *   8. Radix Sort         O(d*(n+b)) - Digit by digit sorting
 * ============================================================================
 */
#include <iostream>
#include <vector>
#include <algorithm>  // for std::swap
using namespace std;

// ============================================================================
// 1. BUBBLE SORT - O(n^2) time, O(1) space
//
// How it works: Repeatedly step through the array, compare adjacent elements,
// and swap them if they are in the wrong order. The largest element "bubbles"
// to the end in each pass.
//
// Optimization: If no swaps occur in a pass, the array is already sorted.
// ============================================================================
void bubbleSort(vector<int>& arr) {
    int n = arr.size();
    // Outer loop: number of passes needed (n-1 passes maximum)
    for (int i = 0; i < n - 1; i++) {
        bool swapped = false;  // Optimization: track if any swap happened
        // Inner loop: compare adjacent elements in unsorted portion
        // After i passes, the last i elements are already in correct place
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);  // Swap if out of order
                swapped = true;
            }
        }
        // If no swaps, array is sorted - early exit
        if (!swapped) break;
    }
}

// ============================================================================
// 2. SELECTION SORT - O(n^2) time, O(1) space
//
// How it works: Divide array into sorted (left) and unsorted (right) parts.
// In each pass, find the minimum element from the unsorted part and place it
// at the beginning of the unsorted part (expanding the sorted part).
// ============================================================================
void selectionSort(vector<int>& arr) {
    int n = arr.size();
    // i is the boundary between sorted and unsorted parts
    for (int i = 0; i < n - 1; i++) {
        int minIdx = i;  // Assume first unsorted element is the minimum
        // Find the actual minimum in the unsorted section
        for (int j = i + 1; j < n; j++) {
            if (arr[j] < arr[minIdx]) {
                minIdx = j;  // Found a smaller element, update minIdx
            }
        }
        // Swap the found minimum with the first unsorted position
        if (minIdx != i) {
            swap(arr[i], arr[minIdx]);
        }
    }
}

// ============================================================================
// 3. INSERTION SORT - O(n^2) worst, O(n) best, O(1) space
//
// How it works: Build sorted array one element at a time by picking each
// element and inserting it into its correct position in the sorted portion.
// Excellent for nearly sorted data and small arrays.
// ============================================================================
void insertionSort(vector<int>& arr) {
    int n = arr.size();
    // Start from the second element (first element is trivially sorted)
    for (int i = 1; i < n; i++) {
        int key = arr[i];   // The element to be positioned
        int j = i - 1;      // Start comparing with element to the left

        // Shift elements right until we find the correct position for key
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];  // Shift element right
            j--;
        }
        arr[j + 1] = key;  // Place key in its correct position
    }
}

// ============================================================================
// 4. MERGE SORT - O(n log n) time, O(n) space
//
// How it works: Classic Divide and Conquer algorithm.
//   1. DIVIDE: Split the array into two halves recursively
//   2. CONQUER: Sort each half
//   3. COMBINE: Merge the two sorted halves back together
// Stable sort - maintains relative order of equal elements.
// ============================================================================

// Helper function: Merge two sorted subarrays into one sorted array
void merge(vector<int>& arr, int left, int mid, int right) {
    // Calculate sizes of two subarrays
    int n1 = mid - left + 1;   // Size of left subarray
    int n2 = right - mid;      // Size of right subarray

    // Create temporary arrays to hold the data
    vector<int> L(n1), R(n2);

    // Copy data from original array to temp arrays
    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    // Merge the two temp arrays back into arr[left..right]
    int i = 0, j = 0;       // Pointers for L and R respectively
    int k = left;           // Pointer for the merged position in arr

    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {    // <= ensures stability
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    // Copy any remaining elements from L (if R was exhausted first)
    while (i < n1) {
        arr[k] = L[i];
        i++; k++;
    }
    // Copy any remaining elements from R (if L was exhausted first)
    while (j < n2) {
        arr[k] = R[j];
        j++; k++;
    }
}

// Recursive merge sort function
void mergeSort(vector<int>& arr, int left, int right) {
    if (left < right) {  // Base case: single element (left == right)
        int mid = left + (right - left) / 2;  // Find middle (avoids overflow)
        mergeSort(arr, left, mid);            // Sort first half
        mergeSort(arr, mid + 1, right);       // Sort second half
        merge(arr, left, mid, right);         // Merge sorted halves
    }
}

// Wrapper for convenient call: mergeSort(arr, 0, n-1)
void mergeSort(vector<int>& arr) {
    mergeSort(arr, 0, arr.size() - 1);
}

// ============================================================================
// 5. QUICK SORT - O(n log n) average, O(n^2) worst, O(log n) space
//
// How it works:
//   1. Choose a pivot element
//   2. Partition: rearrange so elements < pivot are left, > pivot are right
//   3. Recursively sort the left and right partitions
//
// The choice of pivot matters. Bad pivot choice (e.g., always first element
// on already sorted array) degrades to O(n^2).
// ============================================================================

// Partition function using Lomuto partition scheme
int partition(vector<int>& arr, int low, int high) {
    int pivot = arr[high];  // Choose the last element as pivot

    // i tracks the boundary between elements < pivot and > pivot
    int i = low - 1;  // Index of smaller element (initially none found)

    // j traverses the array from low to high-1
    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {  // Current element belongs on the left side
            i++;
            swap(arr[i], arr[j]);  // Move it to the left partition
        }
    }
    // Place pivot in its correct position (between left and right partitions)
    swap(arr[i + 1], arr[high]);
    return i + 1;  // Return the pivot's final index
}

void quickSort(vector<int>& arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);  // Partition index
        quickSort(arr, low, pi - 1);         // Sort left of pivot
        quickSort(arr, pi + 1, high);        // Sort right of pivot
    }
}

void quickSort(vector<int>& arr) {
    quickSort(arr, 0, arr.size() - 1);
}

// ============================================================================
// 6. HEAP SORT - O(n log n) time, O(1) space
//
// How it works:
//   1. BUILD max heap from the array (heapify all non-leaf nodes)
//   2. REPEAT: swap root (max element) with last element, reduce heap size, heapify root
//
// Detailed explanation is in 10_heap.cpp. Here's a self-contained version.
// ============================================================================

void heapify(vector<int>& arr, int n, int i) {
    int largest = i;          // Assume root is largest
    int left = 2 * i + 1;     // Left child index (0-based array)
    int right = 2 * i + 2;    // Right child index

    // If left child exists and is larger than root
    if (left < n && arr[left] > arr[largest]) largest = left;
    // If right child exists and is larger than largest so far
    if (right < n && arr[right] > arr[largest]) largest = right;

    // If largest is not root, swap and recursively heapify affected subtree
    if (largest != i) {
        swap(arr[i], arr[largest]);
        heapify(arr, n, largest);  // Recursively heapify the affected subtree
    }
}

void heapSort(vector<int>& arr) {
    int n = arr.size();
    // Step 1: Build max heap (heapify all non-leaf nodes from bottom-up)
    // n/2 - 1 is the index of the last non-leaf node
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(arr, n, i);
    }
    // Step 2: Extract elements from heap one by one
    for (int i = n - 1; i > 0; i--) {
        swap(arr[0], arr[i]);   // Move current root (max) to end
        heapify(arr, i, 0);     // Call heapify on the reduced heap
    }
}

// ============================================================================
// 7. COUNTING SORT - O(n + k) time, O(k) space (k = range of input)
//
// Non-comparison based sort. Only works for integers (or countable items).
// Counts occurrences of each value, then uses prefix sums to position elements.
// Stable sort when iterating from right to left in final placement.
// ============================================================================
void countingSort(vector<int>& arr) {
    if (arr.empty()) return;

    // Find the range of input (min and max)
    int maxVal = *max_element(arr.begin(), arr.end());
    int minVal = *min_element(arr.begin(), arr.end());
    int range = maxVal - minVal + 1;

    // Create count array initialized to 0
    vector<int> count(range, 0);
    // Create output array (same size as input)
    vector<int> output(arr.size());

    // Step 1: Count occurrences of each value
    for (int num : arr) {
        count[num - minVal]++;  // Shift by minVal to handle negative numbers
    }
    // Step 2: Compute prefix sums (cumulative count) for stable positioning
    for (int i = 1; i < range; i++) {
        count[i] += count[i - 1];
    }
    // Step 3: Build output array (traverse input right-to-left for stability)
    for (int i = arr.size() - 1; i >= 0; i--) {
        int val = arr[i];
        int pos = count[val - minVal] - 1;  // Position in output
        output[pos] = val;
        count[val - minVal]--;  // Decrement count for duplicates
    }
    // Step 4: Copy back to original array
    arr = output;
}

// ============================================================================
// 8. RADIX SORT - O(d * (n + b)) time, O(n + b) space
//     where d = number of digits, b = base (10 typically)
//
// Non-comparison sort. Sorts digit by digit from LSD to MSD.
// Uses counting sort as a subroutine for each digit position.
// ============================================================================

// Counting sort based on a specific digit (10^exp place)
void countingSortByDigit(vector<int>& arr, int exp) {
    int n = arr.size();
    vector<int> output(n);
    vector<int> count(10, 0);  // 10 possible digits (0-9)

    // Count occurrences of each digit at current place value
    for (int i = 0; i < n; i++) {
        int digit = (arr[i] / exp) % 10;
        count[digit]++;
    }
    // Compute prefix sums for stable positioning
    for (int i = 1; i < 10; i++) {
        count[i] += count[i - 1];
    }
    // Build output array (traverse right-to-left for stability)
    for (int i = n - 1; i >= 0; i--) {
        int digit = (arr[i] / exp) % 10;
        output[count[digit] - 1] = arr[i];
        count[digit]--;
    }
    // Copy back
    arr = output;
}

void radixSort(vector<int>& arr) {
    if (arr.empty()) return;
    // Find the maximum value to determine number of digits
    int maxVal = *max_element(arr.begin(), arr.end());
    // Apply counting sort for each digit position (1s, 10s, 100s, ...)
    for (int exp = 1; maxVal / exp > 0; exp *= 10) {
        countingSortByDigit(arr, exp);
    }
}

// ============================================================================
// UTILITY: Display array
// ============================================================================
void printArray(const vector<int>& arr) {
    for (int num : arr) cout << num << " ";
    cout << "\n";
}

// ============================================================================
// SAMPLE main() to demonstrate all sorting algorithms
// ============================================================================
int main() {
    cout << "========== SORTING ALGORITHMS (05_sorting.cpp) ==========\n\n";

    vector<int> original = {64, 34, 25, 12, 22, 11, 90};

    // Test each sorting algorithm with a fresh copy of the original array
    vector<int> arr;

    cout << "Original array: ";
    printArray(original);

    arr = original;
    bubbleSort(arr);
    cout << "Bubble Sort:    ";
    printArray(arr);

    arr = original;
    selectionSort(arr);
    cout << "Selection Sort: ";
    printArray(arr);

    arr = original;
    insertionSort(arr);
    cout << "Insertion Sort: ";
    printArray(arr);

    arr = original;
    mergeSort(arr);
    cout << "Merge Sort:     ";
    printArray(arr);

    arr = original;
    quickSort(arr);
    cout << "Quick Sort:     ";
    printArray(arr);

    arr = original;
    heapSort(arr);
    cout << "Heap Sort:      ";
    printArray(arr);

    arr = {4, 2, 2, 8, 3, 3, 1};
    cout << "\nCounting Sort test:\n";
    cout << "Original: ";
    printArray(arr);
    countingSort(arr);
    cout << "Sorted:   ";
    printArray(arr);

    arr = {170, 45, 75, 90, 802, 24, 2, 66};
    cout << "\nRadix Sort test:\n";
    cout << "Original: ";
    printArray(arr);
    radixSort(arr);
    cout << "Sorted:   ";
    printArray(arr);

    cout << "\n========== END OF SORTING ALGORITHMS ==========\n";

    return 0;
}
