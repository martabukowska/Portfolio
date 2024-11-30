/*
NYU Tandon Bridge - Week 6 (Winter 2024)
Efficient Divisor Calculator

Assignment:
Implement void printDivisors(int num) that:
- Takes a positive integer num
- Prints all divisors in ascending order
- Must run in Θ(√num) time
- Divisors should be space-separated

Key Optimization:
- For each divisor d ≤ √num, its pair num/d is also a divisor
- Only need to check up to √num and calculate pairs
- Avoids checking all numbers up to num

Example interaction:
Please enter a positive integer >= 2: 100
1 2 4 5 10 20 25 50 100

Note: The implementation uses two loops:
1. Forward loop up to √num for smaller divisors
2. Backward loop from √num for larger divisors (pairs)
*/

#include <iostream>
#include <cmath>
using namespace std;

void printDivisors(int num);

int main() {

    int num;

    cout<<"Please enter a positive integer >=2: ";
    cin>>num;
    printDivisors(num);

    return 0;
}

void printDivisors(int num) {
    int div2;
    //printing divisors <= sqrt(num)
    for (int div = 1; div <= sqrt(num); div++) {
        if (num % div == 0) {
            cout << div << " ";
        }
    }
    //printing divisors > sqrt(num)
    for (int div = sqrt(num); div >= 1; div--) {
        if (num % div == 0) {
            div2 = num / div;
            if (div2!=div) {
                cout << div2 << " ";
            }
        }
    }
}