/*
 * ============================================================================
 * FILE: 06_searching.cpp
 * ============================================================================
 * SEARCHING ALGORITHMS - Finding elements in data structures efficiently.
 *
 * EXAM FREQUENCY: Very high. Search algorithms are tested directly or as
 * components of larger problems.
 *
 * Algorithms Covered:
 *   1. Linear Search      O(n) - Sequential scan
 *   2. Binary Search      O(log n) - Divide & conquer on sorted array
 *   3. Binary Search (recursive)
 *   4. First/Last Occurrence using Binary Search
 *   5. Count Occurrences in sorted array
 *   6. Find peak element (local maximum)
 *   7. Search in rotated sorted array
 *   8. Exponential Search
 *   9. Interpolation Search O(log log n) avg
 *   10. Jump Search        O(sqrt(n))
 * ============================================================================
 */
#include <iostream>
#include <vector>
#include <algorithm>  // for sort (if array not already sorted)
#include <cmath>      // for sqrt in jump search
using namespace std;

// ============================================================================
// 1. LINEAR SEARCH - O(n) time, O(1) space
//
// Simplest search. Scan each element until found.
// Works on unsorted data. Use when array is small or unsorted.
// ============================================================================
int linearSearch(const vector<int>& arr, int key) {
    for (size_t i = 0; i < arr.size(); i++) {
        if (arr[i] == key) return i;
    }
    return -1;
}

// ============================================================================
// 2. BINARY SEARCH (Iterative) - O(log n) time, O(1) space
//
// REQUIRES: Sorted array (ascending).
//
// How it works:
//   - Maintain a search interval [low, high]
//   - Compare middle element with key
//   - If key equals middle: found
//   - If key < middle: search left half (high = mid - 1)
//   - If key > middle: search right half (low = mid + 1)
//   - Repeat until interval is empty (low > high)
// ============================================================================
int binarySearch(const vector<int>& arr, int key) {
    int low = 0;
    int high = arr.size() - 1;

    while (low <= high) {
        // Use mid = low + (high - low) / 2 to avoid integer overflow
        // (low + high) can overflow for very large arrays
        int mid = low + (high - low) / 2;

        if (arr[mid] == key) {
            return mid;        // Found at middle
        } else if (arr[mid] < key) {
            low = mid + 1;     // Search in right half
        } else {
            high = mid - 1;    // Search in left half
        }
    }
    return -1;  // Not found
}

// ============================================================================
// 3. BINARY SEARCH (Recursive) - O(log n) time, O(log n) space (call stack)
// ============================================================================
int binarySearchRecursive(const vector<int>& arr, int low, int high, int key) {
    if (low > high) return -1;  // Base case: empty interval

    int mid = low + (high - low) / 2;

    if (arr[mid] == key) return mid;
    else if (arr[mid] < key) return binarySearchRecursive(arr, mid + 1, high, key);
    else return binarySearchRecursive(arr, low, mid - 1, key);
}

// Wrapper for recursive binary search
int binarySearchRecursive(const vector<int>& arr, int key) {
    return binarySearchRecursive(arr, 0, arr.size() - 1, key);
}

// ============================================================================
// 4. FIND FIRST OCCURRENCE in sorted array (with duplicates)
//
// Standard binary search but modified to find the leftmost occurrence.
// When arr[mid] == key, we don't return immediately. Instead, we continue
// searching in the LEFT half to find the first occurrence.
// ============================================================================
int firstOccurrence(const vector<int>& arr, int key) {
    int low = 0, high = arr.size() - 1;
    int result = -1;

    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (arr[mid] == key) {
            result = mid;       // Record this occurrence
            high = mid - 1;     // Continue searching LEFT for earlier occurrence
        } else if (arr[mid] < key) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return result;
}

