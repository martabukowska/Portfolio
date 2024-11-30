/*
NYU Tandon Bridge - Week 3 (Winter 2024)
Quadratic Equation Solver

Original Assignment:
Write a program that does the following:
• Ask user to input three Real numbers a, b and c. They represent the parameters of a
  quadratic equation ax² + bx + c = 0
• Classify to one of the following:
  - 'Infinite number of solutions' (for example, 0x² + 0x + 0 = 0)
  - 'No solution' (for example, 0x² + 0x + 4 = 0)
  - 'No real solution' (for example, x² + 4 = 0)
  - 'One real solution'
  - 'Two real solutions'
• In cases there are 1 or 2 real solutions, also print the solutions.

Notes:
1. If a ≠ 0 and there are real solutions, use quadratic formula: x₁,₂ = (-b ± √(b² - 4ac))/2a
2. Number of solutions depends on whether (b² - 4ac) is positive, zero, or negative
3. Use sqrt function from cmath library for square root calculations

Example interaction:
Please enter value of a: 1
Please enter value of b: 4
Please enter value of c: 4
This equation has a single real solution x=-2.0
*/

#include <iostream>
#include <cmath>
using namespace std;

int main() {

    double a, b, c;
    double delta, sqrtDelta;
    double x, x1, x2;

    cout<<"Please enter value of a: ";
    cin>>a;

    cout<<"Please enter value of b: ";
    cin>>b;

    cout<<"Please enter value of c: ";
    cin>>c;

    delta = b * b - (4.0 * a * c);
    sqrtDelta = sqrt (delta);


     if (delta == 0.0){
         if ((a == 0.0) && (b == 0.0) && (c == 0.0)) {
             cout << "This equation has infinite number of solutions";
         }
         else if ((a == 0.0) && (b == 0.0) && (c != 0.0)) {
             cout << "This equation has no solution";
         }
         else {
             x = (-b) / (2.0 * a);
             cout << "This equation has a single real solution x=" << x;
         }
     }
     else if (delta>0.0) {
         x1 = (-b - sqrtDelta) / (2.0 * a);
         x2 = (-b + sqrtDelta) / (2.0 * a);
         cout << "This equation has two real solutions x_1=" << x1 << " and x_2=" << x2;
     }
    else
        cout<<"This equation has no real solutions";

    return 0;
}
