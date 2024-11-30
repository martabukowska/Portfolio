/*
NYU Tandon Bridge - Week 10 (Winter 2024)
Missing Numbers Finder

Assignment:
Implement int* findMissing(int arr[], int n, int& resArrSize) that:
1. Takes array with numbers in range [0...n]
2. Finds all numbers in range missing from array
3. Creates and returns array of missing numbers
4. Updates resArrSize with count of missing numbers
5. Must run in O(n) time

Example:
Input array: [3, 1, 3, 0, 6, 4]
n: 6
Output array: [2, 5]
resArrSize: 2

Requirements:
- Linear time complexity
- Handle duplicates in input array
- Write main program to test function
*/

#include <iostream>
#include <vector>
using namespace std;

const int ARR_SIZE=6;

int* findMissing(int arr[], int n, int& resArrSize);

//The main function tests the findMissing function by printing the original array, the array of missing numbers,
//and the size of the array of missing numbers.
int main() {

    int arr[ARR_SIZE]={3, 1, 3, 0, 6, 4};

    //Initializing missingNumArrSize, assigning pointer missingNumbersArr with the address of missing numbers array,
    //obtained by calling findMissing
    int missingNumArrSize;
    int *missingNumbersArr=findMissing(arr, ARR_SIZE, missingNumArrSize);

    //Testing findMissing
    cout<<"Original array: [";
    for (int i=0; i<ARR_SIZE; i++){
        if (i==ARR_SIZE-1){
            cout << arr[i]<<"]"<<endl;
        } else {
            cout << arr[i] << ", ";
        }
    }
    cout<<"Array of missing numbers: [";
    for (int i=0; i<missingNumArrSize; i++){
        if (i==missingNumArrSize-1){
            cout << missingNumbersArr[i]<<"]"<<endl;
        } else {
            cout << missingNumbersArr[i] << ", ";
        }
    }
    cout<<"Size of the array of missing numbers: "<<missingNumArrSize<<endl;

    //Deallocating memory, pointing missingNumbersArr to NULL, to prevent dangling pointer
    delete [] missingNumbersArr;
    missingNumbersArr=NULL;

    return 0;
}

//Function findMissing takes an array, its logical size n, and a reference to the size of an array containing missing integers.
//It creates a new dynamic array that stores all numbers in the range {0, 1, ..., n} that didn't occur in the input array.
//Finally, it returns a pointer to the first element of the array containing missing numbers.
int* findMissing(int arr[], int n, int& resArrSize){

    //Creating dynamic array which will store number of occurrences of numbers from input array, initializing all of its
    //elements to 0
    int fullArrSize=n+1, inputArrSize=n;
    int* numOccurrence=new int [fullArrSize];
    for (int i=0; i<fullArrSize; i++){
        numOccurrence[i]=0;
    }

    //Iterating through the input array and updating count of occurring integers (so the elements in numOccurrence array)
    //accordingly. For example, if i-th element of input array is equal to 3, then the occurrence of 3rd element in
    //the numOccurrence array is increased by 1
    for (int i=0; i<inputArrSize; i++){
        numOccurrence[arr[i]]++;
    }

    //Creating a vector that's going to store numbers missing from input array
    vector <int> missingNumArr;

    //Iterating through numOccurrence array in search for the missing numbers (ones which occurrence number is 0)
    resArrSize=0;
    for (int i=0; i<fullArrSize; i++){
        //If the number that didn't occur in the input array is encountered, we increase the size of missingNumArr
        //accordingly, and assign it to the next element in it. Additionally, we are updating resArrSize
        if (numOccurrence[i]==0){
            missingNumArr.push_back(i);
            resArrSize++;
        }
    }

    //Creating a dynamically allocated array to replace the vector
    int *missingNumArrPtr=new int [resArrSize];
    for (int i=0; i<resArrSize; i++){
        missingNumArrPtr[i]=missingNumArr[i];
    }

    //Deallocating memory as numOccurrence won't be needed anymore
    delete [] numOccurrence;
    numOccurrence=NULL;

    //Returning pointer to the array with numbers missing from the input array
    return missingNumArrPtr;

}