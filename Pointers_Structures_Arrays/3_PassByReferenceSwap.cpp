// Exercise 3: Passing by reference with pointers
#include <iostream>
using namespace std;

// Function to swap two integers using pointers
void swap(int* ptr1, int* ptr2) {
    int temp = *ptr1;
    *ptr1 = *ptr2;
    *ptr2 = temp;
}

int main() {
    int x = 10;
    int y = 20;

    cout << "Before swap: x = " << x << ", y = " << y << endl;

    // Pass the addresses of x and y
    swap(&x, &y);

    cout << "After swap: x = " << x << ", y = " << y << endl;

    return 0;
}
