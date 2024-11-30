/*
NYU Tandon Bridge - Week 12 (Winter 2024)
Checkbook Balancing Program

Assignment:
Write a checkbook balancing program that reads the following for all checks that were not
cashed as of the last time you balanced your checkbook:
- The number of each check (int)
- The amount of the check (double)
- Whether or not it has been cashed (1 or 0, boolean in the array)

Implementation Requirements:
1. Use a class for checks with member variables:
   • Check number
   • Check amount (using Money class type)
   • Cashed status
2. Class should include:
   • Constructors
   • Accessor and mutator functions
   • Input/output functions
3. Program should read:
   • All check information
   • All deposits
   • Old and new account balance
4. Program should output:
   • Total of cashed checks
   • Total of deposits
   • Expected new balance
   • Difference from bank's stated balance
   • Two sorted lists:
     - Cashed checks since last balance
     - Checks still not cashed

Note: The Money class (from Display 11.9) should be used for check amounts
*/

#include <iostream>
#include <cstdlib>
#include <string>
using namespace std;

class Money {
private:
    long allCents;
public:
    //CONSTRUCTORS:
    Money ();
    Money (long dollars);
    Money (long dollars, int cents);
    //GETTERS:
    double getValue () const;
    //SETTERS:
    void setValue (double amount);
    //OVERLOADED OPERATORS:
    friend Money operator + (const Money& amount1, const Money& amount2);
    friend Money operator - (const Money& amount1, const Money& amount2);
    friend bool operator < (const Money& left, const Money& right);
    friend istream& operator >> (istream& ins, Money& amount);
    friend ostream& operator << (ostream& outs, const Money& amount);
};
class Check {
private:
    int checkNum;
    Money checkAmount;
    bool wasCashed;
public:
    //CONSTRUCTORS:
    Check ();
    //GETTERS:
    int getCheckNum () const;
    Money getCheckAmount () const;
    bool getWasCashed () const;
    //SETTERS:
    void setCheckNum (int number);
    void setCheckAmount(double amount);
    void setWasCashed (bool wasCashedIndicator);
    //OVERLOADED OPERATORS:
    Check& operator = (const Check& equalTo);
    friend istream& operator >> (istream& ins, Check& checkData);
    friend ostream& operator << (ostream& outs, const Check& checksData);
};

