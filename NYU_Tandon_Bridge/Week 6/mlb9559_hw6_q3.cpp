/*
NYU Tandon Bridge - Week 6 (Winter 2024)
Euler's Number (e) Approximation Calculator

Assignment:
Calculate an approximation of e using the infinite series:
e = 1 + 1/1! + 1/2! + 1/3! + 1/4! + ...

Implement the function:
double eApprox(int n)
- Takes a positive integer n
- Returns approximation of e using first (n+1) terms
- Should run in O(n) time
- Use double for factorial calculations

Example output with precision(30):
n = 1    2.000000000000000000000000000000
n = 2    2.500000000000000000000000000000
...
n = 15   2.718281826198493239183284649960

Note: The actual value of e is approximately 2.71828

Test program should:
- Set precision to 30 decimal places
- Test approximations for n = 1 to 15
*/

#include <iostream>
using namespace std;

double eApprox (int n);

int main() {

    cout.precision(30);
    for (int n = 1; n <= 15; n++) {
        cout<<"n = "<<n<<'\t'<<eApprox(n)<<endl;
    }

    return 0;
}

//eApprox: returns an approximation of e, calculated by the sum of the first (n+1) addends of the infinite series 1+1/1!+1/2!+1/3!+...

double eApprox (int n){

    double denominator=1.0, e=1;
    for (int i=1; i<=n; i++){
        denominator = denominator * i;
        e=e+(1.0/denominator);
    }

    return e;
}