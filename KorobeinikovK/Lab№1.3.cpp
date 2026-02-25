#include <iostream>
#include <cstdlib>
using namespace std;
class Time {
private:
int hours, minutes, seconds;
bool Err(int h, int m, int s) {
	return (h >= 0 && h < 24) && (m >= 0 && m < 60) && (s >= 0 && s < 60);
}
void norm() {
	int allsec = hours * 3600 + minutes * 60 + seconds;
	allsec %= 86400;
	if (allsec < 0) {
		allsec += 86400;
	}
	hours = allsec / 3600;
	minutes = (allsec % 3600) / 60;
	seconds = allsec % 60;
}
public:
	Time(int h = 0, int m = 0, int s = 0) :hours(h), minutes(m), seconds(s) {
	};
	void SetTime(int h, int m, int s) {
		if (!Err(h, m, s)) {
			cout << "Error:incorrect time" << '\n';
			exit(1);
		}
		hours = h;
		minutes = m;
		seconds = s;
	}
	
	void print() {
		if (hours < 10) {
			cout << '0';
		}
		cout << hours << ':';
		if (minutes < 10) {
			cout << '0';
		}
		cout << minutes << ':';
		if (seconds < 10) {
			cout << '0';
		}
		cout << seconds << '\n';
	}
	void Difference(Time& other) {
		int Sec = hours * 3600 + minutes * 60 + seconds;
		int Oth_Sec = other.hours * 3600 + other.minutes * 60 + other.seconds;
		int diffsec = Oth_Sec - Sec;
		char sign = (diffsec >= 0) ? '+' : '-';
		int Diff = diffsec >= 0 ? (diffsec) : (-diffsec);
		int h = Diff / 3600;
		int m = (Diff % 3600) / 60;
		int s = Diff % 60;
		cout << sign;
		if (h < 10) {
			cout << '0';
		}
		cout << h << ':';
		if (m < 10) {
			cout << '0';
		}
		cout << m << ':';
		if (s < 10) {
			cout << '0';
		}
		cout << s << '\n';
	}
	void add(int h, int m, int s) {
		hours += h;
		minutes += m;
		seconds += s;
		norm();
	}
	void sub(int h, int m, int s) {
		hours -= h;
		minutes -= m;
		seconds -= s;
		norm();
	}
};
int main() {
	Time t1(12, 30, 45);
	cout << "t1 = ";
	t1.print();

	Time t2;
	t2.SetTime(8, 13, 10);
	cout << "Difference (t2 - t1) = ";
	t1.Difference(t2);

	t1.add(2, 10, 1);
	cout << "t1 shift +02:10:01 = ";
	t1.print();

	t1.sub(1, 33, 0);
	cout << "t1 shift -01:33:00 = ";
	t1.print();

	Time t3(23, 50, 59);
	t3.add(5, 15, 25);
	cout << "t3 shift + 05:15:25 = ";
	t3.print();

	t3.sub(0, 10, 53);
	cout << "t3 shift -00:10:53 = ";
	t3.print();
}
