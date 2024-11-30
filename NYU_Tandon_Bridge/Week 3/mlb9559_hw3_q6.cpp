/*
NYU Tandon Bridge - Week 3 (Winter 2024)
Long-Distance Call Cost Calculator

Original Assignment:
Write a program that computes the cost of a long-distance call based on:
• Weekday 8:00 A.M. - 6:00 P.M.: $0.40 per minute
• Weekday before 8:00 A.M. or after 6:00 P.M.: $0.25 per minute
• Weekend (Saturday/Sunday): $0.15 per minute

Input format:
- Day: Two-character string (Mo/Tu/We/Th/Fr/Sa/Su)
- Time: 24-hour notation (HH:MM)
- Call length: Positive integer (minutes)

Example interaction:
Please enter the day of the week (use the format: Mo/Tu/We/Th/Fr/Sa/Su): Mo
Please enter time the call started (24-hour notation, divided by colon): 13:30
Please enter the length of the call (in minutes): 20
The cost of the call is: 8 dollars
*/

#include <iostream>
#include <string>
using namespace std;

const double weekendRate = 0.15;
const double workingHoursRate = 0.40;
const double afterWorkingHoursRate = 0.25;

int main() {

    string day;
    int startHour, startMinutes;
    int callLenght;
    double callCost;
    char charDivider;

    cout<<"Please enter the day of the week (use the format: Mo/Tu/We/Th/Fr/Sa/Su): ";
    cin>>day;

    cout<<"Please enter time the call started (24-hour notation, divided by colon): ";
    cin>>startHour>>charDivider>>startMinutes;

    cout<<"Please enter the length of the call (in minutes): ";
    cin>>callLenght;

    if (day=="Sa"||day=="Su") {
        callCost = weekendRate * callLenght;
    }
    else {
        if (((startHour>=8) && (startHour<18)) || ((startHour==18) && (startMinutes==0)))
            callCost = workingHoursRate * callLenght;
        else
            callCost = afterWorkingHoursRate * callLenght;
    }

    cout<<"The cost of the call is: "<<callCost<<" dollars";

    return 0;
}