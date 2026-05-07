#include <iostream>
#include <fstream>
#include <string>

struct Date {
    int day;
    int month;
    int year;

    bool isValid() const {
        if (year < 1900 || year > 2026) return false;
        if (month < 1 || month > 12) return false;

        const int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

        if (day < 1 || day > daysInMonth[month - 1]) return false;

        return true;
    }
};

struct Song {
    std::string title;
    std::string poet;
    std::string composer;
    std::string performer;
    std::string album;
    Date releaseDate;
};

class Songbook {
private:
    Song* songs;
    int songCount;
    int capacity;

    bool isFirstStringSmaller(const std::string& s1, const std::string& s2) const {
        int i = 0;
        while (i < s1.length() && i < s2.length()) {
            char c1 = (s1[i] >= 'A' && s1[i] <= 'Z') ? s1[i] + 32 : s1[i];
            char c2 = (s2[i] >= 'A' && s2[i] <= 'Z') ? s2[i] + 32 : s2[i];
            if (c1 < c2) return true;
            if (c1 > c2) return false;
            i++;
        }
        return s1.length() < s2.length();
    }

    void sortSongs() {
        for (int i = 0; i < songCount - 1; i++) {
            for (int j = 0; j < songCount - i - 1; j++) {
                if (!isFirstStringSmaller(songs[j].title, songs[j + 1].title)) {
                    Song temp = songs[j];
                    songs[j] = songs[j + 1];
                    songs[j + 1] = temp;
                }
            }
        }
    }

    int binarySearch(const std::string& title, const std::string& performer) const {
        int left = 0;
        int right = songCount - 1;

        while (left <= right) {
            int mid = left + (right - left) / 2;

            if (songs[mid].title == title && songs[mid].performer == performer) {
                return mid;
            }

            if (songs[mid].title == title) {
                if (songs[mid].performer < performer) {
                    left = mid + 1;
                }
                else {
                    right = mid - 1;
                }
            }
            else if (isFirstStringSmaller(songs[mid].title, title)) {
                left = mid + 1;
            }
            else {
                right = mid - 1;
            }
        }

        for (int i = 0; i < songCount; i++) {
            if (songs[i].title == title && songs[i].performer == performer) {
                return i;
            }
        }
        return -1;
    }

    void ensureCapacity() {
        if (songCount < capacity) return;

        int newCapacity = (capacity == 0) ? 10 : capacity * 2;
        Song* newSongs = new Song[newCapacity];

        for (int i = 0; i < songCount; i++) {
            newSongs[i] = songs[i];
        }

        delete[] songs;
        songs = newSongs;
        capacity = newCapacity;
    }

public:
    Songbook() : songs(nullptr), songCount(0), capacity(0) {
        ensureCapacity();
    }

    ~Songbook() {
        delete[] songs;
    }

    Songbook(const Songbook&) = delete;
    Songbook& operator=(const Songbook&) = delete;

    void addSong() {
        ensureCapacity();

        Song newSong;

        std::cout << "Enter song title: ";
        std::cin.ignore();
        std::getline(std::cin, newSong.title);

        std::cout << "Enter performer: ";
        std::getline(std::cin, newSong.performer);

        if (binarySearch(newSong.title, newSong.performer) != -1) {
            std::cout << "A song with this title and performer already exists" << std::endl;
            return;
        }

        std::cout << "Enter poet (lyrics author): ";
        std::getline(std::cin, newSong.poet);

        std::cout << "Enter composer (music author): ";
        std::getline(std::cin, newSong.composer);

        std::cout << "Enter album name: ";
        std::getline(std::cin, newSong.album);

        std::cout << "Enter release date:" << std::endl;
        std::cout << "  Day: ";
        std::cin >> newSong.releaseDate.day;
        std::cout << "  Month: ";
        std::cin >> newSong.releaseDate.month;
        std::cout << "  Year: ";
        std::cin >> newSong.releaseDate.year;

        if (!newSong.releaseDate.isValid()) {
            std::cout << "Invalid date. Song not added." << std::endl;
            return;
        }

        songs[songCount] = newSong;
        songCount++;

        sortSongs();

        std::cout << "Song \"" << newSong.title << "\" added successfully" << std::endl;
    }

