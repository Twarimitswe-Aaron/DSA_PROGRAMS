// Group 1: Hash Function Design & Uniform Index Distribution
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int TABLE_SIZE = 100;

// 1. Division Method (using sum of ASCII values)
int divisionHash(const string& key) {
    int sum = 0;
    for (char c : key) {
        sum += c;
    }
    return sum % TABLE_SIZE;
}

// 2. Folding Method (grouping characters by 2s)
int foldingHash(const string& key) {
    int sum = 0;
    for (size_t i = 0; i < key.length(); i += 2) {
        int group = key[i];
        if (i + 1 < key.length()) {
            group = group * 256 + key[i + 1];
        }
        sum += group;
    }
    return sum % TABLE_SIZE;
}

// 3. Mid-Square Method
int midSquareHash(const string& key) {
    int sum = 0;
    for (char c : key) {
        sum += c;
    }
    long long squared = (long long)sum * sum;
    // Extract middle digits (simplified by taking modulo and division)
    return (squared / 10) % TABLE_SIZE;
}

int main() {
    vector<string> names = {"Alice", "Bob", "Charlie", "Diana", "Eve", 
                            "Frank", "Grace", "Heidi", "Ivan", "Judy",
                            "Mallory", "Nina", "Oscar", "Peggy", "Trent"};
                            
    cout << "Hash Distributions:" << endl;
    for (const string& name : names) {
        cout << name << " -> Division: " << divisionHash(name) 
             << " | Folding: " << foldingHash(name) 
             << " | MidSquare: " << midSquareHash(name) << endl;
    }
    return 0;
}
