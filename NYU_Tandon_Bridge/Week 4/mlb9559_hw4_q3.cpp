/*
NYU Tandon Bridge - Week 4 (Winter 2024)
Decimal to Binary Converter

Assignment:
Write a program that reads from the user a positive integer (in a decimal representation), and
prints its binary (base 2) representation.

Implementation Requirements:
1. You are supposed to implement the algorithm that converts to base 2. You should not use
   any string or special cout functionalities to make the conversion.
2. You are not allowed to use arrays.

Example interaction:
Enter decimal number:
76
The binary representation of 76 is 1001100
*/

#include <iostream>
using namespace std;

int main() {

    int decimalNumber;

    cout<<"Enter decimal number:"<<endl;
    cin>>decimalNumber;

    int divResult,binaryDigit, currentDecimalNumber=decimalNumber;
    int position = 1;
    int binaryNumber=0;

    do{//calculating the rightmost binary digit:
        binaryDigit=currentDecimalNumber%2;
        //making sure that the currentDecimalNumber is updated for the next iteration:
        divResult=currentDecimalNumber/2;
        currentDecimalNumber=divResult;
        //the binary number is the sum of all of its digits multiplied by their position:
        binaryNumber=binaryNumber+binaryDigit*position;
        //making sure the position for the next iteration is updated:
        position = position*10;
    } while (divResult>0);

    cout<<"The binary representation of "<<decimalNumber<<" is "<<binaryNumber;

    return 0;
}