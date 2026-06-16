// Group 7: Real-World Application - Frequency Counter Engine
#include <iostream>
#include <string>
#include <sstream>
using namespace std;

// Node for chaining to handle word frequencies
struct HashNode {
    string word;
    int count;
    HashNode* next;
    HashNode(string w) : word(w), count(1), next(nullptr) {}
};

const int TABLE_SIZE = 50;

class FrequencyCounter {
private:
    HashNode* table[TABLE_SIZE];

    int hashFunction(const string& word) {
        int sum = 0;
        for (char c : word) sum += c;
        return sum % TABLE_SIZE;
    }

public:
    FrequencyCounter() {
        for (int i = 0; i < TABLE_SIZE; i++) table[i] = nullptr;
    }

    void processWord(const string& word) {
        int index = hashFunction(word);
        HashNode* current = table[index];
        
        while (current != nullptr) {
            if (current->word == word) {
                current->count++; // Word exists, increment count
                return;
            }
            current = current->next;
        }
        
        // Word not found, add new node
        HashNode* newNode = new HashNode(word);
        newNode->next = table[index];
        table[index] = newNode;
    }

    void displayFrequencies() {
        cout << "\nWord Frequencies:\n";
        for (int i = 0; i < TABLE_SIZE; i++) {
            HashNode* current = table[i];
            while (current != nullptr) {
                cout << current->word << " : " << current->count << "\n";
                current = current->next;
            }
        }
    }
};

int main() {
    FrequencyCounter counter;
    string text = "this is a test this is only a test of the hash table test";
    
    stringstream ss(text);
    string word;
    while (ss >> word) {
        counter.processWord(word);
    }
    
    counter.displayFrequencies();
    return 0;
}
