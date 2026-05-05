#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

struct Date {
    int day;
    int month;
    int year;
    
    bool isValid() const {
        if (year < 1900 || year > 2026) return false;
        if (month < 1 || month > 12) return false;
        int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        if (day < 1 || day > daysInMonth[month - 1]) return false;
        return true;
    }

    bool isSame(const Date& other) const {
        return (day == other.day && month == other.month && year == other.year);
    }

    bool isEarlier(const Date& other) const {
        if (year != other.year) return year < other.year;
        if (month != other.month) return month < other.month;
        return day < other.day;
    }
};

class FloorScales {
private:
    std::string familyMembers[5];
    int familyCount = 0;
    std::string obsNames[100];
    double obsWeights[100] = { 0.0 };
    Date obsDates[100] = {};
    int obsCount = 0;
    Date startDate = { 1,1,2000 };

    bool isFamilyMember(const std::string& name) const {
        for (int i = 0; i < familyCount; i++) {
            if (familyMembers[i] == name) {
                return true;
            }
        }
        return false;
    }

    double roundWeight(double w) const {
        return std::round(w * 20) / 20.0;
    }

    Date inputDate() {
        Date d;
        bool valid = false;
        while (!valid) {
            std::cout << "Day: ";
            std::cin >> d.day;
            std::cout << "Month: ";
            std::cin >> d.month;
            std::cout << "Year: ";
            std::cin >> d.year;

            if (d.isValid()) {
                valid = true;
            }
            else {
                std::cout << "Wrong date! Try again." << std::endl;
            }
        }
        return d;
    }

public:
    FloorScales() {}

    void setStartDate() {
        std::cout << "Setting the starting date of observations" << std::endl;
        Date d = inputDate();

        if (d.isValid()) {
            startDate = d;
            std::cout << "The starting date is set: " << d.day << "." << d.month << "." << d.year << std::endl;
        }
        else {
            std::cout << "Error: incorrect date!" << std::endl;
        }
    }

    void getStartDate() const {
        std::cout << "The starting date of observations" << std::endl;
        std::cout << "Current starting date: " << startDate.day << "." << startDate.month << "." << startDate.year << std::endl;
    }

    void addFamilyMember() {
        std::cout << "Adding a family member" << std::endl;
        std::string name;
        std::cout << "Enter the name of the family member: ";
        std::cin >> name;

        if (familyCount < 5) {
            for (int i = 0; i < familyCount; i++) {
                if (familyMembers[i] == name) {
                    std::cout << "A family member with that name already exists!" << std::endl;
                    return;
                }
            }
            familyMembers[familyCount] = name;
            familyCount++;
            std::cout << "Family member \"" << name << "\" added. Total: " << familyCount << "/5" << std::endl;
        }
        else {
            std::cout << "Error: You cannot add more than 5 family members!" << std::endl;
        }
    }

    void addObservation() {
        std::cout << "Task of observation" << std::endl;
        std::string name;
        double weight;
        Date date;

        std::cout << "Enter a name: ";
        std::cin >> name;
        std::cout << "Enter the weight: ";
        std::cin >> weight;
        if (weight < 0) { 
            std::cout << "Error: weight cannot be negative!" << std::endl;
            return;
        }
        std::cout << "Enter the date of observation: " << std::endl;
        date = inputDate();

        if (!isFamilyMember(name)) {
            std::cout << "Error: " << name << " is not a family member!" << std::endl;
            return;
        }

        if (!date.isValid()) {
            std::cout << "Error: incorrect date!" << std::endl;
            return;
        }

        if (date.isEarlier(startDate)) {
            std::cout << "Error: the date cannot be earlier than the start date!" << std::endl;
            return;
        }

        weight = roundWeight(weight);

        for (int i = 0; i < obsCount; i++) {
            if (obsDates[i].isSame(date)) {
                std::cout << "Replacing the existing surveillance for " << date.day << "." << date.month << "." << date.year << std::endl;
                obsNames[i] = name;
                obsWeights[i] = weight;
                std::cout << "The observation has been updated: " << name << " - " << weight << " kg" << std::endl;
                return;
            }
        }

        obsNames[obsCount] = name;
        obsWeights[obsCount] = weight;
        obsDates[obsCount] = date;
        obsCount++;

        std::cout << "Observation added: " << name << " - " << weight << " kg ("
            << date.day << "." << date.month << "." << date.year << ")" << std::endl;
    }

