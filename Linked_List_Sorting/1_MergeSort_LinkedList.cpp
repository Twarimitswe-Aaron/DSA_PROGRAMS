/*
 * Algorithm: Merge Sort on a Singly Linked List
 * Description: Recursively divides the linked list into two halves using the slow/fast pointer technique, sorts them, and then merges the sorted halves.
 * 
 * Time Complexity:
 *  - Best, Average, Worst Case: O(n log n)
 * 
 * Space (Memory) Complexity: O(log n) due to recursive call stack. Unlike array merge sort, this requires O(1) auxiliary space.
 */

#include <iostream>
using namespace std;

// Node definition
struct Node {
    int data;
    Node* next;
    Node(int val) : data(val), next(nullptr) {}
};

// Function to merge two sorted linked lists
Node* merge(Node* left, Node* right) {
    if (!left) return right;
    if (!right) return left;

    Node* result = nullptr;

    // Pick either left or right, and recurse
    if (left->data <= right->data) {
        result = left;
        result->next = merge(left->next, right);
    } else {
        result = right;
        result->next = merge(left, right->next);
    }
    return result;
}

// Function to find the middle of the linked list
void splitList(Node* source, Node** frontRef, Node** backRef) {
    Node* fast;
    Node* slow;
    slow = source;
    fast = source->next;

    // Advance 'fast' two nodes, and advance 'slow' one node
    while (fast != nullptr) {
        fast = fast->next;
        if (fast != nullptr) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    // 'slow' is before the midpoint in the list, so split it in two at that point
    *frontRef = source;
    *backRef = slow->next;
    slow->next = nullptr;
}

// Main MergeSort function
void mergeSort(Node** headRef) {
    Node* head = *headRef;
    Node* a;
    Node* b;

    // Base case: length 0 or 1
    if ((head == nullptr) || (head->next == nullptr)) {
        return;
    }

    // Split head into 'a' and 'b' sublists
    splitList(head, &a, &b);

    // Recursively sort the sublists
    mergeSort(&a);
    mergeSort(&b);

    // Answer is the merged sorted lists
    *headRef = merge(a, b);
}

// Helper function to insert a node at the beginning
void push(Node** headRef, int new_data) {
    Node* newNode = new Node(new_data);
    newNode->next = *headRef;
    *headRef = newNode;
}

// Helper function to print the linked list
void printList(Node* node) {
    while (node != nullptr) {
        cout << node->data << " -> ";
        node = node->next;
    }
    cout << "NULL\n";
}

int main() {
    Node* res = nullptr;
    Node* a = nullptr;

    // Let us create an unsorted linked list to test the function
    // Created list will be: 15 -> 10 -> 5 -> 20 -> 3 -> 2 -> NULL
    push(&a, 2);
    push(&a, 3);
    push(&a, 20);
    push(&a, 5);
    push(&a, 10);
    push(&a, 15);

    cout << "Unsorted Linked List: \n";
    printList(a);

    // Sort the list
    mergeSort(&a);

    cout << "\nSorted Linked List using Merge Sort: \n";
    printList(a);

    return 0;
}
