/*
NYU Tandon Bridge - Week 9 (Winter 2024)
Anagram Checker

Assignment:
Write a program that:
1. Takes two strings as input
2. Determines if they are anagrams
3. Ignores:
   - Case sensitivity
   - Punctuation
   - Spaces
4. Must run in O(n) time where n is length of input strings

Example interaction:
String 1: Eleven plus two
String 2: Twelve plus one
These strings are anagrams

Note:
- Use character counting for efficiency
- Break implementation into functions
*/

#include <iostream>
#include <string>
using namespace std;

int letterCount (string str);
void toLowerCase (string &str);
void orderAlphabetically (string str, char *orderedStr);
bool areAnagrams (string str1, string str2);

//The program takes two strings as input, checks if they are anagrams, and then prints an appropriate comment to the
//user based on the result of the check
int main() {

    string str1, str2;

    //Getting user input
    cout<<"Please enter first phrase:"<<endl;
    getline (cin, str1);
    cout<<"Please enter second phrase:"<<endl;
    getline (cin, str2);

    //Checking if the two strings are anagrams and printing appropriate comment
    if (areAnagrams(str1, str2) == true){
        cout<<"Entered phrases are anagrams"<<endl;
    } else {
        cout<<"Entered phrases are not anagrams"<<endl;
    }

    return 0;
}

//Function letterCount takes a string as input, counts the occurrence of letters in the input string, and returns
//the total count of letters found
int letterCount (string str){

    int letterCount=0;

    //Iterating through the string in search of letters; if a letter is found, the letterCount is incremented by 1
    for (int i=0; i<str.length(); i++){
        if ((str[i]>='a' && str[i]<='z')||(str[i]>='A' && str[i]<='Z')){
            letterCount++;
        }
    }

    return letterCount;
}

//Function toLowerCase takes a string as input and transforms each uppercase letter within the string into its lowercase
//equivalent
void toLowerCase (string &str) {

    //Creating a new string that going to be equivalent the input string but in lowercase
    string lowerCaseStr="";
    //Calculating the ASCII value difference between upper and lower case letters
    int valueDiff = 'a' - 'A';

    //The loop iterates through the entire string, identifying uppercase letters.
    //When an uppercase letter is found, its lowercase equivalent (lowerCaseLetter) is calculated and appended to
    //lowerCaseStr. The remaining letters, which are all lowercase, get appended to lowerCaseStr without any changes.
    for (int i = 0; i < str.length(); i++) {
        if (str[i] >= 'A' && str[i] <= 'Z') {
            char lowerCaseLetter=str[i] + valueDiff;
            lowerCaseStr=lowerCaseStr+lowerCaseLetter;
        } else {
            lowerCaseStr=lowerCaseStr+str[i];
        }
    }

    //Replacing original string with its lowercase equivalent
    str=lowerCaseStr;

}

//Function orderAlphabetically takes a string as input and a pointer to a dynamic array. It populates the
//array pointed to by 'orderedArr' with the letters from the input string in alphabetical order.
void orderAlphabetically (string str, char *orderedArr){

    int ind=0;

    //Iterating through letters from 'a' to 'z'. If the current letter is found in the string, we assign the current
    //letter (currLetter) to the next available index (ind) in orderedArr.
    for (char currLetter='a'; currLetter<='z'; currLetter++){
        for (int i=0; i<str.length(); i++){
            if (currLetter==str[i]){
                orderedArr[ind]=currLetter;
                ind++;
            }
        }
    }

}

//The function 'areAnagrams' takes two strings as input. It processes them by creating character arrays to store their
//alphabetically ordered, lowercase versions (obtained through the 'toLowerCase' and 'orderAlphabetically' functions).
//It then compares the arrays. If they are identical, indicating the same letters in the same quantities, it confirms they
//are anagrams and returns true. However, if any element in the first array does not match the corresponding element in
//the second array, the function concludes they are not anagrams and returns false.
bool areAnagrams (string str1, string str2){

    //Creating a pointer 'orderedArr1' pointing to a dynamic array of size letterCount1
    int letterCount1=letterCount(str1);
    char *orderedArr1= new char [letterCount1];
    //Converting the first string to its lowercase equivalent
    toLowerCase (str1);
    //Populating 'orderedArr1' with lowercase letters from the first string, in alphabetical order
    orderAlphabetically (str1, orderedArr1);

    //Repeating the steps above for the second string
    int letterCount2=letterCount(str2);
    char *orderedArr2= new char [letterCount2];
    toLowerCase (str2);
    orderAlphabetically (str2, orderedArr2);

    //Checking if corresponding elements in the first and second arrays are equivalent;
    //if they are, the input strings are anagrams.
    bool truthVal=true;
    for (int i=0; i<letterCount1; i++){
        if (orderedArr1[i] !=orderedArr2[i]){
            truthVal = false;
            break;
        }
    }

    //Deallocating the memory
    delete []orderedArr1;
    orderedArr1=NULL;
    delete []orderedArr2;
    orderedArr2=NULL;

    return truthVal;

}