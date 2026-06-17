/*
 * Algorithm: Selection Sort on a Singly Linked List
 * Description: Iterates through the list, finds the minimum element from the unsorted part, and swaps its data with the current node.
 * Note: Swapping data is easier than swapping nodes in a singly linked list.
 * 
 * Time Complexity:
 *  - Best, Average, Worst Case: O(n^2)
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

// Function to sort a singly linked list using selection sort (by swapping data)
void selectionSort(Node* head) {
    Node* temp = head;

    // Traverse the List
    while (temp != nullptr) {
        Node* minNode = temp;
        Node* r = temp->next;

        // Traverse the unsorted sublist
        while (r != nullptr) {
            if (minNode->data > r->data)
                minNode = r;
            r = r->next;
        }

        // Swap Data
        int x = temp->data;
        temp->data = minNode->data;
        minNode->data = x;
        
        temp = temp->next;
    }
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

    selectionSort(a);

    cout << "\nSorted Linked List using Selection Sort: \n";
    printList(a);

    return 0;
}
