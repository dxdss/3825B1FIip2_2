#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <cstring>
#include <cctype>
#include <string>

const int MAX_NAME_LEN = 100;
const int MAX_PHONE_LEN = 20;

struct Date {
    int day;
    int month;
    int year;

    bool isValid() const {
        if (year < 1900 || year > 2026) return false;
        if (month < 1 || month > 12) return false;

        int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

        bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        if (isLeap) daysInMonth[1] = 29;

        return (day >= 1 && day <= daysInMonth[month - 1]);
    }
};

struct Contact {
    std::string lastName;
    std::string firstName;
    std::string patronymic;
    std::string phone;
    Date birthDate;
    bool isFavorite;

    bool isEmptyName() const {
        return lastName.empty() || firstName.empty() || patronymic.empty();
    }

    bool operator<(const Contact& other) const {
        if (lastName != other.lastName) return lastName < other.lastName;
        if (firstName != other.firstName) return firstName < other.firstName;
        return patronymic < other.patronymic;
    }

    bool operator==(const Contact& other) const {
        return lastName == other.lastName &&
            firstName == other.firstName &&
            patronymic == other.patronymic;
    }

    std::string formatForFile() const {
        std::string result;
        result += lastName + "|";
        result += firstName + "|";
        result += patronymic + "|";
        result += phone + "|";
        result += std::to_string(birthDate.day) + "|";
        result += std::to_string(birthDate.month) + "|";
        result += std::to_string(birthDate.year) + "|";
        result += std::to_string(isFavorite ? 1 : 0);
        return result;
    }

    static Contact parseFromFile(const std::string& line) {
        Contact contact;
        size_t pos = 0;
        size_t nextPos;

        nextPos = line.find('|', pos);
        contact.lastName = line.substr(pos, nextPos - pos);

        pos = nextPos + 1;
        nextPos = line.find('|', pos);
        contact.firstName = line.substr(pos, nextPos - pos);

        pos = nextPos + 1;
        nextPos = line.find('|', pos);
        contact.patronymic = line.substr(pos, nextPos - pos);

        pos = nextPos + 1;
        nextPos = line.find('|', pos);
        contact.phone = line.substr(pos, nextPos - pos);

        pos = nextPos + 1;
        nextPos = line.find('|', pos);
        contact.birthDate.day = std::stoi(line.substr(pos, nextPos - pos));

        pos = nextPos + 1;
        nextPos = line.find('|', pos);
        contact.birthDate.month = std::stoi(line.substr(pos, nextPos - pos));

        pos = nextPos + 1;
        nextPos = line.find('|', pos);
        contact.birthDate.year = std::stoi(line.substr(pos, nextPos - pos));

        pos = nextPos + 1;
        contact.isFavorite = std::stoi(line.substr(pos)) == 1;

        return contact;
    }
};

class PhoneBook {
private:
    Contact* contacts;
    int size;
    int capacity;

    void resize() {
        capacity = (capacity == 0) ? 2 : capacity * 2;
        Contact* newContacts = new Contact[capacity];
        for (int i = 0; i < size; i++) {
            newContacts[i] = contacts[i];
        }
        delete[] contacts;
        contacts = newContacts;
    }

    int findByName(const std::string& lastName, const std::string& firstName, const std::string& patronymic) const {
        for (int i = 0; i < size; i++) {
            if (contacts[i].lastName == lastName &&
                contacts[i].firstName == firstName &&
                contacts[i].patronymic == patronymic) {
                return i;
            }
        }
        return -1;
    }

    int findByPhone(const std::string& phone) const {
        for (int i = 0; i < size; i++) {
            if (contacts[i].phone == phone) {
                return i;
            }
        }
        return -1;
    }

    void insertSorted(const Contact& contact) {
        if (size >= capacity) {
            resize();
        }

        int pos = size;
        for (int i = 0; i < size; i++) {
            if (contact < contacts[i]) {
                pos = i;
                break;
            }
        }

        for (int i = size; i > pos; i--) {
            contacts[i] = contacts[i - 1];
        }

        contacts[pos] = contact;
        size++;
    }

