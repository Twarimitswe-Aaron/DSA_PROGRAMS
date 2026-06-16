// Group 6: Real-World Application - Dictionary & Spell-Checker
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int TABLE_SIZE = 101; // Prime size

class SpellChecker {
private:
    string table[TABLE_SIZE];

    int hashFunction(const string& word) {
        int sum = 0;
        for (char c : word) {
            sum = (sum * 31 + c) % TABLE_SIZE;
        }
        return sum;
    }

public:
    SpellChecker() {
        for (int i = 0; i < TABLE_SIZE; i++) {
            table[i] = ""; // Empty string denotes empty slot
        }
    }

    void addWord(const string& word) {
        int index = hashFunction(word);
        while (table[index] != "" && table[index] != word) {
            index = (index + 1) % TABLE_SIZE; // Linear probing
        }
        table[index] = word;
    }

    bool checkWord(const string& word) {
        int index = hashFunction(word);
        int start = index;
        while (table[index] != "") {
            if (table[index] == word) {
                return true; // Found
            }
            index = (index + 1) % TABLE_SIZE;
            if (index == start) break;
        }
        return false; // Not found
    }
};

int main() {
    SpellChecker dictionary;
    // Load mini-dictionary
    vector<string> validWords = {"apple", "banana", "orange", "grape", "hello", "world"};
    for (const string& w : validWords) {
        dictionary.addWord(w);
    }
    
    cout << "Spell Checker Initialized.\n";
    vector<string> wordsToCheck = {"hello", "aple", "banana", "worl"};
    
    for (const string& w : wordsToCheck) {
        if (dictionary.checkWord(w)) {
            cout << "[OK] '" << w << "' is spelled correctly.\n";
        } else {
            cout << "[ERROR] '" << w << "' is NOT in the dictionary.\n";
        }
    }
    
    return 0;
}
