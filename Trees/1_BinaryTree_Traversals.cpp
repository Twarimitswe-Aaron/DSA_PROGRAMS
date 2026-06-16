/*
 * Topic: Binary Tree Depth-First Traversals
 * Description: Implements In-order, Pre-order, and Post-order traversals on a basic Binary Tree.
 *
 * Traversal Rules:
 * - Pre-order: Root -> Left -> Right
 * - In-order: Left -> Root -> Right
 * - Post-order: Left -> Right -> Root
 */

#include <iostream>
using namespace std;

// Definition for a binary tree node.
struct TreeNode {
    int data;
    TreeNode* left;
    TreeNode* right;
    
    TreeNode(int val) : data(val), left(nullptr), right(nullptr) {}
};

// Pre-order Traversal
void preOrder(TreeNode* root) {
    if (root == nullptr) return;
    cout << root->data << " ";
    preOrder(root->left);
    preOrder(root->right);
}

// In-order Traversal
void inOrder(TreeNode* root) {
    if (root == nullptr) return;
    inOrder(root->left);
    cout << root->data << " ";
    inOrder(root->right);
}

// Post-order Traversal
void postOrder(TreeNode* root) {
    if (root == nullptr) return;
    postOrder(root->left);
    postOrder(root->right);
    cout << root->data << " ";
}

int main() {
    /*
     * Creating the following Binary Tree:
     *        1
     *       / \
     *      2   3
     *     / \
     *    4   5
     */
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->right = new TreeNode(3);
    root->left->left = new TreeNode(4);
    root->left->right = new TreeNode(5);

    cout << "Pre-order Traversal: ";
    preOrder(root);
    cout << endl;

    cout << "In-order Traversal: ";
    inOrder(root);
    cout << endl;

    cout << "Post-order Traversal: ";
    postOrder(root);
    cout << endl;

    return 0;
}