    void removeAt(int index) {
        for (int i = index; i < size - 1; i++) {
            contacts[i] = contacts[i + 1];
        }
        size--;
    }

public:
    PhoneBook() : contacts(nullptr), size(0), capacity(0) {
        std::cout << "Phone book created\n";
    }

    ~PhoneBook() {
        delete[] contacts;
        std::cout << "Program finished\n";
    }

    PhoneBook(const PhoneBook&) = delete;
    PhoneBook& operator=(const PhoneBook&) = delete;

    void addContact() {
        Contact newContact;

        do {
            std::cout << "Last name: ";
            std::cin >> newContact.lastName;
            if (newContact.lastName.empty()) {
                std::cout << "Last name cannot be empty!\n";
            }
        } while (newContact.lastName.empty());

        do {
            std::cout << "First name: ";
            std::cin >> newContact.firstName;
            if (newContact.firstName.empty()) {
                std::cout << "First name cannot be empty!\n";
            }
        } while (newContact.firstName.empty());

        do {
            std::cout << "Patronymic: ";
            std::cin >> newContact.patronymic;
            if (newContact.patronymic.empty()) {
                std::cout << "Patronymic cannot be empty!\n";
            }
        } while (newContact.patronymic.empty());

        if (findByName(newContact.lastName, newContact.firstName, newContact.patronymic) != -1) {
            std::cout << "Contact with this full name already exists\n";
            return;
        }

        std::cout << "Phone: ";
        std::cin >> newContact.phone;

        do {
            std::cout << "Birth date (day month year): ";
            std::cin >> newContact.birthDate.day >> newContact.birthDate.month >> newContact.birthDate.year;
            if (!newContact.birthDate.isValid()) {
                std::cout << "Invalid date! Try again.\n";
            }
        } while (!newContact.birthDate.isValid());

        newContact.isFavorite = false;

        insertSorted(newContact);
        std::cout << "Contact added\n";
    }

    void editContact() {
        std::string lastName, firstName, patronymic;
        std::cout << "Enter full name of contact to edit:\n";
        std::cout << "Last name: "; std::cin >> lastName;
        std::cout << "First name: "; std::cin >> firstName;
        std::cout << "Patronymic: "; std::cin >> patronymic;

        int index = findByName(lastName, firstName, patronymic);
        if (index == -1) {
            std::cout << "Contact not found\n";
            return;
        }

        std::cout << "\n--- Editing contact ---\n";
        std::cout << "Current data:\n";
        std::cout << "Full name: " << contacts[index].lastName << " "
            << contacts[index].firstName << " " << contacts[index].patronymic << "\n";
        std::cout << "Phone: " << contacts[index].phone << "\n";
        std::cout << "Birth date: " << contacts[index].birthDate.day << "."
            << contacts[index].birthDate.month << "." << contacts[index].birthDate.year << "\n";

        Contact editedContact = contacts[index];
        int choice;
        bool changed = false;

        do {
            std::cout << "\nWhat to edit?\n";
            std::cout << "1. Last name\n";
            std::cout << "2. First name\n";
            std::cout << "3. Patronymic\n";
            std::cout << "4. Phone\n";
            std::cout << "5. Birth date\n";
            std::cout << "0. Finish editing\n";
            std::cout << "-> ";
            std::cin >> choice;

            switch (choice) {
            case 1:
                do {
                    std::cout << "New last name: "; std::cin >> editedContact.lastName;
                    if (editedContact.lastName.empty()) {
                        std::cout << "Last name cannot be empty!\n";
                    }
                } while (editedContact.lastName.empty());
                changed = true;
                break;
            case 2:
                do {
                    std::cout << "New first name: "; std::cin >> editedContact.firstName;
                    if (editedContact.firstName.empty()) {
                        std::cout << "First name cannot be empty!\n";
                    }
                } while (editedContact.firstName.empty());
                changed = true;
                break;
            case 3:
                do {
                    std::cout << "New patronymic: "; std::cin >> editedContact.patronymic;
                    if (editedContact.patronymic.empty()) {
                        std::cout << "Patronymic cannot be empty!\n";
                    }
                } while (editedContact.patronymic.empty());
                changed = true;
                break;
            case 4:
                std::cout << "New phone: "; std::cin >> editedContact.phone;
                changed = true;
                break;
            case 5:
                do {
                    std::cout << "New birth date (day month year): ";
                    std::cin >> editedContact.birthDate.day >> editedContact.birthDate.month >> editedContact.birthDate.year;
                    if (!editedContact.birthDate.isValid()) {
                        std::cout << "Invalid date!\n";
                    }
                } while (!editedContact.birthDate.isValid());
                changed = true;
                break;
            }
        } while (choice != 0);

        if (changed) {
            if (editedContact.lastName != contacts[index].lastName ||
                editedContact.firstName != contacts[index].firstName ||
                editedContact.patronymic != contacts[index].patronymic) {

                int existing = findByName(editedContact.lastName, editedContact.firstName, editedContact.patronymic);
                if (existing != -1 && existing != index) {
                    std::cout << "Contact with this full name already exists! Edit cancelled.\n";
                    return;
                }
            }

            removeAt(index);
            insertSorted(editedContact);
            std::cout << "Contact edited\n";
        }
        else {
            std::cout << "No changes made\n";
        }
    }

