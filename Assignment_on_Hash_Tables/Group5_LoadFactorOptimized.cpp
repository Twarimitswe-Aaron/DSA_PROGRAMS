// Group 5: Memory Allocation & Load Factor Optimization
#include <iostream>
using namespace std;

class DynamicHashTable {
private:
    int* table;
    int tableSize;
    int numElements;
    const double MAX_LOAD_FACTOR = 0.7;
    const int EMPTY = -1;

    int hashFunction(int key, int size) {
        return key % size;
    }

    void rehash() {
        cout << ">>> Load factor exceeded. Rehashing... <<<\n";
        int oldSize = tableSize;
        int* oldTable = table;

        tableSize *= 2; // Double the size
        table = new int[tableSize];
        for (int i = 0; i < tableSize; i++) table[i] = EMPTY;
        numElements = 0;

        // Re-insert existing elements
        for (int i = 0; i < oldSize; i++) {
            if (oldTable[i] != EMPTY) {
                insert(oldTable[i], true);
            }
        }
        delete[] oldTable;
        cout << ">>> Rehashing complete. New size: " << tableSize << " <<<\n";
    }

public:
    DynamicHashTable() {
        tableSize = 5;
        numElements = 0;
        table = new int[tableSize];
        for (int i = 0; i < tableSize; i++) table[i] = EMPTY;
    }

    void insert(int key, bool isRehashing = false) {
        if (!isRehashing && (double)(numElements + 1) / tableSize > MAX_LOAD_FACTOR) {
            rehash();
        }

        int index = hashFunction(key, tableSize);
        while (table[index] != EMPTY) {
            index = (index + 1) % tableSize; // Linear probing
        }
        table[index] = key;
        numElements++;
        if(!isRehashing) {
            cout << "Inserted " << key << " | Load Factor: " << (double)numElements / tableSize << "\n";
        }
    }
};

int main() {
    DynamicHashTable ht;
    // Insert elements to trigger rehashing
    ht.insert(10);
    ht.insert(20);
    ht.insert(30); 
    ht.insert(40); // This should trigger rehash because 4/5 = 0.8 > 0.7
    ht.insert(50);
    
    return 0;
}
