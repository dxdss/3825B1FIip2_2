//Подключение стандартной библиотеки и пространства имён std.
#include <iostream>
using std::ostream;
using std::cout;
using std::cin;


//Математические функции.(НОД алгоритмом Евклида, сигнум.)
int sgn(int a) {
	if (a > 0) {
		return 1;
	}
	if (a < 0) {
		return -1;
	}
	return 0;
}
unsigned int gcd(unsigned int a, unsigned int b) {
	while (b != 0) {
		unsigned int t = b;
		b = a % b;
		a = t;
	}
	return a;
}

//Сам класс рационального числа.
class RationalNum {

	//Числитель.
	int numerator;
	//Знаменатель.
	unsigned int denominator;
	//Сокращение дроби
	void reduce() {
		unsigned int t = gcd(numerator * sgn(numerator), denominator);
		numerator /= (int)t;
		denominator /= t;
	}

public:

	//Конструктор + по умолчанию с сокращением и проверкой деления на 0.
	RationalNum(int numerator_ = 0, int denominator_ = 1) :numerator(denominator_ < 0 ? -1 * numerator_ : numerator_), denominator(denominator_<0? -1*denominator_:denominator_) {
		if (sgn(denominator) != 0) {
			reduce();
		}
		else {
			cout << "#DIV/0!\n";
		}
	}


	//Арифметические операции (и вывод) через перегрузку оперторов.(Некоторые крайние случаи можно оптимизировать. Например сумму и разность дробей с одинаковыми знаменателями.)
	RationalNum operator + (const RationalNum& rn) const {
		return RationalNum(numerator * rn.denominator + rn.numerator * denominator, denominator * rn.denominator);
	}

	RationalNum operator - (const RationalNum& rn) const {
		return RationalNum(numerator * rn.denominator - rn.numerator * denominator, denominator * rn.denominator);
	}

	RationalNum operator * (const RationalNum& rn) const {
		return RationalNum(numerator * rn.numerator, denominator * rn.denominator);
	}

	//Исправленное деление.Неявное приведение между знаковым и беззнаковым целым ненадёжно работает.
	RationalNum operator / (const RationalNum& rn) const {
		if (sgn(rn.numerator) != 0) {
					return RationalNum(numerator * (int)rn.denominator * sgn(rn.numerator), denominator * (unsigned int)(rn.numerator * sgn(rn.numerator)));
		}
		cout << "#DIV/0!\n";
	}
	
	//Перегрузка через friend.
	friend ostream& operator << (ostream& os, const RationalNum& rn);
};


//Вывод с обработкой целых чисел.
ostream& operator << (ostream& os, const RationalNum& rn) {
	if (rn.denominator == 1 ) {
		os << rn.numerator ;
		return os;
	}
	os << rn.numerator << '/' << rn.denominator;
	return os;

}


//main. Можно проверить корректность операций и поискать ошибки.
int main() {
	RationalNum a(-3, 5);
	RationalNum b(11, 7);
	cout << a + b << '\n';
	cout << a - b << '\n';
	cout << a * b << '\n';
	cout << a / b << '\n';
	cout << a * RationalNum(10,3) << '\n';
	cout << RationalNum(14,-10) << '\n';
	b = a;
	cout << b << '\n';

}