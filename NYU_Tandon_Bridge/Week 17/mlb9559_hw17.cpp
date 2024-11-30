/*
NYU Tandon Bridge - Week 17 (Winter 2024)
Red-Black Tree Implementation

Assignment:
Implement a Red-Black Tree data structure following the provided template and TODO comments.

Requirements:
1. Implementation must maintain Red-Black Tree properties:
   • Root is always black
   • No red node has a red child
   • Every path from root to leaf has same number of black nodes
   • New insertions are red nodes

2. Required operations:
   • Insertion with proper recoloring and rotations
   • Tree balancing after insertions
   • Color management using RED and BLACK constants
   • Node manipulation maintaining RB-tree properties

3. Code structure:
   • Fill in TODO sections as directed
   • Follow provided template structure
   • Can add/modify code except in main()
   • Use RED and BLACK constants instead of 0 and 1

Note: Implementation must pass all test cases in the provided main function
Follow all additional directions in the template comments
*/


#include <iostream>
#include <math.h> // for asserting height
#include <queue>
#include <cassert>

using namespace std;

#define RED 0
#define BLACK 1

template <class T>
class RBT;

// swapColor swaps the color from red to black and vice versa
int swapColor(int c) {
    return (c==0)?1:0;
}

template <class T>
class RBTNode {
    RBTNode<T> *parent, *left, *right;
    T data;
    int color;
public:
    RBTNode(T data) : data(data), color(RED), parent(nullptr), left(nullptr), right(nullptr) {}
    friend class RBT<T>;
    void prettyPrint(int indent) const;
    template <class T1> friend void swapColor(RBTNode<T1> *);
    template <class T1> friend int getColor(RBTNode<T1> *);
    int height() const;
};

template <class T>
int RBTNode<T>::height() const {
    int left_h = 0;
    if (left != nullptr) {
        left_h = left->height();
    }
    int right_h = 0;
    if (right != nullptr) {
        right_h = right->height();
    }
    return 1 + max(left_h, right_h);
}

template <class T>
void RBTNode<T>::prettyPrint(int indent) const {
    if (right != nullptr) {
        right->prettyPrint(indent + 1);
    }
    int margin = indent * 2;
    for (int i = 0; i < margin; ++i) {
        cout << '\t';
    }
    cout << "DATA: " << data << endl;
    for (int i = 0; i < margin; ++i) {
        cout << '\t';
    }
    cout << "COLOR: " << (color == RED ? "RED" : "BLACK") << endl;
    if (left != nullptr) {
        left->prettyPrint(indent + 1);
    }
}

template <class T>
void swapColor(RBTNode<T> *node) {
    if (node != nullptr) {
        node->color = swapColor(node->color);
    }
}

// getColor handles null nodes
template <class T>
int getColor(RBTNode<T> *node) {
    if (node != nullptr) {
        return node->color;
    }
    return BLACK;
}

template <class T>
class RBT {
    RBTNode<T> *root;
    void singleCCR(RBTNode<T> *&point);
    void doubleCR(RBTNode<T> *&point);
    void singleCR(RBTNode<T> *&point);
    void doubleCCR(RBTNode<T> *&point);
    bool isLeftChild (RBTNode<T> *currNode);
    bool isOuterChild (RBTNode<T> *currNode);
    void rotate(RBTNode<T> *currNode);
public:
    RBT() : root(nullptr) {}
    void insert(const T &);
    void insert(const T &, RBTNode<T> *&point, RBTNode<T> *parent);
    void prettyPrint() const { root->prettyPrint(0); }
    int height() const { return root->height(); }
};

template <class T>
bool RBT<T>::isLeftChild (RBTNode<T> *currNode) {
    if (currNode->parent!=nullptr && currNode->parent->left==currNode){
        return true;
    }
    return false;
}
template <class T>
bool RBT<T>::isOuterChild (RBTNode<T> *currNode) {
    if ((isLeftChild(currNode) && isLeftChild(currNode->parent)) || (!isLeftChild(currNode) && !isLeftChild(currNode->parent)) ){
        return true;
    }
    return false;
}

template <class T>
void RBT<T>::doubleCCR(RBTNode<T> *&point) {
    singleCR(point->right);
    singleCCR(point);
}

