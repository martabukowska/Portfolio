/*
NYU Tandon Bridge - Week 11 (Winter 2024)
Recursive Minimum Value Finder

Assignment:
Implement two recursive versions of minInArray:

1. int minInArray1(int arr[], int arrSize)
   - Takes array and its logical size
   - Finds minimum in range [0, arrSize-1]

2. int minInArray2(int arr[], int low, int high)
   - Takes array and range bounds
   - Finds minimum in range [low, high]
   - Assumes low ≤ high

Test with:
int arr[10] = {9, -2, 14, 12, 3, 6, 2, 1, -9, 15};
res1 = minInArray1(arr, 10);      // -9
res2 = minInArray2(arr, 0, 9);    // -9
res3 = minInArray2(arr, 2, 5);    // 3
res4 = minInArray1(arr+2, 4);     // 3

Requirements:
- Must use recursion
- No loops allowed
*/

#include <iostream>
using namespace std;

int minInArray1 (int arr[], int arrSize);
int minInArray2(int arr[], int low, int high);

int main() {

    int arr[10] = {9, -2, 14, 12, 3, 6, 2, 1, -9, 15};
    int res1, res2, res3, res4;

    res1 = minInArray1(arr, 10);
    res2 = minInArray2(arr, 0, 9);
    cout<<res1<<" "<<res2<<endl; //should both be -9

    res3 = minInArray2(arr, 2, 5);
    res4 = minInArray1(arr+2, 4); //arr+2 is equivalent to &(arr[2])
    cout<<res3<<" "<<res4<<endl; //should both be 3

    return 0;

}

//Function minInArray1 is given arr, array of integers, and its logical size, arrSize. It finds the minimum value out of all
//the elements in positions 0,1,2,...,arrSize-1
int minInArray1 (int arr[], int arrSize){

    if (arrSize==1){
        //BASE CASE
        //If array consists of one element only, then that element is a minimum
        return arr[0];
    } else {
        //RECURSIVE STEP
        //Recursively calling minInArray1 with a smaller array size (arrSize - 1)
        int min = minInArray1(arr, arrSize-1);
        //If the last element of the array arr[arrSize-1] is smaller than the minimum, then it becomes a new minimum.
        //If it's greater than ore equal to the minimum, then the minimum remains unchanged
        if (arr[arrSize-1]<min){
            return arr[arrSize-1];
        } else {
            return min;
        }
    }

}

//Function minInArray2 is given arr, an array of integers, and two indices: low and high, such that low<=high.
//It finds the minimum value out of all the elements in range form low to high
int minInArray2(int arr[], int low, int high){

    if (high==low){
        //BASE CASE
        //If there is only one element within the range from low to high, then that element is a minimum
        return arr[low];
    } else {
        //RECURSIVE STEP:
        //Recursively calling minInArray2 with a smaller range (form low to high-1)
        int min=minInArray2(arr, low, high-1);
        //If the last element arr[high] is smaller than the minimum, then it becomes a new minimum. If it's greater than
        //or equal to the minimum, then the minimum remains unchanged
        if (arr[high]<min){
            return arr[high];
        } else {
            return min;
        }
    }

}