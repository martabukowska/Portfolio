/*
NYU Tandon Bridge - Week 7 (Winter 2024)
Perfect and Amicable Numbers Calculator

Assignment:
Implement functions to work with special number relationships:

1. void analyzeDividors(int num, int& outCountDivs, int& outSumDivs)
   - Finds proper divisors (all divisors except number itself)
   - Must run in O(√num) time
   - Updates count and sum of proper divisors

2. bool isPerfect(int num)
   - Perfect number equals sum of its proper divisors
   - Example: 6 = 1 + 2 + 3
   - Uses analyzeDividors function

3. Main program to find:
   - All perfect numbers between 2 and M
   - All amicable number pairs between 2 and M
   - Amicable pairs: each number equals sum of other's proper divisors
   - Example: 220 and 284 are amicable

Example interaction:
Please enter a positive integer M that is >=2: 300
The set of perfect numbers between 2 and entered number consists of:
6
28

The pairs of amicable numbers that are between 2 and entered number are:
(220, 284)
*/

#include <iostream>
#include <cmath>
using namespace std;

void analyzeDividors(int num, int& outCountDivs, int& outSumDivs);

bool isPerfect(int num);

int main() {

    int M, countPerfect=0, countAmi=0;

    cout<<"Please enter a positive integer M that is >=2: ";
    cin>>M;

    //Printing all of the perfect numbers between 2 and M:
    cout<<"The set of perfect numbers between 2 and entered number consists of:"<<endl;
    for (int currNum=2; currNum<=M; currNum++) {
        if (isPerfect(currNum) == 1) {
            cout << currNum << endl;
            countPerfect = countPerfect + 1; //Counting perfect numbers, to be able to say there aren't any, if there aren't
        }
    }

    //Printing that there are no perfect numbers between 2 and M, in case there aren't any
    if (countPerfect==0){
        cout<<"There are no perfect numbers between 2 and entered number"<<endl;
    }

    //Printing pairs of amicable numbers that are between 2 and M
    //Variable ami1 is potentially first, and variable ami2 - potentially second amicable number in the pair
    //ami1 and ami 2 form a pair of amicable numbers, if sum of divisors of ami1 is equal to ami2, and vice versa
    //(ami1=sumDivsAmi2 and ami2=sumDivsAmi1)
    cout<<"The pairs of amicable numbers that are between 2 and entered number are:"<<endl;
    for (int ami1 = 2; ami1 <= M; ami1++) {
        //Calculating sum of divisors of ami1 (sumDivsAmi1)
        int countDivsAmi1, sumDivsAmi1;
        analyzeDividors(ami1, countDivsAmi1, sumDivsAmi1);
        //Assigning ami2 the value of sumDivsAmi1 (ami2=sumDivsAmi1), and calculating the sum of divisors of ami2 (sumDivsAmi2)
        int ami2=sumDivsAmi1, countDivsAmi2, sumDivsAmi2;
        analyzeDividors(ami2, countDivsAmi2, sumDivsAmi2);
        //Checking if ami1=sumDivsAmi2 (since ami2=sumDivsAmi1). If it's true, we can print an amicable pair
        //Making sure that ami2 is within the range [ami1, M], by including conditions (ami2<=M) and (ami2>ami1)
        //The second condition ensures that a given pair of amicable numbers is printed only once (and not twice, in
        //2 different orders)
        if ((ami1==sumDivsAmi2)&&(ami2<=M)&&(ami2>ami1)){
            cout << "(" << ami1 << ", " << ami2 << ")" << endl;
            countAmi=countAmi+1;
        }
    }

    //Printing that there are no pairs of amicable numbers between 2 and M, in case there aren't any
    if (countAmi==0){
        cout<<"There are no pairs of amicable numbers between 2 and entered number"<<endl;
    }

    return 0;
}

//Function analyzeDividors takes as an input a positive integer num (>=2), and updates two output parameters with
//the number of num's proper divisors and their sum
void analyzeDividors(int num, int& outCountDivs, int& outSumDivs){

    int div=1;
    outCountDivs=0, outSumDivs=0;

    while (div<=sqrt(num)) {
        //Checking divisors <= sqrt(num)
        int div_1=div, div_2=num/div;
        if (num % div_1 == 0){
            outSumDivs=outSumDivs+div_1;
            outCountDivs=outCountDivs+1;
            //Checking divisors > sqrt(num)
            if ((div_2 != num) && (div_2 != div_1)){
                outSumDivs=outSumDivs+div_2;
                outCountDivs=outCountDivs+1;
            }
        }
        div=div+1; //Updating div for next iteration
    }
}

//Function isPerfect checks if given number is a perfect number, and accordingly returns true (1) or false (0)
bool isPerfect(int num){

    int sumDivs, countDivs;

    analyzeDividors(num, countDivs, sumDivs);

    return num==sumDivs;

}