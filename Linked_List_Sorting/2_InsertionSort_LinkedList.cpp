/*
 * Algorithm: Insertion Sort on a Singly Linked List
 * Description: Iterates through the list, removing one element at a time and finding its correct place in a new sorted list.
 * 
 * Time Complexity:
 *  - Best Case: O(n) (When already sorted)
 *  - Average/Worst Case: O(n^2)
 * 
 * Space (Memory) Complexity: O(1)
 */

#include <iostream>
using namespace std;

// Node definition
struct Node {
    int data;
    Node* next;
    Node(int val) : data(val), next(nullptr) {}
};

// Function to insert a node in a sorted way
void sortedInsert(Node** head_ref, Node* new_node) {
    Node* current;
    // Special case for the head end
    if (*head_ref == nullptr || (*head_ref)->data >= new_node->data) {
        new_node->next = *head_ref;
        *head_ref = new_node;
    } else {
        // Locate the node before the point of insertion
        current = *head_ref;
        while (current->next != nullptr && current->next->data < new_node->data) {
            current = current->next;
        }
        new_node->next = current->next;
        current->next = new_node;
    }
}

// Function to sort a singly linked list using insertion sort
void insertionSort(Node** head_ref) {
    Node* sorted = nullptr;
    Node* current = *head_ref;

    // Traverse the given linked list and insert every node to sorted
    while (current != nullptr) {
        // Store next for next iteration
        Node* next = current->next;

        // Insert current in sorted linked list
        sortedInsert(&sorted, current);

        // Update current
        current = next;
    }

    // Update head_ref to point to sorted linked list
    *head_ref = sorted;
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
    Node* a = nullptr;

    // Created list will be: 30 -> 3 -> 4 -> 20 -> 5 -> NULL
    push(&a, 5);
    push(&a, 20);
    push(&a, 4);
    push(&a, 3);
    push(&a, 30);

    cout << "Unsorted Linked List: \n";
    printList(a);

    insertionSort(&a);

    cout << "\nSorted Linked List using Insertion Sort: \n";
    printList(a);

    return 0;
}
