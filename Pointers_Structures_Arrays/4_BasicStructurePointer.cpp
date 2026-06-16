// Exercise 4: Basic structure and pointer
#include <iostream>
using namespace std;

// 1. Define a struct called Point
struct Point {
    int x;
    int y;
};

int main() {
    // 2. Declare a Point variable and a pointer to a Point variable
    Point p1;
    Point* ptr = &p1; // 3. Store the address

    // 4. Set values using the arrow operator ->
    ptr->x = 5;
    ptr->y = 10;

    // Display them
    cout << "Point coordinates: (" << ptr->x << ", " << ptr->y << ")" << endl;

    return 0;
}
