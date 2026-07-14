#include<iostream>
#include<cmath>
using namespace std;

const string RESET  = "\033[0m";
const string RED    = "\033[31m";
const string GREEN  = "\033[32m";
const string YELLOW = "\033[33m";
const string BLUE   = "\033[34m";
const string CYAN   = "\033[36m";
const string BOLD   = "\033[1m";

void inputNumbers (double &num1, double &num2){
    while (true){
        cout << "Enter first number : ";
        cin >> num1;
        if (cin.fail()){
        cout << RED << "Error : Invalid input! Please enter a valid number. " << RESET << endl;
        cin.clear();
        cin.ignore(1000, '\n');
        continue;
    }
    break;
 }
    while (true){
        cout << "Enter second number : ";
        cin >> num2;
        if (cin.fail()){
        cout << RED << "Error : Invalid input! Please enter a valid number " << RESET <<endl;
        cin.clear();
        cin.ignore(1000, '\n');
        continue;
    }
    break;
}
}

void inputSingleNumber (double &num){
    while (true){

    cout << "Enter your number : ";
    cin >> num;
    if (cin.fail()){
        cout << RED << "Error : Invalid input! Please enter a valid number. " << RESET <<endl;
        cin.clear();
        cin.ignore(1000, '\n');
        continue;
    }
    break;
}

}

void displayMenu (){
    cout << CYAN << BOLD;
    cout << "===================================\n";
    cout << "         SIMPLE  CALCULATOR\n";
    cout << "===================================\n";
    cout << RESET << endl;
    cout << "1. Addition\n";
    cout << "2. Subtraction\n";
    cout << "3. Multiplication\n";
    cout << "4. Division\n";
    cout << "5. Power\n";
    cout << "6. Square Root\n";
    cout << "7. Modulus\n";
    cout << "8. Exit\n\n";   
}

void runCalculator (){
    while (true){
    int choice;
    cout << "Enter your choice : ";
    cin >> choice;
    if (cin.fail()){
        cout << RED << "Error : Invalid input! Please enter a valid number.  " <<RESET << endl;
        cin.clear();
        cin.ignore(1000, '\n');
        continue;
    }

    cout << "\n\n";

    switch (choice)
{
case 1: {
    double num1, num2;
    inputNumbers(num1, num2);
    double result = num1 + num2;
    cout << BOLD << GREEN << "Result : " << result << RESET << endl; 
    break;
}

case 2:{
    double num1, num2;
    inputNumbers(num1, num2);
    double result = num1 - num2;
    cout << BOLD << GREEN << "Result : " << result << RESET << endl; 
    break;
}
case 3: {
    double num1, num2;
    inputNumbers(num1, num2);
    double result = num1 * num2;
    cout << BOLD << GREEN << "Result : " << result << RESET << endl; 
    break;
}
case 4: {
    double num1, num2;
    inputNumbers(num1, num2);
    if (num2 == 0){
        cout << RED  << "Error : Cannot divide by zero!!" << RESET << endl;
    }else{
    double result = num1 / num2;
    cout << BOLD << GREEN << "Result : " << result << RESET << endl; 
}
break;
}
case 5: {
     double num1, num2;
    inputNumbers(num1, num2);
    double result = pow(num1, num2);
    cout << BOLD << GREEN << "Result : " << result << RESET << endl; 
    break;
}
case 6: {
    double num;
    inputSingleNumber(num);
    if (num < 0){
        cout << RED  << "Error : square root of a negative number is not possible." << RESET << endl;
    } else {
        double result = sqrt(num);
        cout << BOLD << GREEN << "Result : " << result << RESET << endl;
    }
    break;
}
case 7: {
    double num1, num2;
    inputNumbers(num1, num2);
    if (num2 == 0){
        cout << RED << "Error : Cannot perform modulus by zero.!!" << RESET <<endl;
    }else{
    double result = fmod(num1, num2);
    cout << BOLD << GREEN << "Result : " << result << RESET << endl; 
}
break;
}
case 8: {
    cout << YELLOW << "Exiting Calculator..." << RESET <<endl;
    return;
}
default: {
    cout << "Error : Invalid Choice!!\n";
}
}

cout << BLUE << "\n-------------------------------------" << RESET <<"\n\n";
}
}

int main(){
    displayMenu();
    runCalculator();
    return 0;
}