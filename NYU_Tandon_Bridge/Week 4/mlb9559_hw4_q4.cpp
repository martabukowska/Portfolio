/*
NYU Tandon Bridge - Week 4 (Winter 2024)
Geometric Mean Calculator

Assignment:
Write two versions of a program that reads a sequence of positive integers from the user,
calculates their geometric mean, and print the geometric mean.

Notes:
1. In mathematics, geometric mean of a dataset {a₁,a₂,a₃ ...,aₙ} containing positive
   numbers, is given by: ⁿ√(a₁·a₂·a₃···aₙ).
   For example, the geometric mean of 2, 9 and 12 is equal to 6 (³√2·9·12 = 6).
2. In order to calculate the n-th root of a number, you should call the pow function, located
   in the cmath library.

Your two versions should read the integer sequence in two ways:
a) First read the length of the sequence.
   Example execution:
   Please enter the length of the sequence: 3
   Please enter your sequence:
   1
   2
   3
   The geometric mean is: 1.8171

b) Keep reading the numbers until -1 is entered.
   Example execution:
   Please enter a non-empty sequence of positive integers, each one in a separate line. End your
   sequence by typing -1:
   1
   2
   3
   -1
   The geometric mean is: 1.8171
*/

#include <iostream>
#include <cmath>
#include <iomanip>
using namespace std;

int main() {

    //start of section a

    int sequenceLength, sequence, resultOfMultiplication, count;
    double geometricMean;

    cout<<"Please enter the length of the sequence: ";
    cin>>sequenceLength;

    cout<<"Please enter your sequence:"<<endl;

    resultOfMultiplication=1;

    for (count = 1; count <= sequenceLength; count++){
      cin>>sequence;
      resultOfMultiplication = resultOfMultiplication * sequence;
    }

    geometricMean=pow(resultOfMultiplication, (1.0/sequenceLength));
    cout<<"The geometric mean is: "<<setprecision(5)<<geometricMean<<endl;

    cout<<endl;

    //start of section b

    int sequence_b, resultOfMultiplication_b, sequenceLength_b;
    double geometricMean_b;

    cout<<"Please enter a non-empty sequence of positive integers, each one in a separate line. End your sequence by typing -1:"<<endl;

    resultOfMultiplication_b=1;
    sequenceLength_b=0;
    sequence_b=0;

    do {
        cin>>sequence_b;
        if (sequence_b == -1)
            break;
        else{
            resultOfMultiplication_b = resultOfMultiplication_b * sequence_b;
            sequenceLength_b=sequenceLength_b+1;
        }
    } while (sequence_b > -1);

    geometricMean_b=pow(resultOfMultiplication_b, (1.0/(sequenceLength_b)));
    cout<<"The geometric mean is: "<<setprecision(5)<<geometricMean_b;


    return 0;
}