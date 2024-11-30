/*
NYU Tandon Bridge - Week 3 (Winter 2024)
Retail Price Calculator with Discounts

Assignment:
Write a program that computes how much a customer has to pay after purchasing two items.
The price is calculated according to the following rules:
• Buy one get one half off promotion: the lower price item is half price.
• If the customer is club card member, additional 10% off.
• Tax is added.

Example interaction:
Enter price of first item: 10
Enter price of second item: 20
Does customer have a club card? (Y/N): y
Enter tax rate, e.g. 5.5 for 5.5% tax: 8.25
Base price: 30.0
Price after discounts: 22.5
Total price: 24.35625

 */

#include <iostream>
using namespace std;

int main() {

    const double BOGODiscount = 0.5, clubDiscount = 0.1;

    double priceOfItem1, priceOfItem2, taxRate;
    char clubCardOwnership;
    double priceAfterBOGO;
    double basePrice, priceAfterDiscounts, totalPrice;

    cout<<"Enter price of first item: ";
    cin>>priceOfItem1;

    cout<<"Enter price of second item: ";
    cin>>priceOfItem2;

    cout<<"Does customer have a club card? (Y/N): ";
    cin>>clubCardOwnership;

    cout<<"Enter tax rate, e.g. 5.5 for 5.5% tax: ";
    cin>>taxRate;

    basePrice = priceOfItem1 + priceOfItem2;

    if (priceOfItem1 > priceOfItem2) {
        priceAfterBOGO = priceOfItem1 + ((1-BOGODiscount) * priceOfItem2);
    }
    else {
        priceAfterBOGO = priceOfItem2 + ((1-BOGODiscount) * priceOfItem1);
    }

    if ((clubCardOwnership == char ('y')) || (clubCardOwnership == char ('Y'))) {
        priceAfterDiscounts = priceAfterBOGO * (1-clubDiscount);
    }
    else {
        priceAfterDiscounts = priceAfterBOGO;
    }

    totalPrice = priceAfterDiscounts * ((100.0 + taxRate) / 100.0);

    cout<<"Base price: "<<basePrice<<endl;
    cout<<"Price after discounts: "<<priceAfterDiscounts<<endl;
    cout<<"Total price: "<<totalPrice<<endl;

    return 0;
}