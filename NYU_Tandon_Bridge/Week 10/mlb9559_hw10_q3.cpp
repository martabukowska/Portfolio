/*
NYU Tandon Bridge - Week 10 (Winter 2024)
Number Sequence Line Finder

Assignment:
Create two versions of a program that:
1. Reads sequence of positive integers (-1 to end)
2. Takes a number to search for
3. Prints all line numbers where search number appears
4. Must run in O(n) time

Version 1: Without vectors
Version 2: Using vectors

Example interaction:
Please enter a sequence of positive integers, each in a separate line.
End your input by typing -1:
13
5
8
2
9
5
8
8
-1
Please enter a number you want to search: 5
5 shows in lines 2, 6.

Requirements:
- Break into functions
- Linear time complexity
- Implement as main1() and main2()
- Write main() to test both versions
*/

#include <iostream>
#include <vector>
using namespace std;

const int INITIAL_INPUT_SIZE=10;

int main1 ();
void resizeArr (int *&inputArr, int &inputArrSize);
void getInput1 (int *&inputArr, int &inputArrSize);
void printArr (int *arr, int arrSize);
void findAndPrintIndices1 (int *inputArr, int inputArrSize, int num);

int main2 ();
void getInput2 (vector <int> &input);
void printVector (vector <int> v);
void findAndPrintIndices2 (vector <int> input, int num);

//Main tests main1 and main2
int main() {

    cout<<"Version 1"<<endl;
    main1();

    cout<<endl;

    cout<<"Version 2"<<endl;
    main2 ();

    return 0;
}

//Main1 reads a sequence of positive integers from the user, ending with -1, and prompts the user for another positive
//integer 'num' to search for. Then, it prints the line numbers of the entered sequence that contain 'num', or a message
//indicating that 'num' does not appear in the sequence. It does not utilize any vectors.
int main1 () {

    int num;
    int inputArrSize=INITIAL_INPUT_SIZE;
    int *inputArr=new int[inputArrSize];

    cout<<"Please enter a sequence of positive integers, each in a separate line."<<endl;
    cout<<"End you input by typing -1."<<endl;
    getInput1(inputArr, inputArrSize);

    cout<<"Please enter a number you want to search."<<endl;
    cin>>num;

    findAndPrintIndices1 (inputArr, inputArrSize,  num);

    delete [] inputArr;
    inputArr=NULL;

    return 0;
}

//The resizeArr function creates a new dynamic array that is double the size of the original array
//and copies elements from the original array to it.
void resizeArr (int *&inputArr, int &inputArrSize){

    int newInputArrSize=inputArrSize*2;
    //Creating new array with double the size of original array, and copying elements to it
    int *resizedInputArr = new int[newInputArrSize];
    int ind = 0;
    for (int i = 0; i <inputArrSize; i++) {
        resizedInputArr[ind] = inputArr[i];
        ind++;
    }
    //Deallocating the memory of original array and pointing the pointer to resized array
    delete[] inputArr;

    //Updating inputArr and inputArrSize
    inputArrSize=newInputArrSize;
    inputArr = resizedInputArr;

}

//Function getInput1 retrieves user input until -1 is entered and assigns the integers to the inputArr array.
//The array's initial size is INITIAL_INPUT_SIZE. If it's insufficient, resizeArray function is invoked,
//which creates a new array double the size of the previous one and copies existing elements to it.
//Function getInput1 takes a pointer to the input array and its size as input, and updates both variables by reference.
void getInput1 (int *&inputArr, int &inputArrSize){

    int currInt;
    int ind=0;

    do {
        //If the size of the array is sufficient, we only assign currInt to the array
        if (ind<inputArrSize) {
            cin >> currInt;
            inputArr[ind] = currInt;
            ind++;
        } else {
            //If size of the array isn't sufficient, we first resize the array, and then assign currInt to the array
            resizeArr (inputArr,inputArrSize);
            cin >> currInt;
            inputArr[ind] = currInt;
            ind++;
        }
    } while (currInt!=-1);

    inputArrSize=ind;

}

