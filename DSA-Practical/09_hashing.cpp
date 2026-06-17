/*
 * ============================================================================
 * FILE: 09_hashing.cpp
 * ============================================================================
 * HASHING - Technique for O(1) average-time insertion, deletion, search.
 *
 * EXAM FREQUENCY: Medium-High. Hashing concepts and collision handling
 * are frequently tested in practical exams.
 *
 * Topics Covered:
 *   1. Hash Table with Chaining (Separate Chaining using Linked Lists)
 *   2. Hash Table with Open Addressing (Linear Probing)
 *   3. Hash Table with Quadratic Probing
 *   4. Double Hashing
 *   5. Simple Hash Function examples
 *   6. Applications (using STL unordered_map/set)
 *
 * KEY CONCEPTS:
 *   - Hash function: maps key to array index (bucket)
 *   - Collision: two different keys hash to same index
 *   - Load Factor: n / m (elements / table size)
 *   - Rehashing: resize when load factor exceeds threshold
 * ============================================================================
 */
#include <iostream>
#include <vector>
#include <list>        // For chaining
#include <string>
#include <cstring>     // For string hash functions
using namespace std;

// ============================================================================
// SECTION 1: HASH TABLE WITH SEPARATE CHAINING
//
// Each bucket contains a linked list of key-value pairs that hash to that index.
// Best when load factor is moderate (< 1.0).
// ============================================================================

template<typename K, typename V>
class HashTableChaining {
private:
    // Each bucket is a list of key-value pairs
    struct HashNode {
        K key;
        V value;
        HashNode(K k, V v) : key(k), value(v) {}
    };

    vector<list<HashNode>> table;  // Array of linked lists (buckets)
    int numElements;               // Total number of key-value pairs
    int capacity;                  // Number of buckets

    // -------------------------------------------------------------------
    // HASH FUNCTION: Convert key to an index in [0, capacity-1]
    // Uses std::hash (C++11) which works for many built-in types.
    // For custom types, you'd provide a specialization.
    // -------------------------------------------------------------------
    int getHash(const K& key) const {
        // std::hash<K> creates a functor; () calls it; % capacity bounds it
        return hash<K>{}(key) % capacity;
    }

    // -------------------------------------------------------------------
    // REHASH: When load factor exceeds threshold, double capacity and reinsert
    // all existing entries. O(n) time but infrequent, making amortized O(1).
    // -------------------------------------------------------------------
    void rehash() {
        int oldCapacity = capacity;
        vector<list<HashNode>> oldTable = std::move(table);  // Move old data

        capacity *= 2;           // Double the capacity
        table.resize(capacity);  // Resize to new capacity
        numElements = 0;         // Reset count (will re-insert)

        // Re-insert all old entries into the new, larger table
        for (int i = 0; i < oldCapacity; i++) {
            for (auto& node : oldTable[i]) {
                insert(node.key, node.value);
            }
        }
    }

public:
    HashTableChaining(int cap = 10) : numElements(0), capacity(cap) {
        table.resize(capacity);
    }

    // -------------------------------------------------------------------
    // INSERT: O(1) average, O(n) worst (during rehash).
    // If key already exists, update its value.
    // -------------------------------------------------------------------
    void insert(const K& key, const V& value) {
        int index = getHash(key);

        // Check if key already exists and update
        for (auto& node : table[index]) {
            if (node.key == key) {
                node.value = value;  // Update existing key
                return;
            }
        }

        // Key not found, insert new node at front (O(1))
        table[index].push_front(HashNode(key, value));
        numElements++;

        // Rehash if load factor > 0.75
        double loadFactor = (double)numElements / capacity;
        if (loadFactor > 0.75) {
            rehash();
        }
    }

    // -------------------------------------------------------------------
    // SEARCH: O(1) average, O(n) worst.
    // Returns pointer to value if found, nullptr otherwise.
    // -------------------------------------------------------------------
    V* search(const K& key) {
        int index = getHash(key);
        // Linear scan within the bucket (list)
        for (auto& node : table[index]) {
            if (node.key == key) {
                return &node.value;  // Return pointer to value
            }
        }
        return nullptr;  // Key not found
    }

