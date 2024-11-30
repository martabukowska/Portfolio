/*
NYU Tandon Bridge - Week 10 (Winter 2024)
Sentence to Words Array Converter

Assignment:
Implement string* createWordsArray(string sentence, int& outWordsArrSize) that:
1. Takes a sentence string as input
2. Creates array of individual words
3. Updates outWordsArrSize with number of words
4. Must run in O(n) time where n is sentence length
5. Assumes words are separated by single spaces

Example:
Input: "You can do it"
Output array: ["You", "can", "do", "it"]
outWordsArrSize: 4

Requirements:
- Use string methods (find, substr)
- Linear time complexity
- Write main program to test function
*/

#include <iostream>
#include <string>
using namespace std;

string* createWordsArray(string sentence, int& outWordsArrSize);
void printArrAndItsSize (string *wordsArr, int wordsCount);

//The program takes a sentence as input, extracts the words from the sentence, and prints them along with the size of
//the array containing the words
int main() {

    string line;
    int wordsCount;

    //Getting input sentence
    cout<<"Please enter a sentence:"<<endl;
    getline (cin, line);

    string* wordsArr;
    wordsArr=createWordsArray (line, wordsCount);

    //Testing createWordsArray
    printArrAndItsSize (wordsArr, wordsCount);

    //Deallocating memory
    delete []wordsArr;
    wordsArr=NULL;

    return 0;
}

//Function createWordsArray takes a string 'sentence' and a reference to an integer 'outWordsArrSize' as inputs.
//It counts the number of words in the sentence, creates a dynamic array to store the words from the sentence,
//populates it with those words, and updates 'outWordsArrSize' to reflect the number of words in the sentence.
//Finally, it returns a pointer to the first element of the array containing the words.
string* createWordsArray(string sentence, int& outWordsArrSize){

    //Counting the number of words in the sentence
    int wordsNum=0;
    for (int i=0; i<sentence.length(); i++) {
        if (i==sentence.length()-1 && sentence[i] != ' ') {
            wordsNum++;
        } else {
            if (sentence[i] == ' ')
                wordsNum++;
        }
    }

    //Updating outWordsArrSize with the number of words
    outWordsArrSize=wordsNum;

    //Creating dynamic array that's going to store the all words from the sentence
    string* wordsArrPtr = new string [wordsNum];
    string word;

    int ind=0, wordStart=0;

    for (int i=0; i<sentence.length(); i++){
        if (sentence[i]==' '){
            //Separating a word from the sentence every time the space is encountered and adding it to the array
            word=sentence.substr(wordStart, i-wordStart);
            wordsArrPtr[ind]=word;
            ind++;
            //Updating wordStart for next iteration
            wordStart=i+1;
        } else if (i==sentence.length()-1){
            //Handling the case when the last word is encountered and it isn't followed by a space
            word=sentence.substr(wordStart, i-wordStart+1);
            wordsArrPtr[ind]=word;
            ind++;
        }
    }

    return wordsArrPtr;

}

//Function printArrAndItsSize takes a pointer to the first element of the array wordsArr and wordsCount,
//and prints the elements of the array along with its size.
void printArrAndItsSize (string *wordsArr, int wordsCount){

    cout<<"Elements of the array: [";
    for (int i=0; i<wordsCount; i++){
        if (i!=wordsCount-1) {
            cout << "\"" << wordsArr[i] << "\", ";
        } else {
            cout << "\"" << wordsArr[i] << "\"";
        }
    }
    cout<<"]"<<endl;

    cout<<"Size of the array: "<<wordsCount<<endl;

}