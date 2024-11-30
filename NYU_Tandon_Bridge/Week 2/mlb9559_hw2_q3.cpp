/*
NYU Tandon Bridge - Week 2 (Winter 2024)
Time Addition Calculator

Assignment:
Suppose John and Bill worked for some time and we want to calculate the total time both of
them worked. Write a program that reads number of days, hours, minutes each of them worked,
and prints the total time both of them worked together as days, hours, minutes.

Hint: Try to adapt the elementary method for addition of numbers to this use.

Example interaction:
Please enter the number of days John has worked: 2
Please enter the number of hours John has worked: 12
Please enter the number of minutes John has worked: 15
Please enter the number of days Bill has worked: 3
Please enter the number of hours Bill has worked: 15
Please enter the number of minutes Bill has worked: 20
The total time both of them worked together is: 6 days, 3 hours and 35 minutes.
*/

#include <iostream>
using namespace std;

int main() {

    const int minutesPerHour = 60, hoursPerDay = 24;

    int numberOfDaysWorkedByJohn, numberOfHoursWorkedByJohn, numberOfMinutesWorkedByJohn;
    int numberOfDaysWorkedByBill, numberOfHoursWorkedByBill, numberOfMinutesWorkedByBill;
    int totalDays, totalHours, totalMinutes;
    int minutesCarryover, hoursCarryover;

    cout << "Please enter the number of days John has worked:";
    cin >> numberOfDaysWorkedByJohn;

    cout << "Please enter the number of hours John has worked:";
    cin >> numberOfHoursWorkedByJohn;

    cout << "Please enter the number of minutes John has worked:";
    cin >> numberOfMinutesWorkedByJohn;

    cout << endl;

    cout << "Please enter the number of days Bill has worked:";
    cin >> numberOfDaysWorkedByBill;

    cout << "Please enter the number of hours Bill has worked:";
    cin >> numberOfHoursWorkedByBill;

    cout << "Please enter the number of minutes Bill has worked:";
    cin >> numberOfMinutesWorkedByBill;

    totalMinutes = (numberOfMinutesWorkedByJohn + numberOfMinutesWorkedByBill) % minutesPerHour;
    minutesCarryover = (numberOfMinutesWorkedByJohn + numberOfMinutesWorkedByBill) / minutesPerHour;
    totalHours = (numberOfHoursWorkedByJohn + numberOfHoursWorkedByBill + minutesCarryover) % hoursPerDay;
    hoursCarryover = (numberOfHoursWorkedByJohn + numberOfHoursWorkedByBill + minutesCarryover) / hoursPerDay;
    totalDays = numberOfDaysWorkedByJohn + numberOfDaysWorkedByBill + hoursCarryover;

    cout << endl;

    cout << "The total time both of them worked together is: "<< totalDays <<" days,"<< totalHours <<" hours and "<< totalMinutes <<" minutes"<<endl;

    return 0;
}