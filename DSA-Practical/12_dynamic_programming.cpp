/*
 * ============================================================================
 * FILE: 12_dynamic_programming.cpp
 * ============================================================================
 * DYNAMIC PROGRAMMING - Optimizing recursion by storing subproblem results.
 *
 * EXAM FREQUENCY: Medium-High. DP problems are common in exams that test
 * algorithmic thinking.
 *
 * KEY CONCEPTS:
 *   - Optimal Substructure: Optimal solution can be built from optimal
 *     solutions of subproblems
 *   - Overlapping Subproblems: Same subproblems are solved repeatedly
 *
 * TWO APPROACHES:
 *   1. Top-Down (Memoization): Recursion + caching results in a table
 *   2. Bottom-Up (Tabulation): Build table iteratively from base cases
 *
 * Problems Covered:
 *   1. Fibonacci Number (introduces both memoization & tabulation)
 *   2. Factorial (DP version)
 *   3. Longest Common Subsequence (LCS)
 *   4. Longest Increasing Subsequence (LIS)
 *   5. 0/1 Knapsack Problem
 *   6. Edit Distance (Levenshtein Distance)
 *   7. Matrix Chain Multiplication
 *   8. Coin Change (Minimum coins & Number of ways)
 *   9. Longest Palindromic Subsequence
 *   10. Rod Cutting
 *   11. Subset Sum
 * ============================================================================
 */
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>  // For max, min
#include <climits>    // For INT_MAX
#include <cstring>    // For memset
using namespace std;

// ============================================================================
// 1. FIBONACCI NUMBER
//
// Demonstrates the two DP approaches:
//   Top-Down:    recursion + memoization (cache array)
//   Bottom-Up:   iterative table filling
// ============================================================================

// Top-Down (Memoization): O(n) time, O(n) space
int fibMemo(int n, vector<int>& dp) {
    if (n <= 1) return n;                // Base case
    if (dp[n] != -1) return dp[n];       // Already computed? Return cached value
    dp[n] = fibMemo(n - 1, dp) + fibMemo(n - 2, dp);  // Compute and store
    return dp[n];
}

int fibTopDown(int n) {
    vector<int> dp(n + 1, -1);  // -1 means "not computed yet"
    return fibMemo(n, dp);
}

// Bottom-Up (Tabulation): O(n) time, O(n) space (can be O(1) optimized)
int fibBottomUp(int n) {
    if (n <= 1) return n;
    vector<int> dp(n + 1);
    dp[0] = 0;
    dp[1] = 1;
    for (int i = 2; i <= n; i++) {
        dp[i] = dp[i - 1] + dp[i - 2];
    }
    return dp[n];
}

// Space-Optimized Fibonacci (only keep last 2 values): O(n) time, O(1) space
int fibOptimized(int n) {
    if (n <= 1) return n;
    int prev2 = 0, prev1 = 1, curr;
    for (int i = 2; i <= n; i++) {
        curr = prev1 + prev2;
        prev2 = prev1;
        prev1 = curr;
    }
    return curr;
}

// ============================================================================
// 2. LONGEST COMMON SUBSEQUENCE (LCS)
//
// Given two strings, find the length of the longest subsequence present
// in both. A subsequence is a sequence that appears in the same relative
// order but not necessarily contiguous.
//
// Recurrence:
//   if s1[i] == s2[j]: LCS(i,j) = 1 + LCS(i-1, j-1)
//   else:              LCS(i,j) = max(LCS(i-1, j), LCS(i, j-1))
//
// Time: O(m*n), Space: O(m*n)
// ============================================================================
int lcs(const string& s1, const string& s2) {
    int m = s1.size(), n = s2.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));

    // Build table bottom-up (dp[i][j] = LCS of s1[0..i-1] and s2[0..j-1])
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (s1[i - 1] == s2[j - 1]) {
                dp[i][j] = 1 + dp[i - 1][j - 1];  // Characters match
            } else {
                dp[i][j] = max(dp[i - 1][j], dp[i][j - 1]);  // Take max of skipping one char
            }
        }
    }
    return dp[m][n];  // Length of LCS
}

