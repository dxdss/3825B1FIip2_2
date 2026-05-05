/*4.4. Контакты
Разработать класс Контакты.
Класс должен хранить информацию о контактах владельца. Каждый контакт содержит
следующие данные: фамилия; имя; отчество; телефон; день рождения (день, месяц, год);
признак, относится ли контакт к избранным. Контакты хранятся упорядоченно по фамилии,
имени, отчеству. Фамилия, имя, отчество (ФИО) являются обязательными полями.
Класс должен содержать необходимые служебные методы.
Класс должен предоставлять следующие операции: 
1) создать новый контакт,
2) изменить выбранный контакт,
3) найти контакт по ФИО,
4) найти контакт по телефону,
5) вывести все контакты на заданную букву,
6) узнать текущее число контактов,
7) внести контакт в список избранных,
8) удалить контакт из списка избранных,
9) вывести все избранные контакты,
10) удалить контакт,
11) сохранить контакты в файл,
12) считать контакты из файла.
*/
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using std::invalid_argument;
using std::out_of_range;
using std::ifstream;
using std::ofstream;
using std::ostream;
using std::istream;
using std::string;
using std::vector;
using std::cout;
using std::endl;

//Класс даты для хранения дней рождения
class Date {
    unsigned short day, month, year;
    bool isLeapYear(unsigned short year) const {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    }
public:
    Date(unsigned short day_ = 1, unsigned short month_ = 1, unsigned short year_ = 1) {
        if (month_ == 0 || month_ > 12 || day_ == 0 || day_ > 31) {
            cout << "ERR:Invalid date.\n";
            throw invalid_argument("Invalid date.");
        }
        unsigned short daysInMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        if (isLeapYear(year_)) { daysInMonth[2] = 29; }
        if (day_ > daysInMonth[month_]) {
            cout << "ERR:Invalid date.\n";
            throw invalid_argument("Invalid date.");
        }
        day = day_;
        month = month_;
        year = year_;
    }
    //Ввод/вывод
    friend ostream& operator << (ostream& os, const Date& date) {
        os << date.day << '.' << date.month << '.' << date.year;
        return os;
    }
    friend istream& operator >>(istream& is, Date& date) {
        char dot1, dot2;
        is >> date.day >> dot1 >> date.month >> dot2 >> date.year;
        if (dot1 != '.' || dot2 != '.') {
            cout << "ERR:Invalid date format.\n";
            throw invalid_argument("Invalid date format.");
        }
        date = Date(date.day, date.month, date.year);
        return is;
    }
};


//Класс одного контакта, Контактная книжка - вектор из контактов и операции с ними
class Contact {
private:
    //Имя, отчество, фамилия, номер, день рождения, избранный
    string first_name;
    string middle_name;
    string last_name;
    string phone_number;
    Date birth_date;
    bool isfavourite;
public:
    //Конструктор от ФИО, номера и опционально даты(Порядок корявый, ИОФ, но пусть будет так)
    Contact(string first_name_ ="", string middle_name_="", string last_name_="", string phone_number_="", Date birth_date_ = Date()) : first_name(first_name_), middle_name(middle_name_), last_name(last_name_), phone_number(phone_number_), birth_date(birth_date_), isfavourite(false) {
        //Проверка на корректность формата номера(цифры+*#),сообщение об ошибке, выброс ошибки
        for (size_t i = 0; i < phone_number_.size(); ++i) {
            if (phone_number_[i] != '+' && phone_number_[i] != '*' && phone_number_[i] != '#' && (phone_number_[i] > '9' || phone_number_[i] < '0')) {
                cout << "ERR:Invalid phone number.\n";
                throw invalid_argument("Invalid phone number.");
            }
        }
    }
    //Геттеры, необходимы для записи в файл
    string getFirstName() const { return first_name; }
    string getMiddleName() const { return middle_name; }
    string getLastName() const { return last_name; }
    string getPhone() const { return phone_number; }
    Date getBirthDate() const { return birth_date; }
    //Операции со статусом избранное
    void AddToFav() { isfavourite = true; }
    void RemFrFav() { isfavourite = false; }
    bool IsFavourite() const { return isfavourite; }
    //Перегрузка меньше для сортировки списка контактов
    bool operator < (const Contact& other) const {
        if (last_name != other.last_name) {
            return last_name < other.last_name;
        }
        if (first_name != other.first_name) {
            return first_name < other.first_name;
        }
        return middle_name < other.middle_name;
    }
    //Пункты 3 и 4
    bool MatchesFullName(const string& n, const string& m, const string& l) const {
        return first_name == n && middle_name == m && last_name == l;
    }
    bool MatchesPhone(const string& phone) const {
        return phone_number == phone;
    }
    bool StartsWithLetter(char letter) const {
        return last_name.size() != 0 && last_name[0] == letter;
    }
    //Ввод/вывод
    friend ostream& operator << (ostream& os, const Contact& contact) {
        os << contact.first_name << ' ' << contact.middle_name << ' ' << contact.last_name << ' ' << contact.phone_number << ' ' << contact.birth_date << ' ' << contact.isfavourite;
        return os;
    }
    friend istream& operator >>(istream& is, Contact& contact) {
        contact.isfavourite = false;
        is >> contact.first_name >> contact.middle_name >> contact.last_name >> contact.phone_number >> contact.birth_date >> contact.isfavourite;
        return is;
    }
};