    void findByName() const {
        std::string lastName, firstName, patronymic;
        std::cout << "Last name: "; std::cin >> lastName;
        std::cout << "First name: "; std::cin >> firstName;
        std::cout << "Patronymic: "; std::cin >> patronymic;

        int index = findByName(lastName, firstName, patronymic);
        if (index == -1) {
            std::cout << "Contact not found\n";
        }
        else {
            std::cout << "Phone: " << contacts[index].phone << "\n";
            std::cout << "Birth date: " << contacts[index].birthDate.day << "."
                << contacts[index].birthDate.month << "." << contacts[index].birthDate.year << "\n";
            if (contacts[index].isFavorite) {
                std::cout << "In favorites\n";
            }
        }
    }

    void findByPhone() const {
        std::string phone;
        std::cout << "Phone: "; std::cin >> phone;

        int index = findByPhone(phone);
        if (index == -1) {
            std::cout << "Contact with this phone not found\n";
        }
        else {
            std::cout << "Found: " << contacts[index].lastName << " "
                << contacts[index].firstName << " " << contacts[index].patronymic << "\n";
        }
    }

    void showByLetter() const {
        char letter;
        std::cout << "Enter letter: "; std::cin >> letter;
        letter = std::tolower(letter);

        bool found = false;
        for (int i = 0; i < size; i++) {
            char firstChar = std::tolower(contacts[i].lastName[0]);
            if (firstChar == letter) {
                std::cout << contacts[i].lastName << " "
                    << contacts[i].firstName << " " << contacts[i].patronymic << "\n";
                found = true;
            }
        }
        if (!found) {
            std::cout << "No contacts starting with this letter\n";
        }
    }

    void showCount() const {
        std::cout << "Total contacts: " << size << "\n";
    }

    void addToFavorites() {
        std::string lastName, firstName, patronymic;
        std::cout << "Last name: "; std::cin >> lastName;
        std::cout << "First name: "; std::cin >> firstName;
        std::cout << "Patronymic: "; std::cin >> patronymic;

        int index = findByName(lastName, firstName, patronymic);
        if (index == -1) {
            std::cout << "Contact not found\n";
        }
        else {
            contacts[index].isFavorite = true;
            std::cout << "Contact added to favorites\n";
        }
    }

    void removeFromFavorites() {
        std::string lastName, firstName, patronymic;
        std::cout << "Last name: "; std::cin >> lastName;
        std::cout << "First name: "; std::cin >> firstName;
        std::cout << "Patronymic: "; std::cin >> patronymic;

        int index = findByName(lastName, firstName, patronymic);
        if (index == -1) {
            std::cout << "Contact not found\n";
        }
        else {
            contacts[index].isFavorite = false;
            std::cout << "Contact removed from favorites\n";
        }
    }

    void showFavorites() const {
        bool found = false;
        for (int i = 0; i < size; i++) {
            if (contacts[i].isFavorite) {
                std::cout << contacts[i].lastName << " "
                    << contacts[i].firstName << " " << contacts[i].patronymic
                    << " - " << contacts[i].phone << "\n";
                found = true;
            }
        }
        if (!found) {
            std::cout << "No favorite contacts\n";
        }
    }