    void getWeight() const {
        std::cout << "Getting weight by date" << std::endl;
        std::cout << "Enter the date of observation: " << std::endl;
        Date date;
        std::cout << "Day: ";
        std::cin >> date.day;
        std::cout << "Month: ";
        std::cin >> date.month;
        std::cout << "Year: ";
        std::cin >> date.year;
        
        if (!date.isValid()) {
            std::cout << "Error: incorrect date!" << std::endl;
            return;
        }

        for (int i = 0; i < obsCount; i++) {
            if (obsDates[i].isSame(date)) {
                std::cout << "Weight on " << date.day << "." << date.month << "." << date.year << ": " << obsWeights[i] << " kg" << std::endl;
                return;
            }
        }
        std::cout << "Observation of " << date.day << "." << date.month << "." << date.year << " not found" << std::endl;
    }

    void getAverageWeight() const {
        std::cout << "Average weight of a family member" << std::endl;
        std::string name;
        int period;

        std::cout << "Enter a name: ";
        std::cin >> name;

        if (!isFamilyMember(name)) {
            std::cout << "Error: " << name << " is not a family member!" << std::endl;
            return;
        }

        std::cout << "Select a period: " << std::endl;
        std::cout << "1 - for the whole history" << std::endl;
        std::cout << "2 - for a specific month" << std::endl;
        std::cout << "Choice: ";
        std::cin >> period;

        double sum = 0;
        int count = 0;

        if (period == 1) {
            for (int i = 0; i < obsCount; i++) {
                if (obsNames[i] == name) {
                    sum += obsWeights[i];
                    count++;
                }
            }
        }
        else if (period == 2) {
            int month, year;
            std::cout << "Enter the month (1-12): ";
            std::cin >> month;
            std::cout << "Enter the year: ";
            std::cin >> year;

            for (int i = 0; i < obsCount; i++) {
                if (obsNames[i] == name && obsDates[i].month == month && obsDates[i].year == year) {
                    sum += obsWeights[i];
                    count++;
                }
            }
        }
        else {
            std::cout << "Incorrect period selection!" << std::endl;
            return;
        }

        if (count == 0) {
            std::cout << "There are no observations for " << name;
            if (period == 2) std::cout << " for the specified month";
            std::cout << std::endl;
            return;
        }
        std::cout << "Average weight " << name << ": " << (sum / count) << " kg" << std::endl;
    }

    void findMinWeight() const {
        std::cout << "Finding the minimum weight" << std::endl;
        std::string name;
        int period;

        std::cout << "Enter a name: ";
        std::cin >> name;

        if (!isFamilyMember(name)) {
            std::cout << "Error: " << name << " is not a family member!" << std::endl;
            return;
        }

        std::cout << "Select a period: " << std::endl;
        std::cout << "1 - for the whole history" << std::endl;
        std::cout << "2 - for a specific month" << std::endl;
        std::cout << "Choice: ";
        std::cin >> period;

        double minWeight = 999999;
        Date minDate;
        bool found = false;

        if (period == 1) {
            for (int i = 0; i < obsCount; i++) {
                if (obsNames[i] == name) {
                    if (obsWeights[i] < minWeight) {
                        minWeight = obsWeights[i];
                        minDate = obsDates[i];
                        found = true;
                    }
                }
            }
        }
        else if (period == 2) {
            int month, year;
            std::cout << "Enter the month (1-12): ";
            std::cin >> month;
            std::cout << "Enter the year: ";
            std::cin >> year;

            for (int i = 0; i < obsCount; i++) {
                if (obsNames[i] == name && obsDates[i].month == month && obsDates[i].year == year) {
                    if (obsWeights[i] < minWeight) {
                        minWeight = obsWeights[i];
                        minDate = obsDates[i];
                        found = true;
                    }
                }
            }
        }
        else {
            std::cout << "Incorrect period selection!" << std::endl;
            return;
        }

        if (found) {
            std::cout << "Minimum weight " << name << ": " << minWeight
                << " kg (date: " << minDate.day << "." << minDate.month << "." << minDate.year << ")" << std::endl;
        }
        else {
            std::cout << "There are no observations for " << name;
            if (period == 2) std::cout << " for the specified month";
            std::cout << std::endl;
        }
    }

