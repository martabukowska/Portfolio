/*
NYU Tandon Bridge - Week 8 (Winter 2024)
Text Number Masker

Assignment:
Write a program that:
1. Reads a line of text
2. Replaces all standalone integer digits with 'x'
3. Preserves digits that are part of words (e.g., "john17" stays unchanged)
4. Handles spaces between words
5. Assumes input contains only letters, digits, and spaces

Example interaction:
Please enter a line of text:
My userID is john17 and my 4 digit pin is 1234 which is secret
My userID is john17 and my x digit pin is xxxx which is secret

Note: Break implementation into functions
*/

#include <iostream>
#include <string>
using namespace std;

bool isInt (string word);
void replaceWithX (string str);
void replaceWithXIfInt (string word);


//The program takes a string line as input from the user. It then separates the words from the input and replaces digits
//with 'x's if a word is determined to be an integer.
int main() {

    string line;
    string word;
    int wordStart = 0, wordEnd;

    cout << "Please enter a line of text:" << endl;
    getline(cin, line);

    for (wordEnd = 0; wordEnd <= line.length(); wordEnd++) {
        if ((line[wordEnd] == ' ') || (wordEnd == line.length())) {
            int wordLength = wordEnd - wordStart;
            word = line.substr(wordStart, wordLength);
            replaceWithXIfInt(word);
            wordStart = wordEnd + 1;
        }
    }

    return 0;
}

//Function isInt takes a word as an input and goes through its every character in search for characters other than digits.
//If the word doesn't contain any non-digit character, then it is an integer and "true" is returned. Else, "false" is returned.
bool isInt (string word){

    for (int i=0; i<word.length(); i++){
        if (word[i]<'0' || word[i]>'9'){
            return false;
        }
    }

    return true;
}

//Function replaceWithX takes a string str as input and checks if the ASCII value of each character in the string
//falls within the range of ASCII digits. If it does, it indicates that the character is a digit.
//Then, every digit is replaced with the character 'x'.
void replaceWithX (string str){

    int ASCIIval;

    for (int i=0; i<str.length(); i++){
        //Calculating ASCII value of every element in the string
        ASCIIval=str[i];
        if ((ASCIIval>='0') && (ASCIIval<='9')){
            //Replacing digits with 'x'
            str[i]='x';
        }
    }

    //Printing the string with 'x's in places of digits
    cout<<str;
}

//Function replaceWithXIfInt takes a string word as input and utilizes the isInt function to determine if the word
//represents an integer. If it is an integer, the function replaceWithX is called to replace its digits with 'x's. Then,
//the updated word is printed.
void replaceWithXIfInt (string word){

    if (isInt(word)==true){
        replaceWithX(word);
        cout<<" ";
    } else {
        cout<<word<<" ";
    }
}