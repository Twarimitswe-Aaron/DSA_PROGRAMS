/*
 * Topic: AVL Tree (Self-Balancing Binary Search Tree)
 * Description: Implements insertion and necessary rotations (LL, RR, LR, RL) to keep the tree strictly balanced.
 *
 * Balance Factor = Height of Left Subtree - Height of Right Subtree
 * Allowed Balance Factors: -1, 0, 1
 *
 * Time Complexity: O(log N) guaranteed for Search, Insert, Delete due to balancing.
 */

#include <iostream>
#include <algorithm>
using namespace std;

struct AVLNode {
    int key;
    AVLNode* left;
    AVLNode* right;
    int height;
    
    AVLNode(int k) : key(k), left(nullptr), right(nullptr), height(1) {}
};

// Utility function to get the height of the tree
int height(AVLNode* N) {
    if (N == nullptr) return 0;
    return N->height;
}

// Utility function to get balance factor of node N
int getBalance(AVLNode* N) {
    if (N == nullptr) return 0;
    return height(N->left) - height(N->right);
}

// Right rotation (LL Case)
AVLNode* rightRotate(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    // Return new root
    return x;
}

// Left rotation (RR Case)
AVLNode* leftRotate(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    // Update heights
    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    // Return new root
    return y;
}

// Recursive function to insert a key and balance the tree
AVLNode* insert(AVLNode* node, int key) {
    // 1. Perform the normal BST insertion
    if (node == nullptr)
        return new AVLNode(key);

    if (key < node->key)
        node->left = insert(node->left, key);
    else if (key > node->key)
        node->right = insert(node->right, key);
    else // Equal keys are not allowed in BST
        return node;

    // 2. Update height of this ancestor node
    node->height = 1 + max(height(node->left), height(node->right));

    // 3. Get the balance factor to check whether this node became unbalanced
    int balance = getBalance(node);

    // If this node becomes unbalanced, then there are 4 cases:

    // Left Left Case
    if (balance > 1 && key < node->left->key)
        return rightRotate(node);

    // Right Right Case
    if (balance < -1 && key > node->right->key)
        return leftRotate(node);

    // Left Right Case
    if (balance > 1 && key > node->left->key) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // Right Left Case
    if (balance < -1 && key < node->right->key) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    // return the (unchanged) node pointer
    return node;
}

// A utility function to print preorder traversal of the tree
void preOrder(AVLNode* root) {
    if (root != nullptr) {
        cout << root->key << " ";
        preOrder(root->left);
        preOrder(root->right);
    }
}

int main() {
    AVLNode* root = nullptr;

    /* Constructing tree given in the above figure */
    root = insert(root, 10);
    root = insert(root, 20);
    root = insert(root, 30); // RR Case -> Left Rotation
    root = insert(root, 40);
    root = insert(root, 50); // RR Case -> Left Rotation
    root = insert(root, 25); // RL Case -> Right-Left Rotation

    /* The constructed AVL Tree would be
            30
           /  \
         20   40
        /  \     \
       10  25    50
    */

    cout << "Preorder traversal of the constructed AVL tree is: \n";
    preOrder(root);
    cout << endl;

    return 0;
}
