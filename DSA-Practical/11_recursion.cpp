/*
 * ============================================================================
 * FILE: 11_recursion.cpp
 * ============================================================================
 * RECURSION - A function that calls itself to solve smaller subproblems.
 *
 * EXAM FREQUENCY: Very high. Recursion is fundamental to DSA and appears
 * in almost every practical exam either directly or as part of divide &
 * conquer and tree/graph algorithms.
 *
 * KEY CONCEPTS:
 *   - Base case: Condition that stops recursion (prevents infinite loop)
 *   - Recursive case: Function calls itself with smaller/simpler input
 *   - Call stack: Each recursive call is pushed onto the call stack
 *   - Tail recursion: Recursive call is the LAST operation (can be optimized)
 *
 * Problems Covered:
 *   1. Factorial of N
 *   2. Fibonacci Series (Nth term)
 *   3. Sum of first N natural numbers
 *   4. Power function (a^b)
 *   5. Check if array is sorted
 *   6. Print numbers 1 to N (and N to 1)
 *   7. Tower of Hanoi
 *   8. Binary Search (recursive)
 *   9. Reverse a string
 *   10. Palindrome check
 *   11. Generate all subsets of a set
 *   12. Generate all permutations of a string
 *   13. N-Queens Problem
 *   14. Sudoku Solver
 *   15. Rat in a Maze
 * ============================================================================
 */
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

// ============================================================================
// SIMPLE RECURSIVE PROBLEMS
// ============================================================================

// -------------------------------------------------------------------
// 1. FACTORIAL: n! = n * (n-1) * (n-2) * ... * 1
// Base: 0! = 1, 1! = 1
// Recursive: n! = n * (n-1)!
// Time: O(n), Space: O(n) (call stack)
// -------------------------------------------------------------------
int factorial(int n) {
    if (n <= 1) return 1;           // Base case: 0! and 1! = 1
    return n * factorial(n - 1);    // Recursive: n! = n * (n-1)!
}

// -------------------------------------------------------------------
// 2. FIBONACCI: F(n) = F(n-1) + F(n-2), F(0)=0, F(1)=1
// WARNING: Simple recursion is O(2^n) - extremely slow for n > 40.
// Use memoization (DP) for efficiency (see 12_dynamic_programming.cpp).
// Here we show it for completeness and to demonstrate the problem.
// -------------------------------------------------------------------
int fibonacci(int n) {
    if (n <= 1) return n;            // Base: F(0)=0, F(1)=1
    return fibonacci(n - 1) + fibonacci(n - 2);  // Recursive
}

// -------------------------------------------------------------------
// 3. SUM OF FIRST N NATURAL NUMBERS: sum(n) = n + sum(n-1)
// -------------------------------------------------------------------
int sumNatural(int n) {
    if (n == 0) return 0;           // Base case
    return n + sumNatural(n - 1);   // Recursive: n + sum of (n-1) numbers
}

// -------------------------------------------------------------------
// 4. POWER: a^b = a * a^(b-1)
// Optimization: For even b, a^b = (a^(b/2))^2 (reduces to O(log b))
// -------------------------------------------------------------------
int power(int a, int b) {
    if (b == 0) return 1;           // Base: any number ^ 0 = 1
    if (b % 2 == 0) {
        // Even exponent: a^b = (a^(b/2))^2
        int half = power(a, b / 2);
        return half * half;
    } else {
        // Odd exponent: a^b = a * a^(b-1)
        return a * power(a, b - 1);
    }
}

// -------------------------------------------------------------------
// 5. CHECK IF ARRAY IS SORTED (ascending). O(n) time.
// Check if arr[0] <= arr[1], then recurse on rest of array.
// -------------------------------------------------------------------
bool isArraySorted(const vector<int>& arr, int index) {
    if (index == (int)arr.size() - 1) return true;  // Reached end = sorted
    if (arr[index] > arr[index + 1]) return false;   // Found unsorted pair
    return isArraySorted(arr, index + 1);            // Check next pair
}

// Wrapper
bool isArraySorted(const vector<int>& arr) {
    if (arr.empty()) return true;
    return isArraySorted(arr, 0);
}

// -------------------------------------------------------------------
// 6. TOWER OF HANOI: Move n disks from source to destination using auxiliary.
//
// Rules:
//   - Only one disk can be moved at a time
//   - Only the top disk can be moved
//   - No larger disk can be placed on a smaller disk
//
// Algorithm:
//   1. Move n-1 disks from source to auxiliary (using dest as helper)
//   2. Move the nth disk from source to destination
//   3. Move n-1 disks from auxiliary to destination (using source as helper)
//
// Time: O(2^n), Space: O(n) (call stack)
// -------------------------------------------------------------------
void towerOfHanoi(int n, char source, char destination, char auxiliary) {
    if (n == 1) {
        // Base case: Only 1 disk - move it directly
        cout << "  Move disk 1 from " << source << " to " << destination << "\n";
        return;
    }
    // Step 1: Move n-1 disks from source to auxiliary
    towerOfHanoi(n - 1, source, auxiliary, destination);
    // Step 2: Move the nth disk from source to destination
    cout << "  Move disk " << n << " from " << source << " to " << destination << "\n";
    // Step 3: Move n-1 disks from auxiliary to destination
    towerOfHanoi(n - 1, auxiliary, destination, source);
}

