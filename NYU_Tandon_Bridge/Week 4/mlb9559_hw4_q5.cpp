/*
NYU Tandon Bridge - Week 4 (Winter 2024)
Hourglass Pattern Generator

Assignment:
Write a program that asks the user to input a positive integer n, and prints a textual image of an
hourglass made of 2n lines with asterisks.

For example if n=4, the program should print:
*******
 *****
  ***
   *
   *
  ***
 *****
*******

Note: The pattern consists of:
• Top half: decreasing asterisks with increasing spaces
• Middle: single asterisk with maximum spaces
• Bottom half: increasing asterisks with decreasing spaces
*/

#include <iostream>
#include <string>
using namespace std;

int main() {

    int n;
    string asterisk, space;

    cout<<"Please enter a positive integer: ";
    cin>>n;

    n=n*2; //Number of rows is 2n

    //First part of hourglass:

    for (int count = n; count >= 1; count--) {
        space = "";
        asterisk = "";

        for (int countOfSpaces = 1; countOfSpaces <= (n - count)/2; countOfSpaces++){
            space = space + " ";
        }

        for (int countOfAsterisks = 1; countOfAsterisks <= count; countOfAsterisks++){
            asterisk = asterisk + "*";
        }

        if (count%2==1)
            cout << space << asterisk << endl;
    }

    //Second part of hourglass:

    for (int count = 1; count <= n; count++) {
        space = "";
        asterisk = "";

        for (int countOfSpaces = 1; countOfSpaces <= (n - count)/2; countOfSpaces++){
            space = space + " ";
        }

        for (int countOfAsterisks = 1; countOfAsterisks <= count; countOfAsterisks++){
            asterisk = asterisk + "*";
        }

        if (count%2==1)
            cout << space << asterisk << endl;
    }

    return 0;
}