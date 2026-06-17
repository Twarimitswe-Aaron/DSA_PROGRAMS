/*
 * ============================================================================
 * FILE: 07_trees.cpp
 * ============================================================================
 * TREES - Hierarchical data structures (non-linear).
 *
 * EXAM FREQUENCY: Extremely high. Tree problems are among the most common.
 *
 * Topics Covered:
 *   SECTION 1: Binary Tree - Basic operations & traversals
 *   SECTION 2: Binary Search Tree (BST) - Search, Insert, Delete
 *   SECTION 3: Tree Traversals (DFS: Inorder, Preorder, Postorder + BFS: Level Order)
 *   SECTION 4: Common Tree Problems
 *
 * Tree Terminology:
 *   - Root: Topmost node (no parent)
 *   - Leaf: Node with no children
 *   - Height: Number of edges from root to deepest leaf
 *   - Depth: Number of edges from root to node
 *   - Full Binary Tree: Every node has 0 or 2 children
 *   - Complete Binary Tree: All levels filled except possibly last, left-justified
 *   - Perfect Binary Tree: All internal nodes have 2 children, leaves at same level
 * ============================================================================
 */
#include <iostream>
#include <vector>
#include <queue>     // For level-order traversal
#include <stack>     // For iterative traversals
#include <algorithm> // For max, min
#include <climits>   // For INT_MIN, INT_MAX
using namespace std;

// ============================================================================
// NODE STRUCTURE for Binary Tree / BST
// ============================================================================
struct TreeNode {
    int data;
    TreeNode* left;   // Pointer to left child (smaller values in BST)
    TreeNode* right;  // Pointer to right child (larger values in BST)

    // Constructor to create a node with data, children initialized to NULL
    TreeNode(int val) : data(val), left(nullptr), right(nullptr) {}
};

// ============================================================================
// SECTION 1: BINARY TREE - Basic Operations
// ============================================================================

/*
 * KEY CONCEPT: A binary tree is a hierarchical structure where each node has
 * at most 2 children (left and right). Unlike BST, there is NO ordering
 * constraint between parent and children values.
 */

class BinaryTree {
public:
    TreeNode* root;

    BinaryTree() : root(nullptr) {}

    // -------------------------------------------------------------------
    // BUILD a sample tree for testing:
    //       1
    //      / \
    //     2   3
    //    / \   \
    //   4   5   6
    // -------------------------------------------------------------------
    void buildSampleTree() {
        root = new TreeNode(1);
        root->left = new TreeNode(2);
        root->right = new TreeNode(3);
        root->left->left = new TreeNode(4);
        root->left->right = new TreeNode(5);
        root->right->right = new TreeNode(6);
    }

    // -------------------------------------------------------------------
    // HEIGHT of tree: Max depth from root to farthest leaf. O(n) time.
    // Height of a leaf node is 0 (edges), or 1 (nodes). We use node count.
    // -------------------------------------------------------------------
    int height(TreeNode* node) {
        if (node == nullptr) return 0;
        // Height = 1 (current node) + max height of left and right subtrees
        return 1 + max(height(node->left), height(node->right));
    }

    // -------------------------------------------------------------------
    // SIZE (number of nodes) in tree. O(n) time.
    // -------------------------------------------------------------------
    int size(TreeNode* node) {
        if (node == nullptr) return 0;
        // Total = 1 (current) + size(left) + size(right)
        return 1 + size(node->left) + size(node->right);
    }
};

// ============================================================================
// SECTION 2: BINARY SEARCH TREE (BST)
// ============================================================================

/*
 * BST PROPERTY: For EVERY node in the tree:
 *   - All values in LEFT subtree are SMALLER than node's value
 *   - All values in RIGHT subtree are LARGER than node's value
 *   - No duplicate values (typically)
 *
 * This property makes BST excellent for searching, insertion, and deletion
 * in O(log n) average time (O(n) worst for skewed trees).
 */

class BST {
public:
    TreeNode* root;

    BST() : root(nullptr) {}

    // -------------------------------------------------------------------
    // SEARCH: O(log n) average, O(n) worst.
    // Uses BST property to eliminate half the tree at each step.
    // -------------------------------------------------------------------
    TreeNode* search(TreeNode* node, int key) {
        if (node == nullptr || node->data == key) {
            return node;  // Found or reached leaf (not found)
        }
        // BST property: if key < node, go left; else go right
        if (key < node->data) {
            return search(node->left, key);
        } else {
            return search(node->right, key);
        }
    }

