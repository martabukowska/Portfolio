/*
NYU Tandon Bridge - Week 15 (Winter 2024)
Dinner Bill Splitting Calculator

Assignment:
Write a program that calculates how to balance dinner payments among friends, reading
input from a file and using a linked list to process the data.

Requirements:
1. File Input:
   • Read filename from user
   • Parse lines containing name and amount paid
   • Handle full names (2-3 parts) separated by spaces
   • Amount and names are space-separated

2. Data Structure:
   • Design custom linked list and node classes
   • Node should store:
     - Person's name
     - Amount paid
     - Amount owed (initialized to 0)
   • Implement necessary list operations

3. Processing:
   • Calculate target amount (total sum / number of people)
   • Determine who owes and who is owed
   • Generate step-by-step payment instructions

Example:
Input file:
76.8 Jason J. Jones
89 Natalie Johnson
11 Dalton Smith
44.2 Sheryl Carter
0 John Martin

Output:
Dalton Smith, you give Jason J. Jones $32.60
Dalton Smith, you give Natalie Johnson $0.60
John Martin, you give Natalie Johnson $44.20

Note: Don't modify input file, no need to round to 2 decimal places
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <iomanip>
using namespace std;

class Person {
private:
    string name;
    double amountPaid;
    double amountOwed;
public:
    Person() : name(""), amountPaid(0), amountOwed(0) {};
    Person(string newName, double newAmountPaid) : name(newName), amountPaid(newAmountPaid), amountOwed(0) {};
    string getName() const;
    double getAmountPaid() const;
    double getAmountOwed() const;
    void setName(string newName);
    void setAmountPaid(double amount);
    void setAmountOwed(double amount);
};
class Node {
private:
    Person data;
    Node* prev;
    Node* next;
public:
    Node () : data(), prev(nullptr), next(nullptr){}
    Node(const Person& newData, Node* newPrev, Node* newNext) : data(newData), prev(newPrev), next(newNext) {}
    friend class List;
};
class List{
private:
    Node* head;
    Node* tail;
    int size;
    double averageAmount;
public:
    List (): head(nullptr), tail(nullptr), size(0), averageAmount(0){}
    List(const List& rhs); //Copy constructor
    List& operator = (const List& rhs); //Assignment operator
    ~List(); //Destructor
    double getAverageAmount () const;
    int getSize() const;
    void setAverageAmount ();
    void setAmountsOwed();
    void createFirstNode(const Person& firstPerson);
    void insertAtPoint(Node* prevNode, const Person& newData);
    void insertAtEnd(const Person& newData);
    void createAndPopulateVectors (vector <Node*> &peopleWhoOwe,  vector <Node*> &peopleWhoAreOwed,  vector <Node*> &peopleWhoDontNeedToDoAnything);
    void reimbursementInstructions ();
    void clearNodes();
};

void openInputFile (ifstream& inFile){

    string fileName;
    char response;

    cout<<"Please enter the full name of the file which contains the input data (for example, input.txt)"<<endl;
    cin>>fileName;

    inFile.open(fileName);

    while (inFile.fail()){
        cout<<"File failed to open. You entered \""<<fileName<<"\". Did you make a typo or forgot file's extension (y/n)?"<<endl;
        cin>>response;
        cin.ignore(1000,'\n');
        if (response=='y' || response=='Y'){
            cout<<"Please reenter the file name."<<endl;
            cin>>fileName;
            inFile.clear();
            inFile.open(fileName);
        } else {
            cout<<"Error: Program couldn't open a file."<<endl;
            exit(1);
        }

    }

}
void readData (ifstream& inFile, List& list){

    double amount;
    string name;
    int count=0;

        while (inFile >> amount) { //Read amount
            //Ignore the spaces between amount and name
            inFile.ignore(1000, ' ');
            //Read name
            getline(inFile, name);
            //Remove end of line character
            int nameSize = name.length() - 1;
            if (name[nameSize] == '\n') {
                name = name.substr(0, nameSize);
            }
            //Create person object and create node
            Person newPerson(name, amount);
            if (list.getSize() == 0) { //Create first node
                list.createFirstNode(newPerson);
            } else { //Create remaining nodes
                list.insertAtEnd(newPerson);
            }
            count++;
        }

    if (count==0){//Nothing got read - empty file
        cout<<"Error: Empty input file!"<<endl;
        exit(1);
    }
    //Calculate average amount
    list.setAverageAmount();
}

int main() {

    ifstream inFile;
    List list;

    //Open the file
    openInputFile(inFile);
    //Read in data
    readData(inFile, list);
    //Calculate owed amounts
    list.setAmountsOwed();
    //Print instructions
    list.reimbursementInstructions();

    return 0;
}

// Person class member functions
string Person :: getName() const {
    return name;
}
double Person :: getAmountPaid() const {
    return amountPaid;
}
double Person :: getAmountOwed() const {
    return amountOwed;
}
void Person :: setName(string newName){
    name=newName;
}
void Person ::setAmountPaid(double amount) {
    amountPaid=amount;
}
void Person ::setAmountOwed(double amount) {
    amountOwed=amount;
}

// List class member functions
void List :: createFirstNode (const Person& firstPerson){
    Node* firstNode = new Node(firstPerson, nullptr, nullptr);
    //Having created the first node we can set the head and tail to point to the first node
    head=firstNode;
    tail=firstNode;
    size++;
}
void List :: insertAtPoint(Node* prevNode, const Person& newData){
    if (prevNode!=nullptr) {
        //Create new node whose prev is the prevNode's address and whose next is prevNode's next
        Node *newNode = new Node(newData, prevNode, prevNode->next);
        //Update prevNode's pointers accordingly
        prevNode->next = newNode;
        //If newNode is not the last node, update the prev pointer of the next node
        if (newNode->next != nullptr) {
            newNode->next->prev = newNode;
        } else {
            //Otherwise, newNode becomes the new tail
            tail = newNode;
        }
        size++;
    }
}
void List :: insertAtEnd(const Person& newData){
    insertAtPoint(tail, newData);
}
double List :: getAverageAmount () const{
    return averageAmount;
}
void List :: setAverageAmount (){
    double sum=0;
    for (Node* iter=head; iter!=nullptr; iter=iter->next){
        sum=sum+iter->data.getAmountPaid();
    }
    averageAmount=sum/double(size);
}
void List :: setAmountsOwed() {
    for (Node* iter=head; iter!= nullptr; iter=iter->next){
        iter->data.setAmountOwed(iter->data.getAmountPaid()-averageAmount);
    }
}
void List :: createAndPopulateVectors (vector <Node*> &peopleWhoOwe,  vector <Node*> &peopleWhoAreOwed,  vector <Node*> &peopleWhoDontNeedToDoAnything){

    for (Node* iter=head; iter!= nullptr; iter=iter->next) {
        if (iter->data.getAmountOwed() == 0) {
            peopleWhoDontNeedToDoAnything.push_back(iter);
        } else if (iter->data.getAmountOwed() < 0){ // Turn the negative values into absolute values
            double amount=iter->data.getAmountOwed();
            amount=abs(amount);
            iter->data.setAmountOwed(amount);
            peopleWhoOwe.push_back(iter);
        } else {
            peopleWhoAreOwed.push_back(iter);
        }
    }

}
void List :: reimbursementInstructions (){

    vector <Node*> peopleWhoOwe;
    vector <Node*> peopleWhoAreOwed;
    vector <Node*> peopleWhoDontNeedToDoAnything;

    //Populate the above 3 vectors with people who owe money, people who are owed money, and people who don't need
    //to do anything. Turn the negative amounts into absolute values.
    createAndPopulateVectors(peopleWhoOwe, peopleWhoAreOwed, peopleWhoDontNeedToDoAnything);

    //Print the statements for the people who don't need to do anything
    for (unsigned int i=0; i<peopleWhoDontNeedToDoAnything.size(); i++){
        cout << peopleWhoDontNeedToDoAnything[i]->data.getName()<< ", you don't need to do anything"<<endl;
    }

    //Calculate who should pay who what amount and print appropriate statements
    unsigned int i=0, j=0;
    while (i<peopleWhoOwe.size() && j<peopleWhoAreOwed.size()){
        //Get the amount owed and amount to be paid back of two people, one who owes and one who is owed
        double amountOwed = peopleWhoOwe[i]->data.getAmountOwed();
        double amountToBePaidBack = peopleWhoAreOwed[j]->data.getAmountOwed();
        //Calculate the value that will be exchanged (the smallest of amountOwed and amountToBePaidBack)
        double exchangedAmount=min(amountOwed, amountToBePaidBack);
        //Print appropriate statement
        cout.setf(ios::fixed);
        cout.setf(ios::showpoint);
        cout<<peopleWhoOwe[i]->data.getName()<<", you give "<<peopleWhoAreOwed[j]->data.getName()<<" $"<<setprecision(2)<<exchangedAmount<<endl;
        //Update amounts accordingly, so that they reflect the operations that took place
        double amountOwedAfter = amountOwed - exchangedAmount;
        double amountToBePaidBackAfter = amountToBePaidBack - exchangedAmount;
        peopleWhoOwe[i]->data.setAmountOwed(amountOwedAfter);
        peopleWhoAreOwed[j]->data.setAmountOwed(amountToBePaidBackAfter);
        //Update indexes of vectors
        if (amountOwedAfter==0){
            i++;
        } else if (amountToBePaidBackAfter==0){
            j++;
        } else {
            i++;
            j++;
        }
    }
    //Print comment about the amount everyone paid
    cout<<"In the end, you should have all spent around $"<<getAverageAmount()<<endl;

}
List :: List (const List& rhs){

    size=rhs.size;
    averageAmount=rhs.averageAmount;

    //Create a new one node list and copy the first node of rhs list to it
    head=new Node(rhs.head->data, nullptr, nullptr);
    tail=head;

    //Create remaining new nodes and copy the data of the rhs list nodes to it
    for (Node* iter = rhs.head->next; iter != nullptr; iter = iter->next) {
        this->insertAtEnd(iter->data);
    }
}
List :: ~List  () {
    clearNodes();
    delete head;
    delete tail;
    head= nullptr;
    tail= nullptr;
}
int List :: getSize() const {
    return size;
}
List& List :: operator = (const List& rhs){

    //Check for self-assignment
    if (this==&rhs){
        return *this;
    }
    //Clear lhs
    clearNodes();
    //If the rhs list isn't empty, create the first node and copy rhs head's value to it. Then do the same for all remaining nodes
    if (rhs.head!=nullptr) {
        createFirstNode(rhs.head->data);
        for (Node *iter = rhs.head->next; iter != nullptr; iter = iter->next) {
            this->insertAtEnd(iter->data);
        }
    } else {
        head=nullptr;
    }
    //Update remaining variables
    size = rhs.size;
    averageAmount = rhs.averageAmount;
    //Return modified object
    return *this;
}
void List :: clearNodes (){
    while (head != nullptr) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
    tail = nullptr;
    size = 0;
}