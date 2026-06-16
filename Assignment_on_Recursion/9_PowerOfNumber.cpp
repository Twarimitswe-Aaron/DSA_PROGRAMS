// 9. Compute power of a number (x^n)
#include <iostream>
using namespace std;

// Recursive function to compute power
// Base case: if n is 0, return 1
// Recursive step: return x * power(x, n - 1)
double power(double x, int n) {
    if (n == 0) {
        return 1; // Base case
    }
    // Handle negative exponents
    if (n < 0) {
        return 1 / power(x, -n);
    }
    return x * power(x, n - 1); // Recursive step
}

int main() {
    double base = 2.0;
    int exponent = 3;
    cout << base << "^" << exponent << " is: " << power(base, exponent) << endl;
    return 0;
}
