/*
 * Topic: Recursion - Convert String of Digits to Integer
 * Description: Recursively converts a string of numbers like "12345" into an integer 12345.
 */

#include <iostream>
#include <cmath>
using namespace std;

// Helper function to calculate length
int getLength(char arr[]) {
    if (arr[0] == '\0') return 0;
    return 1 + getLength(arr + 1);
}

int stringToInt(char arr[], int n) {
    // Base case: if length is 0, return 0
    if (n == 0) {
        return 0;
    }
    
    // Recursive step: get the integer value of the smaller substring (length n-1)
    int smallAns = stringToInt(arr, n - 1);
    
    // The last digit is at index n-1. Convert char to int by subtracting '0'
    int lastDigit = arr[n - 1] - '0';
    
    // Multiply the result from the smaller substring by 10 and add the last digit
    return smallAns * 10 + lastDigit;
}

int main() {
    char str[] = "12345";
    int length = getLength(str);
    
    cout << "String: \"" << str << "\"\n";
    int result = stringToInt(str, length);
    
    cout << "Converted Integer: " << result << "\n";
    
    // Proof that it's an integer
    cout << "Result + 5 = " << (result + 5) << "\n";
    
    return 0;
}
