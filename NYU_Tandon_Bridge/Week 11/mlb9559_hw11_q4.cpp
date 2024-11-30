/*
NYU Tandon Bridge - Week 11 (Winter 2024)
Jump It Game Solver

Assignment:
Write a recursive function that finds minimum cost path in "Jump It" game where:
1. Board has n columns, first column always 0
2. Each number represents cost to enter that column
3. From any position, can:
   - Move to adjacent column
   - Jump over adjacent to land two columns over
4. Goal: Get from first to last column with lowest total cost

Example board:
[0] [3] [80] [6] [57] [10]

Sample paths:
Path 1: 0 -> 80 -> 57 -> 10 = 147
Path 2: 0 -> 3 -> 6 -> 10 = 19 (optimal)

Requirements:
- Must use recursion
- Return only the minimum cost, not the path
- Function should take array and handle any valid board size
- First column (index 0) always contains 0

Example call:
int board[] = {0, 3, 80, 6, 57, 10};
int minCost = jumpIt(board, 6); // returns 19
*/

#include <iostream>
using namespace std;

int lowestCostOfTheGame (int arr[], int arrSize);
int printArr (int arr[], int arrSize);

int main() {

    int game1[6]={0,3, 80, 6, 57, 10};
    int gameSize1=6;
    int game2[3]={0,13, 22};
    int gameSize2=3;
    int game3[4]={0,5,13, 22};
    int gameSize3=4;
    int game4[5]={0,5,5, 5,6};
    int gameSize4=5;

    cout<<"Game 1: ";
    printArr(game1, gameSize1);
    cout<<endl;
    cout<<"The lowest cost of the Game 1: "<<lowestCostOfTheGame(game1, gameSize1)<<endl;

    cout<<"Game 2: ";
    printArr(game2, gameSize2);
    cout<<endl;
    cout<<"The lowest cost of the Game 2: "<<lowestCostOfTheGame(game2, gameSize2)<<endl;

    cout<<"Game 3: ";
    printArr(game3, gameSize3);
    cout<<endl;
    cout<<"The lowest cost of the Game 3: "<<lowestCostOfTheGame(game3, gameSize3)<<endl;

    cout<<"Game 4: ";
    printArr(game4, gameSize4);
    cout<<endl;
    cout<<"The lowest cost of the Game 4: "<<lowestCostOfTheGame(game4, gameSize4)<<endl;

    return 0;
}

//Function lowestCostOfTheGame takes an array of integers (game), representing the cost of each column in the game,
//and gameSize, which indicates the number of columns in the game. It recursively calculates the lowest cost to
//complete the game and returns it.
int lowestCostOfTheGame (int game[], int gameSize){

    if (gameSize<=3){
        //BASE CASE
        //If we are two steps before the finish, no matter the cost of the columns in between,the least costly option
        //will be moving straight to the finish.
        //If we are one step before the finish, we have no other option than to move to the adjacent column.
        //Therefore, if the game consists of 3 columns or fewer, the cost of the last column is returned.
        return game[gameSize-1];
    } else{
        //RECURSIVE STEP
        //Calculating current total cost in two versions: with one step being made and with two steps being made
        int costWithOneStep=lowestCostOfTheGame(game, gameSize - 1);
        int costWithTwoSteps=lowestCostOfTheGame(game, gameSize - 2);
        //Calculating cost of current step
        int costOfCurrStep=game[gameSize-1];
        //Checking if making one or two steps is less costly, and choosing the appropriate option
        if (costWithOneStep<costWithTwoSteps) {
            return costWithOneStep+costOfCurrStep;
        } else {
            return costWithTwoSteps+costOfCurrStep;
        }
    }

}

//Function printArr is given array of integers, and it's size as input, and it prints all the elements of the array
//separated by space
int printArr (int arr[], int arrSize){

    for (int i=0; i<arrSize; i++){
        cout<<arr[i]<<" ";
    }
}