    void deleteContact() {
        std::string lastName, firstName, patronymic;
        std::cout << "Last name: "; std::cin >> lastName;
        std::cout << "First name: "; std::cin >> firstName;
        std::cout << "Patronymic: "; std::cin >> patronymic;

        int index = findByName(lastName, firstName, patronymic);
        if (index == -1) {
            std::cout << "Contact not found\n";
        }
        else {
            removeAt(index);
            std::cout << "Contact deleted\n";
        }
    }

    void saveToFile() const {
        std::string fileName;
        std::cout << "File name: "; std::cin >> fileName;

        std::ofstream file(fileName);
        if (!file) {
            std::cout << "Error creating file\n";
            return;
        }

        file << size << "\n";
        for (int i = 0; i < size; i++) {
            file << contacts[i].formatForFile() << "\n";
        }

        file.close();
        std::cout << "Data saved to file " << fileName << "\n";
    }

    void loadFromFile() {
        std::string fileName;
        std::cout << "File name: "; std::cin >> fileName;

        std::ifstream file(fileName);
        if (!file) {
            std::cout << "Error opening file\n";
            return;
        }

        int newSize;
        file >> newSize;
        file.ignore();

        if (newSize < 0) {
            std::cout << "Invalid file\n";
            file.close();
            return;
        }

        delete[] contacts;
        contacts = nullptr;
        size = 0;
        capacity = 0;

        std::string line;
        for (int i = 0; i < newSize; i++) {
            if (std::getline(file, line)) {
                Contact contact = Contact::parseFromFile(line);
                addContactDirect(contact);
            }
        }

        file.close();
        std::cout << "Loaded " << size << " contacts from file " << fileName << "\n";
    }

    void addContactDirect(const Contact& contact) {
        if (size >= capacity) {
            resize();
        }
        insertSorted(contact);
    }

    void showAll() const {
        if (size == 0) {
            std::cout << "Phone book is empty\n";
            return;
        }
        std::cout << "\n--- All contacts ---\n";
        for (int i = 0; i < size; i++) {
            std::cout << i + 1 << ". " << contacts[i].lastName << " "
                << contacts[i].firstName << " " << contacts[i].patronymic;
            if (contacts[i].isFavorite) {
                std::cout << " *";
            }
            std::cout << "\n";
        }
    }
};

int main() {
    PhoneBook phoneBook;
    int choice;

    do {
        std::cout << "\n=== PHONE BOOK ===\n";
        std::cout << "1. Add contact\n";
        std::cout << "2. Edit contact\n";
        std::cout << "3. Find by full name\n";
        std::cout << "4. Find by phone\n";
        std::cout << "5. Contacts by letter\n";
        std::cout << "6. Number of contacts\n";
        std::cout << "7. Add to favorites\n";
        std::cout << "8. Remove from favorites\n";
        std::cout << "9. Show favorites\n";
        std::cout << "10. Delete contact\n";
        std::cout << "11. Save to file\n";
        std::cout << "12. Load from file\n";
        std::cout << "13. Show all\n";
        std::cout << "0. Exit\n";
        std::cout << "Choose option: ";
        std::cin >> choice;

        switch (choice) {
        case 1: phoneBook.addContact(); break;
        case 2: phoneBook.editContact(); break;
        case 3: phoneBook.findByName(); break;
        case 4: phoneBook.findByPhone(); break;
        case 5: phoneBook.showByLetter(); break;
        case 6: phoneBook.showCount(); break;
        case 7: phoneBook.addToFavorites(); break;
        case 8: phoneBook.removeFromFavorites(); break;
        case 9: phoneBook.showFavorites(); break;
        case 10: phoneBook.deleteContact(); break;
        case 11: phoneBook.saveToFile(); break;
        case 12: phoneBook.loadFromFile(); break;
        case 13: phoneBook.showAll(); break;
        case 0: std::cout << "Goodbye!\n"; break;
        default: std::cout << "Invalid choice!\n";
        }
    } while (choice != 0);

    return 0;
}