// Reconstruct the actual LCS string from the DP table
string lcsString(const string& s1, const string& s2) {
    int m = s1.size(), n = s2.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (s1[i - 1] == s2[j - 1])
                dp[i][j] = 1 + dp[i - 1][j - 1];
            else
                dp[i][j] = max(dp[i - 1][j], dp[i][j - 1]);
        }
    }

    // Backtrack to find the actual LCS
    string result;
    int i = m, j = n;
    while (i > 0 && j > 0) {
        if (s1[i - 1] == s2[j - 1]) {
            result = s1[i - 1] + result;  // Include this character
            i--; j--;
        } else if (dp[i - 1][j] > dp[i][j - 1]) {
            i--;  // Move up
        } else {
            j--;  // Move left
        }
    }
    return result;
}

// ============================================================================
// 3. LONGEST INCREASING SUBSEQUENCE (LIS)
//
// Find the length of the longest subsequence where elements are strictly
// increasing. Elements need not be contiguous.
//
// DP approach: O(n^2) time, O(n) space
// For each i, dp[i] = 1 + max(dp[j]) for all j < i where arr[j] < arr[i]
// ============================================================================
int lis(const vector<int>& arr) {
    int n = arr.size();
    if (n == 0) return 0;

    vector<int> dp(n, 1);  // Each element is at least LIS of length 1 (itself)
    int maxLen = 1;

    for (int i = 1; i < n; i++) {
        for (int j = 0; j < i; j++) {
            if (arr[j] < arr[i]) {
                dp[i] = max(dp[i], dp[j] + 1);  // Extend subsequence ending at j
            }
        }
        maxLen = max(maxLen, dp[i]);
    }
    return maxLen;
}

// ============================================================================
// 4. 0/1 KNAPSACK PROBLEM
//
// Given items with weights and values, maximize value in a knapsack of
// capacity W. Each item can be taken at most once (0/1 choice).
//
// Recurrence:
//   if w[i] > capacity: can't take item i
//     knap(i, cap) = knap(i-1, cap)
//   else:
//     knap(i, cap) = max(knap(i-1, cap), val[i] + knap(i-1, cap - w[i]))
//
// Time: O(n * W), Space: O(n * W)
// ============================================================================
int knapsack01(const vector<int>& values, const vector<int>& weights, int capacity) {
    int n = values.size();
    vector<vector<int>> dp(n + 1, vector<int>(capacity + 1, 0));

    for (int i = 1; i <= n; i++) {
        for (int w = 0; w <= capacity; w++) {
            if (weights[i - 1] <= w) {
                // Can take item i: max of NOT taking vs TAKING
                dp[i][w] = max(
                    dp[i - 1][w],                                      // Skip item
                    values[i - 1] + dp[i - 1][w - weights[i - 1]]      // Take item
                );
            } else {
                // Can't take item i (too heavy)
                dp[i][w] = dp[i - 1][w];
            }
        }
    }
    return dp[n][capacity];
}

// Space-optimized version: O(W) space (single row, iterate right-to-left)
int knapsack01Optimized(const vector<int>& values, const vector<int>& weights, int capacity) {
    int n = values.size();
    vector<int> dp(capacity + 1, 0);

    for (int i = 0; i < n; i++) {
        // Iterate RIGHT to LEFT to avoid reusing the same item multiple times
        for (int w = capacity; w >= weights[i]; w--) {
            dp[w] = max(dp[w], values[i] + dp[w - weights[i]]);
        }
    }
    return dp[capacity];
}

// ============================================================================
// 5. EDIT DISTANCE (Levenshtein Distance)
//
// Minimum number of operations (insert, delete, replace) to convert
// string s1 into string s2.
//
// Recurrence:
//   if s1[i] == s2[j]: dp[i][j] = dp[i-1][j-1]
//   else:              dp[i][j] = 1 + min(dp[i-1][j], dp[i][j-1], dp[i-1][j-1])
//                    (delete)      (insert)       (replace)
//
// Time: O(m*n), Space: O(m*n)
// ============================================================================
int editDistance(const string& s1, const string& s2) {
    int m = s1.size(), n = s2.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1));

    // Base cases: converting to/from empty string
    for (int i = 0; i <= m; i++) dp[i][0] = i;  // i deletions
    for (int j = 0; j <= n; j++) dp[0][j] = j;  // j insertions

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (s1[i - 1] == s2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];  // Characters match, no operation needed
            } else {
                dp[i][j] = 1 + min({
                    dp[i - 1][j],     // Delete from s1
                    dp[i][j - 1],     // Insert into s1
                    dp[i - 1][j - 1]  // Replace character
                });
            }
        }
    }
    return dp[m][n];
}

