// Group 8: Performance Benchmark - Open Addressing vs. Chaining
#include <iostream>
#include <chrono>
#include <vector>
using namespace std;
using namespace std::chrono;

const int TABLE_SIZE = 1000;

// Simple Node for Chaining
struct Node {
    int key; Node* next;
    Node(int k) : key(k), next(nullptr) {}
};

class BenchmarkTesting {
public:
    int openTable[TABLE_SIZE];
    Node* chainTable[TABLE_SIZE];

    BenchmarkTesting() {
        for(int i=0; i<TABLE_SIZE; i++) {
            openTable[i] = -1;
            chainTable[i] = nullptr;
        }
    }

    int hash(int key) { return key % TABLE_SIZE; }

    void insertOpen(int key) {
        int index = hash(key);
        while(openTable[index] != -1) {
            index = (index + 1) % TABLE_SIZE;
        }
        openTable[index] = key;
    }

    void insertChain(int key) {
        int index = hash(key);
        Node* n = new Node(key);
        n->next = chainTable[index];
        chainTable[index] = n;
    }

    bool lookupOpen(int key) {
        int index = hash(key);
        int start = index;
        while(openTable[index] != -1) {
            if(openTable[index] == key) return true;
            index = (index + 1) % TABLE_SIZE;
            if(index == start) break;
        }
        return false;
    }

    bool lookupChain(int key) {
        int index = hash(key);
        Node* curr = chainTable[index];
        while(curr) {
            if(curr->key == key) return true;
            curr = curr->next;
        }
        return false;
    }
};

int main() {
    BenchmarkTesting test;
    vector<int> data;
    for(int i=0; i<900; i++) data.push_back(rand() % 10000); // 90% load factor

    // Insert Data
    for(int k : data) {
        test.insertOpen(k);
        test.insertChain(k);
    }

    // Benchmark Open Addressing Lookup
    auto start1 = high_resolution_clock::now();
    for(int k : data) test.lookupOpen(k);
    auto end1 = high_resolution_clock::now();
    
    // Benchmark Chaining Lookup
    auto start2 = high_resolution_clock::now();
    for(int k : data) test.lookupChain(k);
    auto end2 = high_resolution_clock::now();

    cout << "Open Addressing Lookup Time: " << duration_cast<microseconds>(end1 - start1).count() << " us\n";
    cout << "Separate Chaining Lookup Time: " << duration_cast<microseconds>(end2 - start2).count() << " us\n";

    return 0;
}
