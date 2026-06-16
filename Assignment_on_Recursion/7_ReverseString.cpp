// 7. Reverse a string with recursion
#include <iostream>
#include <string>
using namespace std;

// Recursive function to reverse a string
// Base case: if start >= end, stop
// Recursive step: swap characters at start and end, then reverse the remaining substring
void reverseString(string& str, int start, int end) {
    if (start >= end) {
        return; // Base case
    }
    // Swap the characters
    char temp = str[start];
    str[start] = str[end];
    str[end] = temp;
    
    // Recursive step
    reverseString(str, start + 1, end - 1);
}

int main() {
    string str = "hello";
    reverseString(str, 0, str.length() - 1);
    cout << "Reversed string: " << str << endl;
    return 0;
}