    // Wrapper for search
    bool search(int key) {
        return search(root, key) != nullptr;
    }

    // -------------------------------------------------------------------
    // INSERT: O(log n) average.
    // Traverse to find the correct position, then insert new node.
    // Returns the new root (important for recursive implementation).
    // -------------------------------------------------------------------
    TreeNode* insert(TreeNode* node, int val) {
        if (node == nullptr) {
            return new TreeNode(val);  // Found insertion point
        }
        if (val < node->data) {
            node->left = insert(node->left, val);  // Go left
        } else if (val > node->data) {
            node->right = insert(node->right, val); // Go right
        }
        // If val == node->data, do nothing (no duplicates allowed)
        return node;  // Return unchanged node
    }

    void insert(int val) {
        root = insert(root, val);
    }

    // -------------------------------------------------------------------
    // DELETE: O(log n) average. Three cases to handle:
    // Case 1: Node is a leaf -> simply remove it
    // Case 2: Node has one child -> replace node with its child
    // Case 3: Node has two children -> find inorder successor (smallest
    //         in right subtree), copy its value, delete successor
    // -------------------------------------------------------------------

    // Helper: Find minimum value node in a subtree
    TreeNode* findMin(TreeNode* node) {
        // The minimum is the leftmost node (keep going left)
        while (node && node->left) {
            node = node->left;
        }
        return node;
    }

    TreeNode* deleteNode(TreeNode* node, int key) {
        if (node == nullptr) return nullptr;  // Key not found

        // First, find the node to delete (BST search)
        if (key < node->data) {
            node->left = deleteNode(node->left, key);
        } else if (key > node->data) {
            node->right = deleteNode(node->right, key);
        } else {
            // Found the node to delete (node->data == key)
            // Case 1 & 2: Leaf or one child
            if (node->left == nullptr) {
                TreeNode* temp = node->right;
                delete node;
                return temp;
            }
            if (node->right == nullptr) {
                TreeNode* temp = node->left;
                delete node;
                return temp;
            }
            // Case 3: Two children
            // Find inorder successor (smallest in right subtree)
            TreeNode* successor = findMin(node->right);
            // Copy successor's value to current node
            node->data = successor->data;
            // Delete the successor (which will be case 1 or 2)
            node->right = deleteNode(node->right, successor->data);
        }
        return node;
    }

    void deleteValue(int key) {
        root = deleteNode(root, key);
    }
};

// ============================================================================
// SECTION 3: TREE TRAVERSALS
// ============================================================================

/*
 * There are two main categories of tree traversal:
 *
 * A. DEPTH-FIRST SEARCH (DFS):
 *    1. Inorder   (Left -> Root -> Right) - gives sorted order for BST
 *    2. Preorder  (Root -> Left -> Right) - used to serialize/copy trees
 *    3. Postorder (Left -> Right -> Root) - used to delete trees, eval expressions
 *
 * B. BREADTH-FIRST SEARCH (BFS) / Level Order:
 *    Visit nodes level by level from top to bottom.
 *
 * Each traversal has recursive (simple) and iterative (efficient) versions.
 */

class Traversals {
public:
    // =====================================================================
    // RECURSIVE TRAVERSALS (Simple, uses call stack)
    // =====================================================================

    // -------------------------------------------------------------------
    // INORDER: Left -> Root -> Right. O(n) time, O(n) space (call stack).
    // For BST, this gives elements in ASCENDING order.
    // -------------------------------------------------------------------
    void inorderRecursive(TreeNode* node, vector<int>& result) {
        if (node == nullptr) return;
        inorderRecursive(node->left, result);   // Visit left subtree
        result.push_back(node->data);           // Visit root
        inorderRecursive(node->right, result);  // Visit right subtree
    }

    // -------------------------------------------------------------------
    // PREORDER: Root -> Left -> Right.
    // Used to create a copy of the tree or serialize it.
    // -------------------------------------------------------------------
    void preorderRecursive(TreeNode* node, vector<int>& result) {
        if (node == nullptr) return;
        result.push_back(node->data);            // Visit root
        preorderRecursive(node->left, result);   // Visit left subtree
        preorderRecursive(node->right, result);  // Visit right subtree
    }

