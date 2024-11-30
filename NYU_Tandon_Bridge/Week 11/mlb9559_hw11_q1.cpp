/*
NYU Tandon Bridge - Week 11 (Winter 2024)
Recursive Triangle Patterns

Assignment:
Implement two recursive functions:

1. void printTriangle(int n)
   - Prints right triangle with n lines
   - Each line has increasing number of asterisks
   Example (n=4):
   *
   **
   ***
   ****

2. void printOppositeTriangles(int n)
   - Prints two opposite right triangles
   - Each triangle has n lines
   Example (n=4):
   ****
   ***
   **
   *
   *
   **
   ***
   ****
3. void printRuler(int n)
   - Prints vertical ruler with 2^n - 1 lines
   - Middle line (1/2) has n '-' marks
   - Lines at 1/4 and 3/4 have (n-1) '-' marks
   - Lines at 1/8, 3/8, 5/8, 7/8 have (n-2) '-' marks
   - Pattern continues with 1 '-' mark at smallest divisions

Example printRuler(4):
-
--
-
---
-
--
-
----
-
--
-
---
-
--
-

Requirements:
- Must use recursion
- No loops allowed
*/

#include <iostream>
using namespace std;

void printTriangle(int n);
void printOpositeTriangles(int n);
void printRuler(int n);

int main() {

    int n=4;

    cout<<"a)"<<endl;
    printTriangle(n);

    cout<<"b)"<<endl;
    printOpositeTriangles(n);

    cout<<"c)"<<endl;
    printRuler(n);

    return 0;
}

//Function printTriangle is given a positive integer n, and prints a textual image of a right triangle
//(aligned to the left) made of n lines with asterisks.
void printTriangle(int n){

    if (n==1){
        //BASE CASE (If n=1 we print a single asterisk)
        cout<<"*"<<endl;
    } else {
        //RECURSIVE STEP (If n>1 we print n-1 rows, with appropriate number of asterisks)
        //Recursive call of the function until the base case is reached
        printTriangle(n-1);
        //Printing the asterisks
        for (int i=1; i<=n; i++){
            cout<<"*";
        }
        cout<<endl;
    }

}

//Function printOpositeTriangles is given a positive integer n, and prints a textual image of a two opposite right
//triangles (aligned to the left) with asterisks, each containing n lines.
void printOpositeTriangles(int n){

    if (n==1){
        // BASE CASE (When n=1, we print '*' twice, each on a separate line)
        cout<<"*"<<endl;
        cout<<"*"<<endl;
    } else {
        //RECURSIVE STEP
        //Printing the top part of the shape:
        for (int i=1; i<=n; i++){
            cout<<"*";
        }
        cout<<endl;
        printOpositeTriangles(n-1);
        //Printing the bottom part of the shape:
        for (int i=1; i<=n; i++){
            cout<<"*";
        }
        cout<<endl;
    }

}

//The function printRuler is given a positive integer n, and prints a vertical ruler of 2^n−1 lines.
void printRuler(int n){

    if (n==1){
        //BASE CASE (When n==1 one "-" is printed)
        cout<<"-"<<endl;
    } else {
        //RECURSIVE STEP
        printRuler (n-1);
        for (int i=1; i<=n; i++) {
            cout << "-";
        }
        cout<<endl;
        printRuler (n-1);
    }

}