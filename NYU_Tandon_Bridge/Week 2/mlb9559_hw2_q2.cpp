/*
NYU Tandon Bridge - Week 2 (Winter 2024)
Coin Change Calculator

Assignment:
Write a program that asks the user to enter an amount of money in the format of dollars and
remaining cents. The program should calculate and print the minimum number of coins
(quarters, dimes, nickels and pennies) that are equivalent to the given amount.

Hint: In order to find the minimum number of coins, first find the maximum number of
quarters that fit in the given amount of money, then find the maximum number of dimes
that fit in the remaining amount, and so on.

Example interaction:
Please enter your amount in the format of dollars and cents separated by a space:
4 37
4 dollars and 37 cents are:
17 quarters, 1 dimes, 0 nickels and 2 pennies
*/

#include <iostream>
using namespace std;

int main() {

    const int centsPerQuarter = 25, centsPerDime = 10, centsPerNickel = 5, centsPerDollar = 100;

    int amountOfDollars, amountOfCents;
    int totalCents;
    int quarters, dimes, nickels, pennies;

    cout<<"Please enter your amount in the format of dollars and cents separated by a space:"<< endl;
    cin>> amountOfDollars >> amountOfCents;

    cout<< amountOfDollars << " dollars and " << amountOfCents << " cents are:"<< endl;

    totalCents = (amountOfDollars * centsPerDollar) + amountOfCents;

    quarters = totalCents / centsPerQuarter;
    dimes = (totalCents % centsPerQuarter) / centsPerDime;
    nickels = ((totalCents % centsPerQuarter) % centsPerDime) / centsPerNickel;
    pennies = ((totalCents % centsPerQuarter) % centsPerDime) % centsPerNickel;

    cout<< quarters << " quarters," << dimes << " dimes," << nickels << " nickels and "<< pennies << " pennies"<< endl;

    return 0;
}