//NON-MEMBER FUNCTIONS
void checkInputInformation (){

    cout<<"CHECKBOOK BALANCING PROGRAM"<<endl;
    cout<<endl;
    cout<<"CHECK DATA ENTRY"<<endl;
    cout << "Please enter the information regarding your un-cashed checks as of your last checkbook balance, using the following format: \n"
            "check number (positive integer), amount of the check (in the format #.##), and '1' if the check was cashed or '0' if it wasn't cashed. \n"
            "Separate each piece of information by a single space, ensure each check is entered on a separate line, and end your input by typing \"0\".\n"
            "Example of a single check data entry: 5 100.99 1"<<endl;

}
void depositsInputInformation () {

    cout<<endl;
    cout<<"DEPOSIT DATA ENTRY"<<endl;
    cout<<"Please enter your deposit amounts each in separate line, in format #.##. End your input by entering \"0\"."<<endl;

}
void accountBalanceInput (Money &oldBalance, Money &userProvidedBalance){

    cout<<endl;
    cout<<"ACCOUNT BALANCE DATA ENTRY"<<endl;

    cout<<"Please enter the prior account balance (in form #.##):"<<endl;
    cin>>oldBalance;

    cout<<"Please enter the new account balance according to account holder (in form #.##):"<<endl;
    cin>>userProvidedBalance;

}
void outputDivider (){

    cout<<endl;
    cout<<"OUTPUT INFORMATION"<<endl;

}
void populateChecksArr (Check *&arr, int &arrSize) {
    int ind = 0, newArrSize;
    int number;
    double amount;
    string wasCashedIndicator;

    while (ind < arrSize) {
        //Get check number, check if it is valid
        cin >> number;
        // Check if the input is 0, if it is break out of the loop (end of input)
        if (number == 0) {
            break;
        }
        //Get the rest of the input
        cin>>amount>>wasCashedIndicator;
        //Check for invalid input
        if (number<0 || (wasCashedIndicator!="0" && wasCashedIndicator!="1")){
            cout<<"Error: Invalid form for check number."<<endl;
            exit(1);
        }
        arr[ind].setCheckNum (number);
        arr[ind].setCheckAmount(amount);
        if (wasCashedIndicator=="0"){
            arr[ind].setWasCashed(false);
        } else {
            arr[ind].setWasCashed(true);
        }
        ind++;
        // Check if the array needs resizing
        if (ind == arrSize) {
            newArrSize = arrSize * 2;
            Check *resizedArr = new Check[newArrSize];
            for (int i = 0; i < arrSize; i++) {
                resizedArr[i] = arr[i];
            }
            delete[] arr;
            arr = resizedArr;
            arrSize = newArrSize;
        }
    }
    // Check if the array needs resizing
    if (ind == arrSize) {
        newArrSize = arrSize * 2;
        Check *resizedArr = new Check[newArrSize];
        for (int i = 0; i < arrSize; i++) {
            resizedArr[i] = arr[i];
        }
        delete[] arr;
        arr = resizedArr;
        arrSize = newArrSize;
    }
    arrSize=ind;
}
void populateDepositsArr (Money *&arr, int &arrSize) {
    int ind = 0, newArrSize;
    double amount;

    while (ind<arrSize) {
        cin >> amount;
        // Check if the input is 0, if it is break out of the loop
        if (amount == 0) {
            break;
        }
        arr[ind].setValue(amount);
        ind++;
        // Check if the array needs resizing
        if (ind == arrSize) {
            newArrSize = arrSize * 2;
            Money *resizedArr = new Money[newArrSize];
            for (int i = 0; i < arrSize; i++) {
                resizedArr[i] = arr[i];
            }
            delete[] arr;
            arr = resizedArr;
            arrSize = newArrSize;
        }
    }
    // Check if the array needs resizing
    if (ind == arrSize) {
        newArrSize = arrSize * 2;
        Money *resizedArr = new Money[newArrSize];
        for (int i = 0; i < arrSize; i++) {
            resizedArr[i] = arr[i];
        }
        delete[] arr;
        arr = resizedArr;
        arrSize = newArrSize;
    }
    arrSize = ind;
}
int digitToInt (char c){
    return (static_cast <int> (c) - static_cast <int> ('0'));
}
Money getTotalCashedChecks (const Check *checksData, int checksNum){

    Money sum = 0;

    for (int i = 0; i < checksNum; i++){
        if (checksData[i].getWasCashed()){
            sum = sum + checksData[i].getCheckAmount();
        }
    }

    return sum;
}
Money getTotalDeposits (const Money *depositsData, int depositsNum){

    Money sum = 0;

    for (int i = 0; i < depositsNum; i++){
        sum = sum + depositsData[i];
    }

    return sum;
}
void sortChecks (Check *checksData, int checksNum) {

    for (int i = 0; i < checksNum - 1; i++) {
        for (int j = 0; j < checksNum - i - 1; j++) {
            if (checksData[j + 1].getCheckNum() < checksData[j].getCheckNum()) {
                Check temp = checksData[j];
                checksData[j] = checksData[j + 1];
                checksData[j + 1] = temp;
            }
        }
    }
}
void printCashedChecks (const Check *checksData, int checksNum){

    cout<<"Cashed checks:"<<endl;

    for (int i = 0; i < checksNum; i++){
        if (checksData[i].getWasCashed()){
            cout<<"\t"<<checksData[i]<<endl;
        }
    }

}
void printUncashedChecks (const Check *checksData, int checksNum){

    cout<<"Un-cashed checks:"<<endl;

    for (int i = 0; i < checksNum; i++){
        if (!checksData[i].getWasCashed()){
            cout<<"\t"<<checksData[i]<<endl;
        }
    }

}

//MONEY CLASS MEMBER FUNCTIONS:
Money :: Money () {
    allCents=0;
}
Money :: Money (long dollars) {
    allCents=dollars*100;
}
Money :: Money (long dollars, int cents){

    if (dollars*cents<0){ //output if cents are negative and dollars are positive or the other way around
        cout<<"Illegal values for dollars and cents.\n";
        exit(1);
    }
    allCents=dollars*100+cents;
}
double Money :: getValue () const {
    return allCents*0.01;
}
void Money :: setValue (double amount){
    this->allCents=amount*100;
}
Money operator + (const Money& amount1, const Money& amount2){
    Money temp;
    temp.allCents=amount1.allCents+amount2.allCents;
    return temp;
}
Money operator - (const Money& amount1, const Money& amount2){
    Money temp;
    temp.allCents = amount1.allCents - amount2.allCents;
    return temp;
}
bool operator < (const Money& left, const Money& right){
    return left.allCents<right.allCents;
}
istream& operator >>(istream& ins, Money& amount){
    char decimalPoint, digit1, digit2; //for input in form dollars.digit1digit2
    long dollars;
    int cents;
    bool isNegative;

    //get user input
    ins>>dollars>>decimalPoint>>digit1>>digit2;
    //check if inputted dollars amount is negative
    if (dollars<0){
        isNegative=true;
    } else {
        isNegative=false;
    }
    //check for invalid input
    if (decimalPoint!='.' || !isdigit(digit1) || !isdigit(digit2)){
        cout<<"Errol illegal form for money input"<<endl;
        exit(1);
    }
    //calculate cents
    cents=digitToInt(digit1)*10+digitToInt(digit2);
    //calculate total amount of cents
    amount.allCents=dollars*100+cents;
    //account for negative number if applies
    if (isNegative){
        amount.allCents=-amount.allCents;
    }

    return ins;
}
ostream& operator<<(ostream& outs, const Money& amount) {

    long positiveCents, dollars, cents;
    positiveCents=labs(amount.allCents);
    dollars=positiveCents/100;
    cents=positiveCents%100;

    if (amount.allCents<0){
        outs<<"-$"<<dollars<<'.';
    } else {
        outs<<'$'<<dollars<<'.';
    }
    if (cents<10){
        outs<<'0';
    }
    outs<<cents;

    return outs;
}