//Класс контактной книжки, служебный метод упорядочивания контактов
class ContactsBook {
private:
    vector<Contact> contacts;
    //Бинарный поиск позиции нового контакта в контакной книжке
    size_t binarySearch(Contact new_contact) {
        size_t left = 0;
        size_t right = contacts.size();
        while (left < right) {
            size_t mid = left + (right - left) / 2;
            if (contacts[mid] < new_contact) {
                left = mid + 1;
            }
            else {
                right = mid;
            }
        }
        return left;
    }
public:
    //Единственное поле - вектор, поэтому конструктор от компилятора. 

    // 1) Создать новый контакт
    void addContact(Contact new_contact) {
        size_t pos = binarySearch(new_contact);
        contacts.insert(contacts.begin() + pos, new_contact);
    }

    // 2) Изменить выбранный контакт
    void editContact(int index, Contact updated_contact) {
        if (index >= contacts.size() || index < 0) {
            cout << "ERR:Index out of range.\n";
            throw out_of_range("Index out of range.");
        }
        contacts.erase(contacts.begin() + index, contacts.begin() + index + 1);
        size_t pos = binarySearch(updated_contact);
        contacts.insert(contacts.begin() + pos, updated_contact);
    }

    // 3) Найти контакт по ФИО
    int findContactByFullName(const string& first_name, const string& middle_name, const string& last_name) const {
        size_t left = 0;
        size_t right = contacts.size();
        while (left < right) {
            size_t mid = left + (right - left) / 2;
            const Contact& mid_contact = contacts[mid];
            if (mid_contact.MatchesFullName(first_name, middle_name, last_name)) {
                return mid;
            }
            if (mid_contact < Contact(first_name, middle_name, last_name)) {
                left = mid + 1;
            }
            else {
                right = mid;
            }
        }
        return -1;
    }
    // 4) Найти контакт по телефону
    int findContactByPhone(const string& phone) const {
        for (int i = 0; i < contacts.size(); ++i) {
            if (contacts[i].MatchesPhone(phone)) {
                return i;
            }
        }
        return -1;
    }
    // 5) Вывести все контакты на заданную букву
    void printContactsByLetter(char letter) const {
        size_t left = 0;
        size_t right = contacts.size();
        while (left < right) {
            size_t mid = left + (right - left) / 2;
            if (contacts[mid].StartsWithLetter(letter)) {
                right = mid;
            }
            else if (contacts[mid] < Contact("", "", string(1, letter))) {
                left = mid + 1;
            }
            else {
                right = mid;
            }
        }
        for (size_t i = left; i < contacts.size() && contacts[i].StartsWithLetter(letter); ++i) {
            cout << contacts[i] << endl;
        }
    }
    // 6) Узнать текущее число контактов
    size_t getContactCount() const { return contacts.size(); }
    // 7) Внести контакт в список избранных
    void addToFavourites(int index) {
        if (index >= contacts.size() || index < 0) {
            cout << "ERR:Index out of range.\n";
            throw out_of_range("Index out of range.");
        }
        contacts[index].AddToFav();
    }
    // 8) Удалить контакт из списка избранных
    void removeFromFavourites(int index) {
        if (index >= contacts.size() || index < 0) {
            cout << "ERR:Index out of range.\n";
            throw out_of_range("Index out of range.");
        }
        contacts[index].RemFrFav();
    }
    // 9) Вывести все избранные контакты
    void printFavourites() const {
        bool found = false;
        for (size_t i = 0; i < contacts.size(); ++i) {
            if (contacts[i].IsFavourite()) {
                std::cout << contacts[i] << endl;
                found = true;
            }
        }
        if (!found) {
            std::cout << "No favourite contacts.\n";
        }
    }
    // 10) Удалить контакт
    void deleteContact(int index) {
        if (index >= contacts.size() || index < 0) {
            cout << "ERR:Index out of range.\n";
            throw std::out_of_range("Index out of range.");
        }
        contacts.erase(contacts.begin() + index);
    }
    // 11) Сохранить контакты в файл
    void saveToFile(const string& filename) const {
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "ERR:Cannot open file for writing: " << filename << endl;
            throw std::invalid_argument("Cannot open file for writing");
        }
        file << contacts.size() << '\n'; 
        for (size_t i = 0; i < contacts.size(); ++i) {
            file << contacts[i] << '\n';
        }
        file.close();
        cout << "Contacts saved to " << filename << " successfully.\n";
    }
    // 12) Считать контакты из файла
    void loadFromFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "ERR:Cannot open file for reading: " << filename << endl;
            throw std::invalid_argument("Cannot open file for reading");
        }
        contacts.clear();
        size_t count;
        file >> count;
        for (size_t i = 0; i < count; ++i) {
            Contact contact;
            file >> contact;
            if (file.fail()) {
                cout << "ERR:Failed to read contact at line " << (i + 1) << endl;
                file.close();
                throw std::invalid_argument("Failed to read contact at line");
            }
            size_t pos = binarySearch(contact);
            contacts.insert(contacts.begin() + pos, contact);
        }
        file.close();
        cout << "Contacts loaded from " << filename << " successfully." << endl;
    }

    //Вывод всего, в критериях не было но для удобаства добавил
    void printAll() const {
        for (size_t i = 0; i < contacts.size(); ++i) {
            cout << i << ": " << contacts[i] << endl;
        }
    }
    void printContact(size_t index) const {
            cout << contacts[index] << endl;
    }
};


