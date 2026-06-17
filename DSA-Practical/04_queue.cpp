/*
 * ============================================================================
 * FILE: 04_queue.cpp
 * ============================================================================
 * QUEUE - First-In-First-Out (FIFO) data structure.
 *
 * EXAM FREQUENCY: High. Queues appear regularly in practical exams.
 *
 * Key operations: enqueue (add to rear), dequeue (remove from front),
 *                 front/peek, isEmpty, isFull.
 *
 * Variants you MUST know:
 *   1. Linear Queue (using array) - simple but wastes space
 *   2. Circular Queue (using array) - efficient, reuses space
 *   3. Queue using Linked List - dynamic size, no overflow
 *   4. Deque (Double-ended queue)
 *   5. Priority Queue (using heap - covered in separate file)
 *
 * Common exam applications:
 *   - Implement queues using stacks
 *   - Implement stacks using queues
 *   - Generate binary numbers from 1 to N
 *   - Reverse a queue
 *   - First non-repeating character in a stream
 * ============================================================================
 */
#include <iostream>
#include <queue>    // STL queue
#include <stack>    // For reversing queue and implementing stack using queues
#include <string>
#include <vector>
using namespace std;

// ============================================================================
// SECTION 1: LINEAR QUEUE USING ARRAY (Simple but Wastes Space)
// ============================================================================

class LinearQueue {
private:
    int* arr;
    int front;     // Index of the front element (dequeue from here)
    int rear;      // Index of the rear element (enqueue at here)
    int capacity;  // Maximum number of elements

public:
    LinearQueue(int size) {
        arr = new int[size];
        capacity = size;
        front = 0;
        rear = -1;  // Queue is empty when rear < front
    }

    ~LinearQueue() { delete[] arr; }

    // -------------------------------------------------------------------
    // ENQUEUE: Add element to rear. O(1) time.
    // -------------------------------------------------------------------
    void enqueue(int val) {
        if (isFull()) {
            cout << "Queue Overflow!\n";
            return;
        }
        arr[++rear] = val;  // Increment rear and insert
    }

    // -------------------------------------------------------------------
    // DEQUEUE: Remove element from front. O(1) time.
    // Problem: front shifts right, left space is wasted.
    // -------------------------------------------------------------------
    int dequeue() {
        if (isEmpty()) {
            cout << "Queue Underflow!\n";
            return -1;
        }
        return arr[front++];  // Return front and move front forward
    }

    int peek() {
        if (isEmpty()) return -1;
        return arr[front];
    }

    // Queue is empty when front has passed rear
    bool isEmpty() { return front > rear; }
    bool isFull()  { return rear == capacity - 1; }
    int size()     { return rear - front + 1; }

    void display() {
        if (isEmpty()) {
            cout << "Queue is empty\n";
            return;
        }
        cout << "Queue [front -> rear]: ";
        for (int i = front; i <= rear; i++) {
            cout << arr[i] << " ";
        }
        cout << "\n";
    }
};

// ============================================================================
// SECTION 2: CIRCULAR QUEUE USING ARRAY (Efficient, No Space Waste)
// ============================================================================

/*
 * KEY CONCEPT: In a circular queue, front and rear wrap around using modulo.
 * When rear reaches the end, it wraps to index 0 (if front has moved ahead).
 * This reuses the space that linear queue wastes.
 *
 * Two approaches to distinguish full vs empty:
 *   Approach A (used here): Keep one slot empty. (rear+1) % cap == front means full.
 *   Approach B: Use a separate 'size' counter variable.
 */

class CircularQueue {
private:
    int* arr;
    int front;
    int rear;
    int capacity;

public:
    CircularQueue(int size) {
        arr = new int[size];
        capacity = size;
        front = 0;
        rear = 0;  // Both start at 0; queue is empty when front == rear
    }

    ~CircularQueue() { delete[] arr; }

    // -------------------------------------------------------------------
    // ENQUEUE: O(1) time.
    // Check if next position is free (not wrapping onto front).
    // -------------------------------------------------------------------
    void enqueue(int val) {
        if (isFull()) {
            cout << "Circular Queue Overflow!\n";
            return;
        }
        arr[rear] = val;          // Insert at current rear
        rear = (rear + 1) % capacity;  // Move rear forward (wrap with %)
    }

    // -------------------------------------------------------------------
    // DEQUEUE: O(1) time.
    // -------------------------------------------------------------------
    int dequeue() {
        if (isEmpty()) {
            cout << "Circular Queue Underflow!\n";
            return -1;
        }
        int val = arr[front];          // Get front value
        front = (front + 1) % capacity; // Move front forward (wrap with %)
        return val;
    }

