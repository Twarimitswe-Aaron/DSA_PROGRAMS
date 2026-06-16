// 15. Print numbers from n down to 1
#include <iostream>
using namespace std;

// Recursive function to print numbers from n down to 1
// Base case: if n < 1, stop
// Recursive step: print n, then call function with n - 1
void printDownTo1(int n) {
    if (n < 1) {
        return; // Base case
    }
    cout << n << " "; // Print current number
    printDownTo1(n - 1); // Recursive step
}

int main() {
    int n = 5;
    cout << "Numbers from " << n << " down to 1: ";
    printDownTo1(n);
    cout << endl;
    return 0;
}
