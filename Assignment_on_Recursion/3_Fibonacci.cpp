// 3. Compute the nth Fibonacci number
#include <iostream>
using namespace std;

// Recursive function to calculate the nth Fibonacci number
// Base case: if n is 0 or 1, return n
// Recursive step: return fibonacci(n - 1) + fibonacci(n - 2)
int fibonacci(int n) {
    if (n <= 1) {
        return n; // Base case
    }
    return fibonacci(n - 1) + fibonacci(n - 2); // Recursive step
}

int main() {
    int n = 6;
    cout << "The " << n << "th Fibonacci number is: " << fibonacci(n) << endl;
    return 0;
}
