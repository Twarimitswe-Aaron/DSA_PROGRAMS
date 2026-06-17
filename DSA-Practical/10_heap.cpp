/*
 * ============================================================================
 * FILE: 10_heap.cpp
 * ============================================================================
 * HEAP - Specialized tree-based data structure satisfying the heap property.
 *
 * EXAM FREQUENCY: High. Heaps are tested in sorting, priority queues,
 * and specific problems.
 *
 * KEY CONCEPT: A binary heap is a COMPLETE binary tree (all levels filled
 * except possibly last, left-justified) that satisfies the heap property:
 *   - Max Heap: parent >= children (root is the MAXIMUM element)
 *   - Min Heap: parent <= children (root is the MINIMUM element)
 *
 * IMPLEMENTATION: Heaps are efficiently implemented using ARRAYS.
 * For node at index i (0-based):
 *   - Parent:  (i - 1) / 2
 *   - Left child:  2 * i + 1
 *   - Right child: 2 * i + 2
 *
 * CORE OPERATIONS:
 *   1. push (insert) - O(log n)
 *   2. pop (extract min/max) - O(log n)
 *   3. top (peek at min/max) - O(1)
 *   4. heapify - O(log n)
 *   5. buildHeap - O(n)
 * ============================================================================
 */
#include <iostream>
#include <vector>
#include <algorithm>  // For heap operations (used in applications section)
#include <queue>      // For priority_queue (STL heap)
#include <climits>
using namespace std;

// ============================================================================
// SECTION 1: MIN HEAP (Priority Queue) Implementation
//
// Complete binary tree where every parent <= its children.
// Smallest element is always at the root (index 0).
// ============================================================================

class MinHeap {
private:
    vector<int> heap;  // Stores heap elements (array representation)

    // -------------------------------------------------------------------
    // Helper functions to navigate between parent and children
    // All O(1) time using array index arithmetic.
    // -------------------------------------------------------------------
    int parent(int i) { return (i - 1) / 2; }
    int leftChild(int i) { return 2 * i + 1; }
    int rightChild(int i) { return 2 * i + 2; }

    // -------------------------------------------------------------------
    // HEAPIFY UP (Bubble Up / Sift Up): O(log n) time.
    //
    // Used after INSERTION. Starting from the newly added leaf, repeatedly
    // swap with parent if parent > current (violates min-heap property).
    // Move upward until heap property is restored.
    // -------------------------------------------------------------------
    void heapifyUp(int i) {
        // While current node is not root and parent > current
        while (i > 0 && heap[parent(i)] > heap[i]) {
            swap(heap[parent(i)], heap[i]);
            i = parent(i);  // Move up to parent
        }
    }

    // -------------------------------------------------------------------
    // HEAPIFY DOWN (Bubble Down / Sift Down): O(log n) time.
    //
    // Used after EXTRACTION (pop). Move root down by swapping with the
    // SMALLER child (to maintain min-heap property: parent <= children).
    // -------------------------------------------------------------------
    void heapifyDown(int i) {
        int smallest = i;           // Assume current node is smallest
        int left = leftChild(i);
        int right = rightChild(i);

        // Find the smallest among node, left child, right child
        if (left < heap.size() && heap[left] < heap[smallest])
            smallest = left;
        if (right < heap.size() && heap[right] < heap[smallest])
            smallest = right;

        // If smallest is not current node, swap and continue down
        if (smallest != i) {
            swap(heap[i], heap[smallest]);
            heapifyDown(smallest);  // Recursively heapify the affected subtree
        }
    }

public:
    MinHeap() {}

    // -------------------------------------------------------------------
    // BUILD HEAP from existing array: O(n) time (NOT O(n log n)!)
    //
    // Start from the last non-leaf node and heapifyDown each going upward.
    // This is more efficient than inserting n elements one by one (O(n log n)).
    //
    // Why O(n)? The number of nodes at height h is at most n/2^(h+1),
    // and heapifyDown at height h takes O(h). Summing gives O(n).
    // -------------------------------------------------------------------
    void buildHeap(const vector<int>& arr) {
        heap = arr;
        int n = heap.size();
        // Last non-leaf node index = n/2 - 1
        for (int i = n / 2 - 1; i >= 0; i--) {
            heapifyDown(i);
        }
    }

    // -------------------------------------------------------------------
    // INSERT (push): Add new element. O(log n) time.
    //
    // 1. Add element at the END (maintains complete tree property)
    // 2. Heapify up to restore heap property
    // -------------------------------------------------------------------
    void push(int val) {
        heap.push_back(val);     // Add at the end
        heapifyUp(heap.size() - 1);  // Restore heap property upward
    }

