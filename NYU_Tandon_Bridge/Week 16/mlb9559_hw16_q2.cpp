/*
NYU Tandon Bridge - Week 16 (Winter 2024)
Vector-Based Queue Implementation

Assignment:
Implement a queue class using a vector as the underlying data structure, optimizing
for performance by tracking start and end positions instead of removing elements.

Requirements:
1. Class structure:
   • Use vector as underlying storage
   • Track start and end positions of queue
   • Reset vector when queue becomes empty

2. Operations to implement:
   • push: Add element to end of queue
   • pop: Remove element from front of queue
   • front: Access first element
   • empty: Check if queue is empty
   • size: Return number of elements in queue

3. Performance considerations:
   • pop should be O(1) by just moving start position
   • Avoid unnecessary vector resizing
   • Reset vector when queue becomes empty to prevent memory waste

Example usage:
Queue: [ _ _ 3 4 5 ]
       start^   ^end
After pop: [ _ _ _ 4 5 ]
              start^ ^end

Note: No main function required, just the class implementation
*/

#include <iostream>
#include <vector>
using namespace std;

template <class T>
class Queue {
private:
    vector <T> queue;
    int startInd;
    int size;
public:
    Queue() :  queue(), startInd(0), size(0) {}
    void push (T newItem);
    T pop ();
    T top ();
    T end ();
    int getSize() const;
    int getStartInd();
    int getEndInd();
    bool isEmpty() const;
    void clear();
};
template <class T> void testQueue (Queue <T> testQueue);
template <class T> void testEmptyQueue (Queue <T> testQueue);

int main() {

    Queue<int> queue;

    cout<<"Pushing 5 elements onto the queue..."<<endl;
    for (int i=0; i<5; i++){
        queue.push(i*6);
    }
    testQueue(queue);

    cout<<"------------------------------------------------"<<endl;

    cout<<"Popping 2 elements..."<<endl;
    for (int i=0; i<2; i++){
        queue.pop();
    }
    testQueue(queue);

    cout<<"------------------------------------------------"<<endl;
    cout<<"Pushing 3 elements..."<<endl;
    for (int i=0; i<3; i++){
        queue.push(i+1);
    }
    testQueue(queue);

    cout<<"------------------------------------------------"<<endl;

    cout<<"Calling clear..."<<endl;
    queue.clear();
    testEmptyQueue(queue);

    return 0;
}

template <class T> void Queue<T> :: push (T newItem){
    queue.push_back(newItem);
    size++;
}
template <class T> T Queue<T> :: pop (){
    if (!isEmpty()){
        T poppedItem=queue[startInd];
        startInd++;
        size--;
        if (isEmpty()){
            clear();
        }
        return poppedItem;
    } else {
        cout<<"Error: Trying to pop on empty queue!"<<endl;
        exit(1);
    }
}
template <class T> T Queue<T> :: top (){
    if (!isEmpty()){
        return queue[startInd];
    } else {
        cout<<"Error: Queue is empty!"<<endl;
        exit(1);
    }
}
template <class T> void Queue<T> :: clear() {
    queue.clear();
    startInd=0;
    size=0;
}
template <class T> T Queue<T> :: end (){
    if (!isEmpty()){
        return queue[getEndInd()];
    } else {
        cout<<"Error: Queue is empty!"<<endl;
        exit(1);
    }
}
template <class T> int Queue<T> :: getSize() const {
    return size;
}
template <class T> int Queue<T> :: getStartInd() {
    if (!isEmpty()) {
        return startInd;
    } else {
        cout<<"Error: Queue is empty!"<<endl;
        exit(1);
    }
}
template <class T> int Queue<T> :: getEndInd() {
    if (!isEmpty()) {
        return startInd + size - 1;
    } else {
        cout<<"Error: Queue is empty!"<<endl;
        exit(1);
    }
}
template <class T> bool Queue<T> :: isEmpty() const {
    return size==0;
}
template <class T> void testQueue (Queue <T> testQueue){
    cout<<"Starting index: "<<testQueue.getStartInd()<<endl;
    cout<<"Ending index: "<<testQueue.getEndInd()<<endl;
    cout<<"Top of the queue value: "<<testQueue.top()<<endl;
    cout<<"End of the queue value: "<<testQueue.end()<<endl;
    cout<<"Size of the queue: "<<testQueue.getSize()<<endl;
    cout<<"Is empty: ";
    if (testQueue.isEmpty()==true){
        cout<<"yes"<<endl;
    } else {
        cout<<"no"<<endl;
    }
}
template <class T> void testEmptyQueue (Queue <T> testQueue){
    cout<<"Size of the queue: "<<testQueue.getSize()<<endl;
    cout<<"Is empty: ";
    if (testQueue.isEmpty()==true){
        cout<<"yes"<<endl;
    } else {
        cout<<"no"<<endl;
    }
}