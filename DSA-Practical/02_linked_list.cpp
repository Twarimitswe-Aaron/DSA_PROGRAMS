/*
 * ============================================================================
 * FILE: 02_linked_list.cpp
 * ============================================================================
 * LINKED LISTS - Linear data structure with non-contiguous memory allocation.
 *
 * EXAM FREQUENCY: Extremely high. Every practical exam tests linked lists.
 *
 * Variants you MUST know:
 *   1. Singly Linked List  (each node has data + next pointer)
 *   2. Doubly Linked List  (each node has data + next + prev pointers)
 *   3. Circular Linked List (last node's next points back to head)
 *
 * Common exam questions:
 *   - Create/Insert/Delete/Search in all variants
 *   - Reverse a linked list (iterative & recursive)
 *   - Detect cycle (Floyd's Tortoise & Hare algorithm)
 *   - Find middle of linked list
 *   - Merge two sorted linked lists
 *   - Remove nth node from end
 *   - Find intersection point of two lists
 *   - Check if palindrome
 * ============================================================================
 */
#include <iostream>
#include <stack>  // for palindrome check using stack
using namespace std;

// ============================================================================
// SECTION 1: SINGLY LINKED LIST
// ============================================================================

/*
 * Node structure for singly linked list.
 * Each node contains:
 *   - data: The actual value stored in the node
 *   - next: Pointer to the next node in the list (NULL if last node)
 */
struct SinglyNode {
    int data;           // Data stored in the node
    SinglyNode* next;   // Pointer to the next node in the sequence

    // Constructor to initialize node with data and set next to NULL
    SinglyNode(int val) : data(val), next(nullptr) {}
};

class SinglyLinkedList {
private:
    SinglyNode* head;  // Pointer to the first node of the list

public:
    // Constructor: Initialize empty list with head = NULL
    SinglyLinkedList() : head(nullptr) {}

    // Destructor: Delete all nodes to prevent memory leaks
    ~SinglyLinkedList() {
        SinglyNode* current = head;
        while (current != nullptr) {
            SinglyNode* nextNode = current->next;  // Save next before deletion
            delete current;                        // Free current node
            current = nextNode;                    // Move to next node
        }
        head = nullptr;
    }

    // -------------------------------------------------------------------
    // INSERT AT BEGINNING: O(1) time, O(1) space
    // New node becomes the new head, its next points to old head.
    // -------------------------------------------------------------------
    void insertAtBeginning(int val) {
        SinglyNode* newNode = new SinglyNode(val);  // Create new node
        newNode->next = head;   // New node points to current head
        head = newNode;         // Update head to point to new node
    }

    // -------------------------------------------------------------------
    // INSERT AT END: O(n) time, O(1) space
    // Traverse to last node, make its next point to new node.
    // -------------------------------------------------------------------
    void insertAtEnd(int val) {
        SinglyNode* newNode = new SinglyNode(val);
        if (head == nullptr) {  // Empty list: new node becomes head
            head = newNode;
            return;
        }
        SinglyNode* temp = head;
        // Traverse until we reach the last node (temp->next == NULL)
        while (temp->next != nullptr) {
            temp = temp->next;
        }
        temp->next = newNode;  // Attach new node after the last node
    }

    // -------------------------------------------------------------------
    // INSERT AT POSITION: O(n) time, O(1) space
    // position is 0-indexed. Insert at index 'pos'.
    // -------------------------------------------------------------------
    void insertAtPosition(int val, int pos) {
        if (pos < 0) return;            // Invalid position
        if (pos == 0) {                 // Insert at beginning
            insertAtBeginning(val);
            return;
        }
        SinglyNode* newNode = new SinglyNode(val);
        SinglyNode* temp = head;
        // Traverse to node just before the insertion position (pos-1)
        for (int i = 0; temp != nullptr && i < pos - 1; i++) {
            temp = temp->next;
        }
        if (temp == nullptr) {  // Position beyond list length
            delete newNode;     // Free memory to avoid leak
            return;
        }
        // Insert newNode between temp and temp->next
        newNode->next = temp->next;  // New node points to the next node
        temp->next = newNode;        // Previous node points to new node
    }