//Function printArr takes a pointer to the first element of an array and the array's size as input, and prints all
//elements in the array separated by commas, with a period at the end.
void printArr (int *arr, int arrSize){

    for (int i=0; i<arrSize; i++){
        if (i==arrSize-1){
            cout<<arr[i]<<"."<<endl;
        } else {
            cout<<arr[i]<<", ";
        }
    }

}

//The function findAndPrintIndices1 takes a pointer to an array, the size of the array, and a number 'num' as input.
//It iterates through the array to count the number of occurrences of 'num'. If 'num' occurs zero times, an appropriate
//statement is printed. If 'num' occurs at least once, a dynamic array 'indicesArr' of size 'indicesCount' is created and
//populated with the line numbers where 'num' occurs, and finally the indices where 'num' occurs are printed.
void findAndPrintIndices1 (int *inputArr, int inputArrSize, int num){

    //Calculating the number of indices in which num occurs
    int indicesCount=0;
    for (int i=0; i<inputArrSize; i++){
        if (inputArr[i]==num){
            indicesCount++;
        }
    }

    if (indicesCount==0){
        cout<<num<<" doesn't show in the entered sequence."<<endl;
    } else {
        //Creating a pointer to dynamic array that's going to store the indices
        int *indicesArr = new int[indicesCount];
        //Populating the array with indices
        int ind = 0;
        for (int i = 0; i < inputArrSize; i++) {
            if (inputArr[i] == num) {
                indicesArr[ind] = i + 1;
                ind++;
            }
        }
        //Printing indices
        cout<<num<<" shows in lines ";
        printArr(indicesArr, indicesCount);
        //Deallocating memory
        delete[] indicesArr;
        indicesArr = NULL;
    }

}

//Main2 reads a sequence of positive integers from the user, ending with -1, and prompts the user for another positive
//integer 'num' to search for. Then, it prints the line numbers of the entered sequence that contain 'num', or a message
//indicating that 'num' does not appear in the sequence. It utilizes vectors.
int main2 () {

    vector <int> input;
    int num;

    cout<<"Please enter a sequence of positive integers, each in a separate line."<<endl;
    cout<<"End you input by typing -1."<<endl;
    getInput2 (input);

    cout<<"Please enter a number you want to search."<<endl;
    cin>>num;

    findAndPrintIndices2 (input, num);

    return 0;
}

//Function printVector takes a vector as input and prints all its elements separated by commas, with a period at the end.
void printVector (vector <int> v){

    for (unsigned int i=0; i<v.size(); i++){
        if (i==v.size()-1){
            cout<<v[i]<<"."<<endl;
        } else {
            cout<<v[i]<<", ";
        }
    }

}

//The function getInput2 takes a vector as input and populates it with user input until -1 is entered, adjusting its size
//using the push_back function.
void getInput2 (vector <int> &input){

    int currInt=0;

    do {cin>>currInt;
        input.push_back(currInt);
    } while (currInt!=-1);

}

//Function findAndPrintIndices2 takes a vector and a number 'num' as input. It creates a new vector, populating it with
//the indices where 'num' occurs in the input vector. If 'num' doesn't occur in the input vector, an appropriate message
//is printed. If 'num' occurs at least once in the vector, the line numbers where it occurs are printed.
void findAndPrintIndices2 (vector <int> input, int num) {

    //Creating a vector that's going to store the indices
    vector<int> indices;

    //Populating the vector with indices
    for (unsigned int i = 0; i < input.size(); i++) {
        if (input[i] == num) {
            indices.push_back(i + 1);
        }
    }

    //Printing indices or a message that num doesn't occur in the sequence
    if (indices.size() == 0) {
        cout << num << " doesn't show in the entered sequence." << endl;
    } else {
        cout << num << " shows in lines ";
        printVector(indices);
    }

}