#include <iostream>
#include <string>
#include <vector>

using std::cin;
using std::cerr;
using std::cout;
using std::vector;
using std::string;

class Product {
private:
	string barcode;
	string name;
	unsigned int price;
	unsigned short discount;
	bool valBarcode(const string& bar) {
		if (bar.length() != 4 || bar == "0000") {
			return false;
		}
		for (size_t i = 0; i < 4; ++i) {
			if (bar[i] < '0' || bar[i]>'9') {
				return false;
			}
		}
		return true;
	}
	bool valDiscount(const unsigned short disc) {
		if (disc < 1 || disc>50) {
			return false;
		}
		return true;
	}
public:
	Product(const string& bar, const string& nm, unsigned int p, unsigned short disc):name(nm), price(p){
		if (valBarcode(bar)){
			barcode = bar;
		}
		else {
			barcode = "0000";
		}
		if (valDiscount(disc)) {
			discount = disc;
		}
		else {
			discount = 1;
		}
	}
	string GetBarcode() const { return barcode; }
	string GetName() const { return name; }
	unsigned int GetPrice() const { return price; }
	unsigned short GetDiscount() const { return discount; }
	void print() const {
		cout << "Barcode: " << barcode << '\n';
		cout << "Name: " << name << '\n';
		cout << "Price: " << price << " rub" << '\n';
		cout << "Discount: " << discount << "%" << '\n';
	}
};
class Warehouse {
private:
	vector<Product> prod_list;
public:
	Warehouse():prod_list(){}
	void AddProd(const Product& prod) {
		prod_list.push_back(prod);
	}
	int find_prod(const string& bar) const {
		for (size_t i = 0; i < prod_list.size(); ++i) {
			if (prod_list[i].GetBarcode() == bar) {
				return i;
			}
		}
		return -1;
	}
	const Product& GetProduct(size_t i) const { return prod_list[i]; }
	void init_prod() {
		AddProd(Product("1564", "Bread", 40, 5));
		AddProd(Product("9318", "Tea", 120, 25));
		AddProd(Product("1488", "Pasta", 65, 40));
		AddProd(Product("2008", "Cookie", 200, 15));
		AddProd(Product("0914", "Chocolate", 175, 12));
	}
};
struct Check {
	Product prod;
	unsigned int quantity;
};
class Cashbox {
private:
	const Warehouse* whouse;
	vector<Check> check;
public:
	Cashbox(const Warehouse* wh):whouse(wh){}
	//1
	string scan_prod(const string& bar) {
		int ind = whouse->find_prod(bar);
		if (ind == -1) {
			cerr << "Product not found\n";
			return "";
		}
		return bar;
	}
	//2
	void scan_and_check(const string& bar) {
		int ind = whouse->find_prod(bar);
		if (ind == -1) {
			cerr << "Product not found\n";
			return;
		}
		whouse->GetProduct(ind).print();
	}
	//3
	void add_to_check(const string& bar, unsigned int qty) {
		if (qty == 0) {
			cerr << "Error: Quantity cannot be zero\n";
			return;
		}
		for (size_t i = 0; i < check.size(); ++i) {
			if (check[i].prod.GetBarcode() == bar) {
				check[i].quantity += qty;
				cout << "Updated quantity in check\n";
				return;
			}
		}
		check.push_back({ whouse->GetProduct(whouse->find_prod(bar)), qty });
		cout << "Added to check\n";
	}
	//4
	void printCheck() const {
		if (check.empty()) {
			cout << "Check is empty\n";
			return;
		}
		cout << "--- Check ---\n";
		for (size_t i = 0; i < check.size(); ++i) {
			const Product& prod = check[i].prod;
			unsigned int qty = check[i].quantity;
			cout << "Name: " << prod.GetName() << "   " << "Price: " << prod.GetPrice() << " rub" << " Quantity: " << qty <<
				" All price: " << prod.GetPrice() * qty << " rub\n";
		}
	}
	//5
	void calculate() const {
		unsigned int TotalSum = 0, TotalDis = 0;
		for (size_t i = 0; i < check.size(); ++i) {
			const Product& prod = check[i].prod;
			unsigned int qty = check[i].quantity;
			unsigned int Sum = prod.GetPrice() * qty;
			TotalSum += Sum;
			TotalDis += Sum * prod.GetDiscount() / 100;
		}
		cout << "----------\n";
		cout << "Total: " << TotalSum << " rub\n";
		cout << "Discount: " << TotalDis << " rub\n";
		cout << "To be paid: " << TotalSum - TotalDis << " rub\n";

	}
	//6
	void delete_prod(const string& bar, unsigned int qty) {
		for (size_t i = 0; i < check.size(); ++i) {
			if (check[i].prod.GetBarcode() == bar) {
				if (qty >= check[i].quantity) {
					check.erase(check.begin() + i);
				}
				else{
					check[i].quantity -= qty;
				}
				cout << "The check has been changed\n";
				return;
			}
		}
		cout << "Product not found in check\n";
	}
};
void InputClear() {
	cin.clear();
	cin.ignore(1000, '\n');
}
void menu() {
	cout << "=== Electronic cashbox ===\n";
	cout << "1.Scan\n";
	cout << "2.Generate a check\n";
	cout << "3.Total amount to be paid\n";
	cout << "4.Delete product\n";
	cout << "0.Exit\n";
	cout << "Your choice: ";
}
void submenu(Cashbox& cashbox, string bar) {
	int ch;
	cout << "1.Product description\n";
	cout << "2.Add data to the check\n";
	cout << "0.Back\n";
	cout << "Your choice: ";
	while (!(cin >> ch)) {
		InputClear();
		cout << "Invalid input. Enter a number: ";
	}
	switch (ch) {
	case 1: {
		cout << "---Product information---\n";
		cashbox.scan_and_check(bar);
		break;
	}
	case 2: {
		unsigned int qty;
		cout << "Enter quantity: ";
		cin >> qty;
		InputClear();
		cashbox.add_to_check(bar, qty);
		break;
	}
	case 0:
		return;
	default: 
		cout << "Incorrect selection. Try again\n";
	}
}
int main() {
	Warehouse wh;
	int ch;
	string bar;
	unsigned int qty;
	wh.init_prod();
	Cashbox cashbox(&wh);
	do {
		menu();
		while (!(cin >> ch)) {
			InputClear();
			cout << "Invalid input. Enter a number: ";
		}
		InputClear();
		switch (ch) {
		case 1: {
			cout << "Scan a barcode: ";
			cin >> bar;
			InputClear();
			submenu(cashbox, bar);
			break;
		}
		case 2: {
			cashbox.printCheck();
			break;
		}
		case 3: {
			cashbox.calculate();
			break;
		}
		case 4: {
			string barcode;
			cout << "Enter product barcode: ";
			cin >> barcode;
			InputClear();
			while (true) {
				cout << "Enter quantity to delete: ";
				if (cin >> qty && qty > 0) {
					break;
				}
				cerr << "Error: Invalid quantity\n";
				InputClear();
			}
			InputClear();
			cashbox.delete_prod(barcode, qty);
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