    int peek() {
        if (isEmpty()) return -1;
        return arr[front];
    }

    // Queue is empty when front and rear point to same position
    bool isEmpty() { return front == rear; }
    // Queue is full when next position of rear wraps to front
    // (We sacrifice one slot to distinguish full vs empty)
    bool isFull()  { return (rear + 1) % capacity == front; }
    int size()     { return (rear - front + capacity) % capacity; }

    void display() {
        if (isEmpty()) {
            cout << "Circular Queue is empty\n";
            return;
        }
        cout << "Circular Queue [front -> rear]: ";
        int i = front;
        while (i != rear) {
            cout << arr[i] << " ";
            i = (i + 1) % capacity;
        }
        cout << "\n";
    }
};

// ============================================================================
// SECTION 3: QUEUE USING LINKED LIST (Dynamic Size)
// ============================================================================

struct QNode {
    int data;
    QNode* next;
    QNode(int val) : data(val), next(nullptr) {}
};

class QueueLinkedList {
private:
    QNode* frontNode;  // Delete from here (dequeue)
    QNode* rearNode;   // Insert from here (enqueue)

public:
    QueueLinkedList() : frontNode(nullptr), rearNode(nullptr) {}

    ~QueueLinkedList() {
        while (frontNode != nullptr) {
            QNode* temp = frontNode;
            frontNode = frontNode->next;
            delete temp;
        }
    }

    // -------------------------------------------------------------------
    // ENQUEUE: Insert at rear. O(1) time.
    // -------------------------------------------------------------------
    void enqueue(int val) {
        QNode* newNode = new QNode(val);
        if (rearNode == nullptr) {
            // First node: both front and rear point to it
            frontNode = rearNode = newNode;
            return;
        }
        rearNode->next = newNode;  // Attach to end
        rearNode = newNode;        // Update rear
    }

    // -------------------------------------------------------------------
    // DEQUEUE: Remove from front. O(1) time.
    // -------------------------------------------------------------------
    int dequeue() {
        if (isEmpty()) {
            cout << "Queue Underflow!\n";
            return -1;
        }
        QNode* temp = frontNode;
        int val = temp->data;
        frontNode = frontNode->next;
        // If queue becomes empty, reset rear to nullptr too
        if (frontNode == nullptr) {
            rearNode = nullptr;
        }
        delete temp;
        return val;
    }

    int peek() {
        if (isEmpty()) return -1;
        return frontNode->data;
    }

    bool isEmpty() { return frontNode == nullptr; }

    void display() {
        if (isEmpty()) {
            cout << "Queue is empty\n";
            return;
        }
        QNode* temp = frontNode;
        cout << "Queue [front -> rear]: ";
        while (temp != nullptr) {
            cout << temp->data << " ";
            temp = temp->next;
        }
        cout << "\n";
    }
};

// ============================================================================
// SECTION 4: DEQUE (Double-Ended Queue) using STL
// ============================================================================

/*
 * Deque allows insertion and deletion at BOTH ends.
 * STL provides deque container which supports:
 *   push_front(), push_back(), pop_front(), pop_back(), front(), back()
 *
 * Common exam problems:
 *   1. Sliding Window Maximum - O(n) using deque
 *   2. Implement deque with array/linked list
 */

// -------------------------------------------------------------------
// PROBLEM: Sliding Window Maximum using Deque. O(n) time.
// Given an array and window size k, find maximum in each window.
// Deque stores indices of useful elements in decreasing order.
// -------------------------------------------------------------------
vector<int> slidingWindowMax(const vector<int>& arr, int k) {
    deque<int> dq;  // Stores indices of elements (not values)
    vector<int> result;

    for (size_t i = 0; i < arr.size(); i++) {
        // Step 1: Remove indices outside the current window (from front)
        if (!dq.empty() && dq.front() <= (int)(i - k)) {
            dq.pop_front();
        }
        // Step 2: Maintain decreasing order in deque
        // Remove indices of elements smaller than current (from back)
        while (!dq.empty() && arr[dq.back()] < arr[i]) {
            dq.pop_back();
        }
        dq.push_back(i);  // Add current index

        // Step 3: The front of deque is the max of this window
        // Start adding to result once we have a full window (i >= k-1)
        if (i >= (size_t)k - 1) {
            result.push_back(arr[dq.front()]);
        }
    }
    return result;
}