    void editSong() {
        if (songCount == 0) {
            std::cout << "The songbook is empty" << std::endl;
            return;
        }

        std::string title, performer;

        std::cout << "Enter song title to find: ";
        std::cin.ignore();
        std::getline(std::cin, title);
        std::cout << "Enter performer to find: ";
        std::getline(std::cin, performer);

        int index = binarySearch(title, performer);

        if (index == -1) {
            std::cout << "Song not found" << std::endl;
            return;
        }

        Song originalSong = songs[index];

        int choice;
        std::cout << "\nWhat would you like to edit?" << std::endl;
        std::cout << "1 - Title" << std::endl;
        std::cout << "2 - Poet" << std::endl;
        std::cout << "3 - Composer" << std::endl;
        std::cout << "4 - Performer" << std::endl;
        std::cout << "5 - Album" << std::endl;
        std::cout << "6 - Release date" << std::endl;
        std::cout << "Choice: ";
        std::cin >> choice;

        switch (choice) {
        case 1: {
            std::cout << "Enter new title: ";
            std::cin.ignore();
            std::string newTitle;
            std::getline(std::cin, newTitle);

            if (binarySearch(newTitle, songs[index].performer) != -1 &&
                (newTitle != title || songs[index].performer != performer)) {
                std::cout << "A song with this title and performer already exists" << std::endl;
                return;
            }
            songs[index].title = newTitle;
            sortSongs();
            break;
        }
        case 2:
            std::cout << "Enter new poet: ";
            std::cin.ignore();
            std::getline(std::cin, songs[index].poet);
            break;
        case 3:
            std::cout << "Enter new composer: ";
            std::cin.ignore();
            std::getline(std::cin, songs[index].composer);
            break;
        case 4: {
            std::cout << "Enter new performer: ";
            std::cin.ignore();
            std::string newPerformer;
            std::getline(std::cin, newPerformer);

            if (binarySearch(songs[index].title, newPerformer) != -1 &&
                (songs[index].title != title || newPerformer != performer)) {
                std::cout << "A song with this title and performer already exists" << std::endl;
                return;
            }
            songs[index].performer = newPerformer;
            break;
        }
        case 5:
            std::cout << "Enter new album: ";
            std::cin.ignore();
            std::getline(std::cin, songs[index].album);
            break;
        case 6:
            std::cout << "Enter new release date:" << std::endl;
            std::cout << "  Day: ";
            std::cin >> songs[index].releaseDate.day;
            std::cout << "  Month: ";
            std::cin >> songs[index].releaseDate.month;
            std::cout << "  Year: ";
            std::cin >> songs[index].releaseDate.year;
            if (!songs[index].releaseDate.isValid()) {
                std::cout << "Invalid date. Changes not saved." << std::endl;
                songs[index] = originalSong;
                return;
            }
            break;
        default:
            std::cout << "Invalid choice" << std::endl;
            return;
        }

        std::cout << "Song updated successfully" << std::endl;
    }

    void findSong() const {
        if (songCount == 0) {
            std::cout << "The songbook is empty" << std::endl;
            return;
        }

        std::string title, performer;
        std::cout << "Enter song title: ";
        std::cin.ignore();
        std::getline(std::cin, title);
        std::cout << "Enter performer: ";
        std::getline(std::cin, performer);

        int index = binarySearch(title, performer);

        if (index == -1) {
            std::cout << "Song not found" << std::endl;
            return;
        }

        std::cout << "\nSong found" << std::endl;
        std::cout << "Title: " << songs[index].title << std::endl;
        std::cout << "Poet: " << songs[index].poet << std::endl;
        std::cout << "Composer: " << songs[index].composer << std::endl;
        std::cout << "Performer: " << songs[index].performer << std::endl;
        std::cout << "Album: " << songs[index].album << std::endl;
        std::cout << "Release date: " << songs[index].releaseDate.day << "."
            << songs[index].releaseDate.month << "." << songs[index].releaseDate.year << std::endl;
    }

    void showSongsByPoet() const {
        if (songCount == 0) {
            std::cout << "The songbook is empty" << std::endl;
            return;
        }

        std::string poet;
        std::cout << "Enter poet name: ";
        std::cin.ignore();
        std::getline(std::cin, poet);

        bool found = false;
        std::cout << "\nSongs with poet \"" << poet << "\":" << std::endl;

        for (int i = 0; i < songCount; i++) {
            if (songs[i].poet == poet) {
                std::cout << "- " << songs[i].title << " (performed by " << songs[i].performer << ")" << std::endl;
                found = true;
            }
        }

        if (!found) {
            std::cout << "No songs found for this poet" << std::endl;
        }
    }

    void showSongsByComposer() const {
        if (songCount == 0) {
            std::cout << "The songbook is empty" << std::endl;
            return;
        }

        std::string composer;
        std::cout << "Enter composer name: ";
        std::cin.ignore();
        std::getline(std::cin, composer);

        bool found = false;
        std::cout << "\nSongs with composer \"" << composer << "\":" << std::endl;

        for (int i = 0; i < songCount; i++) {
            if (songs[i].composer == composer) {
                std::cout << "- " << songs[i].title << " (performed by " << songs[i].performer << ")" << std::endl;
                found = true;
            }
        }

        if (!found) {
            std::cout << "No songs found for this composer" << std::endl;
        }
    }

    void showSongsByPerformer() const {
        if (songCount == 0) {
            std::cout << "The songbook is empty" << std::endl;
            return;
        }

        std::string performer;
        std::cout << "Enter performer name: ";
        std::cin.ignore();
        std::getline(std::cin, performer);

        bool found = false;
        std::cout << "\nSongs performed by \"" << performer << "\":" << std::endl;

        for (int i = 0; i < songCount; i++) {
            if (songs[i].performer == performer) {
                std::cout << "- " << songs[i].title << " (poet: " << songs[i].poet
                    << ", composer: " << songs[i].composer << ")" << std::endl;
                found = true;
            }
        }

        if (!found) {
            std::cout << "No songs found for this performer" << std::endl;
        }
    }

