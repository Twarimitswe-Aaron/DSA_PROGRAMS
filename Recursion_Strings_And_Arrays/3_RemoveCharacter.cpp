/*
 * Topic: Recursion - Remove a Character in Array
 * Description: Recursively removes all occurrences of a specific character from a string.
 */

#include <iostream>
using namespace std;

void removeChar(char arr[], char target) {
    // Base case
    if (arr[0] == '\0') {
        return;
    }
    
    // If the current character is the target, we need to shift everything left
    if (arr[0] == target) {
        int i = 0;
        // Shift all characters to the left by 1
        while (arr[i] != '\0') {
            arr[i] = arr[i + 1];
            i++;
        }
        // Call recursion on the same index because the new character shifted here might also be target
        removeChar(arr, target);
    } else {
        // Otherwise, just move to the next character
        removeChar(arr + 1, target);
    }
}

int main() {
    char str[] = "abacada";
    char target = 'a';
    
    cout << "Original string: " << str << "\n";
    removeChar(str, target);
    cout << "After removing '" << target << "': " << str << "\n";
    
    return 0;
}
