// Exercise 1: Variables and pointers
#include <iostream>
using namespace std;

int main() {
    int a, b;
    cout << "Enter first integer (a): ";
    cin >> a;
    cout << "Enter second integer (b): ";
    cin >> b;

    // Declare pointers and store addresses
    int* ptrA = &a;
    int* ptrB = &b;

    // Display values using pointers
    cout << "Value of a using ptrA: " << *ptrA << endl;
    cout << "Value of b using ptrB: " << *ptrB << endl;

    return 0;
}
