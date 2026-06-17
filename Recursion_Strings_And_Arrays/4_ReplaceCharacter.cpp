/*
 * Topic: Recursion - Replace a Character in Array
 * Description: Recursively replaces all occurrences of a character with another character.
 */

#include <iostream>
using namespace std;

void replaceChar(char arr[], char target, char replacement) {
    // Base case
    if (arr[0] == '\0') {
        return;
    }
    
    // If we find the target, replace it
    if (arr[0] == target) {
        arr[0] = replacement;
    }
    
    // Recursive step: move to the next character
    replaceChar(arr + 1, target, replacement);
}

int main() {
    char str[] = "abacada";
    char target = 'a';
    char replacement = 'x';
    
    cout << "Original string: " << str << "\n";
    replaceChar(str, target, replacement);
    cout << "After replacing '" << target << "' with '" << replacement << "': " << str << "\n";
    
    return 0;
}