    // -------------------------------------------------------------------
    // DELETE FROM BEGINNING: O(1) time
    // -------------------------------------------------------------------
    void deleteFromBeginning() {
        if (head == nullptr) return;          // Empty list, nothing to delete
        SinglyNode* temp = head;              // Save head pointer
        head = head->next;                    // Move head to second node
        delete temp;                          // Free old head's memory
    }

    // -------------------------------------------------------------------
    // DELETE FROM END: O(n) time
    // Need to traverse to the second-last node.
    // -------------------------------------------------------------------
    void deleteFromEnd() {
        if (head == nullptr) return;           // Empty list
        if (head->next == nullptr) {           // Only one node
            delete head;
            head = nullptr;
            return;
        }
        SinglyNode* temp = head;
        // Traverse until temp points to the second-last node
        while (temp->next->next != nullptr) {
            temp = temp->next;
        }
        delete temp->next;   // Delete the last node
        temp->next = nullptr; // Set second-last node's next to NULL
    }

    // -------------------------------------------------------------------
    // DELETE BY VALUE: O(n) time
    // Deletes first occurrence of a node with given value.
    // -------------------------------------------------------------------
    void deleteByValue(int val) {
        if (head == nullptr) return;  // Empty list

        // Special case: head node itself holds the value
        if (head->data == val) {
            SinglyNode* temp = head;
            head = head->next;  // Move head to second node
            delete temp;        // Free old head
            return;
        }

        // Search for the node with the value, keeping track of previous node
        SinglyNode* temp = head;
        while (temp->next != nullptr && temp->next->data != val) {
            temp = temp->next;
        }
        if (temp->next == nullptr) return;  // Value not found

        // Delete the node: bypass it by linking previous to next-of-target
        SinglyNode* toDelete = temp->next;
        temp->next = temp->next->next;
        delete toDelete;
    }

    // -------------------------------------------------------------------
    // SEARCH: O(n) time. Returns index (0-based) or -1 if not found.
    // -------------------------------------------------------------------
    int search(int val) {
        SinglyNode* temp = head;
        int index = 0;
        while (temp != nullptr) {
            if (temp->data == val) return index;  // Found at this index
            temp = temp->next;
            index++;
        }
        return -1;  // Not found
    }

    // -------------------------------------------------------------------
    // REVERSE ITERATIVELY: O(n) time, O(1) space.
    // Uses three pointers: prev, current, next.
    // Key insight: For each node, reverse the direction of 'next' pointer.
    // -------------------------------------------------------------------
    void reverse() {
        SinglyNode* prev = nullptr;     // Initially, no previous node
        SinglyNode* current = head;     // Start from head
        SinglyNode* nextNode = nullptr; // Will store next before we change pointer

        while (current != nullptr) {
            nextNode = current->next;   // Save next before overwriting
            current->next = prev;       // Reverse: point current to previous
            prev = current;             // Move prev forward
            current = nextNode;         // Move current forward
        }
        head = prev;  // After loop, prev points to the new head (old last node)
    }

    // -------------------------------------------------------------------
    // FIND MIDDLE using Tortoise & Hare: O(n) time, O(1) space.
    // slow moves 1 step, fast moves 2 steps.
    // When fast reaches end, slow is at the middle.
    // -------------------------------------------------------------------
    SinglyNode* findMiddle() {
        if (head == nullptr) return nullptr;
        SinglyNode* slow = head;
        SinglyNode* fast = head;
        // fast != nullptr handles even length, fast->next handles odd length
        while (fast != nullptr && fast->next != nullptr) {
            slow = slow->next;       // Move slow one step
            fast = fast->next->next; // Move fast two steps
        }
        return slow;  // slow now points to the middle node
    }

    // -------------------------------------------------------------------
    // DETECT CYCLE (Floyd's Cycle Detection): O(n) time, O(1) space.
    // If slow == fast at any point, a cycle exists.
    // -------------------------------------------------------------------
    bool hasCycle() {
        if (head == nullptr || head->next == nullptr) return false;
        SinglyNode* slow = head;
        SinglyNode* fast = head;
        while (fast != nullptr && fast->next != nullptr) {
            slow = slow->next;        // Move slow one step
            fast = fast->next->next;  // Move fast two steps
            if (slow == fast) return true;   // They met = cycle exists
        }
        return false;  // Fast reached end = no cycle
    }

