// 20. Sum of even and odd numbers in a range
#include <iostream>
using namespace std;

// Struct to hold both sums
struct Sums {
    int evenSum;
    int oddSum;
};

// Recursive function to find sums of even and odd numbers in range [start, end]
// Base case: if start > end, sums are 0
// Recursive step: add current number to respective sum, then recurse
Sums sumEvenOdd(int start, int end) {
    if (start > end) {
        return {0, 0}; // Base case
    }
    
    Sums currentSums = sumEvenOdd(start + 1, end); // Recursive step
    
    if (start % 2 == 0) {
        currentSums.evenSum += start;
    } else {
        currentSums.oddSum += start;
    }
    
    return currentSums;
}

int main() {
    int start = 1, end = 10;
    Sums result = sumEvenOdd(start, end);
    
    cout << "Range [" << start << ", " << end << "]:" << endl;
    cout << "Sum of even numbers: " << result.evenSum << endl;
    cout << "Sum of odd numbers: " << result.oddSum << endl;
    return 0;
}
