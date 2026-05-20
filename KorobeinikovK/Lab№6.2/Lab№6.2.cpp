#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>

using std::cin;
using std::cout;
using std::string;

class Game {
private:
	int bull;
	int cow;
	int Try;
	string secret;
	string gen_num(unsigned short n) {
		string dig = "0123456789";
		if (n < 1 || n > 10) {
			return "";
		}
		for (int i = dig.length() - 1; i > 0; --i) {
			int j = rand() % (i + 1);
			char temp = dig[i];
			dig[i] = dig[j];
			dig[j] = temp;
		}
		secret = dig.substr(0, n);
		if (secret[0] == '0' && n > 1) {
			int ind = 1 + rand() % (n - 1);
			char temp = secret[0];
			secret[0] = secret[ind];
			secret[ind] = temp;
		}
		return secret;
	}
public:
	Game(unsigned short n):bull(0), cow(0), Try(0) {
		gen_num(n);
	}
	void bulls_and_cows(const string& guess) {
		bull = 0;
		cow = 0;
		Try++;
		int n = secret.length();
		for (int i = 0; i < n; ++i) {
			if (secret[i] == guess[i]) {
				bull++;
			}
		}
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < n; ++j) {
				if (i != j && secret[i] == guess[j]) {
					cow++;
				}
			}
		}
	}
	bool validGuess(const string& guess) {
		if (guess.length() != secret.length()) {
			return false;
		}
		if (secret.length() > 1 && guess[0] == '0') {
			return false;
		}
		for (size_t i = 0; i < guess.length(); ++i) {
			if (guess[i] < '0' || guess[i]>'9') {
				return false;
			}
		}
		for (size_t i = 0; i < guess.length(); ++i) {
			for (size_t j = i + 1; j < guess.length(); ++j) {
				if (guess[i] == guess[j]) {
					return false;
				}
			}
		}
		return true;
	}
	int Bulls() const { return bull; }
	int Cows() const { return cow; }
	int GetTry() const { return Try; }
	bool Win() const { return bull == secret.length(); }
};
void InputClear() {
	cin.clear();
	cin.ignore(1000, '\n');
}
int main() {
	srand(time(nullptr));
	unsigned short n;
	cout << "Enter n (1-10): ";
	while (!(cin >> n) || n < 1 || n > 10) {
		InputClear();
		cout << "Invalid input. Enter a number: ";
	}
	InputClear();
	Game BullsAndCows(n);
	string guess;
	while (!BullsAndCows.Win()) {
		cout << "Enter your guess: ";
		cin >> guess;
		InputClear();
		if (!BullsAndCows.validGuess(guess)) {
			cout << "Invalid guess. Must be " << n << " unique digits, first digit not zero.\n";
			continue;
		}
		BullsAndCows.bulls_and_cows(guess);
		cout<< BullsAndCows.GetTry() << " | " << "Bulls: " << BullsAndCows.Bulls()
			<< ", Cows: " << BullsAndCows.Cows() << '\n';
	}
	cout << "You Won!\n";
	cout << "Number of Tries: " << BullsAndCows.GetTry();
}