    // -------------------------------------------------------------------
    // POSTORDER: Left -> Right -> Root.
    // Used to delete the tree (delete children before parent).
    // Also used in expression tree evaluation.
    // -------------------------------------------------------------------
    void postorderRecursive(TreeNode* node, vector<int>& result) {
        if (node == nullptr) return;
        postorderRecursive(node->left, result);  // Visit left subtree
        postorderRecursive(node->right, result); // Visit right subtree
        result.push_back(node->data);            // Visit root
    }

    // =====================================================================
    // ITERATIVE TRAVERSALS (Using explicit stack, no recursion)
    // More efficient: no function call overhead, avoids stack overflow.
    // =====================================================================

    // -------------------------------------------------------------------
    // ITERATIVE INORDER: Uses stack to simulate recursion.
    // Algorithm:
    //   1. Go as far left as possible, pushing each node onto stack
    //   2. Pop a node, visit it (add to result)
    //   3. Move to its right child and repeat
    // -------------------------------------------------------------------
    vector<int> inorderIterative(TreeNode* root) {
        vector<int> result;
        stack<TreeNode*> st;
        TreeNode* curr = root;

        while (curr != nullptr || !st.empty()) {
            // Go left as far as possible, pushing all nodes
            while (curr != nullptr) {
                st.push(curr);
                curr = curr->left;
            }
            // Pop the top node (leftmost unvisited), visit it
            curr = st.top();
            st.pop();
            result.push_back(curr->data);
            // Move to right subtree
            curr = curr->right;
        }
        return result;
    }

    // -------------------------------------------------------------------
    // ITERATIVE PREORDER: Root -> Left -> Right.
    // Simpler: Push root, while stack not empty: pop, visit, push right, push left.
    // -------------------------------------------------------------------
    vector<int> preorderIterative(TreeNode* root) {
        vector<int> result;
        if (root == nullptr) return result;

        stack<TreeNode*> st;
        st.push(root);

        while (!st.empty()) {
            TreeNode* curr = st.top();
            st.pop();
            result.push_back(curr->data);  // Visit root first

            // Push right first (LIFO: left will be processed before right)
            if (curr->right) st.push(curr->right);
            if (curr->left) st.push(curr->left);
        }
        return result;
    }

    // -------------------------------------------------------------------
    // ITERATIVE POSTORDER: Left -> Right -> Root (Tricky!)
    // Approach: Use two stacks or a single stack with a visited marker.
    // Single stack method: push root, maintain previous node to track direction.
    // -------------------------------------------------------------------
    vector<int> postorderIterative(TreeNode* root) {
        vector<int> result;
        if (root == nullptr) return result;

        stack<TreeNode*> st;
        TreeNode* curr = root;
        TreeNode* lastVisited = nullptr;

        while (curr != nullptr || !st.empty()) {
            // Go as far left as possible
            while (curr != nullptr) {
                st.push(curr);
                curr = curr->left;
            }
            TreeNode* top = st.top();
            // If right child exists and hasn't been visited, go right
            if (top->right != nullptr && lastVisited != top->right) {
                curr = top->right;
            } else {
                // Otherwise, process top and pop
                result.push_back(top->data);
                lastVisited = st.top();
                st.pop();
            }
        }
        return result;
    }

    // =====================================================================
    // BFS: LEVEL ORDER TRAVERSAL
    // Uses a queue. Visit root, then its children, then grandchildren, etc.
    // =====================================================================
    vector<vector<int>> levelOrder(TreeNode* root) {
        vector<vector<int>> result;
        if (root == nullptr) return result;

        queue<TreeNode*> q;
        q.push(root);

        while (!q.empty()) {
            int levelSize = q.size();  // Number of nodes at current level
            vector<int> currentLevel;

            // Process all nodes at the current level
            for (int i = 0; i < levelSize; i++) {
                TreeNode* curr = q.front();
                q.pop();
                currentLevel.push_back(curr->data);

                // Enqueue children for the next level
                if (curr->left) q.push(curr->left);
                if (curr->right) q.push(curr->right);
            }
            result.push_back(currentLevel);
        }
        return result;
    }
};

// ============================================================================
// SECTION 4: COMMON TREE PROBLEMS
// ============================================================================

class TreeProblems {
public:
    // -------------------------------------------------------------------
    // CHECK IF TREE IS A VALID BST. O(n) time, O(n) space.
    // For each node, ensure its value is within allowed range [min, max].
    // Root can be any value, so range is (-INF, +INF).
    // -------------------------------------------------------------------
    bool isValidBST(TreeNode* node, long long minVal, long long maxVal) {
        if (node == nullptr) return true;

        // Check if current node violates BST range constraint
        if (node->data <= minVal || node->data >= maxVal) return false;

        // Left subtree must have values < node->data
        // Right subtree must have values > node->data
        return isValidBST(node->left, minVal, node->data)
            && isValidBST(node->right, node->data, maxVal);
    }

