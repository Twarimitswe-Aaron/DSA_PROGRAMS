// 8. Check if a string is a palindrome
#include <iostream>
#include <string>
using namespace std;

// Recursive function to check for palindrome
// Base case: if start >= end, it is a palindrome
// Recursive step: check if start and end characters match, then check the remaining substring
bool isPalindrome(const string& str, int start, int end) {
    if (start >= end) {
        return true; // Base case: reached the middle without mismatch
    }
    if (str[start] != str[end]) {
        return false; // Base case: characters don't match
    }
    return isPalindrome(str, start + 1, end - 1); // Recursive step
}

int main() {
    string str = "racecar";
    if (isPalindrome(str, 0, str.length() - 1)) {
        cout << str << " is a palindrome." << endl;
    } else {
        cout << str << " is not a palindrome." << endl;
    }
    return 0;
}
