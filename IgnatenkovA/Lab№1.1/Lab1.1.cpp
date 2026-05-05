#include <iostream>
#include <cmath>
#include <stdexcept>

using namespace std;

class Rnum {
private:
    int numerator;
    int denominator;

    static int NOD(int a, int b) {
        a = abs(a);
        b = abs(b);
        while (b != 0) {
            int temp = b;
            b = a % b;
            a = temp;
        }
        return a;
    }

    void normalize() {
        if (denominator == 0) {
            throw invalid_argument("Denominator cannot be zero.");
        }
        if (denominator < 0) {
            numerator = -numerator;
            denominator = -denominator;
        }
        int common = NOD(numerator, denominator);
        numerator /= common;
        denominator /= common;
    }

public:
    Rnum() : numerator(0), denominator(1) {}
    Rnum(int num, int den) : numerator(num), denominator(den) {
        normalize();
    }

    Rnum operator+(const Rnum& other) const {
        return Rnum(numerator * other.denominator + other.numerator * denominator,
            denominator * other.denominator);
    }

    Rnum operator-(const Rnum& other) const {
        return Rnum(numerator * other.denominator - other.numerator * denominator,
            denominator * other.denominator);
    }

    Rnum operator*(const Rnum& other) const {
        return Rnum(numerator * other.numerator, denominator * other.denominator);
    }

    Rnum operator/(const Rnum& other) const {
        if (other.numerator == 0) {
            throw invalid_argument("Division by zero is not allowed.");
        }
        return Rnum(numerator * other.denominator, denominator * other.numerator);
    }

    void print() const {
        cout << numerator << "/" << denominator << endl;
    }
};

int main() {
    Rnum current;
    int choice;

    while (true) {
        cout << "1. Enter the number\n";
        cout << "2. Print the number\n";
        cout << "3. Add with the number (+)\n";
        cout << "4. Subtract the number (-)\n";
        cout << "5. Multiply by a number (*)\n";
        cout << "6. Divide by a number (/)\n";
        cout << "7. Exiting the program\n";
        cout << "Your choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(32767, '\n');
            cout << "Enter a number from 1 to 7.\n";
            continue;
        }

        switch (choice) {
        case 1: {
            int num, den;
            cout << "numerator: "; cin >> num;
            do {
                cout << "denominator: "; cin >> den;
                if (den == 0) cout << "The denominator cannot be 0\n";
            } while (den == 0);
            current = Rnum(num, den);
            cout << "The number has been updated: "; current.print();
            break;
        }
        case 2: {
            cout << "Current number: "; current.print();
            break;
        }
        case 3: case 4: case 5: case 6: {
            int num, den;
            cout << "Enter the second number:\n";
            cout << "numerator: "; cin >> num;
            do {
                cout << "denominator: "; cin >> den;
                if (den == 0) cout << "The denominator cannot be 0\n";
            } while (den == 0);

            Rnum other(num, den);
            try {
                switch (choice) {
                case 3: current = current + other; break;
                case 4: current = current - other; break;
                case 5: current = current * other; break;
                case 6: current = current / other; break;
                }
                cout << "Result: "; current.print();
            }
            catch (const invalid_argument& e) {
                cerr << e.what() << endl;
                cout << "Operation cancelled. Number remains unchanged.\n";
            }
            break;
        }
        case 7:
            cout << "Exiting the program\n";
            return 0;
        default:
            cout << "Incorrect menu item.\n";
        }
    }
}