// ============================================================================
// 5. FIND LAST OCCURRENCE in sorted array (with duplicates)
//
// Similar, but when arr[mid] == key, we search RIGHT for later occurrence.
// ============================================================================
int lastOccurrence(const vector<int>& arr, int key) {
    int low = 0, high = arr.size() - 1;
    int result = -1;

    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (arr[mid] == key) {
            result = mid;       // Record this occurrence
            low = mid + 1;      // Continue searching RIGHT for later occurrence
        } else if (arr[mid] < key) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return result;
}

// ============================================================================
// 6. COUNT OCCURRENCES in sorted array
// Uses first and last occurrence. O(log n) time.
// ============================================================================
int countOccurrences(const vector<int>& arr, int key) {
    int first = firstOccurrence(arr, key);
    if (first == -1) return 0;  // Key not found
    int last = lastOccurrence(arr, key);
    return last - first + 1;
}

// ============================================================================
// 7. FIND PEAK ELEMENT (local maximum)
// An element is a peak if it is >= its neighbors.
// Array may have multiple peaks; return any one.
// Uses binary search on a "bitonic"-like property. O(log n) time.
// ============================================================================
int findPeakElement(const vector<int>& arr) {
    int n = arr.size();
    if (n == 0) return -1;
    if (n == 1) return 0;

    int low = 0, high = n - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;

        // Check if mid is a peak (>= neighbors)
        bool leftOk = (mid == 0) || (arr[mid] >= arr[mid - 1]);
        bool rightOk = (mid == n - 1) || (arr[mid] >= arr[mid + 1]);

        if (leftOk && rightOk) return mid;  // Found a peak

        // If left neighbor is greater, there must be a peak on the left side
        if (mid > 0 && arr[mid - 1] > arr[mid]) {
            high = mid - 1;
        } else {
            // Otherwise, search right (since right neighbor must be greater)
            low = mid + 1;
        }
    }
    return -1;
}

// ============================================================================
// 8. SEARCH IN ROTATED SORTED ARRAY - O(log n) time
//
// Array was sorted, then rotated at some pivot. E.g., [4,5,6,7,0,1,2].
// Key insight: After finding mid, at least one half is completely sorted.
// Check which half is sorted, then determine if key lies in that half.
// ============================================================================
int searchRotated(const vector<int>& arr, int key) {
    int low = 0, high = arr.size() - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (arr[mid] == key) return mid;

        // Check if left half is sorted
        if (arr[low] <= arr[mid]) {
            // Left half is sorted. Check if key lies in left half.
            if (key >= arr[low] && key < arr[mid]) {
                high = mid - 1;  // Search left half
            } else {
                low = mid + 1;   // Search right half
            }
        }
        // Otherwise, right half is sorted
        else {
            // Check if key lies in right half
            if (key > arr[mid] && key <= arr[high]) {
                low = mid + 1;   // Search right half
            } else {
                high = mid - 1;  // Search left half
            }
        }
    }
    return -1;
}

// ============================================================================
// 9. EXPONENTIAL SEARCH - O(log n) time, O(1) space
//
// Useful for unbounded/infinite arrays and arrays where element is near start.
// How it works:
//   1. Find range where key could be by doubling the index (1, 2, 4, 8, ...)
//   2. Binary search within that range
// ============================================================================
int exponentialSearch(const vector<int>& arr, int key) {
    int n = arr.size();
    if (n == 0) return -1;

    // If key is at position 0
    if (arr[0] == key) return 0;

    // Find range [2^(i-1), 2^i] where key might be
    int i = 1;
    while (i < n && arr[i] <= key) {
        i *= 2;  // Double the index exponentially
    }

    // Binary search in range [i/2, min(i, n-1)]
    return binarySearchRecursive(arr, i / 2, min(i, n - 1), key);
}

