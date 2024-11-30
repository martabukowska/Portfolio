/*
NYU Tandon Bridge - Week 3 (Winter 2024)
Student Status Calculator

Assignment:
Write a program that:
• Asks the user for their name.
• Asks the user to input their graduation year.
• Asks the user to input the current year.
Assume the student is in a four-year undergraduate program. Display the current status
the student is in. Possible status include: not in college yet, freshman, sophomore,
junior, senior, graduated.

Example interaction:
Please enter your name: Jessica
Please enter your graduation year: 2019
Please enter current year: 2015
Jessica, you are a Freshman

*/

#include <iostream>
#include <string>
using namespace std;

int main() {

    string name, status;
    int graduationYear, currentYear;

    cout<<"Please enter your name: ";
    cin>>name;

    cout<<"Please enter your graduation year: ";
    cin>>graduationYear;

    cout<<"Please enter current year: ";
    cin>> currentYear;

        if(graduationYear-currentYear==1)
            status = "Senior";
        else if (graduationYear-currentYear==2)
            status = "Junior";
        else if (graduationYear-currentYear==3)
            status = "Sophomore";
        else if (graduationYear-currentYear==4)
            status = "Freshman";
        else if (graduationYear-currentYear>4)
            status = "not in college yet";
        else
            status = "Graduated";

    if ((status == "not in college yet") || (status == "Graduated"))
        cout<<name<<", you are "<<status<<endl;
    else
        cout<<name<<", you are a "<<status<<endl;

    return 0;
}