    bool isValidBST(TreeNode* root) {
        return isValidBST(root, LLONG_MIN, LLONG_MAX);
    }

    // -------------------------------------------------------------------
    // CHECK IF TWO TREES ARE IDENTICAL. O(min(n,m)) time.
    // -------------------------------------------------------------------
    bool areIdentical(TreeNode* r1, TreeNode* r2) {
        if (r1 == nullptr && r2 == nullptr) return true;   // Both null
        if (r1 == nullptr || r2 == nullptr) return false;  // One null
        // Check root data AND left subtrees AND right subtrees
        return (r1->data == r2->data)
            && areIdentical(r1->left, r2->left)
            && areIdentical(r1->right, r2->right);
    }

    // -------------------------------------------------------------------
    // DIAMETER OF TREE: Longest path between any two nodes (may not pass root).
    // O(n) time. At each node, diameter = leftHeight + rightHeight + 1.
    //     Maintain max diameter globally.
    // -------------------------------------------------------------------
    int diameter(TreeNode* node, int& maxDia) {
        if (node == nullptr) return 0;
        int leftH = diameter(node->left, maxDia);
        int rightH = diameter(node->right, maxDia);
        // Update max diameter (path passing through this node)
        maxDia = max(maxDia, leftH + rightH + 1);
        // Return height of this subtree
        return 1 + max(leftH, rightH);
    }

    int diameter(TreeNode* root) {
        int maxDia = 0;
        diameter(root, maxDia);
        return maxDia;
    }

    // -------------------------------------------------------------------
    // LOWEST COMMON ANCESTOR (LCA) in BST. O(log n) average.
    // LCA is the deepest node that has both p and q in its subtrees.
    // In BST, LCA is the first node whose value lies between p and q.
    // -------------------------------------------------------------------
    TreeNode* lowestCommonAncestorBST(TreeNode* root, int p, int q) {
        if (root == nullptr) return nullptr;
        // If both are smaller, LCA is in left subtree
        if (p < root->data && q < root->data) {
            return lowestCommonAncestorBST(root->left, p, q);
        }
        // If both are larger, LCA is in right subtree
        if (p > root->data && q > root->data) {
            return lowestCommonAncestorBST(root->right, p, q);
        }
        // Otherwise, root is the LCA (p and q are on different sides or one is root)
        return root;
    }

    // -------------------------------------------------------------------
    // LOWEST COMMON ANCESTOR in Binary Tree (NOT necessarily BST). O(n) time.
    // No ordering property, so we need to search entire tree.
    // -------------------------------------------------------------------
    TreeNode* lowestCommonAncestorBT(TreeNode* root, int p, int q) {
        if (root == nullptr) return nullptr;
        if (root->data == p || root->data == q) return root;

        TreeNode* left = lowestCommonAncestorBT(root->left, p, q);
        TreeNode* right = lowestCommonAncestorBT(root->right, p, q);

        // If both subtrees return non-null, root is the LCA
        if (left && right) return root;
        // Otherwise, LCA is whichever subtree returned non-null
        return (left != nullptr) ? left : right;
    }

    // -------------------------------------------------------------------
    // MIRROR/INVERT TREE. O(n) time.
    // Swap left and right children recursively.
    // -------------------------------------------------------------------
    TreeNode* invertTree(TreeNode* root) {
        if (root == nullptr) return nullptr;
        // Swap left and right children
        TreeNode* temp = root->left;
        root->left = root->right;
        root->right = temp;
        // Recursively invert subtrees
        invertTree(root->left);
        invertTree(root->right);
        return root;
    }

    // -------------------------------------------------------------------
    // SERIALIZE: Convert tree to string representation. O(n) time.
    // Preorder traversal with '#' for null nodes.
    // -------------------------------------------------------------------
    void serialize(TreeNode* root, string& result) {
        if (root == nullptr) {
            result += "#,";  // Marker for null node
            return;
        }
        result += to_string(root->data) + ",";
        serialize(root->left, result);
        serialize(root->right, result);
    }

