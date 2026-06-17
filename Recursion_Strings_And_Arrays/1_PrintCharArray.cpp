/*
 * Topic: Recursion - Print Character Array
 * Description: Recursively prints the characters of an array until '\0' is encountered.
 */

#include <iostream>
using namespace std;

void printArray(char arr[]) {
    // Base case: if we reach the null terminator, stop
    if (arr[0] == '\0') {
        return;
    }
    
    // Print the first character
    cout << arr[0];
    
    // Recursive call on the smaller array (starting from the next character)
    printArray(arr + 1);
}

int main() {
    char str[] = "abc";
    cout << "Original string: " << str << "\n";
    cout << "Printed recursively: ";
    printArray(str);
    cout << endl;
    return 0;
}
