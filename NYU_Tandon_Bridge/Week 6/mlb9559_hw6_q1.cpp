/*
NYU Tandon Bridge - Week 6 (Winter 2024)
Fibonacci Sequence Calculator

Original Assignment:
The Fibonacci numbers sequence, Fn, is defined as follows:
F1 is 1, F2 is 1, and Fn = Fn-1 + Fn-2 for n = 3, 4, 5, ...

1. Write a function int fib(int n) that returns the n-th element of the Fibonacci sequence
2. Write a program that prompts the user to enter a positive integer num, and then prints
   the num's elements in the Fibonacci sequence

First 10 Fibonacci numbers: 1, 1, 2, 3, 5, 8, 13, 21, 34, 55

Example interaction:
Please enter a positive integer: 7
13
*/

#include <iostream>
using namespace std;

const int F_1=1;
const int F_2=1;

int fib (int n);

int main() {

    int num, F_n;

    cout<<"Please enter a positive integer: ";
    cin>>num;
    F_n=fib(num);
    cout<<F_n;

    return 0;
}

//fib: prints n-th element of the Fibonacci sequence
//input: n
//output: F_n (n-th element of the Fibonacci sequence)
//assumption: F1=1, F2=1, each next element is the sum of previous two elements

int fib (int n){
    int F_n, F_n_minus_1=F_1, F_n_minus_2=F_2;
    for (int count =3; count<=n; count++){
        F_n=F_n_minus_1+F_n_minus_2;
        F_n_minus_1=F_n_minus_2;
        F_n_minus_2=F_n;
    }
    F_n=F_n_minus_2;
    return F_n;
}