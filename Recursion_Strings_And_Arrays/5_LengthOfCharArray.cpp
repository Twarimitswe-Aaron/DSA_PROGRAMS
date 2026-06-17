/*
 * Topic: Recursion - Find Length of Character Array
 * Description: Recursively calculates the length of a string.
 */

#include <iostream>
using namespace std;

int getLength(char arr[]) {
    // Base case: if we hit the null terminator, length is 0
    if (arr[0] == '\0') {
        return 0;
    }
    
    // Recursive step: 1 (for current char) + length of the rest of the array
    return 1 + getLength(arr + 1);
}

int main() {
    char str[] = "abcde";
    
    cout << "String: " << str << "\n";
    cout << "Length of string is: " << getLength(str) << "\n";
    
    return 0;
}
