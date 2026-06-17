/*
 * ============================================================================
 * FILE: 03_stack.cpp
 * ============================================================================
 * STACK - Last-In-First-Out (LIFO) data structure.
 *
 * EXAM FREQUENCY: Very high. Stacks appear in almost every practical exam.
 *
 * Key operations: push (insert top), pop (remove top), top/peek, isEmpty.
 *
 * Common exam questions:
 *   1. Implement stack using array (fixed size)
 *   2. Implement stack using linked list (dynamic size)
 *   3. Check balanced parentheses (most common stack application)
 *   4. Infix to Postfix/Prefix conversion
 *   5. Evaluate Postfix expression
 *   6. Reverse a string using stack
 *   7. Next Greater Element (monotonic stack)
 *   8. Stock Span Problem
 *   9. Min Stack (get minimum in O(1))
 *   10. Sort a stack
 * ============================================================================
 */
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <stack>    // Note: this is the STL stack (used in applications)
using namespace std;

// ============================================================================
// SECTION 1: STACK USING ARRAY (Fixed Size)
// ============================================================================

class StackArray {
private:
    int* arr;      // Dynamically allocated array to store elements
    int top;       // Index of the top element (-1 means empty stack)
    int capacity;  // Maximum number of elements the stack can hold

public:
    // Constructor: Initialize stack with given size
    StackArray(int size) {
        arr = new int[size];   // Allocate memory on heap
        capacity = size;
        top = -1;              // Stack is initially empty
    }

    // Destructor: Free dynamically allocated memory
    ~StackArray() {
        delete[] arr;
    }

    // -------------------------------------------------------------------
    // PUSH: Add element to top. O(1) time.
    // Check for overflow before inserting.
    // -------------------------------------------------------------------
    void push(int val) {
        if (isFull()) {
            cout << "Stack Overflow! Cannot push " << val << "\n";
            return;
        }
        top++;           // Move top up
        arr[top] = val;  // Place value at new top position
    }

    // -------------------------------------------------------------------
    // POP: Remove element from top. O(1) time.
    // Check for underflow before removing.
    // -------------------------------------------------------------------
    int pop() {
        if (isEmpty()) {
            cout << "Stack Underflow! Cannot pop from empty stack.\n";
            return -1;
        }
        return arr[top--];  // Return top value and decrement top
    }

    // -------------------------------------------------------------------
    // PEEK/TOP: View top element without removing it. O(1) time.
    // -------------------------------------------------------------------
    int peek() {
        if (isEmpty()) {
            cout << "Stack is empty!\n";
            return -1;
        }
        return arr[top];
    }

    bool isEmpty() { return top == -1; }
    bool isFull()  { return top == capacity - 1; }
    int size()     { return top + 1; }

    void display() {
        cout << "Stack [bottom -> top]: ";
        for (int i = 0; i <= top; i++) {
            cout << arr[i] << " ";
        }
        cout << "\n";
    }
};

// ============================================================================
// SECTION 2: STACK USING LINKED LIST (Dynamic Size, No Overflow)
// ============================================================================

struct StackNode {
    int data;
    StackNode* next;
    StackNode(int val) : data(val), next(nullptr) {}
};

class StackLinkedList {
private:
    StackNode* topNode;  // Pointer to the top node of the stack

public:
    StackLinkedList() : topNode(nullptr) {}

    ~StackLinkedList() {
        while (topNode != nullptr) {
            StackNode* temp = topNode;
            topNode = topNode->next;
            delete temp;
        }
    }

    // -------------------------------------------------------------------
    // PUSH: O(1) time. New node becomes the new top.
    // In linked list, we insert at beginning (which is the top of stack).
    // -------------------------------------------------------------------
    void push(int val) {
        StackNode* newNode = new StackNode(val);
        newNode->next = topNode;  // New node points to current top
        topNode = newNode;        // Update top to new node
    }

    // -------------------------------------------------------------------
    // POP: O(1) time. Remove and return top element.
    // -------------------------------------------------------------------
    int pop() {
        if (isEmpty()) {
            cout << "Stack Underflow!\n";
            return -1;
        }
        StackNode* temp = topNode;
        int poppedValue = temp->data;
        topNode = topNode->next;  // Move top to the next node
        delete temp;              // Free memory of old top
        return poppedValue;
    }

    int peek() {
        if (isEmpty()) {
            cout << "Stack is empty!\n";
            return -1;
        }
        return topNode->data;
    }

    bool isEmpty() { return topNode == nullptr; }

    void display() {
        StackNode* temp = topNode;
        cout << "Stack [top -> bottom]: ";
        while (temp != nullptr) {
            cout << temp->data << " ";
            temp = temp->next;
        }
        cout << "\n";
    }
};

