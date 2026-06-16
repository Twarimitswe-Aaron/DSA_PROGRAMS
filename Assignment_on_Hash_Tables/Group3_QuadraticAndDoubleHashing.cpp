// Group 3: Advanced Open Addressing (Quadratic Probing & Double Hashing)
#include <iostream>
using namespace std;

const int TABLE_SIZE = 11; // Prime number for better distribution
const int EMPTY = -1;

class AdvancedHashTable {
private:
    int table[TABLE_SIZE];

    void resetTable() {
        for (int i = 0; i < TABLE_SIZE; i++) table[i] = EMPTY;
    }

public:
    AdvancedHashTable() { resetTable(); }

    int hash1(int key) { return key % TABLE_SIZE; }
    int hash2(int key) { return 7 - (key % 7); } // Secondary hash

    void insertQuadratic(int key) {
        int index = hash1(key);
        int i = 0;
        
        while (table[(index + i * i) % TABLE_SIZE] != EMPTY) {
            cout << "Collision at " << (index + i * i) % TABLE_SIZE << " for key " << key << endl;
            i++;
            if (i >= TABLE_SIZE) { cout << "Table full/cycle detected\n"; return; }
        }
        int pos = (index + i * i) % TABLE_SIZE;
        table[pos] = key;
        cout << "Inserted " << key << " at index " << pos << " (Quadratic)\n";
    }

    void insertDoubleHash(int key) {
        int index = hash1(key);
        int stepSize = hash2(key);
        int i = 0;
        
        while (table[(index + i * stepSize) % TABLE_SIZE] != EMPTY) {
            cout << "Collision at " << (index + i * stepSize) % TABLE_SIZE << " for key " << key << endl;
            i++;
            if (i >= TABLE_SIZE) { cout << "Table full\n"; return; }
        }
        int pos = (index + i * stepSize) % TABLE_SIZE;
        table[pos] = key;
        cout << "Inserted " << key << " at index " << pos << " (Double Hash)\n";
    }
    
    void demonstrate() {
        cout << "--- Quadratic Probing ---" << endl;
        resetTable();
        insertQuadratic(10);
        insertQuadratic(21); // Collides with 10 (both % 11 == 10)
        insertQuadratic(32); // Collides again
        
        cout << "\n--- Double Hashing ---" << endl;
        resetTable();
        insertDoubleHash(10);
        insertDoubleHash(21); // Collides, step size = 7 - (21%7) = 7
        insertDoubleHash(32); // Collides, step size = 7 - (32%7) = 3
    }
};

int main() {
    AdvancedHashTable ht;
    ht.demonstrate();
    return 0;
}