// ============================================================================
// SECTION 5: QUEUE APPLICATIONS
// ============================================================================

// -------------------------------------------------------------------
// APPLICATION 1: Implement Stack using Two Queues
// push(x): enqueue to q2, move all from q1 to q2, swap q1 and q2
// pop(): dequeue from q1
// -------------------------------------------------------------------

class StackUsingQueues {
private:
    queue<int> q1, q2;

public:
    // push is O(n) - we want the newest element at front of queue
    void push(int x) {
        q2.push(x);                // Add new element to q2
        while (!q1.empty()) {      // Move all elements from q1 to q2
            q2.push(q1.front());
            q1.pop();
        }
        swap(q1, q2);  // q1 now has newest element at front (like stack's top)
    }

    void pop() {
        if (!q1.empty()) q1.pop();
    }

    int top() { return q1.front(); }
    bool empty() { return q1.empty(); }
};

// -------------------------------------------------------------------
// APPLICATION 2: Generate Binary Numbers from 1 to N
// Use queue to generate in order (like BFS). O(n) time.
// Algorithm: enqueue "1", then repeatedly dequeue and append "0" and "1".
// -------------------------------------------------------------------
vector<string> generateBinaryNumbers(int n) {
    vector<string> result;
    queue<string> q;
    q.push("1");  // First binary number

    for (int i = 1; i <= n; i++) {
        string curr = q.front();
        q.pop();
        result.push_back(curr);
        // Append '0' and '1' to generate next binary numbers
        q.push(curr + "0");
        q.push(curr + "1");
    }
    return result;
}

// -------------------------------------------------------------------
// APPLICATION 3: Reverse a Queue using Stack. O(n) time.
// -------------------------------------------------------------------
void reverseQueue(queue<int>& q) {
    stack<int> st;
    // Transfer all elements from queue to stack
    while (!q.empty()) {
        st.push(q.front());
        q.pop();
    }
    // Transfer back from stack to queue (stack reverses the order)
    while (!st.empty()) {
        q.push(st.top());
        st.pop();
    }
}

// ============================================================================
// SAMPLE main() to demonstrate all queue operations
// ============================================================================
int main() {
    cout << "========== LINEAR QUEUE (Array) ==========\n";
    LinearQueue lq(5);
    lq.enqueue(10); lq.enqueue(20); lq.enqueue(30);
    lq.display();
    cout << "Dequeued: " << lq.dequeue() << "\n";
    lq.display();

    cout << "\n========== CIRCULAR QUEUE (Array) ==========\n";
    CircularQueue cq(5);
    cq.enqueue(1); cq.enqueue(2); cq.enqueue(3);
    cq.display();
    cout << "Dequeued: " << cq.dequeue() << "\n";
    cq.enqueue(4); cq.enqueue(5);
    cq.display();
    cout << "Size: " << cq.size() << "\n";

    cout << "\n========== QUEUE USING LINKED LIST ==========\n";
    QueueLinkedList qll;
    qll.enqueue(100); qll.enqueue(200); qll.enqueue(300);
    qll.display();
    cout << "Front: " << qll.peek() << "\n";
    cout << "Dequeued: " << qll.dequeue() << "\n";
    qll.display();

    cout << "\n========== SLIDING WINDOW MAXIMUM ==========\n";
    vector<int> arr = {1, 3, -1, -3, 5, 3, 6, 7};
    int k = 3;
    vector<int> result = slidingWindowMax(arr, k);
    cout << "Array: [1, 3, -1, -3, 5, 3, 6, 7], k=" << k << "\n";
    cout << "Window maximums: ";
    for (int x : result) cout << x << " ";
    cout << "\n";

    cout << "\n========== GENERATE BINARY NUMBERS ==========\n";
    vector<string> binaryNums = generateBinaryNumbers(5);
    cout << "Binary numbers from 1 to 5: ";
    for (const string& s : binaryNums) cout << s << " ";
    cout << "\n";

    cout << "\n========== REVERSE QUEUE ==========\n";
    queue<int> q;
    for (int i = 1; i <= 5; i++) q.push(i * 10);
    cout << "Original: ";
    queue<int> temp = q;
    while (!temp.empty()) { cout << temp.front() << " "; temp.pop(); }
    cout << "\n";
    reverseQueue(q);
    cout << "Reversed: ";
    while (!q.empty()) { cout << q.front() << " "; q.pop(); }
    cout << "\n";

    cout << "\n========== END OF QUEUE OPERATIONS ==========\n";

    return 0;
}
