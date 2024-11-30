/*
NYU Tandon Bridge - Week 8 (Winter 2024)
Palindrome Checker

Assignment:
1. Implement bool isPalindrome(string str) that returns true if str is a palindrome
2. Write a program that reads a word and announces if it's a palindrome

Example interaction:
Please enter a word: level
level is a palindrome
*/

#include <iostream>
#include <string>
using namespace std;

bool isPalindrome(string str);

//The program prompts the user to enter a word and checks if it is a palindrome or not.
int main() {

    string word;

    cout<<"Please enter a word: ";
    cin>>word;

    if (isPalindrome(word)==true){
        cout<<word<< " is a palindrome";
    } else {
        cout<<word<< " is not a palindrome";
    }

    return 0;
}

//Function isPalindrome takes a string, calculates its length, reverses the string, and then checks if the original
//string (str) is a palindrome by comparing it to the reversed string (revStr).
//It returns true if it's a palindrome, and false if it's not.
bool isPalindrome(string str){

    int strSize=str.length();
    string revStr;

    for (int i=strSize-1; i>=0; i--){
        revStr=revStr+str[i];
    }

    return revStr==str;
}
