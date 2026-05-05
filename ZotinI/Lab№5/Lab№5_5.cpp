#include <iostream>
#include <string>

using std::cout;
using std::cin;
using std::string;

class Date {
private:
    unsigned day;
    unsigned month;

    bool prov(unsigned d, unsigned m) {
        if (d > 31 || m > 12 || d == 0 || m == 0) {
            return false;
        }
        else if ((m == 4 || m == 6 || m == 9 || m == 11) && d > 30) {
            return false;
        }
        else if (m == 2 && d > 28) {
            return false;
        }
        return true;
    }

public:
    Date(unsigned d = 1, unsigned m = 1) {
        if (prov(d, m)) {
            day = d;
            month = m;
        }
        else {
            cout << "Warning date \n";
            day = 1;
            month = 1;
        }
    }

    bool operator<(const Date& other) const {
        if (month != other.month) {
            return month < other.month;
        }
        if (day != other.day) {
            return day < other.day;
        }
        return false;
    }
    bool operator==(const Date& other) const {
        return day == other.day && month == other.month;
    }
    unsigned getday() const {
        return day;
    }
    unsigned getmonth() const {
        return month;
    }

    void print_data() const {
        cout << day << '.' << month;
    }
};

class Time {
private:
    unsigned hour;
    unsigned minutes;

    bool prov_time(unsigned h, unsigned m) {
        if (h > 23 || m > 59) {
            return false;
        }
        return true;
    }

public:
    Time(unsigned h = 1, unsigned m = 0) {
        if (prov_time(h, m)) {
            hour = h;
            minutes = m;
        }
        else {
            cout << "Warning time \n";
            hour = 1;
            minutes = 0;
        }
    }

    unsigned getminutes() const {
        return minutes;
    }
    unsigned gethours() const {
        return hour;
    }

    void print_time() const {
        if (minutes < 10) {
            cout << hour << ":0" << minutes;
        }
        else {
            cout << hour << ':' << minutes;
        }
    }
};

class Session {
private:
    Date date;
    Time time;
    string film;
    unsigned hall, cost, rows, seats;
    bool places[20][30];

public:
    Session() : date(1, 1), time(1, 0), film(""), hall(1), cost(200), rows(10), seats(15) {
        for (size_t i = 0; i < 20; i++) {
            for (size_t j = 0; j < 30; j++) {
                places[i][j] = false;
            }
        }
    }

    Session(unsigned d, unsigned m, unsigned h, unsigned min, const string& film, unsigned hall, unsigned rows, unsigned seats, unsigned cost = 200): 
        date(d, m), time(h, min), film(film), hall(hall), cost(cost), rows(rows), seats(seats) {
        for (size_t i = 0; i < 20; i++) {
            for (size_t j = 0; j < 30; j++) {
                places[i][j] = false;
            }
        }
    }

    unsigned ticket_cost(const string& zone) const {
        unsigned ticcost = cost;
        if (time.gethours() < 12) {
            ticcost = (ticcost * 3) / 4;
        }
        else if (time.gethours() >= 18) {
            ticcost = (ticcost*3) / 2;
        }

        if (zone == "VIP") {
            ticcost *= 2;
        }
        return ticcost;
    }

    bool records(const string& zone, unsigned count, unsigned* bookedrows, unsigned* bookedseats) {
        unsigned start, end;
        if (zone == "VIP") {
            start = 0;
            end = rows / 2;
        }
        else {
            start = rows / 2;
            end = rows;
        }
        unsigned i = 0;

        for (size_t r = start; r < end && i < count; r++) {
            for (size_t s = 0; s < seats && i < count; s++) {
                if (!places[r][s]) {
                    places[r][s] = true;
                    bookedrows[i] = r + 1;
                    bookedseats[i] = s + 1;
                    i++;
                }
            }
        }
        return i == count;
    }

    void tofreeplaces(unsigned count, unsigned* rows_booked, unsigned* seats_booked) {
        for (size_t i = 0; i < count; i++) {
            unsigned r = rows_booked[i] - 1;
            unsigned s = seats_booked[i] - 1;
            if (r < rows && s < seats) {
                places[r][s] = false;
            }
        }
    }

