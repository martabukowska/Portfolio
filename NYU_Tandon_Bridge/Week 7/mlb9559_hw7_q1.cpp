/*
NYU Tandon Bridge - Week 7 (Winter 2024)
Calendar Generator

Assignment:
Implement three functions:
1. int printMonthCalender(int numOfDays, int startingDay)
   - Prints formatted monthly calendar
   - Returns day of week for last day of month
   - Uses 1-7 for Mon-Sun

2. bool leap(int year)
   - Determines if year is leap year
   - Uses Gregorian calendar rules:
     * Divisible by 4 but not 100, or
     * Divisible by 400

3. void printYearCalender(int year, int startingDay)
   - Prints complete year calendar
   - Uses above functions
   - Includes month names and year

Example interaction:
Please enter a year: 2024
Please enter an integer from 1 to 7 representing the day of the week for January 1st: 1

January 2024
Mon Tue Wed Thr Fri Sat Sun
1   2   3   4   5   6   7
...

Note: Calendar should handle:
- Varying month lengths
- Leap years
- Proper day alignment
- Month transitions
*/

#include <iostream>
#include <string>
using namespace std;

int printMonthCalender(int numOfDays, int startingDay);

bool leap (int year);

void printYearCalender(int year, int startingDay);

int main() {

    int year, startingDay;

    cout<<"Please enter a year: ";
    cin>>year;

    cout<<"Please enter an integer from 1 to 7 representing the day of the week for January 1st of the entered year: ";
    cin>>startingDay;

    printYearCalender(year, startingDay);

    return 0;
}

//Function printMonthCalender takes as an input the number of days in given month (numOfDays) and
//a number denoting a day of the week of the 1st day of given month (startingDay; integer 1-7, where 1 is Monday).
//It prints the calendar for given month and returns number representing the day in the week of last day in the month (weekDay)
int printMonthCalender(int numOfDays, int startingDay){

    //Printing header of the monthly calendar:
    cout<<"Mon\tTue\tWed\tThr\tFri\tSat\tSun"<<endl;

    //Printing tabs to align the starting day of the month:
    for (int i=1; i<startingDay; i++){
        cout<<"\t";
    }

    //Printing all the days of given month (starting from a startingDay to numOfDays)
    //Starting a new line every time the current day is a Sunday, unless it's also the last day of the month
    //Printing tabs after the number of the day, unless it's the last day of the month
    //Breaking out of the loop if the current day is the last day of the month
    int dayNum=1, weekDay=startingDay;
    for (int i=1; i<=numOfDays; i++) {
        if (dayNum != numOfDays) {
            if (weekDay == 7) {
                cout << dayNum << endl;
                weekDay = 1; //Resetting weekDay to Monday
            } else {
                cout << dayNum << "\t";
                weekDay = weekDay + 1; //Updating the weekDay for next iteration
            }
        } else {
            cout << dayNum;
            break;
        }
        dayNum=dayNum+1; //Updating the day number for the next iteration
    }
        cout<<endl;

    //Returning last day's day of the week:
    return weekDay;
}

//Function leaps takes an integer year from the user and returns true if the year is leap or false if it's not leap
bool leap (int year){
    bool isLeap;
    if (((year%4==0)&&(year%100!=0)) || (year%400==0))
        isLeap=true;
    else
        isLeap=false;
    return isLeap;
}

//Function printYearCalender takes as input a year and a number representing a day of the week of January 1st of given year.
//It prints a calendar for given year, also taking into consideration if its leap or not
void printYearCalender(int year, int startingDay) {

    cout<<"January "<<year<<endl;
    startingDay=(printMonthCalender(31,startingDay)%7)+1;
    cout<<endl;

    cout<<"February "<<year<<endl;
    if (leap(year)==false)
        startingDay=(printMonthCalender(28,startingDay)%7)+1;
    else
        startingDay=(printMonthCalender(29,startingDay)%7)+1;
    cout<<endl;

    cout<<"March "<<year<<endl;
    startingDay=(printMonthCalender(31,startingDay)%7)+1;
    cout<<endl;

    cout<<"April "<<year<<endl;
    startingDay=(printMonthCalender(30,startingDay)%7)+1;
    cout<<endl;

    cout<<"May "<<year<<endl;
    startingDay=(printMonthCalender(31,startingDay)%7)+1;
    cout<<endl;

    cout<<"June "<<year<<endl;
    startingDay=(printMonthCalender(30,startingDay)%7)+1;
    cout<<endl;

    cout<<"July "<<year<<endl;
    startingDay=(printMonthCalender(31,startingDay)%7)+1;
    cout<<endl;

    cout<<"August "<<year<<endl;
    startingDay=(printMonthCalender(31,startingDay)%7)+1;
    cout<<endl;

    cout<<"September "<<year<<endl;
    startingDay=(printMonthCalender(30,startingDay)%7)+1;
    cout<<endl;

    cout<<"October "<<year<<endl;
    startingDay=(printMonthCalender(31,startingDay)%7)+1;
    cout<<endl;

    cout<<"November "<<year<<endl;
    startingDay=(printMonthCalender(30,startingDay)%7)+1;
    cout<<endl;

    cout<<"December "<<year<<endl;
    printMonthCalender(31,startingDay);

}