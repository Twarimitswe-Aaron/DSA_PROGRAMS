/*
 * Topic: Recursion - Remove Consecutive Duplicates
 * Description: Recursively removes consecutive duplicate characters from a string.
 */

#include <iostream>
using namespace std;

void removeConsecutiveDuplicates(char arr[]) {
    // Base case: if string is empty or has only one character left
    if (arr[0] == '\0' || arr[1] == '\0') {
        return;
    }
    
    // If current character matches the next character
    if (arr[0] == arr[1]) {
        // Shift all characters one position to the left
        int i = 0;
        while (arr[i] != '\0') {
            arr[i] = arr[i + 1];
            i++;
        }
        // Since we shifted left, we must check the same index again
        removeConsecutiveDuplicates(arr);
    } else {
        // If they don't match, move to the next pair
        removeConsecutiveDuplicates(arr + 1);
    }
}

int main() {
    char str1[] = "aabbbccdd";
    char str2[] = "abcde";
    
    cout << "Original: " << str1 << " -> ";
    removeConsecutiveDuplicates(str1);
    cout << "Processed: " << str1 << "\n";
    
    cout << "Original: " << str2 << " -> ";
    removeConsecutiveDuplicates(str2);
    cout << "Processed: " << str2 << "\n";
    
    return 0;
}
