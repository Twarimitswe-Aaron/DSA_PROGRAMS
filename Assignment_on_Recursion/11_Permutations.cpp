// 11. Generate all permutations of a string
#include <iostream>
#include <string>
using namespace std;

// Recursive function to generate permutations
// Base case: if left index reaches the end, print the string
// Recursive step: swap characters and recursively generate permutations
void permute(string& str, int left, int right) {
    if (left == right) {
        cout << str << endl; // Base case: printed a permutation
    } else {
        for (int i = left; i <= right; i++) {
            swap(str[left], str[i]); // Choose
            permute(str, left + 1, right); // Explore (Recursive step)
            swap(str[left], str[i]); // Un-choose (Backtrack)
        }
    }
}

int main() {
    string str = "ABC";
    cout << "Permutations of " << str << " are:" << endl;
    permute(str, 0, str.length() - 1);
    return 0;
}