// ============================================================================
// SECTION 3: STACK APPLICATIONS (Using STL stack)
// ============================================================================

// -------------------------------------------------------------------
// APPLICATION 1: CHECK BALANCED PARENTHESES
// Key algorithm: Push opening brackets, pop on closing brackets.
// At the end, stack must be empty for balanced expression.
// -------------------------------------------------------------------
bool areParenthesesBalanced(const string& expr) {
    stack<char> st;
    // Iterate through each character in the expression
    for (char ch : expr) {
        // If it's an opening bracket, push onto stack
        if (ch == '(' || ch == '{' || ch == '[') {
            st.push(ch);
        }
        // If it's a closing bracket
        else if (ch == ')' || ch == '}' || ch == ']') {
            // If stack is empty, no matching opening bracket
            if (st.empty()) return false;
            // Check if the top of stack matches this closing bracket
            char top = st.top();
            st.pop();
            // Mismatch: opening and closing don't correspond
            if ((ch == ')' && top != '(') ||
                (ch == '}' && top != '{') ||
                (ch == ']' && top != '[')) {
                return false;
            }
        }
    }
    // Stack must be empty at the end (all brackets closed properly)
    return st.empty();
}

// -------------------------------------------------------------------
// APPLICATION 2: INFIX TO POSTFIX CONVERSION
// Shunting-yard algorithm by Edsger Dijkstra.
// Rules:
//   - Operands (numbers/letters) go directly to output
//   - Operators: pop from stack to output while top has higher/equal precedence
//     then push current operator
//   - '(' : push to stack
//   - ')' : pop to output until '(' is found
// -------------------------------------------------------------------
int precedence(char op) {
    if (op == '^') return 3;       // Highest precedence (right-associative)
    if (op == '*' || op == '/') return 2;
    if (op == '+' || op == '-') return 1;
    return -1;  // Not an operator
}

string infixToPostfix(const string& infix) {
    stack<char> st;
    string postfix;

    for (char ch : infix) {
        // If character is alphanumeric (operand), add to output directly
        if (isalnum(ch)) {
            postfix += ch;
        }
        // '(' : push to stack
        else if (ch == '(') {
            st.push(ch);
        }
        // ')' : pop until matching '('
        else if (ch == ')') {
            while (!st.empty() && st.top() != '(') {
                postfix += st.top();
                st.pop();
            }
            if (!st.empty() && st.top() == '(') {
                st.pop();  // Remove the '('
            }
        }
        // Operator: process based on precedence
        else {
            while (!st.empty() && precedence(ch) <= precedence(st.top())) {
                postfix += st.top();
                st.pop();
            }
            st.push(ch);
        }
    }
    // Pop any remaining operators to output
    while (!st.empty()) {
        postfix += st.top();
        st.pop();
    }
    return postfix;
}

// -------------------------------------------------------------------
// APPLICATION 3: EVALUATE POSTFIX EXPRESSION
// Algorithm: Scan left to right.
//   - If operand: push to stack
//   - If operator: pop two operands, apply operator, push result
// -------------------------------------------------------------------
int evaluatePostfix(const string& postfix) {
    stack<int> st;
    for (char ch : postfix) {
        if (isdigit(ch)) {
            st.push(ch - '0');  // Convert char digit to int
        } else {
            int b = st.top(); st.pop();  // Second operand (right)
            int a = st.top(); st.pop();  // First operand (left)
            switch (ch) {
                case '+': st.push(a + b); break;
                case '-': st.push(a - b); break;
                case '*': st.push(a * b); break;
                case '/': st.push(a / b); break;
                case '^': st.push((int)pow(a, b)); break;
            }
        }
    }
    return st.top();  // Final result
}

// -------------------------------------------------------------------
// APPLICATION 4: NEXT GREATER ELEMENT (Monotonic Stack)
// For each element, find the next larger element to its right.
// If no larger, output -1.
// Uses decreasing stack (elements in stack are in decreasing order).
// -------------------------------------------------------------------
vector<int> nextGreaterElement(const vector<int>& arr) {
    int n = arr.size();
    vector<int> result(n, -1);  // Initialize all to -1 (default if no NGE)
    stack<int> st;              // Stack stores indices, not values

    // Traverse from right to left (or left to right with different logic)
    for (int i = n - 1; i >= 0; i--) {
        // Pop elements smaller than or equal to current element
        // (they can never be NGE for elements to the left)
        while (!st.empty() && arr[st.top()] <= arr[i]) {
            st.pop();
        }
        // If stack is not empty, top has the next greater element
        if (!st.empty()) {
            result[i] = arr[st.top()];
        }
        st.push(i);  // Push current index
    }
    return result;
}

