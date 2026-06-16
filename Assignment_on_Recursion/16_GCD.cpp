// 16. Calculate Greatest Common Divisor (GCD) of two numbers
#include <iostream>
using namespace std;

// Recursive function to calculate GCD using Euclidean algorithm
// Base case: if b is 0, return a
// Recursive step: return GCD of b and (a % b)
int gcd(int a, int b) {
    if (b == 0) {
        return a; // Base case
    }
    return gcd(b, a % b); // Recursive step
}

int main() {
    int a = 48, b = 18;
    cout << "GCD of " << a << " and " << b << " is: " << gcd(a, b) << endl;
    return 0;
}