//CHECK CLASS MEMBER FUNCTIONS:
Check :: Check () {
    checkNum=0;
    checkAmount=Money();
    wasCashed=false;
}
int Check :: getCheckNum () const {
    return checkNum;
}
Money Check :: getCheckAmount () const {
    return checkAmount;
}
bool Check :: getWasCashed () const {
    return wasCashed;
}
void Check :: setCheckNum (int number){
    this->checkNum=number;
}
void Check::setCheckAmount(double amount) {
    checkAmount.setValue(amount);
}
void Check :: setWasCashed (bool wasCashedIndicator){
    this->wasCashed=wasCashedIndicator;
}
Check& Check :: operator = (const Check& equalTo){
    checkNum = equalTo.checkNum;
    checkAmount = equalTo.checkAmount;
    wasCashed = equalTo.wasCashed;
    return *this;
}
istream& operator>>(istream& ins, Check& checkData) {
    int number;
    double amount;
    string wasCashedIndicator;

    ins >> number;

    if (number < 0) {
        cout << "Error: Check number must be a positive integer." << endl;
        exit(1);
    } else {
        checkData.setCheckNum(number);
    }

    // Read the check amount as a double
    ins >> amount;
    checkData.setCheckAmount(amount);

    // Read the wasCashedIndicator
    ins >> wasCashedIndicator;

    if (wasCashedIndicator != "1" && wasCashedIndicator != "0") {
        cout << "Error: illegal form indicating whether the check has been cashed or not\n";
        exit(1);
    } else {
        if (wasCashedIndicator=="0") {
            checkData.setWasCashed(false);
        } else {
            checkData.setWasCashed(true);
        }
    }

    return ins;
}
ostream& operator<<(ostream& outs, const Check& checksData) {
    outs<<"Check number "<<checksData.getCheckNum()<<" with amount "<<checksData.getCheckAmount();
    if (checksData.getWasCashed()) {
        outs<<" (cashed)";
    } else {
        outs<<" (un-cashed)";
    }

    return outs;
}

int main() {

    int checksNum=10, depositsNum=10;
    Money oldBalance, userProvidedBalance, calculatedBalance, difference, totalDeposits, totalCashedChecks;

    //Printing input instructions regarding checks, creating dynamic array for storing checks data and populating it with user input
    checkInputInformation();
    Check *checksData = new Check [checksNum];
    populateChecksArr (checksData, checksNum);

    //Printing input instructions regarding deposits, creating dynamic array for storing deposits amounts and populating it with user input
    depositsInputInformation ();
    Money *depositsData = new Money [depositsNum];
    populateDepositsArr (depositsData, depositsNum);

    //Getting old account balance and new account balance calculated by the bank
    accountBalanceInput(oldBalance, userProvidedBalance);

    //Calculating the new balance
    totalDeposits=getTotalDeposits(depositsData, depositsNum);
    totalCashedChecks=getTotalCashedChecks(checksData, checksNum);
    calculatedBalance=oldBalance+totalDeposits-totalCashedChecks;
    difference=calculatedBalance-userProvidedBalance;

    //Printing program's output
    outputDivider ();
    cout<<"Total of the checks cashed: "<<totalCashedChecks<<endl;
    cout<<"Total of the deposits: "<<totalDeposits<<endl;
    cout<<"Calculated new account balance: "<<calculatedBalance<<endl;
    cout<<"Difference between the entered balance and the calculated balance: "<<difference<<endl;
    sortChecks (checksData, checksNum);
    printCashedChecks(checksData, checksNum);
    printUncashedChecks(checksData, checksNum);

    delete [] checksData;
    delete [] depositsData;
    checksData=nullptr;
    depositsData=nullptr;

    return 0;
}