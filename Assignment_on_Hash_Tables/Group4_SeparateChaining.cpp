// Group 4: Collision Resolution via Separate Chaining
#include <iostream>
using namespace std;

// Node for Linked List
struct Node {
    int key;
    Node* next;
    Node(int k) : key(k), next(nullptr) {}
};

const int TABLE_SIZE = 7;

class ChainingHashTable {
private:
    Node* table[TABLE_SIZE];

public:
    ChainingHashTable() {
        for (int i = 0; i < TABLE_SIZE; i++) {
            table[i] = nullptr;
        }
    }

    int hashFunction(int key) {
        return key % TABLE_SIZE;
    }

    void insert(int key) {
        int index = hashFunction(key);
        Node* newNode = new Node(key);
        // Insert at the beginning of the list (O(1))
        newNode->next = table[index];
        table[index] = newNode;
        cout << "Inserted " << key << " at index " << index << endl;
    }

    void remove(int key) {
        int index = hashFunction(key);
        Node* current = table[index];
        Node* prev = nullptr;

        while (current != nullptr && current->key != key) {
            prev = current;
            current = current->next;
        }

        if (current == nullptr) {
            cout << "Key " << key << " not found.\n";
            return;
        }

        if (prev == nullptr) { // First node
            table[index] = current->next;
        } else {
            prev->next = current->next;
        }
        delete current;
        cout << "Deleted " << key << "\n";
    }

    void display() {
        cout << "\nHash Table (Chaining):\n";
        for (int i = 0; i < TABLE_SIZE; i++) {
            cout << "[" << i << "] -> ";
            Node* current = table[i];
            while (current != nullptr) {
                cout << current->key << " -> ";
                current = current->next;
            }
            cout << "NULL\n";
        }
    }
};

int main() {
    ChainingHashTable ht;
    ht.insert(10);
    ht.insert(17); // Collision with 10 (17 % 7 == 3, 10 % 7 == 3)
    ht.insert(24); // Collision again
    ht.insert(5);
    
    ht.display();
    
    ht.remove(17);
    ht.display();
    
    return 0;
}
