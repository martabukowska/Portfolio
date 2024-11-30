/*
NYU Tandon Bridge - Week 2 (Winter 2024)
Basic Arithmetic Operations Calculator

Original Assignment:
Write a program that reads from the user two positive integers, and prints the result
of when we add, subtract multiply, divide, div and mod them.

Example interaction:
Please enter two positive integers, separated by a space:
14 4
14 + 4 = 18
14 - 4 = 10
14 * 4 = 56
14 / 4 = 3.5
14 div 4 = 3
14 mod 4 = 2
*/

#include <iostream>
using namespace std;

int main() {

    int integer1, integer2;
    int resultOfAddition,  resultOfSubtraction, resultOfMultiplication, resultOfIntegerDivision, resultOfMod;
    double resultOfDivision;

    cout<< "Please enter two positive integers, separated by a space:"<<endl;
    cin>> integer1 >> integer2;

    resultOfAddition = integer1 + integer2;
    resultOfSubtraction = integer1 - integer2;
    resultOfMultiplication = integer1 * integer2;
    resultOfDivision = (double) integer1 / (double) integer2;
    resultOfIntegerDivision = integer1 / integer2;
    resultOfMod = integer1 % integer2;

    cout << integer1 << " + " << integer2 << " = " << resultOfAddition << endl;
    cout << integer1 << " - " << integer2 << " = " <<  resultOfSubtraction << endl;
    cout << integer1 << " * " << integer2 << " = " << resultOfMultiplication << endl;
    cout << integer1 << " / " << integer2 << " = " <<  resultOfDivision << endl;
    cout << integer1 << " div " << integer2 << " = " << resultOfIntegerDivision << endl;
    cout << integer1 << " mod " << integer2 << " = " << resultOfMod<< endl;

    return 0;
}