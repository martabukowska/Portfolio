/*
NYU Tandon Bridge - Week 8 (Winter 2024)
PIN Authentication Simulator

Assignment:
Create a program that simulates a randomized challenge-response PIN authentication system:
1. Store a constant 5-digit PIN (00000-99999)
2. Generate random mapping where each digit (0-9) gets assigned 1, 2, or 3
3. Display PIN-to-NUM mapping
4. Verify user's input against actual PIN using mapping

Example interaction:
Please enter your PIN according to the following mapping:
PIN: 0 1 2 3 4 5 6 7 8 9
NUM: 3 2 3 1 1 3 2 2 1 3
23113
Your PIN is correct

Please enter your PIN according to the following mapping:
PIN: 0 1 2 3 4 5 6 7 8 9
NUM: 1 1 2 3 1 2 2 3 3 3
23113
Your PIN is not correct

Note: Break implementation into functions
*/

#include <iostream>
#include <string>
using namespace std;

const int NUM_OF_DIGITS=10;
const int PIN_LENGTH=5;
const int pin[PIN_LENGTH]={1,2,3,4,5};

void randomArr (int arr[], int arrSize);
void intToArr (int integer, int intDigs, int arr[]);
void translatePin (const int pin[], int mapping[], int translatedPin[], int pinSize, int mappingSize);
bool areArrEqual (int arr1[], int arr2[], int arrSize);

int main() {

    int mapping[NUM_OF_DIGITS];
    int input;
    int enteredPin[PIN_LENGTH];
    int translatedPin[PIN_LENGTH];

    cout<<"Please enter your PIN according to the following mapping:"<<endl;
    cout<<"PIN: 0123456789"<<endl;
    cout<<"NUM: ";

    //Calling function randArr to assign elements of array 'mapping' with random numbers, and then printing the 'mapping':
    randomArr (mapping, NUM_OF_DIGITS);
    for (int i=0; i<NUM_OF_DIGITS; i++){
        cout<<mapping[i];
    }
    cout<<endl;

    //Getting user input:
    cin>>input;

    //Turning user input into an array 'enteredPin':
    intToArr (input, PIN_LENGTH, enteredPin);

    //Translating original pin (array 'pin') in accordance with mapping (array 'mapping'). Storing the result of
    //translation in array 'translatedPin':
    translatePin (pin, mapping, translatedPin, PIN_LENGTH, NUM_OF_DIGITS);

    //Calling the function areArrEqual to determine equality between 'translatedPin' and 'enteredPin', and printing an
    //appropriate statement:
    if (areArrEqual (translatedPin, enteredPin, PIN_LENGTH)==true){
        cout<<"Your PIN is correct"<<endl;
    } else {
        cout<<"Your PIN is not correct"<<endl;
    }

    return 0;
}

//Function randomArr assigns array arr of size arrSize with random digits.
void randomArr (int arr[], int arrSize){

    int randomNum;
    srand(time(0));

    for (int i=0; i<arrSize; i++) {
        //Generating random number:
        randomNum = (rand() % 10);
        //Assigning random number to the i-th element of the array:
        arr[i] = randomNum;
    }

}

//Function intToArr takes an integer with number of digits equal to intDigs, and assigns its every digit to an element in
//the array arr.
void intToArr (int integer, int intDigs, int arr[]){

    int currNum=integer, currDig;

    for (int i=intDigs-1; i>=0; i--){
        currDig=currNum%10;
        arr[i]=currDig;
        currNum=currNum/10;
    }

}

//Function translatePin takes array 'pin' (which is a constant, and of size pinSize), and translates it in accordance with
//array 'mapping' (of size mappingSize). The results of translation are stored in array 'translatedPin' (of size pinSize).
//Key observation: if pin[i]=j, then translatedPin[i]=mapping[j] (for example, if pin[1]=6, and mapping[6]=7, then
//translatedPin[1]=mapping[6]=7)
void translatePin (const int pin[], int mapping[], int translatedPin[], int pinSize, int mappingSize){

    for (int i=0; i<pinSize; i++){
        for (int j=0; j<mappingSize; j++){
            if (pin[i]==j){
                translatedPin[i]=mapping[j];
            }
        }
    }

}

//Function areArrEqual compares two arrays arr1 and arr2 of the same size arrSize, and returns true if they are equal, or
//false, if they aren't equal
bool areArrEqual (int arr1[], int arr2[], int arrSize) {

    for (int i = 0; i < arrSize; i++) {
        if (arr1[i] != arr2[i]) {
            return false;
        }
    }

    return true;
}