    // -------------------------------------------------------------------
    // DESERIALIZE: Reconstruct tree from serialized string. O(n) time.
    // -------------------------------------------------------------------
    TreeNode* deserialize(queue<string>& values) {
        if (values.empty()) return nullptr;
        string val = values.front();
        values.pop();
        if (val == "#") return nullptr;  // Null marker

        TreeNode* node = new TreeNode(stoi(val));
        node->left = deserialize(values);
        node->right = deserialize(values);
        return node;
    }
};

// ============================================================================
// HELPER: Print utilities
// ============================================================================

// Print a simple vector
void printVec(const vector<int>& v) {
    for (int x : v) cout << x << " ";
    cout << "\n";
}

// Print level-order vector of vectors
void printLevelOrder(const vector<vector<int>>& levels) {
    for (size_t i = 0; i < levels.size(); i++) {
        cout << "Level " << i << ": ";
        for (int x : levels[i]) cout << x << " ";
        cout << "\n";
    }
}

// ============================================================================
// SAMPLE main() to demonstrate all tree operations
// ============================================================================
int main() {
    cout << "========== TREES (07_trees.cpp) ==========\n\n";

    // ---- Build BST ----
    BST bst;
    vector<int> values = {50, 30, 70, 20, 40, 60, 80};
    cout << "Building BST with: ";
    for (int v : values) cout << v << " ";
    cout << "\n";
    for (int v : values) bst.insert(v);

    // ---- BST Search ----
    cout << "\nSearch 40: " << (bst.search(40) ? "Found" : "Not found") << "\n";
    cout << "Search 90: " << (bst.search(90) ? "Found" : "Not found") << "\n";

    // ---- BST Delete ----
    cout << "\nDelete 20 (leaf): ";
    bst.deleteValue(20);
    cout << "Done.\nDelete 50 (root with 2 children): ";
    bst.deleteValue(50);
    cout << "Done.\n";

    // ---- Traversals ----
    BinaryTree bt;
    bt.buildSampleTree();
    Traversals trav;
    TreeProblems tp;

    cout << "\n========== TREE TRAVERSALS ==========\n";
    cout << "Tree structure:\n";
    cout << "      1\n";
    cout << "     / \\\n";
    cout << "    2   3\n";
    cout << "   / \\   \\\n";
    cout << "  4   5   6\n\n";

    vector<int> result;

    result.clear(); trav.inorderRecursive(bt.root, result);
    cout << "Inorder (recursive):   "; printVec(result);

    result.clear(); trav.preorderRecursive(bt.root, result);
    cout << "Preorder (recursive):  "; printVec(result);

    result.clear(); trav.postorderRecursive(bt.root, result);
    cout << "Postorder (recursive): "; printVec(result);

    cout << "\nIterative:\n";
    cout << "Inorder (iterative):   "; printVec(trav.inorderIterative(bt.root));
    cout << "Preorder (iterative):  "; printVec(trav.preorderIterative(bt.root));
    cout << "Postorder (iterative): "; printVec(trav.postorderIterative(bt.root));

    cout << "\nLevel Order (BFS):\n";
    auto levels = trav.levelOrder(bt.root);
    printLevelOrder(levels);

    // ---- Common Problems ----
    cout << "\n========== TREE PROBLEMS ==========\n";
    cout << "Height: " << bt.height(bt.root) << "\n";
    cout << "Size: " << bt.size(bt.root) << "\n";
    cout << "Diameter: " << tp.diameter(bt.root) << "\n";

    // Build a BST for LCA test
    BST lcaTree;
    for (int v : {20, 10, 30, 5, 15, 25, 35}) lcaTree.insert(v);
    cout << "\nLCA in BST (5, 15): " << tp.lowestCommonAncestorBST(lcaTree.root, 5, 15)->data << "\n";
    cout << "LCA in BST (5, 35): " << tp.lowestCommonAncestorBST(lcaTree.root, 5, 35)->data << "\n";
    cout << "LCA in BT (4, 6): " << tp.lowestCommonAncestorBT(bt.root, 4, 6)->data << "\n";

    cout << "\nIs valid BST: " << (tp.isValidBST(bt.root) ? "Yes" : "No") << "\n";

    // ---- Serialize/Deserialize ----
    string serialized;
    tp.serialize(bt.root, serialized);
    cout << "\nSerialized: " << serialized << "\n";

    // ---- Invert tree ----
    cout << "\nInvert tree:\n";
    tp.invertTree(bt.root);
    result.clear(); trav.inorderRecursive(bt.root, result);
    cout << "Inorder after invert: "; printVec(result);

    cout << "\n========== END OF TREE OPERATIONS ==========\n";

    return 0;
}