// ============================================================================
// 6. MATRIX CHAIN MULTIPLICATION
//
// Find minimum number of scalar multiplications to multiply a chain of matrices.
// Given array p[] where matrix Ai has dimensions p[i-1] x p[i].
//
// Recurrence:
//   MCM(i,j) = minimum cost to multiply matrices i through j
//   MCM(i,j) = min over k of (MCM(i,k) + MCM(k+1,j) + p[i-1]*p[k]*p[j])
//
// Time: O(n^3), Space: O(n^2)
// ============================================================================
int matrixChainMultiplication(const vector<int>& p) {
    int n = p.size() - 1;  // Number of matrices
    vector<vector<int>> dp(n, vector<int>(n, 0));

    // dp[i][j] = min cost to multiply matrices i..j
    // Chain length l = j - i + 1
    for (int len = 2; len <= n; len++) {          // Length of chain
        for (int i = 0; i < n - len + 1; i++) {
            int j = i + len - 1;
            dp[i][j] = INT_MAX;
            for (int k = i; k < j; k++) {
                int cost = dp[i][k] + dp[k + 1][j] + p[i] * p[k + 1] * p[j + 1];
                dp[i][j] = min(dp[i][j], cost);
            }
        }
    }
    return dp[0][n - 1];
}

// ============================================================================
// 7. COIN CHANGE - Minimum Number of Coins
//
// Given coin denominations and a target amount, find the minimum number of
// coins needed to make the amount. You have infinite supply of each coin.
//
// Recurrence:
//   dp[amt] = min over coins of (1 + dp[amt - coin])
//
// Time: O(amount * n), Space: O(amount)
// ============================================================================
int coinChangeMinCoins(const vector<int>& coins, int amount) {
    vector<int> dp(amount + 1, INT_MAX - 1);  // INT_MAX-1 to avoid overflow
    dp[0] = 0;  // 0 coins needed to make amount 0

    for (int amt = 1; amt <= amount; amt++) {
        for (int coin : coins) {
            if (coin <= amt) {
                dp[amt] = min(dp[amt], 1 + dp[amt - coin]);
            }
        }
    }
    return dp[amount] == INT_MAX - 1 ? -1 : dp[amount];
}

// ============================================================================
// 8. COIN CHANGE - Number of Ways (Combinations)
//
// Count the number of ways to make the amount using given coins (order doesn't matter).
// This is similar to unbounded knapsack.
//
// Time: O(amount * n), Space: O(amount)
// ============================================================================
int coinChangeWays(const vector<int>& coins, int amount) {
    vector<int> dp(amount + 1, 0);
    dp[0] = 1;  // 1 way to make amount 0: use no coins

    // Iterate over coins first to count combinations (not permutations)
    for (int coin : coins) {
        for (int amt = coin; amt <= amount; amt++) {
            dp[amt] += dp[amt - coin];
        }
    }
    return dp[amount];
}

// ============================================================================
// 9. LONGEST PALINDROMIC SUBSEQUENCE (LPS)
//
// Length of the longest subsequence that is a palindrome.
// Can be solved using LCS of string and its reverse.
//
// Alternatively: DP approach
//   if s[i] == s[j]: dp[i][j] = 2 + dp[i+1][j-1]
//   else:            dp[i][j] = max(dp[i+1][j], dp[i][j-1])
//
// Time: O(n^2), Space: O(n^2)
// ============================================================================
int longestPalindromicSubseq(const string& s) {
    int n = s.size();
    vector<vector<int>> dp(n, vector<int>(n, 0));

    // Every single character is a palindrome of length 1
    for (int i = 0; i < n; i++) dp[i][i] = 1;

    // Build DP table for increasing substring lengths
    for (int len = 2; len <= n; len++) {
        for (int i = 0; i < n - len + 1; i++) {
            int j = i + len - 1;
            if (s[i] == s[j]) {
                dp[i][j] = 2 + (len == 2 ? 0 : dp[i + 1][j - 1]);
            } else {
                dp[i][j] = max(dp[i + 1][j], dp[i][j - 1]);
            }
        }
    }
    return dp[0][n - 1];
}

// ============================================================================
// 10. ROD CUTTING
//
// Given a rod of length n and prices for each length, find the maximum
// revenue obtainable by cutting the rod and selling the pieces.
//
// Recurrence:
//   dp[len] = max over cuts of (price[cut] + dp[len - cut])
//
// Time: O(n^2), Space: O(n)
// ============================================================================
int rodCutting(const vector<int>& prices, int n) {
    vector<int> dp(n + 1, 0);

    for (int len = 1; len <= n; len++) {
        int maxVal = INT_MIN;
        for (int cut = 1; cut <= len; cut++) {
            maxVal = max(maxVal, prices[cut - 1] + dp[len - cut]);
        }
        dp[len] = maxVal;
    }
    return dp[n];
}

