/*
 * ============================================================================
 * FILE: 01_arrays.cpp
 * ============================================================================
 * ARRAYS - The most fundamental data structure in DSA practical exams.
 *
 * EXAM FREQUENCY: Every single DSA practical exam tests arrays.
 * Common questions:
 *   1. Find min/max in array
 *   2. Reverse an array (in-place)
 *   3. Find kth smallest/largest (quick-select)
 *   4. Rotate array by k positions
 *   5. Find missing number (1 to N)
 *   6. Find duplicates in array
 *   7. Two Sum problem
 *   8. Kadane's Algorithm (Maximum Subarray Sum)
 *   9. Move all zeros to end
 *   10. Merge two sorted arrays
 *   11. Find equilibrium index (prefix sum)
 *   12. Spiral traversal of matrix
 * ============================================================================
 */
#include <iostream>
#include <algorithm>  // for std::swap, std::sort, std::reverse
#include <climits>    // for INT_MIN, INT_MAX
#include <vector>     // for dynamic array (commonly used in exams)
#include <unordered_set> // for duplicate detection
using namespace std;

// ============================================================================
// TIP: All functions below accept vector<int>& (pass by reference) to avoid
// copying the entire array, which would be O(n) extra space unnecessarily.
// Always use & with vectors in function parameters for efficiency.
// ============================================================================

// ----------------------------------------------------------------------------
// 1. LINEAR SEARCH: Scan array from left to right. O(n) time, O(1) space.
// ----------------------------------------------------------------------------
int linearSearch(const vector<int>& arr, int key) {
    // Iterate through each element of the array
    for (size_t i = 0; i < arr.size(); i++) {
        // If current element matches the key, return its index immediately
        if (arr[i] == key) return i;
    }
    // Key not found in the entire array
    return -1;
}

// ----------------------------------------------------------------------------
// 2. FIND MINIMUM & MAXIMUM in a single pass. O(n) time, O(1) space.
// ----------------------------------------------------------------------------
pair<int, int> findMinMax(const vector<int>& arr) {
    // Initialize min to the largest possible integer value
    int minVal = INT_MAX;
    // Initialize max to the smallest possible integer value
    int maxVal = INT_MIN;
    // Single pass through array to find both min and max simultaneously
    for (int num : arr) {
        if (num < minVal) minVal = num;  // Update min if smaller element found
        if (num > maxVal) maxVal = num;  // Update max if larger element found
    }
    // Return as pair (min, max)
    return {minVal, maxVal};
}

// ----------------------------------------------------------------------------
// 3. REVERSE ARRAY IN-PLACE: Two-pointer technique. O(n) time, O(1) space.
// ----------------------------------------------------------------------------
void reverseArray(vector<int>& arr) {
    // 'left' pointer starts at beginning, 'right' pointer at end
    int left = 0, right = arr.size() - 1;
    // Swap elements from outside moving inward until pointers cross
    while (left < right) {
        // Swap elements at left and right positions
        swap(arr[left], arr[right]);
        // Move pointers toward the center
        left++;
        right--;
    }
}

// ----------------------------------------------------------------------------
// 4. ROTATE ARRAY LEFT by k positions. O(n) time, O(1) space.
// Uses the "reversal algorithm" - 3 reverse operations:
//   Step 1: Reverse first k elements
//   Step 2: Reverse remaining n-k elements
//   Step 3: Reverse entire array
// ----------------------------------------------------------------------------
void rotateLeft(vector<int>& arr, int k) {
    int n = arr.size();
    if (n == 0) return;       // Empty array, nothing to rotate
    k = k % n;                // Handle k > n: rotating n times gives original
    if (k == 0) return;       // No rotation needed

    // Step 1: Reverse first k elements (0 to k-1)
    reverse(arr.begin(), arr.begin() + k);
    // Step 2: Reverse remaining n-k elements (k to n-1)
    reverse(arr.begin() + k, arr.end());
    // Step 3: Reverse entire array for final result
    reverse(arr.begin(), arr.end());
}

