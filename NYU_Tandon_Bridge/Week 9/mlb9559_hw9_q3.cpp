/*
NYU Tandon Bridge - Week 9 (Winter 2024)
Positive Numbers Array Generator

Assignment:
Implement 4 versions of getPosNums that creates array of positive numbers:

1. int* getPosNums1(int* arr, int arrSize, int& outPosArrSize)
   - Returns array address
   - Updates size by reference

2. int* getPosNums2(int* arr, int arrSize, int* outPosArrSizePtr)
   - Returns array address
   - Updates size through pointer

3. void getPosNums3(int* arr, int arrSize, int*& outPosArr, int& outPosArrSize)
   - Updates array address by reference
   - Updates size by reference

4. void getPosNums4(int* arr, int arrSize, int** outPosArrPtr, int* outPosArrSizePtr)
   - Updates array address through pointer to pointer
   - Updates size through pointer

Example:
Input array: [3, -1, -3, 0, 6, 4]
Output array: [3, 6, 4]

Note: Write main program to test all versions
*/

#include <iostream>
using namespace std;

const int arrSize=6;

int* getPosNums1(int* arr, int arrSize, int& outPosArrSize);
int* getPosNums2(int* arr, int arrSize, int* outPosArrSizePtr);
void getPosNums3(int* arr, int arrSize, int*& outPosArr, int& outPosArrSize);
void getPosNums4(int* arr, int arrSize,int** outPosArrPtr, int* outPosArrSizePtr);
void printArr (int arr[], int arrSize);
void countPositiveNumbers (int* arr, int arrSize, int &posArrSize);
void test (int* arr, int arrSize, int* posArr, int posArrSize);

int main() {

    int arr[arrSize]={3,-1,-3, 0, 6, 4};
    int *posArr1, *posArr2, *posArr3, *posArr4;
    int posArrSize1, posArrSize2, posArrSize3, posArrSize4;

    cout<<"Function getPosNums1:"<<endl;
    posArr1=getPosNums1(arr, arrSize, posArrSize1);
    test (arr, arrSize, posArr1, posArrSize1);

    cout<<"Function getPosNums2:"<<endl;
    int *posArrSizePtr2=&posArrSize2;
    posArr2 = getPosNums2(arr, arrSize,  posArrSizePtr2);
    test (arr, arrSize, posArr2, posArrSize2);

    cout<<"Function getPosNums3:"<<endl;
    getPosNums3(arr, arrSize, posArr3, posArrSize3);
    test (arr, arrSize, posArr3, posArrSize3);

    cout<<"Function getPosNums4:"<<endl;
    int *posArrSizePtr4=&posArrSize4;
    getPosNums4(arr, arrSize,  &posArr4, posArrSizePtr4);
    test (arr, arrSize, posArr4, posArrSize4);

    delete [] posArr1;
    delete [] posArr2;
    delete [] posArr3;
    delete [] posArr4;

    posArr1=NULL;
    posArr2=NULL;
    posArr3=NULL;
    posArr4=NULL;
    posArrSizePtr2=NULL;
    posArrSizePtr4=NULL;

    return 0;
}

int* getPosNums1(int* arr, int arrSize, int& outPosArrSize){

    //Calculating the size of array that will contain the positive numbers only, and updating outPosArrSize accordingly
    countPositiveNumbers (arr, arrSize, outPosArrSize);

    //Creating a pointer to a dynamic array which will store positive numbers from original array
    int *posArr = new int [outPosArrSize];

    //Populating the new array with positive numbers from original array
    int ind=0;
    for (int i=0; i<arrSize; i++){
        if (arr[i]>0){
            posArr[ind]=arr[i];
            ind++;
        }
    }

    //Returning the base address of the array with positive numbers only
    return posArr;

}

int* getPosNums2(int* arr, int arrSize, int* outPosArrSizePtr) {

    //Calculating the size of array that will contain the positive numbers only, and updating outPosArrSizePtr accordingly
    int posArrSize2;
    countPositiveNumbers (arr, arrSize, posArrSize2);
    *outPosArrSizePtr=posArrSize2;

    //Creating a pointer to a dynamic array which will store positive numbers from original array
    int *posArr2 = new int [*outPosArrSizePtr];

    //Populating the new array with positive numbers from original array
    int ind=0;
    for (int i=0; i<arrSize; i++){
        if (arr[i]>0){
            posArr2[ind]=arr[i];
            ind++;
        }
    }

    //Returning the base address of array with positive numbers only
    return posArr2;

}

void getPosNums3(int* arr, int arrSize, int*& outPosArr, int& outPosArrSize){

    //Calculating the size of array that will contain the positive numbers only, and updating outPosArrSize accordingly
    countPositiveNumbers (arr, arrSize, outPosArrSize);

    //Creating a dynamic array that will contain positive numbers only
    outPosArr = new int [outPosArrSize];

    //Populating dynamic array with positive numbers from original array
    int ind=0;
    for (int i=0; i<arrSize; i++){
        if (arr[i]>0){
            outPosArr[ind]=arr[i];
            ind++;
        }
    }

}

void getPosNums4(int* arr, int arrSize,int** outPosArrPtr, int* outPosArrSizePtr){

    //Calculating the size of array that will contain the positive numbers only, and updating outPosArrSizePtr accordingly
    int posArrSize3;
    countPositiveNumbers (arr, arrSize, posArrSize3);
    *outPosArrSizePtr=posArrSize3;

    //Creating a dynamic array that will contain positive numbers only
    *outPosArrPtr = new int [posArrSize3];

    //Populating dynamic array with positive numbers from original array
    int ind=0;
    for (int i=0; i<arrSize; i++){
        if (arr[i]>0){
            (*outPosArrPtr)[ind]=arr[i];
            ind++;
        }
    }

}

void printArr (int arr[], int arrSize){

    for (int i=0; i<arrSize; i++){
        cout<<arr[i]<<" ";
    }
    cout<<endl;

}

void countPositiveNumbers (int* arr, int arrSize, int &posArrSize){

    posArrSize=0;
    for (int i=0; i<arrSize; i++){
        if (arr[i]>0){
            posArrSize++;
        }
    }

}

void test (int* arr, int arrSize, int* posArr, int posArrSize){

    cout<<"Original array: ";
    printArr (arr, arrSize);
    cout<<"New array: ";
    printArr (posArr, posArrSize);
    cout<<"Base address of the new array: "<<posArr<<endl;
    cout<<"New array's logical size: "<<posArrSize<<endl;
    cout<<endl;

}