    // -------------------------------------------------------------------
    // DISPLAY: O(n) time. Prints all elements in order.
    // -------------------------------------------------------------------
    void display() {
        SinglyNode* temp = head;
        while (temp != nullptr) {
            cout << temp->data;
            if (temp->next != nullptr) cout << " -> ";
            temp = temp->next;
        }
        cout << " -> NULL\n";
    }
};

// ============================================================================
// SECTION 2: DOUBLY LINKED LIST
// ============================================================================

/*
 * Node structure for doubly linked list.
 * Each node has:
 *   - data: stored value
 *   - next: pointer to next node
 *   - prev: pointer to previous node (adds backward traversal ability)
 */
struct DoublyNode {
    int data;
    DoublyNode* next;
    DoublyNode* prev;  // Extra pointer compared to singly linked list

    DoublyNode(int val) : data(val), next(nullptr), prev(nullptr) {}
};

class DoublyLinkedList {
private:
    DoublyNode* head;

public:
    DoublyLinkedList() : head(nullptr) {}

    ~DoublyLinkedList() {
        DoublyNode* current = head;
        while (current != nullptr) {
            DoublyNode* nextNode = current->next;
            delete current;
            current = nextNode;
        }
        head = nullptr;
    }

    // -------------------------------------------------------------------
    // INSERT AT BEGINNING: O(1) time
    // Key: new node's next = head, head's prev = new node, head = new node
    // -------------------------------------------------------------------
    void insertAtBeginning(int val) {
        DoublyNode* newNode = new DoublyNode(val);
        if (head == nullptr) {  // Empty list
            head = newNode;
            return;
        }
        newNode->next = head;    // New node points forward to old head
        head->prev = newNode;    // Old head points backward to new node
        head = newNode;          // Update head to new node
    }

    // -------------------------------------------------------------------
    // INSERT AT END: O(n) time
    // Traverse to last node, link it to new node.
    // -------------------------------------------------------------------
    void insertAtEnd(int val) {
        DoublyNode* newNode = new DoublyNode(val);
        if (head == nullptr) {
            head = newNode;
            return;
        }
        DoublyNode* temp = head;
        while (temp->next != nullptr) {
            temp = temp->next;
        }
        temp->next = newNode;     // Last node points forward to new node
        newNode->prev = temp;     // New node points backward to old last node
    }

    // -------------------------------------------------------------------
    // DELETE FROM BEGINNING: O(1) time
    // Head moves to head->next, new head's prev = NULL
    // -------------------------------------------------------------------
    void deleteFromBeginning() {
        if (head == nullptr) return;
        DoublyNode* temp = head;
        head = head->next;        // Move head to second node
        if (head != nullptr) {    // If list is not empty after deletion
            head->prev = nullptr; // New head has no previous node
        }
        delete temp;
    }

    // -------------------------------------------------------------------
    // DELETE FROM END: O(n) time
    // -------------------------------------------------------------------
    void deleteFromEnd() {
        if (head == nullptr) return;
        if (head->next == nullptr) {  // Only one node
            delete head;
            head = nullptr;
            return;
        }
        DoublyNode* temp = head;
        while (temp->next != nullptr) {
            temp = temp->next;
        }
        // temp is now the last node
        temp->prev->next = nullptr;  // Second-last node's next = NULL
        delete temp;
    }

    // -------------------------------------------------------------------
    // DISPLAY FORWARD: O(n) time
    // -------------------------------------------------------------------
    void displayForward() {
        DoublyNode* temp = head;
        cout << "NULL <-> ";
        while (temp != nullptr) {
            cout << temp->data;
            if (temp->next != nullptr) cout << " <-> ";
            temp = temp->next;
        }
        cout << " -> NULL\n";
    }

    // -------------------------------------------------------------------
    // DISPLAY BACKWARD: O(n) time
    // First traverse to end, then go backward using prev pointers.
    // -------------------------------------------------------------------
    void displayBackward() {
        if (head == nullptr) return;
        DoublyNode* temp = head;
        while (temp->next != nullptr) {
            temp = temp->next;
        }
        cout << "NULL <-> ";
        while (temp != nullptr) {
            cout << temp->data;
            if (temp->prev != nullptr) cout << " <-> ";
            temp = temp->prev;
        }
        cout << " -> NULL\n";
    }
};