    unsigned free_count(const string& zone) const {
        unsigned start;
        if (zone == "VIP") {
            start = 0;
        }
        else {
            start = rows / 2;
        }

        unsigned end;
        if (zone == "VIP") {
            end = rows / 2;
        }
        else {
            end = rows;
        }
        unsigned count = 0;
        for (size_t r = start; r < end; r++) {
            for (size_t s = 0; s < seats; s++) {
                if (!places[r][s]) {
                    count++;
                }
            }
        }
        return count;
    }

    bool cansell(unsigned curh, unsigned curm) const {
        unsigned session_min = time.gethours() * 60 + time.getminutes();
        unsigned current_min = curh * 60 + curm;
        return (current_min <= session_min + 10);
    }

    const string& getfilm() const { return film; }
    unsigned gethall() const { return hall; }
    unsigned getcost() const { return cost; }
    unsigned gethour() const { return time.gethours(); }
    unsigned getminutes() const { return time.getminutes(); }
    unsigned getday() const { return date.getday(); }
    unsigned getmonth() const { return date.getmonth(); }

    void print_session() const {
        date.print_data();
        cout << " ";
        time.print_time();
        cout << "\nFilm: " << film;
        cout << "\nHall: " << hall;
        cout << "\nBase cost: " << cost << " rub";
    }
};

class Cinema {
private:
    static const size_t max_count = 90;
    Session* s;
    size_t count;
    size_t hall_count = 10;
    unsigned cost;

    

public:
    Cinema(unsigned c = 200): count(0), hall_count(10), cost(c) {
        s = new Session[max_count];
    }
    
    Cinema(const Cinema& other) : count(other.count), cost(other.cost), hall_count(other.hall_count) {
        s = new Session[max_count];
        std::copy(other.s, other.s + max_count, s);
    }

    Cinema& operator=(const Cinema& other) {
        if (this != &other) {
            delete[] s;
            s = new Session[max_count];
            count = other.count;
            hall_count = other.hall_count;
            cost = other.cost;
            for (size_t i = 0; i < count; i++) {
                s[i] = other.s[i];
            }
        }
        return *this;
    }

    int between(unsigned d1, unsigned m1, unsigned d2, unsigned m2) const {
        int days[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        int total1 = d1;
        for (unsigned i = 1; i < m1; i++) {
            total1 += days[i];
        }

        int total2 = d2;
        for (unsigned i = 1; i < m2; i++) {
            total2 += days[i];
        }

        return total2 - total1;
    }

    void add(unsigned d, unsigned m, unsigned h, unsigned min, const string& film, unsigned hall, unsigned rows, unsigned seats) {
        if (count >= max_count || hall == 0 || hall > hall_count) {
            return;
        }
        if (count == 0) {
            s[count] = Session(d, m, h, min, film, hall, rows, seats, cost);
            count++;
            return;
        }
        int diff = between(s[0].getday(), s[0].getmonth(), d, m);
        if (diff < 0 || diff > 30) {
            cout << "Session date is outside the 30-day window\n";
            return;
        }
        s[count] = Session(d, m, h, min, film, hall, rows, seats, cost);
        count++;
    }

    const Session& gets(size_t i) const {
        return s[i];
    }
    Session& gets_id(size_t i) {
        return s[i];
    }

    unsigned get_count() const {
        return count;
    }

    void available(unsigned curh, unsigned curm, unsigned curd, unsigned curmonth) const {
        cout << "\n---- AVAILABLE SESSIONS ----" << '\n';
        Date curdate(curd, curmonth);

        for (size_t i = 0; i < count; i++) {
            Date sessiondate(s[i].getday(), s[i].getmonth());
            if (between(curd, curmonth, s[i].getday(), s[i].getmonth()) > 3 || between(curd, curmonth, s[i].getday(), s[i].getmonth()) < 0) {
                continue;
            }
            if (curdate < sessiondate || curdate == sessiondate) {
                if (curdate == sessiondate) {
                    if (!s[i].cansell(curh, curm)) {
                        continue;
                    }
                }
                cout << "Session " << (i + 1) << ": \n";
                s[i].print_session();
                unsigned vipplaces = s[i].free_count("VIP");
                unsigned baseplaces = s[i].free_count("base");

                cout << "\nVIP: " << vipplaces << " free";
                cout << "\nBase: " << baseplaces << " free\n\n";
            }
        }
        cout << "=============================\n";
    }

    ~Cinema() {
        delete[] s;
    }
};

class Ticket_office {
private: 
    Cinema* cinema;
    Date currdate;
    Time currtime;
    unsigned sold;
    unsigned allcost;
    int current_index;

