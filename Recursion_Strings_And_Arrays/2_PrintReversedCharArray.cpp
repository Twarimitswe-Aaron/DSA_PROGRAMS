/*
 * Topic: Recursion - Print Reversed Character Array
 * Description: Recursively prints the characters of an array in reverse order.
 */

#include <iostream>
using namespace std;

void printReversed(char arr[]) {
    // Base case: if we reach the null terminator, stop
    if (arr[0] == '\0') {
        return;
    }
    
    // Recursive call on the smaller array FIRST
    printReversed(arr + 1);
    
    // Print the character AFTER the recursive call returns (prints from last to first)
    cout << arr[0];
}

int main() {
    char str[] = "abc";
    cout << "Original string: " << str << "\n";
    cout << "Reversed recursively: ";
    printReversed(str);
    cout << endl;
    return 0;
}
