/*
NYU Tandon Bridge - Week 9 (Winter 2024)
Array Odd-Even Reorderer

Assignment:
Implement void oddsKeepEvensFlip(int arr[], int arrSize) that:
1. Places all odd numbers before even numbers
2. Maintains original order of odd numbers
3. Reverses order of even numbers
4. Must run in O(n) time

Example:
Input array:  [5, 2, 11, 7, 6, 4]
Output array: [5, 11, 7, 4, 6, 2]

Requirements:
- Linear time complexity
- Write main program to test function
*/

#include <iostream>
using namespace std;

const int arrSize=6;

void oddsKeepEvensFlip(int arr[], int arrSize);
void printArr (int arr[], int arrSize);
void printOgAndTransformedArr (int arr[], int arrSize);


int main() {

    int arr1[arrSize]={5,2,11,7,6,4};
    int arr2[arrSize]={-6,12,3,102,0,-29};
    int arr3[arrSize]={9,-25,19,0,0,1};

    printOgAndTransformedArr (arr1, arrSize);
    printOgAndTransformedArr (arr2, arrSize);
    printOgAndTransformedArr (arr3, arrSize);

    return 0;
}

//Function oddsKeepEvensFlip takes an array of integers and its size as input.
//It reorders the elements of the array such that odd numbers come before even numbers,
//maintaining their original relative order for odd numbers, while placing even numbers
//in reverse relative order.
void oddsKeepEvensFlip(int arr[], int arrSize){

    int *oddBeforeEven = new int [arrSize];
    int indStart=0, indEnd=arrSize-1;

    for (int i=0; i<arrSize; i++){
        if (arr[i]%2==0){
            //Even numbers are placed in reverse order, so we start assigning from the end of the array
            oddBeforeEven[indEnd]=arr[i];
            indEnd--;
        } else {
            //Odd numbers keep their order, so we start assigning from the beginning of the array
            oddBeforeEven[indStart]=arr[i];
            indStart++;
        }
    }

    //Overwriting the original array with transformed one
    for (int i=0; i<arrSize; i++){
        arr[i]=oddBeforeEven[i];
    }

    //Deallocating memory
    delete[]oddBeforeEven;
    oddBeforeEven=NULL;

}

//Function printArr takes an array of integers and its size as input and prints the elements of the array separated by spaces.
void printArr (int arr[], int arrSize){

    for (int i=0; i<arrSize; i++){
        cout<<arr[i]<<" ";
    }
    cout<<endl;

}

//Function printOgAndTransformedArr takes an array of integers and its size as input.
//It prints the original array as well as the transformed array obtained by calling the function oddsKeepEvensFlip.
void printOgAndTransformedArr (int arr[], int arrSize){

    cout<<"Original array is:"<<endl;
    printArr(arr, arrSize);
    cout<<"Transformed array is:"<<endl;
    oddsKeepEvensFlip (arr, arrSize);
    printArr(arr, arrSize);
    cout<<endl;

}