// ============================================================================
// 10. INTERPOLATION SEARCH - O(log log n) average, O(n) worst
//
// Improvement over binary search for uniformly distributed sorted data.
// Instead of always using the middle, it estimates the position using
// the formula: pos = low + (key - arr[low]) * (high - low) / (arr[high] - arr[low])
// This is like searching a phonebook - you open near the name you're looking for.
//
// WORKS BEST when data is uniformly distributed (e.g., 1, 2, 3, ..., 100).
// ============================================================================
int interpolationSearch(const vector<int>& arr, int key) {
    int low = 0, high = arr.size() - 1;

    while (low <= high && key >= arr[low] && key <= arr[high]) {
        // If all elements are equal, avoid division by zero
        if (arr[high] == arr[low]) {
            if (arr[low] == key) return low;
            else return -1;
        }

        // Probing the position using interpolation formula
        int pos = low + ((double)(high - low) / (arr[high] - arr[low])) * (key - arr[low]);

        if (arr[pos] == key) return pos;

        if (arr[pos] < key) {
            low = pos + 1;   // Search right
        } else {
            high = pos - 1;  // Search left
        }
    }
    return -1;  // Not found
}

// ============================================================================
// 11. JUMP SEARCH - O(sqrt(n)) time, O(1) space
//
// Like binary search but jumps by sqrt(n) steps instead of halving.
// Optimal step size = sqrt(n) gives O(sqrt(n)) time.
// Works on sorted arrays.
// ============================================================================
int jumpSearch(const vector<int>& arr, int key) {
    int n = arr.size();
    int step = sqrt(n);  // Optimal jump step size

    // Find the block where key might be
    int prev = 0;
    while (arr[min(step, n) - 1] < key) {
        prev = step;
        step += sqrt(n);
        if (prev >= n) return -1;  // Key is beyond the array bounds
    }

    // Linear search within the identified block [prev, min(step, n)-1]
    while (prev < min(step, n)) {
        if (arr[prev] == key) return prev;
        prev++;
    }
    return -1;
}

// ============================================================================
// SAMPLE main() to demonstrate all searching algorithms
// ============================================================================
int main() {
    cout << "========== SEARCHING ALGORITHMS (06_searching.cpp) ==========\n\n";

    // -- Test arrays --
    vector<int> sortedArr = {2, 5, 8, 12, 16, 23, 38, 56, 72, 91};
    vector<int> withDuplicates = {1, 2, 3, 3, 3, 4, 5, 5, 6};
    vector<int> rotatedArr = {15, 18, 2, 3, 6, 12};
    vector<int> uniformArr = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};

    int key = 23;

    cout << "Sorted array: ";
    for (int x : sortedArr) cout << x << " ";
    cout << "\nSearching for: " << key << "\n\n";

    cout << "Linear Search:           " << linearSearch(sortedArr, key) << "\n";
    cout << "Binary Search (iter):    " << binarySearch(sortedArr, key) << "\n";
    cout << "Binary Search (recur):   " << binarySearchRecursive(sortedArr, key) << "\n";
    cout << "Exponential Search:      " << exponentialSearch(sortedArr, key) << "\n";
    cout << "Interpolation Search:    " << interpolationSearch(sortedArr, key) << "\n";
    cout << "Jump Search:             " << jumpSearch(sortedArr, key) << "\n\n";

    cout << "Array with duplicates: ";
    for (int x : withDuplicates) cout << x << " ";
    cout << "\nSearching for 3:\n";
    cout << "First occurrence:  " << firstOccurrence(withDuplicates, 3) << "\n";
    cout << "Last occurrence:   " << lastOccurrence(withDuplicates, 3) << "\n";
    cout << "Count occurrences: " << countOccurrences(withDuplicates, 3) << "\n\n";

    cout << "Rotated array: ";
    for (int x : rotatedArr) cout << x << " ";
    cout << "\nSearch 6:  " << searchRotated(rotatedArr, 6) << "\n";
    cout << "Search 15: " << searchRotated(rotatedArr, 15) << "\n\n";

    cout << "Find peak in [1, 3, 20, 4, 1, 0]: ";
    vector<int> peakArr = {1, 3, 20, 4, 1, 0};
    int peak = findPeakElement(peakArr);
    cout << "index " << peak << " (value " << peakArr[peak] << ")\n";

    cout << "\n========== END OF SEARCHING ALGORITHMS ==========\n";
    return 0;
}
