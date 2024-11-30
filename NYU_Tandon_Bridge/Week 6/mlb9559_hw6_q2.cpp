/*
NYU Tandon Bridge - Week 6 (Winter 2024)
Pine Tree Pattern Generator

Assignment:
Write a program that prints a 'pine tree' consisting of triangles of increasing sizes,
filled with a character (eg '*' or '+' or '$' etc.).

Required Functions:
1. void printShiftedTriangle(int n, int m, char symbol)
   - Prints an n-line triangle filled with symbol characters
   - Shifted m spaces from left margin
   Example: printShiftedTriangle(3, 4, '+') produces:
       +
      +++
     +++++

2. void printPineTree(int n, char symbol)
   - Prints n triangles of increasing sizes
   - Smallest triangle is 2-line triangle
   - All triangles filled with symbol character
   Example: printPineTree(3, '#') produces:
      #
     ###
      #
     ###
    #####
      #
     ###
    #####
   #######

Example interaction:
Please enter the number of triangles in the tree: 3
Please enter the character filling the tree: #
[pine tree pattern appears]
*/

#include <iostream>
using namespace std;

void printShiftedTriangle (int n, int m, char symbol);
void printPineTree(int n, char symbol);

int main() {

    int trianglesNumber;
    char symbol;

    cout<<"Please enter the number of triangles in the tree: ";
    cin>>trianglesNumber;
    cout<<"Please enter the character filling the tree: ";
    cin>>symbol;

    printPineTree (trianglesNumber, symbol);

    return 0;
}

//printPineTree: prints a shape of pine tree, which consists of n triangles, filled with character 'symbol';
//first, it calculates the number of rows in a single triangle of pine tree, and the number of spaces necessary to maintain
//the shape of a tree; then it prints a single triangle; the loop repeats itself n times, forming a shape of pine tree;
//input: n, symbol

//key observations:
// 1) number of rows in triangles: first triangle has 2 rows (we start with rows=2), each next triangle has one more
// row that the previous one (rows=rows+1)
// 2) number of spaces in each row: we need to tie up the number of spaces in the last row with the number of triangles;
// the number of spaces in the last row of given triangle can be expressed as (n-i), where n is the amount of triangles
// and i is the number of current iteration

void printPineTree(int n, char symbol){
    int rows=2, spaces;
    //calculating number of rows:
    for (int i = 1; i<=n; i++) {
        if (i==1){
            rows=rows;
        } else {
        rows=rows+1;
        }
    //calculating number of spaces:
        spaces=n-i;
    //printing a single triangle:
        printShiftedTriangle(rows, spaces, symbol);
    }
}

//printShiftedTriangle: prints a triangle which is shifted m spaces from the left margin, consists of n rows, and
//is filled with characters 'symbol'
//input: n (number of rows), m (number of spaces), symbol

//key observations:
// 1) number of spaces: decrements (by 1); can be expressed as m+(n-1), where m is the number of spaces in the bottom row;
// 2) number of symbols: increments; first row always has one symbol; each next row has two more symbols than the
// previous row; can be expressed as 2(i-1)+1, where i is the number of current iteration

void printShiftedTriangle (int n, int m, char symbol){
    char space = ' ';
    for (int i=1; i<=n; i++) {
        //printing spaces:
        for (int i2=m+(n-i); i2>=1;i2--){
            cout<<space;
        }
        //printing symbols:
        for (int i3=1; i3<=2*(i-1)+1; i3++){
            cout<<symbol;
        }
        cout<<endl;
    }

}