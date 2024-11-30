/*
NYU Tandon Bridge - Week 8 (Winter 2024)
Array Minimum Value Finder

Assignment:
1. Implement int minInArray(int arr[], int arrSize)
   - Returns minimum value in array

2. Write a program that:
   - Reads 20 integers into array
   - Finds minimum value
   - Prints all indices where minimum appears
   - Values don't need to be unique

Example interaction:
Please enter 20 integers separated by a space:
14 5 12 5 6 14 5 12 14 12 14 6 8 7 5 136 9 2189 10 6
The minimum value is 5, and it is located in the following indices: 1 3 6 14

Implementation Notes:
- Uses constant for array size
- Separates minimum finding and index finding into functions
- Handles multiple occurrences of minimum value
- Assumes valid input (20 integers)
*/

#include <iostream>
using namespace std;

const int ARR_SIZE = 20;

int minInArray(int arr[], int arrSize);
void indices (int arr [], int arrSize, int n);

//This program reads from the user a sequence of 20 integers into an array, and outputs the minimum value, and all the
//indices it appears in the array.
int main() {

    int arr[ARR_SIZE];

    //Reading from the user a sequence of 20 integers into an array:
    cout<<"Please enter 20 integers separated by a space:"<<endl;
    for (int i=0; i<ARR_SIZE; i++){
        cin>>arr[i];
    }

    //Printing minimum value in array:
    int min = minInArray(arr, ARR_SIZE);
    cout<<"The minimum value is "<<min;

    //Printing indices where the minimum value appears in the array:
    cout<<", and it is located in the following indices: ";
    indices (arr, ARR_SIZE, min);

    return 0;
}

//The function minInArray returns the minimum value in the array of integers 'arr' of size 'arrSize'.
//It initializes the first element in the array to 'min', and then compares each element of the array with 'min'.
//If the current element is less than 'min', it becomes the new minimum.
int minInArray(int arr[], int arrSize){

    int min=arr[0];

    for (int i=0; i<arrSize; i++){
        if (arr[i]<min) {
            min = arr[i];
        }
    }

    return min;

}

//The function 'indices' iterates through the entire array of size 'arrSize' and prints the indexes at which the number
//'n' appears in the array.
void indices (int arr [], int arrSize, int n){

    for (int i=0; i<arrSize; i++){
        if (n==arr[i]) {
            cout << i<<" ";
        }
    }

}