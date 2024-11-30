/*
NYU Tandon Bridge - Week 4 (Winter 2024)
Even-Digit Dominant Numbers

Assignment:
Write a program that asks the user to input a positive integer n, and print all of the numbers
from 1 to n that have more even digits than odd digits.

For example, if n=30, the program should print:
2
4
6
8
20
22
24
26
28

Note: A number has more even digits than odd digits if it contains more even digits (0,2,4,6,8)
than odd digits (1,3,5,7,9) in its decimal representation.
*/

#include <iostream>
using namespace std;

int main() {

    int n;

    cout<<"Please enter a positive integer: ";
    cin>>n;

    for (int currentNum = 1; currentNum <=n; currentNum++) {
        //we introduce numToCheck, because we will be splitting currentNum into digits, and can't confuse the two:
        int numToCheck = currentNum;
        //making sure that number of even digits and odd digits resets for each currentNum:
        int evenDigits=0;
        int oddDigits=0;

        while (numToCheck!=0){
            int digit=numToCheck%10; //we obtain the rightmost digit, by dividing our numToCheck by 10
            //we check if the digit is odd or even + count number of odd and even digits:
            if (digit%2==0)
                evenDigits=evenDigits+1;
            else
                oddDigits=oddDigits+1;
            //we get rid of the digit we just checked:
            numToCheck = numToCheck / 10;
        }
        //we print the numbers that have more even digits than odd digits:
        if (evenDigits>oddDigits) {
            cout << currentNum << endl;
        }

    }

    return 0;
}