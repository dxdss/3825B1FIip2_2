#include <iostream>
#include <fstream>

struct Date {
    int day;
    int month;
    int year;
};

struct Time {
    int hour;  
};


struct Observation {
    Date date;
    Time time;
    double temperature;
};

class Thermometer {
private:
    static const int MAX_DAYS = 365;
    static const int HOURS_PER_DAY = 24;

    Observation*** history;
    int totalDays;

    Date startDate;
    Time startTime;

    int daysInMonth(int month) const {
        int days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        return days[month - 1];
    }

    int dateToDayIndex(const Date& date) const {
        return daysBetween(startDate, date);
    }

    int daysBetween(const Date& from, const Date& to) const {
        int daysFrom = 0;
        for (int m = 1; m < from.month; ++m) {
            daysFrom += daysInMonth(m);
        }
        daysFrom += (from.day - 1);
        daysFrom += from.year * 365;

        int daysTo = 0;
        for (int m = 1; m < to.month; ++m) {
            daysTo += daysInMonth(m);
        }
        daysTo += (to.day - 1);
        daysTo += to.year * 365;

        return daysTo - daysFrom;
    }

    bool isDateInRange(const Date& date) const {
        int diff = daysBetween(startDate, date);
        return (diff >= 0 && diff < MAX_DAYS);
    }

    bool isValidDate(const Date& date) const {
        if (date.year < 0)
            return false;
        if (date.month < 1 || date.month > 12)
            return false;
        if (date.day < 1 || date.day > daysInMonth(date.month))
            return false;
        return true;
    }

    bool isValidTime(const Time& time) const {
        return (time.hour >= 0 && time.hour < 24);
    }

    Date dayIndexToDate(int dayIndex) const {
        Date result = startDate;
        int remaining = dayIndex;

        while (remaining > 0) {
            int daysInCurrentMonth = daysInMonth(result.month);

            if (result.day + remaining <= daysInCurrentMonth) {
                result.day += remaining;
                remaining = 0;
            }
            else {
                remaining -= (daysInCurrentMonth - result.day + 1);
                result.day = 1;

                if (result.month == 12) {
                    result.month = 1;
                    ++result.year;
                }
                else {
                    ++result.month;
                }
            }
        }

        return result;
    }

    void allocateHistory() {
        history = new Observation**[totalDays];

        for (int i = 0; i < totalDays; ++i) {
            history[i] = new Observation*[HOURS_PER_DAY];

            for (int j = 0; j < HOURS_PER_DAY; ++j) {
                history[i][j] = nullptr;
            }
        }
    }

    void deallocateHistory() {
        if (history == nullptr) return;

        for (int i = 0; i < totalDays; ++i) {
            if (history[i] != nullptr) {
                for (int j = 0; j < HOURS_PER_DAY; ++j) {
                    if (history[i][j] != nullptr) {
                        delete history[i][j];
                        history[i][j] = nullptr;
                    }
                }
                delete[] history[i];
                history[i] = nullptr;
            }
        }
        delete[] history;
        history = nullptr;
    }

    void copyFrom(const Thermometer& other) {
        startDate = other.startDate;
        startTime = other.startTime;
        totalDays = other.totalDays;

        allocateHistory();

        for (int i = 0; i < totalDays; ++i) {
            for (int j = 0; j < HOURS_PER_DAY; ++j) {
                if (other.history[i][j] != nullptr) {
                    history[i][j] = new Observation;
                    history[i][j]->date = other.history[i][j]->date;
                    history[i][j]->time = other.history[i][j]->time;
                    history[i][j]->temperature = other.history[i][j]->temperature;
                }
                else {
                    history[i][j] = nullptr;
                }
            }
        }
    }

public:
    Thermometer(): startDate({ 1, 1, 2024 }), startTime({ 0 }), totalDays(MAX_DAYS), history(nullptr) { allocateHistory(); }