// ============================================================================
// STRING RECURSION PROBLEMS
// ============================================================================

// -------------------------------------------------------------------
// 7. REVERSE A STRING (in-place, two-pointer recursion). O(n).
// -------------------------------------------------------------------
void reverseString(string& s, int left, int right) {
    if (left >= right) return;          // Base: pointers crossed
    swap(s[left], s[right]);            // Swap characters at ends
    reverseString(s, left + 1, right - 1);  // Recurse on inner substring
}

string reverseString(const string& s) {
    string result = s;
    reverseString(result, 0, result.size() - 1);
    return result;
}

// -------------------------------------------------------------------
// 8. CHECK IF STRING IS PALINDROME. O(n) time, O(n) space.
// -------------------------------------------------------------------
bool isPalindrome(const string& s, int left, int right) {
    if (left >= right) return true;     // Base: all characters checked
    if (s[left] != s[right]) return false;  // Mismatch found
    return isPalindrome(s, left + 1, right - 1);  // Check inner substring
}

bool isPalindrome(const string& s) {
    return isPalindrome(s, 0, s.size() - 1);
}

// ============================================================================
// BACKTRACKING PROBLEMS (Advanced Recursion)
//
// Backtracking: Try all possibilities; if a partial solution can't lead
// to a valid solution, backtrack (undo last choice and try another).
// ============================================================================

// -------------------------------------------------------------------
// 9. GENERATE ALL SUBSETS (Power Set) of a given set. O(2^n) time.
//
// For each element, we have 2 choices: include or exclude.
// Recursively explore both possibilities.
// -------------------------------------------------------------------
void generateSubsets(const vector<int>& arr, int index, vector<int>& current, vector<vector<int>>& result) {
    if (index == (int)arr.size()) {
        // Base: processed all elements, add current subset to result
        result.push_back(current);
        return;
    }
    // Choice 1: INCLUDE the current element
    current.push_back(arr[index]);
    generateSubsets(arr, index + 1, current, result);

    // Choice 2: EXCLUDE (backtrack - remove what we just added)
    current.pop_back();
    generateSubsets(arr, index + 1, current, result);
}

vector<vector<int>> subsets(const vector<int>& arr) {
    vector<vector<int>> result;
    vector<int> current;
    generateSubsets(arr, 0, current, result);
    return result;
}

// -------------------------------------------------------------------
// 10. GENERATE ALL PERMUTATIONS OF A STRING. O(n!) time.
//
// Swapping technique: for each position i, swap with all later positions j.
// Backtrack by swapping back after recursion.
// -------------------------------------------------------------------
void permute(string& s, int left, int right, vector<string>& result) {
    if (left == right) {
        // Base: all positions filled, add permutation
        result.push_back(s);
        return;
    }
    for (int i = left; i <= right; i++) {
        swap(s[left], s[i]);        // Choose: swap current with position i
        permute(s, left + 1, right, result);  // Recurse on next position
        swap(s[left], s[i]);        // Backtrack: undo the swap
    }
}

vector<string> permutations(string s) {
    vector<string> result;
    permute(s, 0, s.size() - 1, result);
    return result;
}

// -------------------------------------------------------------------
// 11. N-QUEENS PROBLEM: Place N queens on NxN chessboard so none attack.
//
// Constraint: No two queens share same row, column, or diagonal.
// Backtrack row by row; for each row, try each column.
// Time: O(N!) in practice (worst-case is O(N^N) but heavily pruned).
// -------------------------------------------------------------------
bool isSafe(const vector<string>& board, int row, int col, int n) {
    // Check column (above current row)
    for (int i = 0; i < row; i++) {
        if (board[i][col] == 'Q') return false;
    }
    // Check upper-left diagonal
    for (int i = row - 1, j = col - 1; i >= 0 && j >= 0; i--, j--) {
        if (board[i][j] == 'Q') return false;
    }
    // Check upper-right diagonal
    for (int i = row - 1, j = col + 1; i >= 0 && j < n; i--, j++) {
        if (board[i][j] == 'Q') return false;
    }
    return true;
}

void solveNQueens(vector<string>& board, int row, int n, vector<vector<string>>& solutions) {
    if (row == n) {
        // Base: all queens placed successfully
        solutions.push_back(board);
        return;
    }
    for (int col = 0; col < n; col++) {
        if (isSafe(board, row, col, n)) {
            board[row][col] = 'Q';                // Place queen
            solveNQueens(board, row + 1, n, solutions);  // Recurse next row
            board[row][col] = '.';                // Backtrack: remove queen
        }
    }
}

vector<vector<string>> nQueens(int n) {
    vector<vector<string>> solutions;
    vector<string> board(n, string(n, '.'));  // Initialize empty board
    solveNQueens(board, 0, n, solutions);
    return solutions;
}

