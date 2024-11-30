/*
NYU Tandon Bridge - Week 4 (Winter 2024)
Simplified Roman Numerals Converter

Assignment:
Write a program that converts decimal numbers to a simplified Roman numeral system.
Requirements:
• Convert decimal numbers using Roman digits I, V, X, L, C, D and M
• Follow monotonically non-increasing sequence rule
• Enforce digit appearance limits:
  - M: unlimited appearances
  - D, L, V: max 1 appearance each
  - C, X, I: max 4 appearances each
• Validate input and generate proper Roman numeral output

Example interaction:
Enter decimal number:
147
147 is CXXXXVII
*/

#include <iostream>
#include <string>
using namespace std;

int main() {

    int decimalNumber;

    cout<<"Enter decimal number: "<<endl;
    cin>>decimalNumber;

    int currentNumber=decimalNumber;

    //if the user input is 0:
    if (decimalNumber==0) {
        cout << "Roman numbers don't represent 0";
    }
    //if the user input is negative decimal number:
    else if (decimalNumber<0) {
        cout << "Roman numbers don't represent negative numbers";
    }
    else {
        //if the user input is positive decimal number:
        cout << decimalNumber << " is ";
    }

    //printing 1000s
    if (currentNumber>=1000) {
        //determining the number of 1000s
        int numberOf1000s = currentNumber/1000;
        //printing M times numberOf1000s
        while (numberOf1000s>0) {
            string RomanNumber="M";
            cout<<RomanNumber;
            numberOf1000s=numberOf1000s-1;
        }
        //removing 1000s form current number, as they are already printed
        currentNumber=currentNumber%1000;
    }

    //printing 500s
    if (currentNumber>=500) {
        //determining the number of 500s
        int numberOf500s = currentNumber/500;
        //printing D times numberOf500s
        while (numberOf500s>0) {
            string RomanNumber="D";
            cout<<RomanNumber;
            numberOf500s=numberOf500s-1;
        }
        //removing 500s form current number, as they are already printed
        currentNumber=currentNumber%500;
    }

    //printing 100s
    if (currentNumber>=100) {
        //determining the number of 100s
        int numberOf100s = currentNumber/100;
        //printing C times numberOf100s
        while (numberOf100s>0) {
            string RomanNumber="C";
            cout<<RomanNumber;
            numberOf100s=numberOf100s-1;
        }
        //removing 100s form current number, as they are already printed
        currentNumber=currentNumber%100;
    }

    //printing 50s
    if (currentNumber>=50) {
        //determining the number of 50s
        int numberOf50s = currentNumber/50;
        //printing L times numberOf50s
        while (numberOf50s>0) {
            string RomanNumber="L";
            cout<<RomanNumber;
            numberOf50s=numberOf50s-1;
        }
        //removing 50s form current number, as they are already printed
        currentNumber=currentNumber%50;
    }

    //printing 10s
    if (currentNumber>=10) {
        //determining the number of 10s
        int numberOf10s = currentNumber/10;
        //printing X times numberOf10s
        while (numberOf10s>0) {
            string RomanNumber="X";
            cout<<RomanNumber;
            numberOf10s=numberOf10s-1;
        }
        //removing 10s form current number, as they are already printed
        currentNumber=currentNumber%10;
    }

    //printing 5s
    if (currentNumber>=5) {
        //determining the number of 5s
        int numberOf5s = currentNumber/5;
        //printing X times numberOf10s
        while (numberOf5s>0) {
            string RomanNumber="V";
            cout<<RomanNumber;
            numberOf5s=numberOf5s-1;
        }
        //removing 5s form current number, as they are already printed
        currentNumber=currentNumber%5;
    }

    //printing 1s
    if (currentNumber>=1) {
        //determining the number of 1s
        int numberOf1s = currentNumber/1;
        //printing X times numberOf1s
        while (numberOf1s>0) {
            string RomanNumber="I";
            cout<<RomanNumber;
            numberOf1s=numberOf1s-1;
        }
    }

    return 0;
}