template <class T>
void RBT<T>::doubleCR(RBTNode<T> *&point) {
    singleCCR(point->left);
    singleCR(point);
}

template <class T>
void RBT<T>::rotate(RBTNode<T> *currNode) {
    //If currNode is outer node, perform single rotation on grandparent; else double rotation on grandparent
    if (isOuterChild(currNode)){ //Single rotation
        if (isLeftChild(currNode)){
            singleCR(currNode->parent->parent);
        } else {
            singleCCR(currNode->parent->parent);
        }
    } else {//Double rotation
        if (isLeftChild(currNode)){
            doubleCCR(currNode->parent->parent);
        } else {
            doubleCR(currNode->parent->parent);
        }
    }
}

template <class T>
void RBT<T>::singleCR(RBTNode<T> *&point) {
    RBTNode<T> *currGrandparent = point;
    RBTNode<T> *currParent = point->left;
    // TODO: RIGHT ROTATION
    // 1) currParent's pointers:
    // - parent (initially currGrandparent; changes to the initial parent of currGrandparent)
    // - right (becomes currGrandparent's left; its place is taken by the currGrandparent)
    // 2) currGrandparent's pointers:
    // - parent (replaced with currParent)
    // - left (initially currParent; changes to the currParent's right)
    // 3) Grandparent's parent's pointers: parent/left/right (only one of them changes, depending on whether the currGrandparent was a root, left child or right child)
    // 4) Parent's right child's pointers: parent (replaced with Grandparent)


    //Updating currParent's right child's parent, if it exists
    if (currParent->right!=nullptr) {
        currParent->right->parent = currGrandparent;
    }
    //Updating currGrandparent's left (currParent’s right child becomes currGrandparent’s left child)
    currGrandparent->left=currParent->right;
    //Updating currParent's right pointer (currGrandparent becomes currParent’s right child)
    currParent->right=currGrandparent;

    //Updating currParent's parent pointer (currGrandparent's parent becomes currParent's parent)
    currParent->parent=currGrandparent->parent;
    //Updating currGrandparent's parent's pointer
    if (currGrandparent->parent==nullptr) { //if currGrandparent was a root, currParent becomes a new root
        root = currParent;
    } else if (currGrandparent->parent->left==currGrandparent) { //if currGrandparent was a left child
        currGrandparent->parent->left=currParent;//in this case, we also need to adjust currGrandparent's parent's left
    } else { //if currGrandparent was a right child
        currGrandparent->parent->right=currParent;//in this case, we also need to adjust currGrandparent's parent's right
    }

    //Updating currGrandparent's parent pointer
    currGrandparent->parent=currParent;

    //Update colors (only the colors of rotated nodes change)
    swapColor(currParent);
    swapColor(currGrandparent);
}

template <class T>
void RBT<T>::singleCCR(RBTNode<T> *&point) {
    RBTNode<T> *currGrandparent = point;
    RBTNode<T> *currParent = point->right;
    // TODO: LEFT ROTATION
    // 1) currParent's pointers:
    // - parent (initially currGrandparent; changes to the initial parent of currGrandparent)
    // - left (becomes currGrandparent's right; its place is taken by the currGrandparent)
    // 2) currGrandparent's pointers:
    // - parent (replaced with currParent)
    // - right (initially currParent; changes to the currParent's left)
    // 3) Grandparent's parent's pointers: parent/left/right (only one of them changes, depending on whether the currGrandparent was a root, left child or right child)
    // 4) Parent's left child's pointers: parent (replaced with Grandparent)


    //Updating currParent's left child's parent, if it exists
    if (currParent->left!=nullptr) {
        currParent->left->parent = currGrandparent;
    }
    //Updating currGrandparent's right (currParent’s left child becomes currGrandparent’s right child)
    currGrandparent->right=currParent->left;
    //Updating currParent's parent pointer (currGrandparent's parent becomes currParent's parent)
    currParent->parent=currGrandparent->parent;

    //Updating currGrandparent's parent's pointer
    if (currGrandparent->parent==nullptr) { //if currGrandparent was a root, currParent becomes a new root
        root = currParent;
    } else if (currGrandparent->parent->right==currGrandparent) { //if currGrandparent was a right child
        currGrandparent->parent->right=currParent;//in this case, we also need to adjust currGrandparent's parent's right
    } else { //if currGrandparent was a left child
        currGrandparent->parent->left=currParent;//in this case, we also need to adjust currGrandparent's parent's left
    }

    //Updating currParent's left pointer (currGrandparent becomes currParent’s left child)
    currParent->left=currGrandparent;
    //Updating currGrandparent's parent pointer
    currGrandparent->parent=currParent;

    //Update colors (only the colors of rotated nodes change)
    swapColor(currParent);
    swapColor(currGrandparent);
}


