/*
NYU Tandon Bridge - Week 16 (Winter 2024)
Pascal Symbol Balance Checker

Assignment:
Implement a symbol balance checker for Pascal programming language that verifies
matching pairs of symbols.

Requirements:
1. Check for matching pairs of:
   • {}  (curly braces)
   • ()  (parentheses)
   • []  (square brackets)
   • begin end  (keywords)

2. Function specifications:
   • Input: ifstream object (already open)
   • Output: bool (true if symbols match, false if they don't)
   • Must ignore program code (assignments, expressions)
   • No need to handle comments

Example cases:
begin [{}][] end      -> returns True
begin [{}(]) end      -> returns False

Note: All programs must start with 'begin' and end with 'end'
Include a main function with test cases demonstrating the checker's functionality.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <list>
using namespace std;

const char BEGIN_INDICATOR='b';

class Stack{
private:
    list<char> data;
public:
    void push (char newChar) {data.push_front(newChar);}
    void pop () {if (!isEmpty()){data.pop_front();}}
    char top () {return *data.begin();}
    int size () const {return data.size();}
    bool isEmpty () const {return data.size()==0;}
    void clear () {data.clear();}
};
void openFile (ifstream& inFile){

    string fileName;
    char response;

    cout<<"Please enter filename"<<endl;
    cin>>fileName;
    inFile.open(fileName);

    while (inFile.fail()){
        cout<<"Program couldn't open a file."<<endl;
        cout<<"You entered \""<<fileName<<"\". Did you make a typo or forgot file's extension? (y/n)"<<endl;
        cin>>response;
        cin.ignore(999,'\n');
        if (response=='y' || response=='Y'){
            cout<<"Please reenter file name."<<endl;
            cin>>fileName;
            inFile.clear();
            inFile.open(fileName);
        } else {
            cout<<"Error: Couldn't open a file."<<endl;
            exit (1);
        }
    }

}
bool isLeftBracket (char character){
    return character=='{' || character=='[' || character=='(';
}
bool isRightBracket (char character){
    return character=='}' || character==']' || character==')';
}
bool isBegin (string str){
    return str=="begin";
}
bool isEnd (string str){
    return str=="end";
}

//Function symbolBalanceChecker takes an input file as an input. It reads the data from the input file, pushing
//the pairs allowed in the Pascal programming language ((), [], {}, begin end) onto the stack. Then, it returns
//true if the pairs match or false, if they don't.
bool symbolBalanceChecker(ifstream &inFile) {

    Stack symbols;
    string phrase;
    char character;
    bool atBeginningOfLine=true;
    bool isFileEmpty=true;

    while (inFile.get(character)) {
        isFileEmpty=false;
        if ((character == ' ' && atBeginningOfLine) || (character == '\t' && atBeginningOfLine)) {
            continue; // Skip leading spaces or tabs
        }
        else if (character == ' '){
            atBeginningOfLine=false;
        } else if (character == '\n') {
            atBeginningOfLine=true;
            phrase = ""; // Reset phrase
        } else {
            // Accumulate character into the phrase
            phrase += character;
            // Check if accumulated string is "begin" or "end"
            if (isBegin(phrase) && atBeginningOfLine) {
                symbols.push(BEGIN_INDICATOR);
                phrase = ""; // Reset phrase after encountering "begin"
                atBeginningOfLine=false;
            } else if (isEnd(phrase) && atBeginningOfLine) {
                if (symbols.top() == BEGIN_INDICATOR) {
                    symbols.pop();
                    phrase = ""; // Reset phrase after encountering "end"
                    atBeginningOfLine=false;
                } else {
                    return false;
                }
            }
            // Push left brackets onto the stack
            if (isLeftBracket(character)) {
                symbols.push(character);
            } else if (isRightBracket(character)) {
                // Check and pop matching brackets
                if ((character == ']' && symbols.top() != '[') ||
                    (character == '}' && symbols.top() != '{') ||
                    (character == ')' && symbols.top() != '(')) {
                    return false;
                } else {
                    symbols.pop();
                }
            }
        }
    }

    if (isFileEmpty) {
        cout << "Error: Empty input file!" << endl;
        exit(1);
    } else {
        // Check if all symbols are balanced
        if (symbols.isEmpty()) {
            return true;
        } else {
            return false;
        }
    }
}

int main() {

    ifstream inFile;

    openFile(inFile);

    if (symbolBalanceChecker(inFile)){
        cout<<"All of the pairs match."<<endl;
    } else {
        cout<<"All of the pairs don't match."<<endl;
    }

    inFile.close();

    return 0;
}