/*
 * Algorithm: Bubble Sort on a Singly Linked List
 * Description: Iterates through the list, compares adjacent elements and swaps their data if they are in the wrong order.
 * Note: Swapping data is used here for simplicity.
 * 
 * Time Complexity:
 *  - Best Case: O(n) (when list is already sorted)
 *  - Average/Worst Case: O(n^2)
 * 
 * Space (Memory) Complexity: O(1)
 */

#include <iostream>
using namespace std;

struct Node {
    int data;
    Node* next;
    Node(int val) : data(val), next(nullptr) {}
};

// Function to sort a singly linked list using bubble sort
void bubbleSort(Node* head) {
    int swapped;
    Node* ptr1;
    Node* lptr = nullptr;

    /* Checking for empty list */
    if (head == nullptr)
        return;

    do {
        swapped = 0;
        ptr1 = head;

        while (ptr1->next != lptr) {
            if (ptr1->data > ptr1->next->data) { 
                // Swap data of adjacent nodes
                int temp = ptr1->data;
                ptr1->data = ptr1->next->data;
                ptr1->next->data = temp;
                
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    }
    while (swapped);
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

    push(&a, 5);
    push(&a, 20);
    push(&a, 4);
    push(&a, 3);
    push(&a, 30);

    cout << "Unsorted Linked List: \n";
    printList(a);

    bubbleSort(a);

    cout << "\nSorted Linked List using Bubble Sort: \n";
    printList(a);

    return 0;
}
