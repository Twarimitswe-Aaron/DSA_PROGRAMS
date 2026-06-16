// 17. Print all subsets (power set) of a given set
#include <iostream>
#include <vector>
using namespace std;

// Recursive function to print all subsets
// Base case: if we reach the end of the set, print the current subset
// Recursive step: 1. exclude current element, 2. include current element
void generateSubsets(int set[], int n, int index, vector<int>& subset) {
    if (index == n) {
        // Base case: print the subset
        cout << "{ ";
        for (int i : subset) {
            cout << i << " ";
        }
        cout << "}" << endl;
        return;
    }
    
    // Recursive step 1: exclude the current element
    generateSubsets(set, n, index + 1, subset);
    
    // Recursive step 2: include the current element
    subset.push_back(set[index]);
    generateSubsets(set, n, index + 1, subset);
    
    // Backtrack (remove the element to explore other combinations)
    subset.pop_back();
}

int main() {
    int set[] = {1, 2, 3};
    int n = sizeof(set) / sizeof(set[0]);
    vector<int> subset;
    
    cout << "All subsets of the set are:" << endl;
    generateSubsets(set, n, 0, subset);
    
    return 0;
}
