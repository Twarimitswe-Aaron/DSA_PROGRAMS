// 18. Recursive multiplication without using '*' operator
#include <iostream>
using namespace std;

// Recursive function to multiply two numbers
// Base case: if y is 0, return 0
// Recursive step: return x + multiply(x, y - 1)
int multiply(int x, int y) {
    if (y == 0) {
        return 0; // Base case
    }
    // Handle negative numbers if y is negative
    if (y < 0) {
        return -multiply(x, -y);
    }
    return x + multiply(x, y - 1); // Recursive step
}

int main() {
    int a = 5, b = 4;
    cout << a << " multiplied by " << b << " is: " << multiply(a, b) << endl;
    return 0;
}