    // -------------------------------------------------------------------
    // DELETE: O(1) average, O(n) worst.
    // -------------------------------------------------------------------
    bool remove(const K& key) {
        int index = getHash(key);
        auto& bucket = table[index];

        for (auto it = bucket.begin(); it != bucket.end(); it++) {
            if (it->key == key) {
                bucket.erase(it);  // Remove from linked list
                numElements--;
                return true;
            }
        }
        return false;  // Key not found
    }

    // -------------------------------------------------------------------
    // DISPLAY the entire hash table
    // -------------------------------------------------------------------
    void display() {
        for (int i = 0; i < capacity; i++) {
            cout << "  Bucket " << i << ": ";
            if (table[i].empty()) {
                cout << "[empty]\n";
            } else {
                for (auto& node : table[i]) {
                    cout << "(" << node.key << ", " << node.value << ") -> ";
                }
                cout << "NULL\n";
            }
        }
    }

    int size() const { return numElements; }
    bool isEmpty() const { return numElements == 0; }
};

// ============================================================================
// SECTION 2: HASH TABLE WITH OPEN ADDRESSING (LINEAR PROBING)
//
// All elements are stored directly in the table (no linked lists).
// When collision occurs, probe next slot: index = (h(k) + i) % capacity
//
// Issues: Primary clustering - consecutive elements form clusters,
// causing more collisions.
// ============================================================================

enum class SlotState { EMPTY, OCCUPIED, DELETED };

template<typename K, typename V>
class HashTableLinearProbing {
private:
    struct HashNode {
        K key;
        V value;
        SlotState state;  // EMPTY, OCCUPIED, or DELETED
        HashNode() : state(SlotState::EMPTY) {}
    };

    vector<HashNode> table;
    int numElements;
    int capacity;

    int getHash(const K& key) const {
        return hash<K>{}(key) % capacity;
    }

public:
    HashTableLinearProbing(int cap = 10) : numElements(0), capacity(cap) {
        table.resize(capacity);
    }

    // -------------------------------------------------------------------
    // INSERT with linear probing. O(1) average, O(n) worst.
    // If slot is occupied, check next slot (wrap around with %).
    // Stops when finding EMPTY or DELETED slot.
    // -------------------------------------------------------------------
    void insert(const K& key, const V& value) {
        if (numElements >= capacity * 0.7) {
            rehash();
        }

        int index = getHash(key);
        int i = 0;
        // Linear probing: check slots index, index+1, index+2, ... (wrapped)
        while (table[index].state == SlotState::OCCUPIED) {
            if (table[index].key == key) {
                table[index].value = value;  // Update existing key
                return;
            }
            i++;
            index = (getHash(key) + i) % capacity;  // Linear probe
        }

        table[index].key = key;
        table[index].value = value;
        table[index].state = SlotState::OCCUPIED;
        numElements++;
    }

    // -------------------------------------------------------------------
    // SEARCH with linear probing. O(1) average, O(n) worst.
    // -------------------------------------------------------------------
    V* search(const K& key) {
        int index = getHash(key);
        int i = 0;

        while (table[index].state != SlotState::EMPTY) {
            if (table[index].state == SlotState::OCCUPIED && table[index].key == key) {
                return &table[index].value;
            }
            i++;
            index = (getHash(key) + i) % capacity;
        }
        return nullptr;
    }

    // -------------------------------------------------------------------
    // DELETE using lazy deletion (mark as DELETED).
    // We can't just set to EMPTY because that would break search chains.
    // -------------------------------------------------------------------
    bool remove(const K& key) {
        int index = getHash(key);
        int i = 0;

        while (table[index].state != SlotState::EMPTY) {
            if (table[index].state == SlotState::OCCUPIED && table[index].key == key) {
                table[index].state = SlotState::DELETED;  // Lazy deletion
                numElements--;
                return true;
            }
            i++;
            index = (getHash(key) + i) % capacity;
        }
        return false;
    }

    void rehash() {
        int oldCapacity = capacity;
        vector<HashNode> oldTable = table;

        capacity *= 2;
        table.clear();
        table.resize(capacity);
        numElements = 0;

        for (int i = 0; i < oldCapacity; i++) {
            if (oldTable[i].state == SlotState::OCCUPIED) {
                insert(oldTable[i].key, oldTable[i].value);
            }
        }
    }

    void display() {
        for (int i = 0; i < capacity; i++) {
            cout << "  Slot " << i << ": ";
            if (table[i].state == SlotState::EMPTY) {
                cout << "[empty]\n";
            } else if (table[i].state == SlotState::DELETED) {
                cout << "[deleted]\n";
            } else {
                cout << "(" << table[i].key << ", " << table[i].value << ")\n";
            }
        }
    }