    // -------------------------------------------------------------------
    // EXTRACT MIN (pop): Remove and return the smallest element. O(log n).
    //
    // 1. Replace root with the last element (swap then pop_back)
    // 2. Heapify down from root to restore heap property
    // -------------------------------------------------------------------
    int pop() {
        if (isEmpty()) {
            cout << "Heap is empty!\n";
            return -1;
        }
        int minVal = heap[0];           // Root is the minimum
        heap[0] = heap.back();          // Move last element to root
        heap.pop_back();                // Remove last element
        if (!heap.empty()) {
            heapifyDown(0);             // Restore heap property
        }
        return minVal;
    }

    // -------------------------------------------------------------------
    // PEEK (top): View minimum without removing. O(1).
    // -------------------------------------------------------------------
    int top() {
        if (isEmpty()) {
            cout << "Heap is empty!\n";
            return -1;
        }
        return heap[0];
    }

    bool isEmpty() { return heap.empty(); }
    int size() { return heap.size(); }

    void display() {
        cout << "  Heap array: ";
        for (int val : heap) cout << val << " ";
        cout << "\n";
    }
};

// ============================================================================
// SECTION 2: MAX HEAP Implementation
//
// Same as MinHeap but comparison operators are reversed.
// Parent >= children (root is the MAXIMUM element).
// ============================================================================

class MaxHeap {
private:
    vector<int> heap;

    int parent(int i) { return (i - 1) / 2; }
    int leftChild(int i) { return 2 * i + 1; }
    int rightChild(int i) { return 2 * i + 2; }

    void heapifyUp(int i) {
        // Note: condition is flipped (parent < current for max heap)
        while (i > 0 && heap[parent(i)] < heap[i]) {
            swap(heap[parent(i)], heap[i]);
            i = parent(i);
        }
    }

    void heapifyDown(int i) {
        int largest = i;  // Track LARGEST instead of smallest
        int left = leftChild(i);
        int right = rightChild(i);

        if (left < heap.size() && heap[left] > heap[largest])
            largest = left;
        if (right < heap.size() && heap[right] > heap[largest])
            largest = right;

        if (largest != i) {
            swap(heap[i], heap[largest]);
            heapifyDown(largest);
        }
    }

public:
    MaxHeap() {}

    void buildHeap(const vector<int>& arr) {
        heap = arr;
        int n = heap.size();
        for (int i = n / 2 - 1; i >= 0; i--) {
            heapifyDown(i);
        }
    }

    void push(int val) {
        heap.push_back(val);
        heapifyUp(heap.size() - 1);
    }

    int pop() {
        if (isEmpty()) return -1;
        int maxVal = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        if (!heap.empty()) heapifyDown(0);
        return maxVal;
    }

    int top() {
        if (isEmpty()) return -1;
        return heap[0];
    }

    bool isEmpty() { return heap.empty(); }
    int size() { return heap.size(); }

    void display() {
        cout << "  Heap array: ";
        for (int val : heap) cout << val << " ";
        cout << "\n";
    }
};

// ============================================================================
// SECTION 3: APPLICATIONS OF HEAP
// ============================================================================

// -------------------------------------------------------------------
// APPLICATION 1: Heap Sort using Max Heap. O(n log n) time, O(1) extra space.
// (In-place version detailed in 05_sorting.cpp)
// Here's a version using our MaxHeap class (not in-place, uses extra space).
// -------------------------------------------------------------------
vector<int> heapSort(const vector<int>& arr) {
    MaxHeap heap;
    heap.buildHeap(arr);  // Build max heap from array

    vector<int> sorted;
    // Extract max repeatedly (pop gives elements in descending order)
    // To get ascending, we use MaxHeap and reverse
    while (!heap.isEmpty()) {
        sorted.push_back(heap.pop());
    }
    reverse(sorted.begin(), sorted.end());  // Reverse: descending -> ascending
    return sorted;
}

// -------------------------------------------------------------------
// APPLICATION 2: Kth Smallest/Largest Element. O(k log n) or O(n log k).
//
// Approach using max heap for kth smallest:
//   Build max heap of first k elements, then for each remaining element:
//   if element < heap.top(), pop and push new element.
//   After processing all, heap.top() is the kth smallest.
// -------------------------------------------------------------------
int kthSmallest(const vector<int>& arr, int k) {
    // Max heap to track k smallest elements seen so far
    priority_queue<int> maxHeap;  // STL priority_queue is MAX heap by default

    for (int num : arr) {
        maxHeap.push(num);
        // Keep size at most k; when size > k, remove the largest (which is top)
        if ((int)maxHeap.size() > k) {
            maxHeap.pop();  // Removes the largest among current k+1 elements
        }
    }
    // Top of max heap with k elements = kth smallest
    return maxHeap.top();
}

