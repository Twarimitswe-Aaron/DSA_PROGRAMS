// Group 2: Collision Resolution using Linear Probing
#include <iostream>
using namespace std;

const int TABLE_SIZE = 10;
const int EMPTY = -1;

class HashTable {
private:
    int table[TABLE_SIZE];

public:
    HashTable() {
        for (int i = 0; i < TABLE_SIZE; i++) {
            table[i] = EMPTY;
        }
    }

    int hashFunction(int key) {
        return key % TABLE_SIZE;
    }

    void insert(int key) {
        int index = hashFunction(key);
        int startIndex = index;
        
        while (table[index] != EMPTY) {
            cout << "Collision for key " << key << " at index " << index << ". Linear probing..." << endl;
            index = (index + 1) % TABLE_SIZE;
            if (index == startIndex) {
                cout << "Hash table is full!" << endl;
                return;
            }
        }
        table[index] = key;
        cout << "Inserted " << key << " at index " << index << endl;
    }

    int lookup(int key) {
        int index = hashFunction(key);
        int startIndex = index;
        
        while (table[index] != EMPTY) {
            if (table[index] == key) {
                return index;
            }
            index = (index + 1) % TABLE_SIZE;
            if (index == startIndex) {
                break;
            }
        }
        return -1; // Not found
    }
    
    void display() {
        cout << "\nHash Table:\n";
        for (int i = 0; i < TABLE_SIZE; i++) {
            cout << i << " : " << (table[i] == EMPTY ? "EMPTY" : to_string(table[i])) << endl;
        }
    }
};

int main() {
    HashTable ht;
    // Keys designed to cause collisions (multiples of 10)
    ht.insert(10);
    ht.insert(20);
    ht.insert(30);
    ht.insert(40);
    ht.insert(50);
    
    ht.display();
    
    cout << "\nLookup 30: Index " << ht.lookup(30) << endl;
    return 0;
}