    Thermometer(const Date& date, const Time& time): startDate(date), startTime(time), totalDays(MAX_DAYS), history(nullptr){
        if (!isValidDate(date) || !isValidTime(time)) {
            std::cout << "Error: inccorect date or time\n " << "Value was sat by defult (1 Jan 2024, 0:00).\n\n";
            startDate.day = 1;
            startDate.month = 1;
            startDate.year = 2024;
            startTime.hour = 0;
        }

        allocateHistory();
    }


    ~Thermometer() {
        deallocateHistory();
    }

    Thermometer(const Thermometer& other): history(nullptr), totalDays(0){
        copyFrom(other);
    }

    Thermometer& operator=(const Thermometer& other) {
        if (this == &other) {
            return *this;
        }

        deallocateHistory();

        copyFrom(other);

        return *this;
    }

    void setStartDateTime() {
        Date date;
        Time time;

        std::cout << "Enter start date (day month year): ";
        std::cin >> date.day >> date.month >> date.year;

        std::cout << "Enter start time (0-23): ";
        std::cin >> time.hour;

        if (!isValidDate(date)) {
            std::cout << "Error: incorrect date\n" << std::endl;
            return;
        }
        if (!isValidTime(time)) {
            std::cout << "Error: incorrect time\n" << std::endl;
            return;
        }

        deallocateHistory();

        startDate = date;
        startTime = time;
        totalDays = MAX_DAYS;

        allocateHistory();

        std::cout << "Start date and time were sat!\n\n";
    }

    void getStartDateTime() const {
        std::cout << "Date: ";

        if (startDate.day < 10) std::cout << "0";
        std::cout << startDate.day << ".";

        if (startDate.month < 10) std::cout << "0";
        std::cout << startDate.month << "." << startDate.year << std::endl;

        std::cout << "Time: ";

        if (startTime.hour < 10) std::cout << "0";
        std::cout << startTime.hour << ":00" << std::endl;
    }

    void setObservation() {
        Date date;
        Time time;
        double temperature;

        std::cout << "Enter date of observation: ";
        std::cin >> date.day >> date.month >> date.year;

        std::cout << "Enter time of observation(0-23): ";
        std::cin >> time.hour;

        std::cout << "Enter temperature: ";
        std::cin >> temperature;

        if (!isValidDate(date) || !isValidTime(time)) {
            std::cout << "Error: incorret date or time\n" << std::endl;
            return;
        }

        if (!isDateInRange(date)) {
            Date endDate = dayIndexToDate(MAX_DAYS - 1);
            std::cout << "Error: date is out of range\n" << std::endl;
            return;
        }

        int dayIndex = dateToDayIndex(date);
        int hourIndex = time.hour;

        if (history[dayIndex][hourIndex] != nullptr) {
            delete history[dayIndex][hourIndex];
            std::cout << "Old observation was updated\n" << std::endl;
        }

        Observation* obs = new Observation;
        obs->date = date;
        obs->time = time;
        obs->temperature = temperature;

        history[dayIndex][hourIndex] = obs;
        std::cout << "Observation was added\n" << std::endl;
    }

    void getTemperature() const {
        Date date;
        Time time;

        std::cout << "Enter date of observation: ";
        std::cin >> date.day >> date.month >> date.year;

        std::cout << "Enter time of observation(0-23): ";
        std::cin >> time.hour;

        if (!isValidDate(date) || !isValidTime(time)) {
            std::cout << "Error: incorrect date or time\n" << std::endl;
            return;
        }

        int dayIndex = dateToDayIndex(date);

        if (dayIndex < 0 || dayIndex >= totalDays) {
            std::cout << "Error: date is out of range\n" << std::endl;
            return;
        }

        int hourIndex = time.hour;

        if (history[dayIndex][hourIndex] == nullptr) {
            std::cout << "Error: observation wasn't found\n" << std::endl;
            return;
        }

        std::cout << "Temperature: " << history[dayIndex][hourIndex]->temperature << " °C" << std::endl;
    }

