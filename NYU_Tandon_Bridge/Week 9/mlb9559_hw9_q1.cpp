/*
NYU Tandon Bridge - Week 9 (Winter 2024)
Word and Letter Counter

Assignment:
Write a program that:
1. Reads a line of text
2. Counts number of words (delimited by whitespace, period, comma, or line start/end)
3. Counts occurrences of each letter (case-insensitive)
4. Outputs results in alphabetical order
5. Lists only letters that appear in the text
6. Must run in O(n) time where n is input length

Example interaction:
Please enter a line of text:
I say Hi.
3    words
1    a
1    h
2    i
1    s
1    y

Note: Input contains only letters, whitespace, commas, and periods
*/

#include <iostream>
#include <string>
using namespace std;

int wordsCounter (string line);
void toLowerCase (string &str);
void letterCounter (string str, int &totalLetters);
void letterOccurrences (string str);


//The program reads a line of text from the user, outputs the number of words in the line and the number of occurrences
//of each letter
int main() {

    string line;

    //Getting user input:
    cout<<"Please enter a line of text:"<<endl;
    getline (cin, line);

    //Printing the number of words in the line:
    cout<<wordsCounter(line)<<"\t"<<"words"<<endl;

    //Transforming user input into lowercase letters and printing the letters that occur in the line, along with the
    //number of their occurrences:
    toLowerCase (line);
    letterOccurrences (line);

    return 0;
}

//Function wordsCounter counts the number of words in the input string.
int wordsCounter (string line){

    int wordCount=0;

    for (int i=0; i<line.length(); i++){
        if (i==0){
            //Handling a case when the only entered character is one letter
            if (i==line.length()-1 && line[i]!=' ' && line[i]!=',' && line[i]!='.'){
                wordCount = wordCount + 1;
            } else {
                continue;
            }
        } else if (i==line.length()-1) {
            //Increasing the wordCount by one, when we reach the end of the line
            wordCount = wordCount + 1;
        } else {
            //Handling any other case
            if ((line[i]==' ' || line[i]==',' || line[i]=='.')){
                //Making sure that multiple consecutive spaces/periods/commas aren't counted as words:
                if (i < line.length() - 1 && line[i+1]!=' ' && line[i+1]!=',' &&  line[i+1]!='.') {
                    wordCount = wordCount + 1;
                }
            }
        }
    }

    return wordCount;
}

//The toLowerCase function takes a string as input and transforms every uppercase letter to lowercase.
void toLowerCase (string &str){

    //Calculating the difference between ASCII values of upper and lower case letters
    int valueDiff='a'-'A';

    //Replacing every upper case letter in the line with lower case letter
    for (int i=0; i<str.length(); i++){
        if (str[i]>='A' && str[i]<='Z'){
            str[i]=str[i]+valueDiff;
        }
    }

}

//Function letterCounter calculates the total number of letters in input string
void letterCounter (string str, int &totalLetters){

    totalLetters=0;

    for (int i=0; i<str.length(); i++){
        if ((str[i]>='a' && str[i]<='z') || (str[i]>='A' && str[i]<='Z')){
            totalLetters++;
        }
    }

}

//The letterOccurrences function takes a string as input, prints the letters that occur in it, and their count
void letterOccurrences (string str){

    //Initializing starting index of orderedLetters array
    int ind=0;

    //Calculating the number of lower case letters in input string, to know the array size
    int arrSize;
    letterCounter (str, arrSize);

    //Creating a pointer to an array that's going to consist of the letters that appear in the input string in
    //alphabetical order (without letter duplicates)
    char *orderedLetters = new char[arrSize];

    //Iterating thorough every letter of alphabet in search for letters that occur in input string.
    //If current letter is found in input string, we assign it to the next element in the array orderedLetters and then
    //print the array. We also calculate the number of occurrences of given letter in the string and print it.
    for (char currLetter='a'; currLetter<='z'; currLetter++) {

        if (str.find(currLetter) != string::npos) {
            //Printing the number of occurrences of given letter in the inout string (letterCount)
            int letterCount = 0;
            for (int i=0; i<str.length(); i++){
                if (str[i]==currLetter){
                    letterCount++;
                }
            }
            cout<<letterCount<<"\t";

            //Assigning current letter to the element of array of ordered letters and printing it
            orderedLetters[ind] = currLetter;
            cout << orderedLetters[ind] << endl;
            //Updating the index of orderedLetters array
            ind++;
        }

    }

    delete []orderedLetters;
    orderedLetters=NULL;

}