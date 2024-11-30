/*
NYU Tandon Bridge - Week 8 (Winter 2024)
Array Manipulation Functions

Assignment:
1. Implement void reverseArray(int arr[], int arrSize)
   - Reorders array elements in reverse order
   - Example: [1,2,3,4] becomes [4,3,2,1]

2. Implement void removeOdd(int arr[], int& arrSize)
   - Removes odd numbers, keeps even numbers in original order
   - Updates arrSize to new logical size
   - Example: [1,2,3,4] becomes [2,4] with arrSize=2

3. Implement void splitParity(int arr[], int arrSize)
   - Reorders so odd numbers appear first, even numbers last
   - Inner order within odd/even groups doesn't matter
   - Example: [1,2,3,4] could become [3,1,2,4]

Requirements:
- No auxiliary arrays allowed
- Must run in O(arrSize) time
*/

#include <iostream>
using namespace std;

void reverseArray(int arr[], int arrSize);
void removeOdd(int arr[], int& arrSize);
void splitParity(int arr[],int arrSize);
void printArray(int arr[], int arrSize);

int main() {

    int arr1[10] = {9, 2, 14, 12, -3};
    int arr1Size = 5;
    int arr2[10] = {21, 12, 6, 7, 14};
    int arr2Size = 5;
    int arr3[10] = {3, 6, 4, 1, 12};
    int arr3Size = 5;
    reverseArray(arr1, arr1Size);
    printArray(arr1, arr1Size);
    removeOdd(arr2, arr2Size);
    printArray(arr2, arr2Size);
    splitParity(arr3, arr3Size);
    printArray(arr3, arr3Size);

    return 0;
}

//The function reverseArray reverses the elements in the array arr by swapping the elements from the beginning and the
//end until they meet at the middle of the array. It iterates through half of the array (arrSize/2 times), swapping
//two elements each time.
void reverseArray(int arr[], int arrSize){

    //j is an int indicating the index of the last element in the array; og_i is an int storing original value of arr[i]
    int j=arrSize-1;
    int og_i;

    //Swapping the elements of the array from the beginning with the elements from the end until they meet at the middle
    //of the array
    for (int i=0; i<arrSize/2 && j>=arrSize/2; i++){
        og_i=arr[i];

        arr[i] = arr[j];
        arr[j] = og_i;

        j = j - 1;
    }
}

//The function removeOdd removes odd integers from the given array and updates the array's indexes to accommodate the
//new array, which consists only of even integers.
void removeOdd(int arr[], int& arrSize){

    int j=0;

     for (int i=0; i<arrSize; i++){
         //If the current integer in the array is even, we assign it to the element in the updated array with a proper index.
         if (arr[i]%2==0){
             arr[j]=arr[i];
             j=j+1;
         }
     }

    //Replacing original size of the array with updated size of the array.
    arrSize=j;
}

//The function splitParity changes the order of numbers in the array so that odd numbers appear before even numbers.
void splitParity(int arr[],int arrSize){

    int oddInd=0, evenInd=arrSize-1;

    //The loop iterates as long as oddInd is less than evenInd. Since odd numbers are intended to be on the left and
    //even numbers on the right, the index of odd numbers should always be smaller than the index of even numbers.
    while (oddInd<evenInd) {
        //If the number is odd and its index falls within the range of odd numbers (so its index is smaller than the
        //current index of even numbers), we keep it in place and increment the current index of odd numbers by 1.
        while (oddInd<evenInd && arr[oddInd]%2==1) {
            oddInd=oddInd+1;
        }
        //If the number is even and its index falls within the range of even numbers (so its index is greater than the
        //current index of odd numbers), we keep it in place and decrement the current index of even numbers by 1.
        while (oddInd<evenInd && arr[evenInd]%2==0) {
            evenInd=evenInd-1;
        }
        //If the current number is not within its expected range (so even number is within odd range or the other way
        //around), it needs to be swapped:
        if (oddInd<evenInd && (arr[evenInd]%2==1 || arr[oddInd]%2==0)) {
            int storage = arr[oddInd];
            arr[oddInd] = arr[evenInd];
            arr[evenInd] = storage;
            //Updating the indexes:
            oddInd = oddInd + 1;
            evenInd = evenInd - 1;
        }
    }
}

void printArray(int arr[], int arrSize){
    int i;
    for (i = 0; i < arrSize; i++) {
        cout<<arr[i]<<' ';
    }
    cout<<endl;
}