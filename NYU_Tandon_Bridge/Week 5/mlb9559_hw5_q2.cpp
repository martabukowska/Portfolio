/*
NYU Tandon Bridge - Week 5 (Winter 2024)
Number Guessing Game

Assignment:
Implement a number guessing game. The program should randomly choose an integer
between 1 and 100 (inclusive), and have the user try to guess that number.

Implementation guidelines:
1. The user can guess at most 5 times.
2. Before each guess the program announces:
   • An updated guessing range, taking into account previous guesses and responses.
   • The number of guesses that the user has left.
3. If the user guessed correctly, the program should announce that, and also tell how many
   guesses were used.
4. If the user guessed wrong, and there are still guesses left, the program should tell the
   user if their number is bigger or smaller than the number that they guessed.
5. If the user didn't guess the number in all of the 5 tries, the program should reveal the
   number it chose.
6. Follow the execution examples below for the exact format.

Note: Random number generation requires:
• #include <cstdlib> and <ctime>
• using namespace std;
• srand(time(0)) for initialization
• rand() function for generating random integers
*/

#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

const int MAX_GUESSES=5;

int main() {

    int random_number, range_min=1, range_max=100, guess, number_of_guesses;

    //generating random number:
    srand(time(0));
    random_number=(rand()%100)+1;

    cout<<"I thought of a number between 1 and 100! Try to guess it,"<<endl;

    for (int count=MAX_GUESSES; count>=1; count--){
        cout<<"Range: ["<<range_min<<", "<<range_max<<"], Number of guesses left: "<<count<<endl;
        //getting user input
        cout<<"Your guess: ";
        cin>>guess;
        //checking if the guessed number is =, >, or < than our random number
        if (guess==random_number){
            number_of_guesses=MAX_GUESSES-count;
            cout<<"Congrats! You guessed my number in "<<number_of_guesses<<" guesses."<<endl;
            break;
        } else if (guess>random_number){
            if (count!=1) {
                cout << "Wrong! My number is smaller" << endl;
                cout << endl;
                //updating range for next iteration (guess is the maximum of the updated range)
                range_max = guess;
            } else {
                cout<<"Out of guesses! My number is "<<random_number<<endl;
            }
        } else {
            if (count!=1) {
                cout << "Wrong! My number is bigger" << endl;
                cout << endl;
                //updating range for next iteration (guess is the minimum of the updated range)
                range_min = guess;
            } else {
                cout<<"Out of guesses! My number is "<<random_number<<endl;
            }
        }
    }

    return 0;
}