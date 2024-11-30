/*
NYU Tandon Bridge - Week 3 (Winter 2024)
BMI Calculator

Original Assignment:
Body mass index (BMI) is a number calculated from a person's weight and height using the
formula: weight/height². Where weight is in kilograms and height is in meters.

BMI ranges and corresponding weight status:
- Below 18.5: Underweight
- [18.5, 25): Normal
- [25, 30): Overweight
- 30 and above: Obese

Write a program that prompts for weight (in pounds) and height (in inches) of a person,
and prints the weight status of that person.

Note: 1 pound is 0.453592 kilograms and 1 inch is 0.0254 meters.

Example interaction:
Please enter weight (in pounds): 135
Please enter height (in inches): 71
The weight status is: Normal
*/

#include <iostream>
#include <string>
using namespace std;

const double poundToKilogram = 0.453592;
const double inchToMeter = 0.0254;

int main() {

    double weightPounds, heightInches, weightKilograms, heightMeters, BMI;
    string status;

    cout<<"Please enter weight (in pounds): ";
    cin>>weightPounds;

    cout<<"Please enter height (in inches): ";
    cin>>heightInches;

    weightKilograms = weightPounds * poundToKilogram;
    heightMeters = heightInches * inchToMeter;

    BMI = (weightKilograms) / (heightMeters * heightMeters);

    if (BMI < 18.5) {
        status = "Underweight";
    }
    else if ((BMI < 25.0) && (BMI >= 18.5)) {
        status = "Normal";
    }
    else if ((BMI < 30.0) && (BMI >= 25.0)) {
        status = "Overweight";
    }
    else{
        status = "Obese";
        }

    cout<<"The weight status is: "<<status;

    return 0;
}