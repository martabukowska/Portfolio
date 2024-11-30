/*
NYU Tandon Bridge - Week 2 (Winter 2024)
Coin Calculator

Assignment:
Write a program that asks the user to enter a number of quarters, dimes, nickels
and pennies and then outputs the monetary value of the coins in the format of
dollars and remaining cents. Your program should interact with the user exactly
as shown in the following example:

Please enter number of coins:
# of quarters: 13
# of dimes: 4
# of nickels: 1
# of pennies: 17
The total is 4 dollars and 37 cents
*/

#include <iostream>
using namespace std;

int main() {

    const int centsPerQuarter = 25, centsPerDime = 10, centsPerNickel = 5, centsPerDollar = 100;

    int numberOfQuarters, numberOfDimes, numberOfNickels, numberOfPennies;
    int totalInCents, totalDollars, totalCents;

    cout << "Please enter number of coins:" << endl;
    cout << "# of quarters:";
    cin >> numberOfQuarters;

    cout << "# of dimes:";
    cin >> numberOfDimes;

    cout << "# of nickels:";
    cin >> numberOfNickels;

    cout << "# of pennies:";
    cin >> numberOfPennies;

    totalInCents = (numberOfQuarters * centsPerQuarter) + (numberOfDimes * centsPerDime) + (numberOfNickels * centsPerNickel) + numberOfPennies;
    totalDollars = totalInCents / centsPerDollar;
    totalCents = totalInCents % centsPerDollar;

    cout << "The total is " << totalDollars << " dollars and " << totalCents << " cents" << endl;

    return 0;
}