// -------------------------------------------------------------------
// APPLICATION 5: MIN STACK (getMin in O(1) time)
// Uses two stacks: main stack and min stack that tracks current minimum.
// -------------------------------------------------------------------
class MinStack {
private:
    stack<int> mainSt;  // Regular stack for all elements
    stack<int> minSt;   // Tracks minimums (stores the min so far)

public:
    void push(int val) {
        mainSt.push(val);
        // Min stack: push val if min stack is empty or val <= current min
        if (minSt.empty() || val <= minSt.top()) {
            minSt.push(val);
        }
    }

    void pop() {
        if (mainSt.empty()) return;
        // If the element being popped is the current minimum, pop from minSt too
        if (mainSt.top() == minSt.top()) {
            minSt.pop();
        }
        mainSt.pop();
    }

    int top() { return mainSt.top(); }
    int getMin() { return minSt.top(); }
};

// -------------------------------------------------------------------
// APPLICATION 6: SORT A STACK (using recursion)
// Recursively pop all elements, then insert each in sorted order.
// -------------------------------------------------------------------
void insertSorted(stack<int>& st, int val) {
    // Base: if stack is empty or top <= val, just push
    if (st.empty() || st.top() <= val) {
        st.push(val);
        return;
    }
    // Otherwise, pop top, recurse, then push back
    int topVal = st.top();
    st.pop();
    insertSorted(st, val);
    st.push(topVal);
}

void sortStack(stack<int>& st) {
    if (st.empty()) return;
    int val = st.top();
    st.pop();
    sortStack(st);          // Recursively sort remaining stack
    insertSorted(st, val);  // Insert popped element in sorted order
}

// ============================================================================
// SAMPLE main() to demonstrate all stack operations
// ============================================================================
int main() {
    cout << "========== STACK USING ARRAY ==========\n";
    StackArray sa(5);
    sa.push(10); sa.push(20); sa.push(30);
    sa.display();
    cout << "Popped: " << sa.pop() << "\n";
    cout << "Top/Peek: " << sa.peek() << "\n";
    sa.display();

    cout << "\n========== STACK USING LINKED LIST ==========\n";
    StackLinkedList sll;
    sll.push(100); sll.push(200); sll.push(300);
    sll.display();
    cout << "Popped: " << sll.pop() << "\n";
    sll.display();

    cout << "\n========== BALANCED PARENTHESES ==========\n";
    string expr1 = "{[()]}";
    string expr2 = "{[(])}";
    cout << expr1 << " -> " << (areParenthesesBalanced(expr1) ? "Balanced" : "Not Balanced") << "\n";
    cout << expr2 << " -> " << (areParenthesesBalanced(expr2) ? "Balanced" : "Not Balanced") << "\n";

    cout << "\n========== INFIX TO POSTFIX ==========\n";
    string infix = "A+B*(C^D-E)";
    cout << "Infix: " << infix << "\n";
    cout << "Postfix: " << infixToPostfix(infix) << "\n";

    cout << "\n========== EVALUATE POSTFIX ==========\n";
    string postfix = "23*54*+";  // (2*3)+(5*4) = 6+20 = 26
    cout << "Postfix: " << postfix << "\n";
    cout << "Result: " << evaluatePostfix(postfix) << "\n";

    cout << "\n========== NEXT GREATER ELEMENT ==========\n";
    vector<int> arr = {4, 5, 2, 25, 7, 8};
    vector<int> nge = nextGreaterElement(arr);
    cout << "Element -> NGE\n";
    for (size_t i = 0; i < arr.size(); i++) {
        cout << "  " << arr[i] << "    ->    " << nge[i] << "\n";
    }

    cout << "\n========== MIN STACK ==========\n";
    MinStack ms;
    ms.push(5); ms.push(2); ms.push(7); ms.push(1);
    cout << "Top: " << ms.top() << ", Min: " << ms.getMin() << "\n";
    ms.pop();
    cout << "After pop, Top: " << ms.top() << ", Min: " << ms.getMin() << "\n";

    cout << "\n========== SORT STACK ==========\n";
    stack<int> st;
    st.push(30); st.push(10); st.push(50); st.push(20); st.push(40);
    sortStack(st);
    cout << "Sorted stack [top -> bottom]: ";
    while (!st.empty()) {
        cout << st.top() << " ";
        st.pop();
    }
    cout << "\n";

    cout << "\n========== END OF STACK OPERATIONS ==========\n";

    return 0;
}