int main() {
    ContactsBook book;
    try {
        Contact c1("Ivan", "Ivanovich", "Ivanov", "+79123456789", Date(15, 5, 1990));
        Contact c2("Peter", "Petrovich", "Petrov", "+79223456780", Date(20, 8, 1985));
        Contact c3("Anna", "Sergeyevna", "Sidorova", "+79334567890", Date(10, 1, 1992));
        book.addContact(c1);
        book.addContact(c2);
        book.addContact(c3);
        cout << book.getContactCount()<<endl;
        cout << "All contacts:\n";
        book.printAll();
        cout << "\nContacts starting with 'I':\n";
        book.printContactsByLetter('I');

        cout << "\nFavourites:\n";
        size_t idx = book.findContactByFullName("Ivan", "Ivanovich", "Ivanov");
        if (idx != -1) {
            book.addToFavourites(idx);
        }
        book.printFavourites();
        book.editContact(book.findContactByPhone("+79334567890"), Contact("Anna", "Sergeyevna", "Frolova", "+79999999999", Date(10, 1, 1992)));
        cout << "\nDeleting contact \n";
        book.deleteContact(book.findContactByFullName("Ivan", "Ivanovich", "Ivanov"));
        book.printAll();
        cout << "Saving to file...\n";
        book.saveToFile("contacts.txt");
        cout << "Loading from file...\n";
        ContactsBook book2;
        book2.loadFromFile("contacts.txt");
        book2.printAll();
    }
    catch (const std::exception& e) {
        cout << "Exception: " << e.what() << '\n';
    }
    return 0;
}