// -------------------------------------------------------------------
// APPLICATION 3: Merge K Sorted Arrays
// Use a min heap to always pick the smallest among all arrays.
// -------------------------------------------------------------------
vector<int> mergeKSortedArrays(const vector<vector<int>>& arrays) {
    // Min-heap stores {value, {arrayIndex, elementIndex}}
    using HeapNode = pair<int, pair<int, int>>;
    priority_queue<HeapNode, vector<HeapNode>, greater<HeapNode>> minHeap;

    int k = arrays.size();
    vector<int> result;

    // Push first element of each array into min heap
    for (int i = 0; i < k; i++) {
        if (!arrays[i].empty()) {
            minHeap.push({arrays[i][0], {i, 0}});
        }
    }

    while (!minHeap.empty()) {
        pair<int, pair<int, int>> top = minHeap.top();
        int val = top.first;
        int arrIdx = top.second.first;
        int elemIdx = top.second.second;
        minHeap.pop();

        result.push_back(val);

        // Push next element from the same array
        if (elemIdx + 1 < (int)arrays[arrIdx].size()) {
            minHeap.push({arrays[arrIdx][elemIdx + 1], {arrIdx, elemIdx + 1}});
        }
    }
    return result;
}

// ============================================================================
// SECTION 4: STL HEAP OPERATIONS
// ============================================================================

// The C++ STL provides heap operations in <algorithm>:
//   make_heap()  - convert range to heap
//   push_heap()  - add element to heap
//   pop_heap()   - remove largest from heap
//   sort_heap()  - sort the heap
//
// By default, STL creates MAX heaps. Use greater<>() for min heap.

void stlHeapDemo() {
    vector<int> v = {10, 20, 30, 5, 15};

    cout << "  Original: ";
    for (int x : v) cout << x << " ";

    // Convert to max heap
    make_heap(v.begin(), v.end());
    cout << "\n  After make_heap: ";
    for (int x : v) cout << x << " ";
    cout << " (max = " << v.front() << ")";

    // Push new element
    v.push_back(25);
    push_heap(v.begin(), v.end());
    cout << "\n  After push 25: ";
    for (int x : v) cout << x << " ";

    // Pop largest
    pop_heap(v.begin(), v.end());  // Moves largest to v.back()
    cout << "\n  Popped: " << v.back();
    v.pop_back();
    cout << "\n  After pop: ";
    for (int x : v) cout << x << " ";

    // Sort heap
    sort_heap(v.begin(), v.end());
    cout << "\n  After sort_heap: ";
    for (int x : v) cout << x << " ";
    cout << "\n";
}

// ============================================================================
// SAMPLE main() to demonstrate all heap operations
// ============================================================================
int main() {
    cout << "========== HEAP (10_heap.cpp) ==========\n\n";

    cout << "===== MIN HEAP =====\n";
    MinHeap minHeap;
    minHeap.buildHeap({10, 5, 3, 2, 8, 15});
    minHeap.display();
    cout << "  Minimum: " << minHeap.top() << "\n";
    cout << "  Pop min: " << minHeap.pop() << "\n";
    minHeap.display();
    minHeap.push(1);
    cout << "  After push(1):\n";
    minHeap.display();
    cout << "  New min: " << minHeap.top() << "\n";

    cout << "\n===== MAX HEAP =====\n";
    MaxHeap maxHeap;
    maxHeap.buildHeap({3, 1, 6, 5, 2, 4});
    maxHeap.display();
    cout << "  Maximum: " << maxHeap.top() << "\n";
    cout << "  Pop max: " << maxHeap.pop() << "\n";
    maxHeap.display();

    cout << "\n===== HEAP APPLICATIONS =====\n";

    // Kth Smallest
    vector<int> arr = {7, 10, 4, 3, 20, 15};
    int k = 3;
    cout << "  Array: ";
    for (int x : arr) cout << x << " ";
    cout << "\n  " << k << "rd smallest: " << kthSmallest(arr, k) << "\n";

    // Merge K sorted arrays
    vector<vector<int>> sortedArrays = {
        {1, 4, 7},
        {2, 5, 8},
        {3, 6, 9}
    };
    auto merged = mergeKSortedArrays(sortedArrays);
    cout << "  Merged K sorted arrays: ";
    for (int x : merged) cout << x << " ";
    cout << "\n";

    // Heap Sort
    vector<int> sortArr = {12, 11, 13, 5, 6, 7};
    auto sorted = heapSort(sortArr);
    cout << "  Heap Sort: ";
    for (int x : sorted) cout << x << " ";
    cout << "\n";

    cout << "\n===== STL HEAP DEMO =====\n";
    stlHeapDemo();

    cout << "\n========== END OF HEAP OPERATIONS ==========\n";

    return 0;
}