    void setDailyObservations() {
        Date date;
        double temperatures[24];

        std::cout << "Enter date: ";
        std::cin >> date.day >> date.month >> date.year;

        if (!isValidDate(date)) {
            std::cout << "Error: inccorect date\n" << std::endl;
            return;
        }

        int dayIndex = dateToDayIndex(date);

        if (!isDateInRange(date)) {
            std::cout << "Error: date is out of range\n" << std::endl;
            return;
        }

        std::cout << "Enter temperature for 24 hours: " << std::endl;
        for (int hour = 0; hour < 24; ++hour) {
            std::cin >> temperatures[hour];
        }

        for (int hour = 0; hour < 24; ++hour) {
            Time t;
            t.hour = hour;

            if (history[dayIndex][hour] != nullptr) {
                delete history[dayIndex][hour];
            }

            Observation* obs = new Observation;
            obs->date = date;
            obs->time = t;
            obs->temperature = temperatures[hour];

            history[dayIndex][hour] = obs;
        }

        std::cout << "Series of observation were added\n" << std::endl;
    }

    void getAverageDailyTemperature() const {
        Date date;

        std::cout << "Enter date: ";
        std::cin >> date.day >> date.month >> date.year;

        if (!isValidDate(date)) {
            std::cout << "Error: incorrect date\n" << std::endl;
            return;
        }

        int dayIndex = dateToDayIndex(date);

        if (!isDateInRange(date)) {
            std::cout << "Error: date is out of range\n" << std::endl;
            return;
        }

        double sum = 0;
        int count = 0;

        for (int hour = 0; hour < HOURS_PER_DAY; ++hour) {
            if (history[dayIndex][hour] != nullptr) {
                sum += history[dayIndex][hour]->temperature;
                ++count;
            }
        }

        if (count == 0) {
            std::cout << "Error: observation wasn't found\n" << std::endl;
            return;
        }

        std::cout << "Avg temp for date: " << date.day << "." << date.month << "." << date.year << ": " << (sum / count) << " °C (for " << count << " observations)" << std::endl;
    }

    void getAverageMonthlyTemperature() const {
        int month;
        int choice;

        std::cout << "Enter month(1-12): ";
        std::cin >> month;

        if (month < 1 || month > 12) {
            std::cout << "Error: incorrect month\n" << std::endl;
            return;
        }

        std::cout << "Choose time of day:\n";
        std::cout << "  1 - Day (6:00 - 17:59)\n";
        std::cout << "  2 - Night (18:00 - 5:59)\n";
        std::cout << "Choice: ";
        std::cin >> choice;

        bool isDay;
        if (choice == 1) {
            isDay = true;
        }
        else if (choice == 2) {
            isDay = false;
        }
        else {
            std::cout << "Error: incorrect choice\n" << std::endl;
            return;
        }

        double sum = 0.0;
        int count = 0;

        for (int dayOfYear = 0; dayOfYear < totalDays; ++dayOfYear) {
            Date currentDate = dayIndexToDate(dayOfYear);


            if (currentDate.month != month) continue;

            if (isDay) {
                for (int h = 6; h <= 17; ++h) {
                    if (history[dayOfYear][h] != nullptr) {
                        sum += history[dayOfYear][h]->temperature;
                        ++count;
                    }
                }
            }
            else {
                for (int h = 0; h <= 5; ++h) {
                    if (history[dayOfYear][h] != nullptr) {
                        sum += history[dayOfYear][h]->temperature;
                        ++count;
                    }
                }
                for (int h = 18; h <= 23; ++h) {
                    if (history[dayOfYear][h] != nullptr) {
                        sum += history[dayOfYear][h]->temperature;
                        ++count;
                    }
                }
            }
        }

        if (count == 0) {
            std::cout << "Error: no observation for that month\n" << std::endl;
            return;
        }

        const char* months[] = {
            "", "January", "February", "March", "April", "May", "June",
            "July", "August", "September", "October", "November", "December"
        };

        std::cout << "Average " << (isDay ? "day" : "night") << " temperature for " << months[month] << ": " << (sum / count) << " °C (for " << count << " observation)" << std::endl;
    }

