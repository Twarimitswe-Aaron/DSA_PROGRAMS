// 19. Solve Tower of Hanoi puzzle
#include <iostream>
using namespace std;

// Recursive function to solve Tower of Hanoi
// Base case: if n is 1, move disk from source to destination
// Recursive step: 
// 1. move n-1 disks from source to auxiliary
// 2. move 1 disk from source to destination
// 3. move n-1 disks from auxiliary to destination
void towerOfHanoi(int n, char source, char auxiliary, char destination) {
    if (n == 1) {
        cout << "Move disk 1 from " << source << " to " << destination << endl;
        return; // Base case
    }
    
    // Step 1: move n-1 disks to auxiliary
    towerOfHanoi(n - 1, source, destination, auxiliary);
    
    // Step 2: move the nth disk to destination
    cout << "Move disk " << n << " from " << source << " to " << destination << endl;
    
    // Step 3: move n-1 disks from auxiliary to destination
    towerOfHanoi(n - 1, auxiliary, source, destination);
}

int main() {
    int n = 3; // Number of disks
    cout << "Tower of Hanoi solution for " << n << " disks:" << endl;
    towerOfHanoi(n, 'A', 'B', 'C');
    return 0;
}
