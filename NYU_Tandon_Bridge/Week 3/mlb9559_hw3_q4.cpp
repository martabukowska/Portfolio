/*
NYU Tandon Bridge - Week 3 (Winter 2024)
Number Rounding Calculator

Assignment:
Write a program that asks the user to enter a Real number, then asks the user to enter the
method by which they want to round that number (floor, ceiling or to the nearest integer).
The program will then print the rounded result.

Define the following constants:
const int FLOOR_ROUND = 1;
const int CEILING_ROUND = 2;
const int ROUND = 3;

Implementation requirements:
1. Use a switch statement.
2. You are not allowed to include and use the math library.

Example interaction:
Please enter a Real number:
4.78
Choose your rounding method:
1. Floor round
2. Ceiling round
3. Round to the nearest whole number
2
5
*/

#include <iostream>
using namespace std;

const int FLOOR_ROUND = 1;
const int CEILING_ROUND = 2;
const int ROUND = 3;

int main() {

    double realNumber;
    int roundingMethod;
    int integerPart;
    double fractionalPart;
    int roundedNumber;

    cout<<"Please enter a Real number:"<<endl;
    cin>>realNumber;

    cout<<"Choose your rounding method:"<<endl;
    cout<<"1. Floor round"<<endl;
    cout<<"2. Ceiling round"<<endl;
    cout<<"3. Round to the nearest whole number"<<endl;
    cin>>roundingMethod;

    integerPart = realNumber;
    fractionalPart = realNumber - integerPart;

    switch(roundingMethod) {
        case FLOOR_ROUND:
            if (realNumber >= 0) {
                    roundedNumber = integerPart;
            }
            else {
                if (fractionalPart ==0)
                    roundedNumber = integerPart;
                else
                    roundedNumber = integerPart - 1;
            }
            break;
        case CEILING_ROUND:
            if (realNumber >= 0) {
                if (fractionalPart == 0)
                    roundedNumber = integerPart;
                else
                    roundedNumber = integerPart + 1;
            } else {
                if (fractionalPart == 0)
                    roundedNumber = integerPart;
                else
                    roundedNumber = realNumber - fractionalPart;
            }
            break;
        case ROUND:
            if (realNumber >= 0) {
                if (fractionalPart >= 0.5)
                    roundedNumber = integerPart + 1;
                else
                    roundedNumber = integerPart;
            }
            else {
                if (fractionalPart <= -0.5) {
                    roundedNumber = integerPart - 1;
                } else
                    roundedNumber = integerPart;
            }
            break;
        default:
            break;
    }

    cout<<roundedNumber<<endl;

    return 0;
}
