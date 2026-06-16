/*
 * Topic: Binary Tree Breadth-First Search (Level Order Traversal)
 * Description: Traverses the tree level by level from top to bottom, left to right, using a Queue.
 *
 * Time Complexity: O(N) where N is number of nodes.
 * Space Complexity: O(W) where W is maximum width of the tree.
 */

#include <iostream>
#include <queue>
using namespace std;

struct TreeNode {
    int data;
    TreeNode* left;
    TreeNode* right;
    
    TreeNode(int val) : data(val), left(nullptr), right(nullptr) {}
};

void levelOrderTraversal(TreeNode* root) {
    if (root == nullptr) return;

    queue<TreeNode*> q;
    q.push(root);

    while (!q.empty()) {
        TreeNode* current = q.front();
        q.pop();

        cout << current->data << " ";

        // Enqueue left child
        if (current->left != nullptr) {
            q.push(current->left);
        }
        
        // Enqueue right child
        if (current->right != nullptr) {
            q.push(current->right);
        }
    }
}

int main() {
    /*
     * Creating the following Binary Tree:
     *        10
     *       /  \
     *      20   30
     *     /  \    \
     *    40  50   60
     */
    TreeNode* root = new TreeNode(10);
    root->left = new TreeNode(20);
    root->right = new TreeNode(30);
    root->left->left = new TreeNode(40);
    root->left->right = new TreeNode(50);
    root->right->right = new TreeNode(60);

    cout << "Level Order Traversal (BFS): ";
    levelOrderTraversal(root);
    cout << endl;

    return 0;
}
