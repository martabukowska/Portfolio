/*
NYU Tandon Bridge - Week 11 (Winter 2024)
Recursive Array Functions

Assignment:
Implement two recursive functions:

1. int sumOfSquares(int arr[], int arrSize)
   - Takes array and its size
   - Returns sum of squares of all elements
   Example:
   Input array: [2, -1, 3, 10]
   Output: 114 (2² + (-1)² + 3² + 10² = 114)

2. bool isSorted(int arr[], int arrSize)
   - Takes array and its size
   - Returns true if array is sorted in ascending order
   - Returns false otherwise
   Example:
   [1, 2, 3, 4] -> true
   [1, 3, 2, 4] -> false

Requirements:
- Must use recursion
- No loops allowed
*/

#include <iostream>
using namespace std;

int sumOfSquares(int arr[], int arrSize);
bool isSorted (int arr[], int arrSize);
void printArr (int arr[], int arrSize);

int main() {

    int arr[]={2, -1, 3, 10};
    int arrSize=4;

    cout<<"Array: ";
    printArr(arr, arrSize);

    cout<<"a)Sum of squares: "<<sumOfSquares (arr, arrSize)<<endl;

    cout<<"b)Sorted array: ";
    if (isSorted(arr, arrSize)==1){
        cout<<"true"<<endl;
    } else {
        cout<<"false"<<endl;
    }

    return 0;
}

//Function sumOfSquares is given arr, an array of integers, and its logical size, arrSize. When called,
//it returns the sum of the squares of each of the values in arr.
int sumOfSquares(int arr[], int arrSize){

    if (arrSize==0) {
        //BASE CASE:
        //If array is empty, we consider the sum of its elements as 0
        return 0;
    } else {
        //RECURSIVE STEP:
        //If array's size is greater than 1, then we sum the squares of all of its elements
        return sumOfSquares(arr, arrSize-1)+(arr[arrSize-1]*arr[arrSize-1]);
    }
}

//Function isSorted is given arr, an array of integers, and its logical size, arrSize. When called,
//returns true if the elements in arr are sorted in an ascending order, or false if they are not.
bool isSorted (int arr[], int arrSize){

    if (arrSize==0 || arrSize==1) {
        //BASE CASE
        //If array is empty or of size 1, then it's considered as sorted
        return true;
    } else {
        //RECURSIVE STEP
        //Checking if the last two elements of the array of size (arrSize-1) are in the ascending order
        if (arr[arrSize - 2] <= arr[arrSize - 1]) {
            //If the last two elements are in the ascending order, then we check remaining elements using recursion
            return isSorted(arr, arrSize - 1);
        } else {
            //If the last two elements are not in the ascending order, then we already know that the array isn't in
            //ascending order and we can return false
            return false;
        }
    }
}

//Function printArr is given array of integers, and it's size as input, and it prints all the elements of the array
//separated by space
void printArr (int arr[], int arrSize){

    for (int i=0; i<arrSize; i++){
        cout<<arr[i]<<" ";
    }
    cout<<endl;

}