// ----------------------------------------------------------------------------
// 5. ROTATE ARRAY RIGHT by k positions. O(n) time, O(1) space.
// Similar reversal algorithm, just different first two reversals.
// ----------------------------------------------------------------------------
void rotateRight(vector<int>& arr, int k) {
    int n = arr.size();
    if (n == 0) return;
    k = k % n;
    if (k == 0) return;

    // Step 1: Reverse entire array
    reverse(arr.begin(), arr.end());
    // Step 2: Reverse first k elements
    reverse(arr.begin(), arr.begin() + k);
    // Step 3: Reverse remaining n-k elements
    reverse(arr.begin() + k, arr.end());
}

// ----------------------------------------------------------------------------
// 6. FIND MISSING NUMBER from 1 to N. O(n) time, O(1) space.
// Array contains n-1 numbers from 1..n with exactly one missing.
// Uses XOR property: a ^ a = 0 and a ^ 0 = a
// XOR all numbers 1..n and all array elements; duplicates cancel out.
// ----------------------------------------------------------------------------
int findMissingNumber(const vector<int>& arr, int n) {
    // Approach 1: XOR method (no overflow risk)
    int xor1 = 0, xor2 = 0;
    // XOR all numbers from 1 to n
    for (int i = 1; i <= n; i++) xor1 ^= i;
    // XOR all elements in array
    for (int num : arr) xor2 ^= num;
    // Result = missing number (since duplicates cancel)
    return xor1 ^ xor2;

    // Approach 2: Sum formula (watch for integer overflow in large arrays)
    // int expectedSum = n * (n + 1) / 2;
    // int actualSum = accumulate(arr.begin(), arr.end(), 0);
    // return expectedSum - actualSum;
}

// ----------------------------------------------------------------------------
// 7. FIND DUPLICATES using hash set. O(n) time, O(n) space.
// Commonly asked: "Find all duplicates in an array"
// ----------------------------------------------------------------------------
vector<int> findDuplicates(const vector<int>& arr) {
    unordered_set<int> seen;    // Tracks elements already encountered
    vector<int> duplicates;     // Stores duplicate elements found
    for (int num : arr) {
        // If num is already in the set, it's a duplicate
        if (seen.find(num) != seen.end()) {
            duplicates.push_back(num);
        } else {
            // First time seeing this number, add to set
            seen.insert(num);
        }
    }
    return duplicates;
}

// ----------------------------------------------------------------------------
// 8. TWO SUM: Find pair with given target sum. O(n) time, O(n) space.
// Returns indices of the two numbers that add up to target.
// Uses hash map: for each element, check if (target - element) exists.
// ----------------------------------------------------------------------------
vector<int> twoSum(const vector<int>& arr, int target) {
    unordered_map<int, int> mp;  // Key: array element, Value: its index
    for (size_t i = 0; i < arr.size(); i++) {
        int complement = target - arr[i];  // What we need to reach target
        // If complement exists in map, we've found the pair
        if (mp.find(complement) != mp.end()) {
            return {(int)mp[complement], (int)i};  // Return indices
        }
        // Store current element with its index for future lookups
        mp[arr[i]] = i;
    }
    return {};  // No pair found (return empty vector)
}

// ----------------------------------------------------------------------------
// 9. KADANE'S ALGORITHM: Maximum Subarray Sum. O(n) time, O(1) space.
// Finds contiguous subarray with the largest sum.
// Key insight: If running sum becomes negative, reset to 0 (discard prefix).
// ----------------------------------------------------------------------------
int kadaneMaxSubarraySum(const vector<int>& arr) {
    int maxSoFar = arr[0];     // Global maximum (final answer)
    int maxEndingHere = arr[0]; // Running sum of current subarray

    for (size_t i = 1; i < arr.size(); i++) {
        // Either extend the existing subarray OR start fresh at arr[i]
        maxEndingHere = max(arr[i], maxEndingHere + arr[i]);
        // Update global maximum if current subarray sum is larger
        maxSoFar = max(maxSoFar, maxEndingHere);
    }
    return maxSoFar;  // Return the maximum subarray sum
}

