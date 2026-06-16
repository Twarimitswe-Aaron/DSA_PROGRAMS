// 2. Factorial of a number
#include <iostream>
using namespace std;

// Recursive function to calculate the factorial of a number
// Base case: if n is 0 or 1, return 1
// Recursive step: return n * factorial of (n - 1)
long long factorial(int n) {
    if (n <= 1) {
        return 1; // Base case
    }
    return n * factorial(n - 1); // Recursive step
}

int main() {
    int num = 5;
    cout << "Factorial of " << num << " is: " << factorial(num) << endl;
    return 0;
}
