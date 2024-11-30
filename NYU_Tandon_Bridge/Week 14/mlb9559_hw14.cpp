/*
NYU Tandon Bridge - Week 14 (Winter 2024)
Simultaneous Maximum and Minimum Finder

Assignment (Weiss 7.40):
Implement a divide-and-conquer algorithm to find both the maximum and minimum values
in a vector simultaneously. The algorithm should:

1. Base cases:
   • For one item: it is both max and min
   • For two items: compare them once to find max and min

2. Recursive case:
   • Split input into two halves (as evenly as possible)
   • Recursively find max and min of each half
   • Use two comparisons to determine overall max and min

Requirements:
• Function should take a vector as input
• Return a vector of two elements [min, max]
• Include a main function with test cases
• Use divide-and-conquer strategy for efficiency

Note: When N is odd, one half will contain one more element than the other
*/

#include <iostream>
#include <vector>
using namespace std;

int MIN_IND = 0;
int MAX_IND = 1;

// Assumption: the minimum values are stored as the first elements, and the maximum values - as the second elements of the inputted vectors
vector <int> mergedResult (vector <int> leftResult, vector <int> rightResult){

    int min, max;

    if (leftResult[MIN_IND] < rightResult[MIN_IND]) {
        min = leftResult[MIN_IND];
    } else {
        min = rightResult[MIN_IND];
    }
    if (leftResult[MAX_IND] > rightResult[MAX_IND]) {
        max = leftResult[MAX_IND];
    } else {
        max = rightResult[MAX_IND];
    }

    vector <int> mergedRes = {min, max};

    return mergedRes;
}

// Assumption: the minimum value is stored as the first element, and the maximum - as the second element of the outputted vector;
// The output vector is in the form {min, max}
vector <int> findMinAndMax (vector<int> v, int low, int high) {

    if (high == low || high - low == 1) { // BASE CASES
        vector <int> result;
        if (high == low) { // For 1 element vector, its only element is the minimum and the maximum at the same time
            result = {v[low], v[low]};
            return result;
        } else { // For 2 elements vector we compare the only two elements; the greater one is maximum, the smaller one is minimum
            if (v[low] > v[high]) {
                result = {v[high], v[low]};
            } else {
                result = {v[low], v[high]};
            }
            return result;
        }
    } else { // RECURSIVE STEP
        int mid = low + (high - low) / 2;
        vector <int> leftResult = findMinAndMax(v, low, mid);
        vector <int> rightResult = findMinAndMax(v, mid + 1, high);
        return mergedResult(leftResult, rightResult);
    }
}

vector <int> findMinAndMax (vector<int> v){

    int low = 0;
    int high = v.size() - 1;

    if (v.size() == 0){
        cout<<"Input vector is empty. No minimum or maximum value exists."<<endl;
        exit(1);
    } else {
        return findMinAndMax(v,low, high);
    }
}

int main() {

    vector <int> v = {17, 29, 7 , -12, 22, 9, 1, 3, 0, -15};

    // Print original vector's elements:
    cout<<"The original vector: ";
    for (unsigned int i = 0; i < v.size(); i++){
        cout<<v[i]<<" ";
    }
    cout<<endl;

    // Call findMinAndMax function, print minimum and maximum (as mentioned in assumption for findMinAndMax, the minimum value
    // is stored as the first element of the outputted vector, and the maximum - as the second element)
    vector <int> result = findMinAndMax(v);
    cout<<"Minimum: "<<result[MIN_IND]<<endl;
    cout<<"Maximum: "<<result[MAX_IND]<<endl;

    return 0;
}