template <class T>
void RBT<T>::insert(const T &toInsert, RBTNode<T> *&point, RBTNode<T> *parent) {
    if (point == nullptr) {  // BASE CASE: point is a leaf

        //Create new node (newly created node is by default red)
        point = new RBTNode<T>(toInsert);
        point->parent = parent;
        RBTNode<T> *currNode = point;

        //CASE 1: Insert first node (a root) -> recolor it to black -> insertion complete
        if (parent == nullptr) {
            swapColor(currNode);
            return;
        }
        //CASE 2: Insert second node -> root is black, currNode is red -> insertion complete
        if (parent->parent == nullptr) {
            return;
        }
        //CASE 3: Insert third/third+ node -> parent is red -> can go in multiple ways depending on uncle node
        while (getColor(currNode->parent) == RED) {
            //Finding uncle node
            RBTNode<T> *uncle;
            if (isLeftChild(currNode->parent)) {
                uncle = currNode->parent->parent->right;
            } else {
                uncle = currNode->parent->parent->left;
            }
            //Uncle is black -> rotate
            if (getColor(uncle) == BLACK) {
                rotate(currNode);
            } else { //Uncle is red -> swap colors of parent, uncle and grandparent -> grandparent becomes currNode
                swapColor(currNode->parent);
                swapColor(uncle);
                swapColor(currNode->parent->parent);
                currNode=currNode->parent->parent;
            }
            root->color = BLACK; //Making sure root is always black
        }
    } else {//RECURSIVE STEP: recurse down the tree to find correct leaf location
        if (toInsert < point->data) { // Go to the left
            insert(toInsert, point->left, point);
        } else { // Go to the right
            insert(toInsert, point->right, point);
        }
    }
}

template <class T>
void RBT<T>::insert(const T &toInsert) {
    insert(toInsert, root, nullptr);
}

// NOTE: DO NOT MODIFY THE MAIN FUNCTION BELOW
int main() {
    RBT<int> b;
    int count = 10;
    for (int i = 0; i < count; i++) {
        b.insert(i);
    }
    b.prettyPrint();
    /* EXPECTED OUTPUT:
                                                                    Data: 9
                                                                    COLOR: RED
                                                    Data: 8
                                                    COLOR: BLACK
                                    Data: 7
                                    COLOR: RED
                                                    Data: 6
                                                    COLOR: BLACK
                    Data: 5
                    COLOR: BLACK
                                    Data: 4
                                    COLOR: BLACK
    Data: 3
    COLOR: BLACK
                                    Data: 2
                                    COLOR: BLACK
                    Data: 1
                    COLOR: BLACK
                                    Data: 0
                                    COLOR: BLACK
    */
    // TEST
    // the below tests the validity of the height of the RBT
    // if the assertion fails, then your tree does not properly self-balance
    int height = b.height();
    assert(height <= 2 * log2(count));
    cout<<endl;
    cout<<"---------------------"<<endl;
    cout<<endl;
    RBT<int> c;
    count = 5;
    for (int i = count; i > 0; i--) {
        c.insert(i);
    }
    c.prettyPrint();
    /* EXPECTED OUTPUT:
                    Data: 5
                    COLOR: BLACK
    Data: 4
    COLOR: BLACK
                                    Data: 3
                                    COLOR: RED
                    Data: 2
                    COLOR: BLACK
                                    Data: 1
                                    COLOR: RED
    */
    height = c.height();
    assert(height <= 2 * log2(count));
}