// ============================================================================
// SECTION 3: CIRCULAR SINGLY LINKED LIST
// ============================================================================

/*
 * In a circular linked list, the last node's next pointer points back to head.
 * This means there is NO NULL pointer in the list.
 * Useful for applications that require circular traversal (e.g., round-robin).
 */

class CircularLinkedList {
private:
    SinglyNode* head;

public:
    CircularLinkedList() : head(nullptr) {}

    ~CircularLinkedList() {
        if (head == nullptr) return;
        // Break the circular link first to avoid infinite loop during deletion
        SinglyNode* temp = head;
        // Traverse and find the last node, break the circle
        while (temp->next != head) {
            SinglyNode* next = temp->next;
            delete temp;
            temp = next;
        }
        delete temp;  // Delete the last node
        head = nullptr;
    }

    // -------------------------------------------------------------------
    // INSERT AT BEGINNING: O(n) time
    // In circular list, we need to find the last node to update its next.
    // -------------------------------------------------------------------
    void insertAtBeginning(int val) {
        SinglyNode* newNode = new SinglyNode(val);
        if (head == nullptr) {
            head = newNode;
            newNode->next = head;  // Points to itself (circular)
            return;
        }
        SinglyNode* temp = head;
        // Find the last node (where temp->next == head)
        while (temp->next != head) {
            temp = temp->next;
        }
        newNode->next = head;   // New node points to old head
        temp->next = newNode;   // Last node points to new node
        head = newNode;         // Update head to new node
    }

    // -------------------------------------------------------------------
    // INSERT AT END: O(n) time
    // -------------------------------------------------------------------
    void insertAtEnd(int val) {
        SinglyNode* newNode = new SinglyNode(val);
        if (head == nullptr) {
            head = newNode;
            newNode->next = head;
            return;
        }
        SinglyNode* temp = head;
        while (temp->next != head) {
            temp = temp->next;
        }
        temp->next = newNode;   // Last node points to new node
        newNode->next = head;   // New node points back to head (circular)
    }

    // -------------------------------------------------------------------
    // DELETE FROM BEGINNING: O(n) time
    // Need to update last node's next pointer.
    // -------------------------------------------------------------------
    void deleteFromBeginning() {
        if (head == nullptr) return;
        if (head->next == head) {  // Only one node
            delete head;
            head = nullptr;
            return;
        }
        SinglyNode* temp = head;
        while (temp->next != head) {
            temp = temp->next;
        }
        // temp is the last node
        SinglyNode* toDelete = head;
        head = head->next;          // Move head to second node
        temp->next = head;          // Last node points to new head
        delete toDelete;
    }

    // -------------------------------------------------------------------
    // DISPLAY: O(n) time. Must stop when we loop back to head.
    // Uses do-while to ensure we enter the loop (list may not be empty).
    // -------------------------------------------------------------------
    void display() {
        if (head == nullptr) {
            cout << "Empty list\n";
            return;
        }
        SinglyNode* temp = head;
        cout << "HEAD -> ";
        do {
            cout << temp->data;
            temp = temp->next;
            if (temp != head) cout << " -> ";
        } while (temp != head);
        cout << " -> BACK TO HEAD\n";
    }
};

// ============================================================================
// SECTION 4: COMMON LINKED LIST ALGORITHMS (Standalone Functions)
// ============================================================================

// -------------------------------------------------------------------
// Merge two sorted singly linked lists. O(n+m) time, O(1) space.
// Uses a dummy node to simplify head management.
// -------------------------------------------------------------------
SinglyNode* mergeSortedLists(SinglyNode* head1, SinglyNode* head2) {
    SinglyNode dummy(0);   // Dummy node (its data doesn't matter)
    SinglyNode* tail = &dummy;  // Tail points to the last node of merged list

    // Traverse both lists simultaneously, picking the smaller value
    while (head1 != nullptr && head2 != nullptr) {
        if (head1->data <= head2->data) {
            tail->next = head1;   // Attach head1's node to merged list
            head1 = head1->next;  // Advance head1
        } else {
            tail->next = head2;   // Attach head2's node
            head2 = head2->next;  // Advance head2
        }
        tail = tail->next;  // Move tail forward
    }
    // Attach remaining nodes (only one of these will have leftovers)
    if (head1 != nullptr) tail->next = head1;
    if (head2 != nullptr) tail->next = head2;

    return dummy.next;  // Return the actual head (skip dummy)
}