    void findMaxWeight() const {
        std::cout << "Finding the maximum weight" << std::endl;
        std::string name;
        int period;

        std::cout << "Enter a name: ";
        std::cin >> name;

        if (!isFamilyMember(name)) {
            std::cout << "Error: " << name << " is not a family member!" << std::endl;
            return;
        }

        std::cout << "Select a period: " << std::endl;
        std::cout << "1 - for the whole history" << std::endl;
        std::cout << "2 - for a specific month" << std::endl;
        std::cout << "Choice: ";
        std::cin >> period;

        double maxWeight = -1;
        Date maxDate;
        bool found = false;

        if (period == 1) {
            for (int i = 0; i < obsCount; i++) {
                if (obsNames[i] == name) {
                    if (obsWeights[i] > maxWeight) {
                        maxWeight = obsWeights[i];
                        maxDate = obsDates[i];
                        found = true;
                    }
                }
            }
        }
        else if (period == 2) {
            int month, year;
            std::cout << "Enter the month (1-12): ";
            std::cin >> month;
            std::cout << "Enter the year: ";
            std::cin >> year;

            for (int i = 0; i < obsCount; i++) {
                if (obsNames[i] == name && obsDates[i].month == month && obsDates[i].year == year) {
                    if (obsWeights[i] > maxWeight) {
                        maxWeight = obsWeights[i];
                        maxDate = obsDates[i];
                        found = true;
                    }
                }
            }
        }
        else {
            std::cout << "Incorrect period selection!" << std::endl;
            return;
        }

        if (found) {
            std::cout << "Maximum weight " << name << ": " << maxWeight
                << " kg (date: " << maxDate.day << "." << maxDate.month << "." << maxDate.year << ")" << std::endl;
        }
        else {
            std::cout << "There are no observations for " << name;
            if (period == 2) std::cout << " for the specified month";
            std::cout << std::endl;
        }
    }

    void saveToFile() const {
        std::cout << "Saving the history to a file" << std::endl;
        std::string filename;
        std::cout << "Enter the file name: ";
        std::cin >> filename;

        std::ofstream file(filename.c_str());

        if (!file.is_open()) {
            std::cout << "Error: failed to create a file!" << std::endl;
            return;
        }

        file << startDate.day << " " << startDate.month << " " << startDate.year << std::endl;
        file << familyCount << std::endl;
        for (int i = 0; i < familyCount; i++) {
            file << familyMembers[i] << std::endl;
        }

        file << obsCount << std::endl;
        for (int i = 0; i < obsCount; i++) {
            file << obsDates[i].day << " " << obsDates[i].month << " " << obsDates[i].year << " ";
            file << obsNames[i] << " ";
            file.precision(2);
            file << std::fixed << obsWeights[i] << std::endl;
        }

        file.close();
        std::cout << "The data is saved to a file: " << filename << std::endl;
    }

