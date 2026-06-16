// 4. Sum of digits of a number
#include <iostream>
using namespace std;

// Recursive function to find the sum of digits
// Base case: if n is 0, return 0
// Recursive step: return last digit (n % 10) + sumOfDigits of remaining digits (n / 10)
int sumOfDigits(int n) {
    if (n == 0) {
        return 0; // Base case
    }
    return (n % 10) + sumOfDigits(n / 10); // Recursive step
}

int main() {
    int num = 12345;
    cout << "Sum of digits of " << num << " is: " << sumOfDigits(num) << endl;
    return 0;
}