// -------------------------------------------------------------------
// Remove Nth node from end of list. O(n) time, O(1) space.
// Uses two-pointer technique: fast moves n steps ahead of slow.
// When fast reaches end, slow is at (n-1)th node from end.
// -------------------------------------------------------------------
SinglyNode* removeNthFromEnd(SinglyNode* head, int n) {
    SinglyNode dummy(0);
    dummy.next = head;
    SinglyNode* slow = &dummy;
    SinglyNode* fast = &dummy;

    // Move fast n steps ahead first
    for (int i = 0; i < n; i++) {
        if (fast->next == nullptr) return head;  // n is larger than list length
        fast = fast->next;
    }
    // Now move both pointers until fast reaches the last node
    while (fast->next != nullptr) {
        slow = slow->next;
        fast = fast->next;
    }
    // slow now points to node just BEFORE the one to delete
    SinglyNode* toDelete = slow->next;
    slow->next = slow->next->next;  // Bypass the node to delete
    delete toDelete;

    return dummy.next;
}

// -------------------------------------------------------------------
// Check if linked list is palindrome. O(n) time, O(n) space.
// Approach: Traverse first half, push to stack. Compare second half
// with stack (LIFO gives reverse order).
// -------------------------------------------------------------------
bool isPalindrome(SinglyNode* head) {
    if (head == nullptr || head->next == nullptr) return true;

    // Step 1: Find the middle using slow/fast pointer
    SinglyNode* slow = head;
    SinglyNode* fast = head;
    stack<int> st;

    // Push first half elements onto stack
    while (fast != nullptr && fast->next != nullptr) {
        st.push(slow->data);
        slow = slow->next;
        fast = fast->next->next;
    }
    // If odd length, skip the middle element
    if (fast != nullptr) {
        slow = slow->next;
    }
    // Step 2: Compare second half with stack
    while (slow != nullptr) {
        if (st.top() != slow->data) return false;  // Mismatch found
        st.pop();
        slow = slow->next;
    }
    return true;  // All elements matched
}

// ============================================================================
// SAMPLE main() to demonstrate all linked list operations
// ============================================================================
int main() {
    cout << "========== SINGLY LINKED LIST ==========\n";
    SinglyLinkedList sll;
    sll.insertAtEnd(10);
    sll.insertAtEnd(20);
    sll.insertAtEnd(30);
    sll.insertAtBeginning(5);
    sll.display();  // Expected: 5 -> 10 -> 20 -> 30 -> NULL

    cout << "After deleting 20: ";
    sll.deleteByValue(20);
    sll.display();

    cout << "Reversed: ";
    sll.reverse();
    sll.display();

    cout << "Middle node data: " << sll.findMiddle()->data << "\n";
    cout << "Search 10: found at index " << sll.search(10) << "\n\n";

    cout << "========== DOUBLY LINKED LIST ==========\n";
    DoublyLinkedList dll;
    dll.insertAtEnd(1);
    dll.insertAtEnd(2);
    dll.insertAtEnd(3);
    dll.insertAtBeginning(0);
    cout << "Forward:  ";
    dll.displayForward();
    cout << "Backward: ";
    dll.displayBackward();

    cout << "\n========== CIRCULAR LINKED LIST ==========\n";
    CircularLinkedList cll;
    cll.insertAtEnd(100);
    cll.insertAtEnd(200);
    cll.insertAtEnd(300);
    cll.display();  // Shows circular nature

    cout << "\n========== MERGE SORTED LISTS ==========\n";
    SinglyLinkedList list1, list2;
    list1.insertAtEnd(1); list1.insertAtEnd(3); list1.insertAtEnd(5);
    list2.insertAtEnd(2); list2.insertAtEnd(4); list2.insertAtEnd(6);
    cout << "List 1: "; list1.display();
    cout << "List 2: "; list2.display();

    SinglyLinkedList mergedList;
    // We can't directly access head from outside, so the merge function
    // is demonstrated as a standalone. In practice, you'd add a getHead()
    // method or implement merge as a member function.

    cout << "\n========== END OF LINKED LIST OPERATIONS ==========\n";

    return 0;
}