    void loadFromFile() {
        std::cout << "Downloading a story from a file" << std::endl;
        std::string filename;
        std::cout << "Enter the file name: ";
        std::cin >> filename;

        std::ifstream file(filename.c_str());
        if (!file.is_open()) {
            std::cout << "Error: couldn't open the file!" << std::endl;
            return;
        }

        Date tempStartDate;
        int tempFamilyCount, tempObsCount;

        if (!(file >> tempStartDate.day >> tempStartDate.month >> tempStartDate.year) || !tempStartDate.isValid()) {
            std::cout << "Error: invalid start date in file!" << std::endl;
            file.close();
            return;
        }

        if (!(file >> tempFamilyCount) || tempFamilyCount < 0 || tempFamilyCount > 5) {
            std::cout << "Error: invalid number of family members in file!" << std::endl;
            file.close();
            return;
        }

        std::string tempFamilyMembers[5];
        for (int i = 0; i < tempFamilyCount; i++) {
            if (!(file >> tempFamilyMembers[i])) {
                std::cout << "Error: failed to read family member data!" << std::endl;
                file.close();
                return;
            }
        }

        if (!(file >> tempObsCount) || tempObsCount < 0 || tempObsCount > 100) {
            std::cout << "Error: invalid number of observations in file!" << std::endl;
            file.close();
            return;
        }

        std::string tempObsNames[100];
        double tempObsWeights[100];
        Date tempObsDates[100];

        for (int i = 0; i < tempObsCount; i++) {
            if (!(file >> tempObsDates[i].day >> tempObsDates[i].month >> tempObsDates[i].year) || !tempObsDates[i].isValid()) {
                std::cout << "Error: invalid date in observation " << i + 1 << "!" << std::endl;
                file.close();
                return;
            }

            if (!(file >> tempObsNames[i] >> tempObsWeights[i]) || tempObsWeights[i] < 0 || tempObsDates[i].isEarlier(tempStartDate)) {
                std::cout << "Error: invalid data in observation " << i + 1 << "!" << std::endl;
                file.close();
                return;
            }
        }

        startDate = tempStartDate;
        familyCount = tempFamilyCount;
        for (int i = 0; i < familyCount; i++) familyMembers[i] = tempFamilyMembers[i];

        obsCount = tempObsCount;
        for (int i = 0; i < obsCount; i++) {
            obsDates[i] = tempObsDates[i];
            obsNames[i] = tempObsNames[i];
            obsWeights[i] = tempObsWeights[i];
        }

        file.close();
        std::cout << "The data is downloaded from a file: " << filename << std::endl;
    }

    void showAll() const {
        std::cout << "The whole story" << std::endl;
        std::cout << "Starting date: " << startDate.day << "." << startDate.month << "." << startDate.year << std::endl;
        std::cout << "Family member (" << familyCount << "/5): ";
        for (int i = 0; i < familyCount; i++) {
            std::cout << familyMembers[i] << " ";
        }
        std::cout << std::endl;

        std::cout << "Observations:" << std::endl;
        for (int i = 0; i < obsCount; i++) {
            std::cout << obsDates[i].day << "." << obsDates[i].month << "." << obsDates[i].year << "  ";
            std::cout << obsNames[i] << "  " << obsWeights[i] << " kg" << std::endl;
        }
    }
};

int main() {
    FloorScales scales;
    int choice;

    do {
        std::cout << "Menu" << std::endl;
        std::cout << "1 - Set the starting date" << std::endl;
        std::cout << "2 - Find out the starting date" << std::endl;
        std::cout << "3 - Add a family member" << std::endl;
        std::cout << "4 - Add an observation" << std::endl;
        std::cout << "5 - Find out the weight by date" << std::endl;
        std::cout << "6 - Average weight" << std::endl;
        std::cout << "7 - Minimum weight" << std::endl;
        std::cout << "8 - Maximum weight" << std::endl;
        std::cout << "9 - Save to a file" << std::endl;
        std::cout << "10 - Upload from a file" << std::endl;
        std::cout << "11 - Show the whole story" << std::endl;
        std::cout << "0 - Exit" << std::endl;
        std::cout << "Select an action: " << std::endl;
        std::cin >> choice;

        switch (choice) {
        case 1: scales.setStartDate(); break;
        case 2: scales.getStartDate(); break;
        case 3: scales.addFamilyMember(); break;
        case 4: scales.addObservation(); break;
        case 5: scales.getWeight(); break;
        case 6: scales.getAverageWeight(); break;
        case 7: scales.findMinWeight(); break;
        case 8: scales.findMaxWeight(); break;
        case 9: scales.saveToFile(); break;
        case 10: scales.loadFromFile(); break;
        case 11: scales.showAll(); break;
        case 0: std::cout << "Exit" << std::endl; break;
        default: std::cout << "Incorrect selection! Please select an item from 0 to 11." << std::endl;
        }
    } while (choice != 0);

    return 0;
}