    struct request {
        unsigned day;
        unsigned month;
        unsigned hour;
        unsigned min;
        string film;
        unsigned hall;
        string zone;
        unsigned count;
        request() : day(0), month(0), hour(0), min(0), hall(0), count(0) {}
    };

    struct transaction {
        unsigned index;
        string zone;
        unsigned count;
        unsigned cost;
        unsigned rows[50];
        unsigned seats[50];
        bool isvalid;
        transaction() : index(0), count(0), cost(0), isvalid(false) {
            for (int i = 0; i < 50; i++) {
                rows[i] = 0;
                seats[i] = 0;
            }
        }
    };

    transaction last;
    request now;

    void input_date(unsigned& d, unsigned& m) {
        cout << "Enter session date (day month): ";
        while (true) {
            if (cin >> d >> m) {
                Date a(d, m);
                if (a.getday() == d && a.getmonth() == m) {
                    return;
                }
                cout << "Invalid date! Enter day (1-31), month (1-12): ";
            }
            else {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid input! Enter two numbers (day month): ";
            }
        }
    }

    void input_time(unsigned& h, unsigned& min) {
        cout << "Enter session time (hour minute): ";
        while (true) {
            if (cin >> h >> min) {
                if (h <= 23 && min <= 59) {
                    return;
                }
                cout << "Invalid time! Enter hour (0-23), minute (0-59): ";
            }
            else {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid input! Enter two numbers (hour minute): ";
            }
        }
    }

    void input_name(string& film) {
        cin.ignore(10000, '\n');
        while (true) {
            cout << "Enter film name: ";
            getline(cin, film);
            if (!film.empty()) {
                return;
            }
            cout << "Film name cannot be empty!\n";
        }
    }

    void input_hall(unsigned& hall) {
        cout << "Enter hall number (1-10): ";
        do {
            cin >> hall;
            if (cin.fail()) {
                cin.clear();
                cin.ignore(10000, '\n');
            }
        } while (hall < 1 || hall > 10);
    }

    void input_zone(string& zone) {
        cin.ignore(10000, '\n');
        cout << "Enter zone type (VIP | base): ";
        while (true) {
            getline(cin, zone);
            if (zone == "VIP" || zone == "base") {
                return;
            }
            cout << "Invalid zone! Enter 'VIP' or 'base': ";
        }
    }

    void input_count(unsigned& count, unsigned max) {
        if (max == 0) {
            cout << "No free places available!\n";
            return;
        }
        cout << "Enter number of tickets (1-" << max << "): ";
        do {
            cin >> count;
            if (cin.fail()) {
                cout << "Invalid number";
                cin.clear();
                cin.ignore(10000, '\n');
            }
        } while (count < 1 || count > max);
    }

    int findSession(unsigned d, unsigned m, unsigned h, unsigned min, const string& film, unsigned hall) {
        for (size_t i = 0; i < cinema->get_count(); i++) {
            const Session& sess = cinema->gets(i);
            if (sess.getday() == d && sess.getmonth() == m && sess.gethour() == h && sess.getminutes() == min && sess.getfilm() == film && sess.gethall() == hall) {
                return i;
            }
        }
        return -1;
    }

