#include <iostream>
#include <vector>
#include <cmath>
using std::cout;
using std::cin;
using std::cerr;
using std::vector;

class Polynomial {
private:
	vector<double> cf;
public:
    Polynomial(){
        cf.resize(1);
        cf[0] = 0.0;
    }
    explicit Polynomial(int deg) {
        cf.assign(deg + 1, 0.0);
    }
    Polynomial(const Polynomial& other) : cf(other.cf) {}
    ~Polynomial(){}
    Polynomial& operator=(const Polynomial& other) {
        if (this != &other) {
            cf = other.cf;
        }
        return *this;
    }
    void SetDeg(int nd){
        cf.assign(nd + 1, 0.0);
    }
    void SetCf(int ind, double val){
        cf[ind] = val;
    }
    int GetDeg() const {
        return (int)(cf.size()) - 1;
    }
    double GetCf(int ind){
        return cf[ind];
    }
    double CalcVal(double x) const {
        double res = 0.0;
        for(int i = cf.size()-1;i >= 0;--i){
            res = res * x + cf[i];
        }
        return res;
    }
    Polynomial Deriv() const {
        int deg = GetDeg();
        if (deg == 0) {
            Polynomial res(0);
            res.SetCf(0, 0.0);
            return res;
        }
        Polynomial res(deg - 1);
        for (int i = 1; i <= deg; ++i) {
            res.SetCf(i - 1, cf[i] * i);
        }
        return res;
    }
    void print() const {
        const char* list_deg[] = {"", "x", "x^2", "x^3", "x^4", "x^5", "x^6", "x^7", "x^8", "x^9", "x^10", "x^11", "x^12"};
        bool fst = true;
        for (int i = cf.size() - 1; i >= 0; --i) {
            double c = cf[i];
            if (c == 0) {
                continue;
            }
            if (!fst) {
                cout << ((c > 0) ? "+" : "-");
            }
            double absC = std::abs(c);
            if (i == 0 || absC != 1.0) {
                cout << absC;
            }
            cout << list_deg[i];
            fst = false;
        }
        if (fst) {
            cout << "0";
        }
    }
};
void InputClear() {
    cin.clear();
    cin.ignore(1000, '\n');
}
int main() {
    Polynomial p;
    int ch=0;
    do {
        cout << "------------------------------------------\n";
        cout << "Menu:\n";
        cout << "1.Set the degree of a polynomial\n";
        cout << "2.Set the coefficients of the polynomial\n";
        cout << "3.Find out the degree of a polynomial\n";
        cout << "4.Find out the value of a coefficient by its number\n";
        cout << "5.Calculate the value at point x\n";
        cout << "6.Derivative of a polynomial\n";
        cout << "7.Derive a polynomial\n";
        cout << "0.Exit\n";
        cout << "Select an action:";       
        while (!(cin >> ch)) {
            InputClear();
            cout << "Invalid input. Enter a number: ";
        }
        InputClear();
        switch (ch) {
        case 1: {
            int deg = 0;
            cout << "Select degree (0-12):";
            while (!(cin >> deg) || deg < 0 || deg > 12) {
                InputClear();
                cout << "Invalid degree. Enter number 0-12: ";
            }
            InputClear();
            p.SetDeg(deg);
            cout << "The degree is set, all coefficients are zeroed\n";
            break;
        }
        case 2: {
            int deg = p.GetDeg();
            cout << "Enter " << deg + 1 << " coefficients from lowest to highest degree:\n";
            vector<double> cf(deg + 1);
            for (int i = 0; i <= deg; ++i) {
                cout << "Coefficient for x^" << i << ": ";
                while (!(cin >> cf[i])) {
                    InputClear();
                    cout << "Invalid number. Try again: ";
                }
                InputClear();
            }
            for (int i = 0; i <= deg; ++i) {
                p.SetCf(i, cf[i]);
            }
            cout << "Coefficients set.\n";
            break;
        }
        case 3: {
            cout << "Degree of a polynomial:" << p.GetDeg() << '\n';
            break;
        }
        case 4: {
            int deg = p.GetDeg();
            int ind;
            cout << "Enter index (0.." << deg << "): ";
            while (!(cin >> ind) || ind < 0 || ind > deg) {
                InputClear();
                cout << "Invalid index. Enter 0.." << deg << ": ";
            }
            InputClear();
            cout << "Coefficient at x^" << ind << " = " << p.GetCf(ind) << "\n";
            break;
        }
        case 5:{
            double x;
            cout << "Enter x: ";
            while (!(cin >> x)) {
                InputClear();
                cout << "Invalid number. Enter x: ";
            }
            InputClear();
            cout << "Value at x = " << x << " is " << p.CalcVal(x) << "\n";
            break;
        }
        case 6:{
            Polynomial drt = p.Deriv();
            cout << "Derivative: ";
            drt.print();
            cout << "\n";
            break;
        }
        case 7:{
            cout << "Polynomial: ";
            p.print();
            cout << "\n";
            break;
        }
        case 0:
            cout << "End\n";
            break;

        default:
            cout << "Incorrect selection. Try again\n";
        }
    } while (ch != 0);
    return 0;
}
