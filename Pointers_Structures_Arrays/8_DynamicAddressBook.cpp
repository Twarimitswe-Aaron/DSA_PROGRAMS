// Exercise 8: Structures, pointers, and dynamic memory
#include <iostream>
#include <string>
using namespace std;

// 1. Define Contact struct
struct Contact {
    string name;
    string address;
    string phone_number;
};

int capacity = 2; // Initial capacity
int currentCount = 0;
Contact* addressBook = new Contact[capacity];

// Function to dynamically resize the array
void resizeArray() {
    capacity *= 2;
    Contact* newArray = new Contact[capacity];
    for (int i = 0; i < currentCount; i++) {
        newArray[i] = addressBook[i];
    }
    delete[] addressBook;
    addressBook = newArray;
}

// Function to add new entries
void addEntry(string name, string address, string phone) {
    if (currentCount == capacity) {
        resizeArray();
    }
    addressBook[currentCount].name = name;
    addressBook[currentCount].address = address;
    addressBook[currentCount].phone_number = phone;
    currentCount++;
    cout << "Contact added!" << endl;
}

// Function to display all entries
void displayAll() {
    if (currentCount == 0) {
        cout << "Address book is empty." << endl;
        return;
    }
    for (int i = 0; i < currentCount; i++) {
        cout << "Name: " << addressBook[i].name 
             << " | Address: " << addressBook[i].address 
             << " | Phone: " << addressBook[i].phone_number << endl;
    }
}

// Function to search for a contact
void searchContact(string searchName) {
    for (int i = 0; i < currentCount; i++) {
        if (addressBook[i].name == searchName) {
            cout << "Found - Name: " << addressBook[i].name 
                 << " | Address: " << addressBook[i].address 
                 << " | Phone: " << addressBook[i].phone_number << endl;
            return;
        }
    }
    cout << "Contact not found." << endl;
}

// Function to free memory
void freeMemory() {
    delete[] addressBook;
    cout << "Memory freed." << endl;
}

int main() {
    addEntry("Alice", "123 Main St", "555-1234");
    addEntry("Bob", "456 Oak St", "555-5678");
    addEntry("Charlie", "789 Pine St", "555-9012"); // This will trigger resize
    
    cout << "\nAll Contacts:\n";
    displayAll();
    
    cout << "\nSearching for Bob:\n";
    searchContact("Bob");
    
    freeMemory();
    return 0;
}