    void print_tic() const {
        cout << "\nFilm: " << now.film;
        cout << "\nDate: ";
        if (now.day < 10) {
            cout << '0';
        }
        cout << now.day << ".";
        if (now.month < 10) {
            cout << '0';
        }
        cout << now.month;
        cout << "\nTime: ";
        if (now.hour < 10) {
            cout << '0';
        }
        cout << now.hour << ":";
        if (now.min < 10) {
            cout << '0';
        }
        cout << now.min;
        cout << "\nHall: " << now.hall << '\n';
    }

public:
    Ticket_office(Cinema* c, unsigned d, unsigned m, unsigned h, unsigned min): currdate(d, m), currtime(h, min), cinema(c), sold(0), allcost(0), current_index(-1) {
            last.isvalid = false;
    }

    void accept() {
        if (last.isvalid) {
            cout << "Warning: Previous reservation will be lost!\n";
        }
        now = request();
        cinema->available(currtime.gethours(), currtime.getminutes(), currdate.getday(), currdate.getmonth());
        input_date(now.day, now.month);
        input_time(now.hour, now.min);
        input_name(now.film);
        input_hall(now.hall);
        input_zone(now.zone);

        while (true) {
            current_index = findSession(now.day, now.month, now.hour, now.min, now.film, now.hall);
            if (current_index != -1) {
                break;
            }
            cout << "Session not found. Try again\n";
            input_date(now.day, now.month);
            input_time(now.hour, now.min);
            input_name(now.film);
            input_hall(now.hall);
            input_zone(now.zone);
        }

        if (!cinema->gets(current_index).cansell(currtime.gethours(), currtime.getminutes())) {
            cout << "Tickets for this session are no longer available\n";
            now.count = 0;
            current_index = -1;
            return;
        }

        if (cinema->between(currdate.getday(), currdate.getmonth(), now.day, now.month) > 3 || cinema->between(currdate.getday(), currdate.getmonth(), now.day, now.month) < 0) {
            cout << "Tickets can only be sold within 3 days from current date\n";
            now.count = 0;
            current_index = -1;
            return;
        }
        unsigned free_places = cinema->gets(current_index).free_count(now.zone);
        cout << "Available places in " << now.zone << " zone: " << free_places << '\n';

        input_count(now.count, free_places);
        cout << "Data accepted successfully\n";
    }

    void prov_count() {
        if (now.count == 0 || current_index == -1) {
            cout << "Please enter customer data first.\n";
            return;
        }

        const Session& sess = cinema->gets(current_index);
        unsigned free = sess.free_count(now.zone);

        cout << "\n---AVAILABILITY CHECK ---\n";
        cout << "Session: ";
        sess.print_session();
        cout << "\nZone: " << now.zone;
        cout << "\nRequested tickets: " << now.count;
        cout << "\nAvailable places: " << free << '\n';

        if (free >= now.count) {
            cout << "AVAILABLE\n";
            cout << "-----------------------\n";
            return;;
        }
        else {
            cout << "NOT AVAILABLE\n";
            cout << "==============================\n";
            return;
        }
    }

    void reserve() {
        if (now.count == 0 || current_index == -1) {
            cout << "Please enter customer data first.\n";
            return;
        }
        if (last.isvalid) {
            cout << "Have an active reservation.\n";
            return;
        }

        Session& sess = cinema->gets_id(current_index);
        unsigned bookedrows[50], bookedseats[50];

        if (!sess.records(now.zone, now.count, bookedrows, bookedseats)) {
            cout << "Failed to reserve places\n";
            return;
        }

        last.index = current_index;
        last.zone = now.zone;
        last.count = now.count;
        last.isvalid = true;

        for (unsigned j = 0; j < now.count; j++) {
            last.rows[j] = bookedrows[j];
            last.seats[j] = bookedseats[j];
        }
        cout << "\nReserved " << now.count << " places in " << now.zone << " zone\n";
        cout << "Places: ";
        for (unsigned j = 0; j < now.count; j++) {
            cout << "Row " << bookedrows[j] << ", Seat " << bookedseats[j];
            if (j < now.count - 1) cout << "; ";
        }
        cout << '\n';
    }

