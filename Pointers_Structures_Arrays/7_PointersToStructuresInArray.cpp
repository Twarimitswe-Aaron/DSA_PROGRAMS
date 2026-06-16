// Exercise 7: Pointers to structures in an array
#include <iostream>
using namespace std;

// 1. Define Product struct
struct Product {
    int id;
    double price;
};

int main() {
    // 2. Create array of Product structures
    Product products[3] = {
        {101, 19.99},
        {102, 29.50},
        {103, 5.75}
    };

    // 3. Create array of pointers to Product
    Product* ptrArray[3];

    // 4. Initialize pointer array to point to Product elements
    for (int i = 0; i < 3; i++) {
        ptrArray[i] = &products[i];
    }

    // 5. Iterate and print details using pointers
    cout << "Product Details:" << endl;
    for (int i = 0; i < 3; i++) {
        cout << "ID: " << ptrArray[i]->id << ", Price: $" << ptrArray[i]->price << endl;
    }

    return 0;
}
