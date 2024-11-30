/*
NYU Tandon Bridge - Week 4 (Winter 2024)
First N Even Numbers Generator

Assignment:
Write two versions of a program that reads a positive integer n and prints the first n even numbers.
Requirements:
• First implementation using a while loop
• Second implementation using a for loop
• Program should validate for positive integer input
• Output each even number on a new line

Example interaction:
Please enter a positive integer: 3
2
4
6
*/

#include <iostream>
using namespace std;

int main() {

    int positiveInteger_a, evenNumber_a;
    int counter_a;

    cout<<"Please enter a positive integer: ";
    cin>>positiveInteger_a;

    counter_a = 1;

    while (counter_a <= positiveInteger_a) {
        evenNumber_a = 2*counter_a;
        cout<<evenNumber_a<<endl;
        counter_a++;
    }

    cout<<endl;

    int positiveInteger_b, evenNumber_b;
    int counter_b;

    cout<<"Please enter a positive integer: ";
    cin>>positiveInteger_b;

    for (counter_b = 1; counter_b <= positiveInteger_b; counter_b++){
        evenNumber_b = 2*counter_b;
        cout<<evenNumber_b<<endl;
    }

    return 0;
}