    unsigned cost() {
        if (now.count == 0 || current_index == -1) {
            cout << "Please enter customer data first.\n";
            return 0;
        }

        const Session& sess = cinema->gets(current_index);
        unsigned cost_ticket = sess.ticket_cost(now.zone);
        unsigned total_cost = cost_ticket * now.count;

        print_tic();
        cout << "Zone: " << now.zone << '\n';
        cout << "Base cost: " << sess.getcost() << " rub\n";
        cout << "Cost per ticket: " << cost_ticket << " rub\n";
        cout << "Number of tickets: " << now.count << '\n';
        cout << "Total cost: " << total_cost << " rub\n";
        last.cost = total_cost;
        return total_cost;
    }

    void cancel() {
        if (!last.isvalid) {
            cout << "No active order to cancel!\n";
            return;
        }

        cout << "\n----- CURRENT ORDER -----\n";
        print_tic();
        cout << "Zone: " << now.zone << '\n';
        cout << "Tickets: " << last.count << '\n';
        cout << "Amount: " << last.cost << " rub\n";

        Session& sess = cinema->gets_id(last.index);
        sess.tofreeplaces(last.count, last.rows, last.seats);

        if (sold >= last.count) {
            sold -= last.count;
            allcost -= last.cost;
        }

        cout << "Order cancelled\n";
        last.isvalid = false;
    }

    void ticket() {
        if (!last.isvalid || current_index == -1) {
            cout << "No active reservation\n";
            return;
        }

        const Session& sess = cinema->gets(current_index);
        unsigned cost_ticket = sess.ticket_cost(last.zone);
        unsigned total_cost = cost_ticket * last.count;

        for (size_t j = 0; j < last.count; j++) {
            print_tic();
            cout << "Zone: " << last.zone;
            cout << "\nRow: " << last.rows[j] << "   Seat: " << last.seats[j] << '\n';
            cout << "--------------------------\n";
        }

        sold += last.count;
        allcost += total_cost;
        last.cost = total_cost;

        cout << "\nTotal tickets: " << last.count;
        cout << "\nTotal amount: " << total_cost << " rub\n";
        cout << "----------------------------\n";
        last.isvalid = false;
        now = request();
        current_index = -1;
    }

};



int main() {
    Cinema c;
    c.add(15, 11, 20, 0, "Interstellar", 3, 10, 15);
    c.add(15, 11, 10, 0, "The Matrix", 1, 10, 15);
    c.add(15, 11, 14, 30, "Inception", 2, 8, 12);

    c.add(16, 11, 9, 0, "Oppenheimer", 2, 10, 15);
    c.add(16, 11, 12, 0, "Dune 2", 1, 8, 12);
    c.add(16, 11, 16, 30, "The Matrix", 3, 10, 15);
    c.add(16, 11, 21, 0, "Interstellar", 5, 10, 15);

    c.add(17, 11, 8, 30, "Inception", 2, 8, 12);
    c.add(17, 11, 13, 0, "Oppenheimer", 1, 10, 15);
    c.add(17, 11, 22, 0, "Dune 2", 4, 10, 15);
    Ticket_office office(&c, 15, 11, 0, 0);
    int s;

    do {
        cout << "---Ticket office---\n";
        cout << "1. accept the buyer's details\n";
        cout << "2. check the availability of the required number of available seats in the required area\n";
        cout << "3. reserve the required number of seats\n";
        cout << "4. calculate the total cost of tickets\n";
        cout << "5. cancel your ticket order\n";
        cout << "6. generate tickets\n";
        cout << "0. exit\n";
        cout << "Choice: ";

        cin >> s;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        switch (s) {
        case 1:
            office.accept();
            break;
        case 2:
            office.prov_count();
            break;
        case 3:
            office.reserve();
            break;
        case 4:
            office.cost();
            break;
        case 5:
            office.cancel();
            break;
        case 6:
            office.ticket();
            break;
        case 0:
            break;
        default:
            cout << "Input error. Try again\n";
            break;
        }
        cout << '\n';
    } while (s != 0);

    return 0;
}