// -------------------------------------------------------------------
// 12. RAT IN A MAZE: Find path from (0,0) to (n-1,n-1) through 1's.
//
// Maze is n x n grid. 1 = open path, 0 = blocked.
// Can move: U(p), D(own), L(eft), R(ight).
// Uses backtracking to explore all paths.
// -------------------------------------------------------------------
bool solveMazeUtil(const vector<vector<int>>& maze, int x, int y,
                   vector<vector<int>>& solution, int n) {
    // Reached destination
    if (x == n - 1 && y == n - 1) {
        solution[x][y] = 1;
        return true;
    }
    // Check if position is valid
    if (x >= 0 && x < n && y >= 0 && y < n && maze[x][y] == 1 && solution[x][y] == 0) {
        solution[x][y] = 1;  // Mark as part of path

        // Try moving DOWN
        if (solveMazeUtil(maze, x + 1, y, solution, n)) return true;
        // Try moving RIGHT
        if (solveMazeUtil(maze, x, y + 1, solution, n)) return true;
        // Try moving UP
        if (solveMazeUtil(maze, x - 1, y, solution, n)) return true;
        // Try moving LEFT
        if (solveMazeUtil(maze, x, y - 1, solution, n)) return true;

        // Backtrack: no path from here
        solution[x][y] = 0;
        return false;
    }
    return false;
}

vector<vector<int>> ratInMaze(const vector<vector<int>>& maze) {
    int n = maze.size();
    vector<vector<int>> solution(n, vector<int>(n, 0));
    if (maze[0][0] == 0 || maze[n-1][n-1] == 0) return solution;  // No path

    solveMazeUtil(maze, 0, 0, solution, n);
    return solution;
}

// -------------------------------------------------------------------
// 13. SUBSET SUM: Check if a subset sums to a given target. O(2^n).
// -------------------------------------------------------------------
bool subsetSum(const vector<int>& arr, int index, int target) {
    if (target == 0) return true;   // Base: target reached
    if (index == (int)arr.size()) return false;  // No elements left
    if (target < 0) return false;   // Overshot target

    // Choice 1: Include arr[index]
    if (subsetSum(arr, index + 1, target - arr[index])) return true;
    // Choice 2: Exclude arr[index]
    return subsetSum(arr, index + 1, target);
}

// ============================================================================
// SAMPLE main() to demonstrate all recursion problems
// ============================================================================
int main() {
    cout << "========== RECURSION (11_recursion.cpp) ==========\n\n";

    cout << "= SIMPLE RECURSION =\n";
    cout << "Factorial(5): " << factorial(5) << "\n";
    cout << "Fibonacci(10): " << fibonacci(10) << "\n";
    cout << "SumNatural(10): " << sumNatural(10) << "\n";
    cout << "Power(2, 10): " << power(2, 10) << "\n";
    cout << "Array [1,2,3,4,5] sorted: " << (isArraySorted({1,2,3,4,5}) ? "Yes" : "No") << "\n";
    cout << "Array [1,3,2,4,5] sorted: " << (isArraySorted({1,3,2,4,5}) ? "Yes" : "No") << "\n";

    cout << "\nTower of Hanoi (3 disks):\n";
    towerOfHanoi(3, 'A', 'C', 'B');

    cout << "\n= STRING RECURSION =\n";
    cout << "Reverse 'hello': " << reverseString("hello") << "\n";
    cout << "'racecar' palindrome: " << (isPalindrome("racecar") ? "Yes" : "No") << "\n";
    cout << "'hello' palindrome: " << (isPalindrome("hello") ? "Yes" : "No") << "\n";

    cout << "\n= BACKTRACKING =\n";
    cout << "Subsets of {1, 2}:\n";
    auto subs = subsets({1, 2});
    for (auto& s : subs) {
        cout << "  { ";
        for (int x : s) cout << x << " ";
        cout << "}\n";
    }

    cout << "\nPermutations of 'ABC':\n";
    auto perms = permutations("ABC");
    for (auto& p : perms) cout << "  " << p << "\n";

    cout << "\n4-Queens solutions:\n";
    auto queens = nQueens(4);
    for (size_t i = 0; i < queens.size(); i++) {
        cout << "  Solution " << i+1 << ":\n";
        for (auto& row : queens[i]) {
            cout << "    " << row << "\n";
        }
    }

    cout << "\nRat in Maze:\n";
    vector<vector<int>> maze = {
        {1, 0, 0, 0},
        {1, 1, 0, 1},
        {0, 1, 0, 0},
        {1, 1, 1, 1}
    };
    auto path = ratInMaze(maze);
    cout << "  Solution path (1 = part of path):\n";
    for (auto& row : path) {
        cout << "    ";
        for (int cell : row) cout << cell << " ";
        cout << "\n";
    }

    cout << "\nSubset Sum target 9 in {3, 34, 4, 12, 5, 2}: "
         << (subsetSum({3, 34, 4, 12, 5, 2}, 0, 9) ? "Found" : "Not found") << "\n";

    cout << "\n========== END OF RECURSION ==========\n";

    return 0;
}