// ----------------------------------------------------------------------------
// 10. MOVE ZEROS TO END (in-place). O(n) time, O(1) space.
// Maintains relative order of non-zero elements.
// Uses two-pointer technique: 'nonZero' tracks where to place next non-zero.
// ----------------------------------------------------------------------------
void moveZerosToEnd(vector<int>& arr) {
    int nonZero = 0;  // Position to place the next non-zero element
    // First pass: move all non-zero elements to the front
    for (size_t i = 0; i < arr.size(); i++) {
        if (arr[i] != 0) {
            swap(arr[i], arr[nonZero]);  // Place non-zero at correct position
            nonZero++;                    // Advance the placement pointer
        }
    }
    // Note: elements after 'nonZero' are automatically zeros (swapped)
}

// ----------------------------------------------------------------------------
// 11. MERGE TWO SORTED ARRAYS. O(n+m) time, O(n+m) space.
// Standard two-pointer merge (like Merge Sort's merge step).
// ----------------------------------------------------------------------------
vector<int> mergeSortedArrays(const vector<int>& arr1, const vector<int>& arr2) {
    vector<int> result;
    int i = 0, j = 0;  // Pointers for arr1 and arr2 respectively
    // Traverse both arrays simultaneously
    while (i < (int)arr1.size() && j < (int)arr2.size()) {
        // Pick the smaller element and advance the corresponding pointer
        if (arr1[i] <= arr2[j]) {
            result.push_back(arr1[i]);
            i++;
        } else {
            result.push_back(arr2[j]);
            j++;
        }
    }
    // Append remaining elements from arr1 (if any)
    while (i < (int)arr1.size()) {
        result.push_back(arr1[i]);
        i++;
    }
    // Append remaining elements from arr2 (if any)
    while (j < (int)arr2.size()) {
        result.push_back(arr2[j]);
        j++;
    }
    return result;
}

// ----------------------------------------------------------------------------
// 12. EQUILIBRIUM INDEX (Prefix Sum). O(n) time, O(1) space.
// An index i is equilibrium if sum of elements left of i == sum of right of i.
// Uses total sum and running left sum to avoid O(n) extra space.
// ----------------------------------------------------------------------------
int findEquilibriumIndex(const vector<int>& arr) {
    int totalSum = 0;    // Sum of all elements
    int leftSum = 0;     // Running sum of elements to the left of current index

    // Compute total sum of the entire array
    for (int num : arr) totalSum += num;

    // Iterate through each index to check equilibrium condition
    for (size_t i = 0; i < arr.size(); i++) {
        // Right sum = totalSum - leftSum - arr[i]
        // (Remove left part and current element from total)
        int rightSum = totalSum - leftSum - arr[i];
        if (leftSum == rightSum) return i;  // Found equilibrium index
        leftSum += arr[i];  // Add current element to left sum for next iteration
    }
    return -1;  // No equilibrium index exists
}

// ----------------------------------------------------------------------------
// 13. SPIRAL TRAVERSAL of a matrix. O(m*n) time, O(1) space (excluding result).
// Traverses matrix in spiral order: right -> down -> left -> up -> repeat.
// ----------------------------------------------------------------------------
vector<int> spiralTraversal(const vector<vector<int>>& matrix) {
    vector<int> result;
    if (matrix.empty() || matrix[0].empty()) return result;

    int top = 0;                           // Top row boundary
    int bottom = matrix.size() - 1;        // Bottom row boundary
    int left = 0;                          // Left column boundary
    int right = matrix[0].size() - 1;      // Right column boundary

    // Continue until boundaries cross (all elements visited)
    while (top <= bottom && left <= right) {
        // Step 1: Traverse RIGHT along the top row
        for (int col = left; col <= right; col++)
            result.push_back(matrix[top][col]);
        top++;  // Shrink the top boundary downward

        // Step 2: Traverse DOWN along the right column
        for (int row = top; row <= bottom; row++)
            result.push_back(matrix[row][right]);
        right--;  // Shrink the right boundary leftward

        // Step 3: Traverse LEFT along the bottom row (if still valid)
        if (top <= bottom) {
            for (int col = right; col >= left; col--)
                result.push_back(matrix[bottom][col]);
            bottom--;  // Shrink the bottom boundary upward
        }

        // Step 4: Traverse UP along the left column (if still valid)
        if (left <= right) {
            for (int row = bottom; row >= top; row--)
                result.push_back(matrix[row][left]);
            left++;  // Shrink the left boundary rightward
        }
    }
    return result;
}