    void showSongCount() const {
        std::cout << "Current number of songs in the songbook: " << songCount << std::endl;
        std::cout << "Current capacity: " << capacity << std::endl;
    }

    void deleteSong() {
        if (songCount == 0) {
            std::cout << "The songbook is empty" << std::endl;
            return;
        }

        std::string title, performer;
        std::cout << "Enter song title to delete: ";
        std::cin.ignore();
        std::getline(std::cin, title);
        std::cout << "Enter performer to delete: ";
        std::getline(std::cin, performer);

        int index = binarySearch(title, performer);

        if (index == -1) {
            std::cout << "Song not found" << std::endl;
            return;
        }

        for (int i = index; i < songCount - 1; i++) {
            songs[i] = songs[i + 1];
        }

        songCount--;
        std::cout << "Song \"" << title << "\" deleted successfully" << std::endl;
    }

    void saveToFile() const {
        if (songCount == 0) {
            std::cout << "The songbook is empty" << std::endl;
            return;
        }

        std::string filename;
        std::cout << "Enter filename: ";
        std::cin >> filename;

        std::ofstream file(filename.c_str());

        if (!file.is_open()) {
            std::cout << "Cannot open file for writing" << std::endl;
            return;
        }

        file << songCount << std::endl;

        for (int i = 0; i < songCount; i++) {
            file << songs[i].title << std::endl;
            file << songs[i].poet << std::endl;
            file << songs[i].composer << std::endl;
            file << songs[i].performer << std::endl;
            file << songs[i].album << std::endl;
            file << songs[i].releaseDate.day << " "
                << songs[i].releaseDate.month << " "
                << songs[i].releaseDate.year << std::endl;
        }

        file.close();
        std::cout << "Songbook saved to file \"" << filename << "\" successfully ("
            << songCount << " songs saved)" << std::endl;
    }

    void loadFromFile() {
        std::string filename;
        std::cout << "Enter filename: ";
        std::cin >> filename;

        std::ifstream file(filename.c_str());

        if (!file.is_open()) {
            std::cout << "Cannot open file for reading" << std::endl;
            return;
        }

        int count;
        file >> count;
        file.ignore();

        delete[] songs;
        songCount = 0;
        capacity = 0;
        ensureCapacity();

        for (int i = 0; i < count; i++) {
            ensureCapacity();

            Song newSong;
            std::getline(file, newSong.title);
            std::getline(file, newSong.poet);
            std::getline(file, newSong.composer);
            std::getline(file, newSong.performer);
            std::getline(file, newSong.album);
            file >> newSong.releaseDate.day
                >> newSong.releaseDate.month
                >> newSong.releaseDate.year;
            file.ignore();

            songs[songCount] = newSong;
            songCount++;
        }

        file.close();
        sortSongs();
        std::cout << "Songbook loaded from file \"" << filename << "\" successfully ("
            << songCount << " songs loaded)" << std::endl;
    }
};

int main() {
    Songbook songbook;
    int choice;

    std::cout << "========== SONGBOOK ==========" << std::endl;

    do {
        std::cout << "\nMenu" << std::endl;
        std::cout << "1. Add a song" << std::endl;
        std::cout << "2. Edit a song" << std::endl;
        std::cout << "3. Find a song by title and performer" << std::endl;
        std::cout << "4. Show all songs by poet" << std::endl;
        std::cout << "5. Show all songs by composer" << std::endl;
        std::cout << "6. Show all songs by performer" << std::endl;
        std::cout << "7. Show current number of songs" << std::endl;
        std::cout << "8. Delete a song" << std::endl;
        std::cout << "9. Save songbook to file" << std::endl;
        std::cout << "10. Load songbook from file" << std::endl;
        std::cout << "0. Exit" << std::endl;
        std::cout << "Choice: ";
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Please enter a number" << std::endl;
            continue;
        }

        switch (choice) {
        case 1:
            songbook.addSong();
            break;
        case 2:
            songbook.editSong();
            break;
        case 3:
            songbook.findSong();
            break;
        case 4:
            songbook.showSongsByPoet();
            break;
        case 5:
            songbook.showSongsByComposer();
            break;
        case 6:
            songbook.showSongsByPerformer();
            break;
        case 7:
            songbook.showSongCount();
            break;
        case 8:
            songbook.deleteSong();
            break;
        case 9:
            songbook.saveToFile();
            break;
        case 10:
            songbook.loadFromFile();
            break;
        case 0:
            std::cout << "Goodbye!" << std::endl;
            break;
        default:
            std::cout << "Invalid choice" << std::endl;
        }
    } while (choice != 0);

    return 0;
}