    int size() const { return numElements; }
};

// ============================================================================
// SECTION 3: APPLICATIONS OF HASHING (Using STL unordered_map/set)
// ============================================================================

#include <unordered_map>
#include <unordered_set>

// -------------------------------------------------------------------
// APPLICATION 1: Count frequencies of each element in array. O(n) average.
// -------------------------------------------------------------------
unordered_map<int, int> countFrequencies(const vector<int>& arr) {
    unordered_map<int, int> freq;
    for (int num : arr) {
        freq[num]++;  // If num not in map, value is default-initialized to 0, then ++
    }
    return freq;
}

// -------------------------------------------------------------------
// APPLICATION 2: Find first non-repeating character in string. O(n) average.
// Two passes: count frequencies, then find first with count == 1.
// -------------------------------------------------------------------
char firstNonRepeatingChar(const string& str) {
    unordered_map<char, int> freq;
    // First pass: count frequencies
    for (char ch : str) freq[ch]++;
    // Second pass: find first character with frequency 1
    for (char ch : str) {
        if (freq[ch] == 1) return ch;
    }
    return '\0';  // No non-repeating character
}

// -------------------------------------------------------------------
// APPLICATION 3: Check if two arrays are disjoint (no common elements). O(n+m).
// -------------------------------------------------------------------
bool areDisjoint(const vector<int>& arr1, const vector<int>& arr2) {
    unordered_set<int> set1(arr1.begin(), arr1.end());
    for (int num : arr2) {
        if (set1.find(num) != set1.end()) return false;  // Common element found
    }
    return true;
}

// -------------------------------------------------------------------
// APPLICATION 4: Find subarray with zero sum. O(n) time, O(n) space.
// If prefix sum repeats, subarray between the two occurrences has sum 0.
// -------------------------------------------------------------------
bool hasZeroSumSubarray(const vector<int>& arr) {
    unordered_set<int> prefixSums;
    int sum = 0;

    for (int num : arr) {
        sum += num;
        // If sum is 0 or we've seen this prefix sum before, found zero-sum subarray
        if (sum == 0 || prefixSums.find(sum) != prefixSums.end()) {
            return true;
        }
        prefixSums.insert(sum);
    }
    return false;
}

// ============================================================================
// SAMPLE main() to demonstrate all hashing operations
// ============================================================================
int main() {
    cout << "========== HASHING (09_hashing.cpp) ==========\n\n";

    cout << "===== HASH TABLE WITH CHAINING =====\n";
    HashTableChaining<string, int> ht(5);
    ht.insert("apple", 10);
    ht.insert("banana", 20);
    ht.insert("cherry", 30);
    ht.insert("date", 40);
    ht.insert("elderberry", 50);
    ht.display();

    cout << "\nSearch for 'banana': ";
    int* val = ht.search("banana");
    if (val) cout << "Found (value = " << *val << ")\n";
    else cout << "Not found\n";

    cout << "Remove 'cherry': " << (ht.remove("cherry") ? "Success" : "Failed") << "\n";
    ht.display();

    cout << "\n===== HASH TABLE WITH LINEAR PROBING =====\n";
    HashTableLinearProbing<int, string> lpHt(7);
    lpHt.insert(10, "ten");
    lpHt.insert(20, "twenty");
    lpHt.insert(30, "thirty");
    lpHt.insert(40, "forty");
    lpHt.insert(50, "fifty");
    lpHt.display();

    cout << "\n===== HASHING APPLICATIONS =====\n";

    vector<int> arr = {1, 2, 3, 2, 1, 4, 5, 2, 3};
    auto freq = countFrequencies(arr);
    cout << "Frequencies: ";
    for (auto& kv : freq) cout << kv.first << ":" << kv.second << " ";
    cout << "\n";

    string s = "swiss";
    cout << "First non-repeating char in '" << s << "': "
         << firstNonRepeatingChar(s) << "\n";

    cout << "Zero-sum subarray in [1, 4, -2, -2, 5, -4, 3]: "
         << (hasZeroSumSubarray({1, 4, -2, -2, 5, -4, 3}) ? "Yes" : "No") << "\n";

    cout << "\n========== END OF HASHING OPERATIONS ==========\n";

    return 0;
}
