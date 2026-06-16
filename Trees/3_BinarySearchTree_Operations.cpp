/*
 * Topic: Binary Search Tree (BST) Operations
 * Description: Implements insertion, searching, and deletion in a BST.
 *
 * Properties of BST:
 * - Left subtree of a node contains only nodes with keys lesser than the node's key.
 * - Right subtree of a node contains only nodes with keys greater than the node's key.
 *
 * Time Complexity (Average): O(log N) for Search, Insert, Delete
 * Time Complexity (Worst): O(N) for skewed trees
 */

#include <iostream>
using namespace std;

struct BSTNode {
    int data;
    BSTNode* left;
    BSTNode* right;
    
    BSTNode(int val) : data(val), left(nullptr), right(nullptr) {}
};

// Insert a node into BST
BSTNode* insert(BSTNode* root, int val) {
    if (root == nullptr) {
        return new BSTNode(val);
    }
    
    if (val < root->data) {
        root->left = insert(root->left, val);
    } else if (val > root->data) {
        root->right = insert(root->right, val);
    }
    return root;
}

// Search for a value in BST
bool search(BSTNode* root, int val) {
    if (root == nullptr) return false;
    if (root->data == val) return true;
    
    if (val < root->data)
        return search(root->left, val);
    else
        return search(root->right, val);
}

// Find the minimum value node in a given tree
BSTNode* findMin(BSTNode* root) {
    while (root->left != nullptr) {
        root = root->left;
    }
    return root;
}

// Delete a node from BST
BSTNode* deleteNode(BSTNode* root, int val) {
    if (root == nullptr) return root;

    if (val < root->data) {
        root->left = deleteNode(root->left, val);
    } else if (val > root->data) {
        root->right = deleteNode(root->right, val);
    } else {
        // Node to be deleted found
        
        // Case 1: Node with only one child or no child
        if (root->left == nullptr) {
            BSTNode* temp = root->right;
            delete root;
            return temp;
        } else if (root->right == nullptr) {
            BSTNode* temp = root->left;
            delete root;
            return temp;
        }
        
        // Case 2: Node with two children
        // Get the inorder successor (smallest in the right subtree)
        BSTNode* temp = findMin(root->right);
        
        // Copy the inorder successor's content to this node
        root->data = temp->data;
        
        // Delete the inorder successor
        root->right = deleteNode(root->right, temp->data);
    }
    return root;
}

// In-order traversal to print the tree (Will print sorted elements for BST)
void inOrder(BSTNode* root) {
    if (root != nullptr) {
        inOrder(root->left);
        cout << root->data << " ";
        inOrder(root->right);
    }
}

int main() {
    BSTNode* root = nullptr;
    root = insert(root, 50);
    insert(root, 30);
    insert(root, 20);
    insert(root, 40);
    insert(root, 70);
    insert(root, 60);
    insert(root, 80);

    cout << "In-order traversal of the given BST: ";
    inOrder(root);
    cout << endl;

    cout << "Search for 40: " << (search(root, 40) ? "Found" : "Not Found") << endl;
    cout << "Search for 90: " << (search(root, 90) ? "Found" : "Not Found") << endl;

    cout << "Delete 20\n";
    root = deleteNode(root, 20);
    cout << "In-order after deletion: ";
    inOrder(root);
    cout << endl;

    cout << "Delete 30\n";
    root = deleteNode(root, 30);
    cout << "In-order after deletion: ";
    inOrder(root);
    cout << endl;

    cout << "Delete 50\n";
    root = deleteNode(root, 50);
    cout << "In-order after deletion: ";
    inOrder(root);
    cout << endl;

    return 0;
}