// ============================================================================
// SAMPLE main() to demonstrate usage of all array functions
// ============================================================================
int main() {
    cout << "========== ARRAY OPERATIONS (01_arrays.cpp) ==========\n\n";

    // ---- Test Linear Search ----
    vector<int> arr1 = {10, 20, 30, 40, 50};
    cout << "Array: [10, 20, 30, 40, 50]\n";
    cout << "Linear search for 30: found at index "
         << linearSearch(arr1, 30) << "\n";
    cout << "Linear search for 99: found at index "
         << linearSearch(arr1, 99) << "\n\n";

    // ---- Test Min/Max ----
    vector<int> arr2 = {3, 7, 2, 9, 1, 5};
    pair<int, int> minMax = findMinMax(arr2);
    cout << "Array: [3, 7, 2, 9, 1, 5]\n";
    cout << "Min: " << minMax.first << ", Max: " << minMax.second << "\n\n";

    // ---- Test Reverse ----
    vector<int> arr3 = {1, 2, 3, 4, 5};
    cout << "Original: [1, 2, 3, 4, 5]\n";
    reverseArray(arr3);
    cout << "Reversed: [";
    for (size_t i = 0; i < arr3.size(); i++)
        cout << (i ? ", " : "") << arr3[i];
    cout << "]\n\n";

    // ---- Test Rotations ----
    vector<int> arr4 = {1, 2, 3, 4, 5};
    rotateLeft(arr4, 2);
    cout << "Left rotate by 2: [";
    for (size_t i = 0; i < arr4.size(); i++)
        cout << (i ? ", " : "") << arr4[i];
    cout << "]\n";

    vector<int> arr5 = {1, 2, 3, 4, 5};
    rotateRight(arr5, 2);
    cout << "Right rotate by 2: [";
    for (size_t i = 0; i < arr5.size(); i++)
        cout << (i ? ", " : "") << arr5[i];
    cout << "]\n\n";

    // ---- Test Missing Number ----
    vector<int> arr6 = {1, 2, 4, 5, 6};  // n=6, missing=3
    cout << "Array: [1, 2, 4, 5, 6] (n=6)\n";
    cout << "Missing number: " << findMissingNumber(arr6, 6) << "\n\n";

    // ---- Test Kadane's Algorithm ----
    vector<int> arr7 = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
    cout << "Array: [-2, 1, -3, 4, -1, 2, 1, -5, 4]\n";
    cout << "Max subarray sum (Kadane): " << kadaneMaxSubarraySum(arr7) << "\n\n";

    // ---- Test Move Zeros ----
    vector<int> arr8 = {0, 1, 0, 3, 12};
    moveZerosToEnd(arr8);
    cout << "Move zeros to end: [";
    for (size_t i = 0; i < arr8.size(); i++)
        cout << (i ? ", " : "") << arr8[i];
    cout << "]\n\n";

    // ---- Test Two Sum ----
    vector<int> arr9 = {2, 7, 11, 15};
    vector<int> indices = twoSum(arr9, 9);
    cout << "Two Sum (target=9): indices [" << indices[0] << ", " << indices[1] << "]\n";
    cout << "  Values: " << arr9[indices[0]] << " + " << arr9[indices[1]]
         << " = 9\n\n";

    // ---- Test Merge Sorted Arrays ----
    vector<int> a = {1, 3, 5, 7};
    vector<int> b = {2, 4, 6, 8};
    vector<int> merged = mergeSortedArrays(a, b);
    cout << "Merge [1,3,5,7] + [2,4,6,8]: [";
    for (size_t i = 0; i < merged.size(); i++)
        cout << (i ? ", " : "") << merged[i];
    cout << "]\n\n";

    // ---- Test Equilibrium Index ----
    vector<int> arr10 = {-7, 1, 5, 2, -4, 3, 0};
    int eqIdx = findEquilibriumIndex(arr10);
    cout << "Equilibrium index of [-7,1,5,2,-4,3,0]: " << eqIdx << "\n";
    if (eqIdx != -1) {
        cout << "  Left sum = Right sum at index " << eqIdx << "\n";
    }

    cout << "\n========== END OF ARRAY OPERATIONS ==========\n";

    return 0;
}
