/*
NYU Tandon Bridge - Week 8 (Winter 2024)
Name Format Converter

Assignment:
Write a program that:
1. Reads name in format: First Middle Last
2. Outputs name as: Last, First M.
3. Handles both middle name and middle initial inputs
4. Always outputs middle initial with period

Example interactions:
Input: Mary Average User
Output: User, Mary A.

Input: Mary A. User
Output: User, Mary A.

Note: Consider using separate string variables for first, middle, and last names
*/

#include <iostream>
#include <string>
using namespace std;

int main() {

    string firstName, middleName, lastName, initial;

    cout<<"Please enter your name in the format first name, middle name/initial, and last name:"<<endl;
    cin>>firstName>>middleName>>lastName;

    initial=middleName.substr(0,1);

    cout<<lastName<<", "<<firstName<<" "<<initial<<"."<<endl;

    return 0;
}