    void saveToFile() const {
        char filename[256];

        std::cout << "Enter filename: ";
        std::cin >> filename;

        std::ofstream file(filename);

        if (!file.is_open()) {
            std::cout << "Error: file cannot be openned\n" << std::endl;
            return;
        }

        file << startDate.day << " " << startDate.month << " " << startDate.year << std::endl;
        file << startTime.hour << std::endl;

        int totalObs = 0;
        for (int i = 0; i < totalDays; ++i) {
            for (int j = 0; j < HOURS_PER_DAY; ++j) {
                if (history[i][j] != nullptr) {
                    ++totalObs;
                }
            }
        }

        file << totalObs << std::endl;

        for (int i = 0; i < totalDays; ++i) {
            for (int j = 0; j < HOURS_PER_DAY; ++j) {
                if (history[i][j] != nullptr) {
                    file << history[i][j]->date.day << " " << history[i][j]->date.month << " " << history[i][j]->date.year << " " << history[i][j]->time.hour << " " << history[i][j]->temperature << std::endl;
                }
            }
        }

        file.close();
        std::cout << "History saved in file \"" << filename << "\" (" << totalObs << " writes)" << std::endl;
    }

    void loadFromFile() {
        char filename[256];

        std::cout << "Enter filename: ";
        std::cin >> filename;

        std::ifstream file(filename);

        if (!file.is_open()) {
            std::cout << "Error: file cannot be openned\n" << std::endl;
            return;
        }

        deallocateHistory();

        file >> startDate.day >> startDate.month >> startDate.year;
        file >> startTime.hour;

        allocateHistory();

        int totalObs;
        file >> totalObs;

        for (int i = 0; i < totalObs; ++i) {
            Date d;
            Time t;
            double temp;

            file >> d.day >> d.month >> d.year >> t.hour >> temp;

            if (!isValidDate(d) || !isValidTime(t)) {
                std::cout << "Warning: incorrecr write in file was skipped\n" << std::endl;
                continue;
            }

            int dayIndex = dateToDayIndex(d);

            if (!isDateInRange(d)) {
                std::cout << "Warning: date is out of range, obs was sskipped\n" << std::endl;
                continue;
            }

            int hourIndex = t.hour;

            if (history[dayIndex][hourIndex] != nullptr) {
                delete history[dayIndex][hourIndex];
            }

            Observation* obs = new Observation;
            obs->date = d;
            obs->time = t;
            obs->temperature = temp;

            history[dayIndex][hourIndex] = obs;
        }

        file.close();
        std::cout << "History was loaded from \"" << filename << "\" (" << totalObs << " writes)" << std::endl;
    }

};

int main() {
    std::cout << "========== THERMOMETR ==========\n";
    Thermometer thermometer;
    int choice;

    do {
        std::cout << " 1. Set start date and time\n";
        std::cout << " 2. Show start date and time\n";
        std::cout << " 3. Add observation\n";
        std::cout << " 4. Show temperature for date and time\n";
        std::cout << " 5. Set series of observation for date(24 hours)\n";
        std::cout << " 6. Average temperature for choosen date\n";
        std::cout << " 7. Average date/night temperaure for choosen month\n";
        std::cout << " 8. Save journal in file\n";
        std::cout << " 9. Load journal from file\n";
        std::cout << " 0. Exit\n" << std::endl;

        std::cout << "Choice: ";
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Error: enter correct number(0-9)" << std::endl;
            continue;
        }

        switch (choice) {
        case 1:
            thermometer.setStartDateTime();
            break;
        case 2:
            thermometer.getStartDateTime();
            break;
        case 3:
            thermometer.setObservation();
            break;
        case 4:
            thermometer.getTemperature();
            break;
        case 5:
            thermometer.setDailyObservations();
            break;
        case 6:
            thermometer.getAverageDailyTemperature();
            break;
        case 7:
            thermometer.getAverageMonthlyTemperature();
            break;
        case 8:
            thermometer.saveToFile();
            break;
        case 9:
            thermometer.loadFromFile();
            break;
        case 0:
            std::cout << "Goodbye!\n" << std::endl;
            break;
        default:
            std::cout << "Error: incorrect choice\n" << std::endl;
        }
    } while (choice != 0);

    return 0;
}