// ============================================================================
// 11. SUBSET SUM (DP version)
//
// Check if there exists a subset with sum equal to target.
// Time: O(n * target), Space: O(n * target)
// ============================================================================
bool subsetSumDP(const vector<int>& arr, int target) {
    int n = arr.size();
    vector<vector<bool>> dp(n + 1, vector<bool>(target + 1, false));

    // Sum 0 is always achievable (by taking no elements)
    for (int i = 0; i <= n; i++) dp[i][0] = true;

    for (int i = 1; i <= n; i++) {
        for (int sum = 1; sum <= target; sum++) {
            if (arr[i - 1] <= sum) {
                dp[i][sum] = dp[i - 1][sum] || dp[i - 1][sum - arr[i - 1]];
            } else {
                dp[i][sum] = dp[i - 1][sum];
            }
        }
    }
    return dp[n][target];
}

// ============================================================================
// SAMPLE main() to demonstrate all DP problems
// ============================================================================
int main() {
    cout << "========== DYNAMIC PROGRAMMING (12_dynamic_programming.cpp) ==========\n\n";

    cout << "= FIBONACCI =\n";
    cout << "Fibonacci(10) - TopDown: " << fibTopDown(10) << "\n";
    cout << "Fibonacci(10) - BottomUp: " << fibBottomUp(10) << "\n";
    cout << "Fibonacci(10) - Optimized: " << fibOptimized(10) << "\n\n";

    cout << "= LCS =\n";
    cout << "LCS of 'ABCDGH' and 'AEDFHR': " << lcs("ABCDGH", "AEDFHR") << "\n";
    cout << "  Actual LCS: " << lcsString("ABCDGH", "AEDFHR") << "\n\n";

    cout << "= LIS =\n";
    cout << "LIS of [10, 22, 9, 33, 21, 50, 41, 60]: " << lis({10, 22, 9, 33, 21, 50, 41, 60}) << "\n\n";

    cout << "= 0/1 KNAPSACK =\n";
    vector<int> values = {60, 100, 120};
    vector<int> weights = {10, 20, 30};
    int W = 50;
    cout << "Values: [60, 100, 120], Weights: [10, 20, 30], Capacity: 50\n";
    cout << "Max value: " << knapsack01(values, weights, W) << "\n";
    cout << "Optimized: " << knapsack01Optimized(values, weights, W) << "\n\n";

    cout << "= EDIT DISTANCE =\n";
    cout << "Edit distance 'kitten' -> 'sitting': " << editDistance("kitten", "sitting") << "\n\n";

    cout << "= MATRIX CHAIN MULTIPLICATION =\n";
    vector<int> dims = {1, 2, 3, 4};  // A1:1x2, A2:2x3, A3:3x4
    cout << "Dims: [1, 2, 3, 4]\n";
    cout << "Min multiplications: " << matrixChainMultiplication(dims) << "\n\n";

    cout << "= COIN CHANGE =\n";
    vector<int> coins = {1, 2, 5};
    int amount = 11;
    cout << "Coins: [1, 2, 5], Amount: 11\n";
    cout << "Min coins needed: " << coinChangeMinCoins(coins, amount) << "\n";
    cout << "Number of ways: " << coinChangeWays(coins, amount) << "\n\n";

    cout << "= LONGEST PALINDROMIC SUBSEQUENCE =\n";
    cout << "LPS of 'BBABCBCAB': " << longestPalindromicSubseq("BBABCBCAB") << "\n\n";

    cout << "= ROD CUTTING =\n";
    vector<int> prices = {1, 5, 8, 9, 10, 17, 17, 20};
    cout << "Prices: [1, 5, 8, 9, 10, 17, 17, 20]\n";
    cout << "Max revenue for length 8: " << rodCutting(prices, 8) << "\n\n";

    cout << "= SUBSET SUM =\n";
    vector<int> setArr = {3, 34, 4, 12, 5, 2};
    cout << "Subset sum of 9 in [3, 34, 4, 12, 5, 2]: "
         << (subsetSumDP(setArr, 9) ? "Yes" : "No") << "\n";
    cout << "Subset sum of 30: "
         << (subsetSumDP(setArr, 30) ? "Yes" : "No") << "\n";

    cout << "\n========== END OF DYNAMIC PROGRAMMING ==========\n";

    return 0;
}
