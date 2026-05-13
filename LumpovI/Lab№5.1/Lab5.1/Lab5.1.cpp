#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

const int MAX_CASSETTES = 6; 
const int MAX_BILLS_PER_OPERATION = 40; 
const int CASSETTE_CAPACITY = 2000000;  
const double INITIAL_LOAD_PERCENT = 0.8; 

const int BILL_VALUES[MAX_CASSETTES] = { 100, 200, 500, 1000, 2000, 5000 };

const int MIN_CARD_NUMBER = 1;
const int MAX_CARD_NUMBER = 9999;
const int MAX_CLIENTS = 9999; 

struct Client {
    char cardNumber[5];
    char fullName[100];
    long balance; 
    char pinCode[5];   
    bool isBlocked;        
    bool isActive;        
};

class ProcessingCenter {
private:
    Client* clients;  
    int maxClients;    
    int clientCount;    

    bool isDigitsOnly(const char* str) const {
        for (int i = 0; str[i] != '\0'; ++i) {
            if (str[i] < '0' || str[i] > '9') {
                return false;
            }
        }
        return true;
    }

public:
    ProcessingCenter(): clients(nullptr), maxClients(MAX_CLIENTS), clientCount(0){
        clients = new Client[maxClients];

        for (int i = 0; i < maxClients; ++i) {
            clients[i].isActive = false;
            clients[i].isBlocked = false;
            clients[i].cardNumber[0] = '\0';
            clients[i].fullName[0] = '\0';
            clients[i].pinCode[0] = '\0';
            clients[i].balance = 0;
        }
    }

    ProcessingCenter(const ProcessingCenter& other): clients(nullptr), maxClients(other.maxClients), clientCount(other.clientCount){
        clients = new Client[maxClients];
        for (int i = 0; i < maxClients; ++i) {
            clients[i] = other.clients[i];
        }
    }

    ProcessingCenter& operator=(const ProcessingCenter& other) {
        if (this == &other) {
            return *this;
        }

        delete[] clients;
        maxClients = other.maxClients;
        clientCount = other.clientCount;
        clients = new Client[maxClients];

        for (int i = 0; i < maxClients; ++i) {
            clients[i] = other.clients[i];
        }

        return *this;
    }

    ~ProcessingCenter() {
        delete[] clients;
    }

    Client* findClient(const char* cardNumber) {
        if (!isDigitsOnly(cardNumber)) {
            return nullptr;
        }

        int num = atoi(cardNumber);
        if (num < MIN_CARD_NUMBER || num > MAX_CARD_NUMBER) {
            return nullptr;
        }

        for (int i = 0; i < maxClients; ++i) {
            if (clients[i].isActive) {
                if (atoi(clients[i].cardNumber) == num) {
                    return &clients[i];
                }
            }
        }

        return nullptr;
    }

    bool addClient(const char* cardNumber, const char* fullName,
        const char* pinCode, long long balance) {
        if (!isDigitsOnly(cardNumber) || !isDigitsOnly(pinCode)) {
            return false;
        }

        if (std::strlen(cardNumber) != 4 || std::strlen(pinCode) != 4) {
            return false;
        }

        int num = atoi(cardNumber);
        if (num < MIN_CARD_NUMBER || num > MAX_CARD_NUMBER) {
            return false;
        }

        if (findClient(cardNumber) != nullptr) {
            return false;
        }

        for (int i = 0; i < maxClients; ++i) {
            if (!clients[i].isActive) {
                std::strcpy(clients[i].cardNumber, cardNumber);
                std::strcpy(clients[i].fullName, fullName);
                std::strcpy(clients[i].pinCode, pinCode);
                clients[i].balance = balance;
                clients[i].isBlocked = false;
                clients[i].isActive = true;
                ++clientCount;
                return true;
            }
        }

        return false;
    }

    bool verifyPinCode(const Client* client, const char* pinCode) const {
        if (client == nullptr || !client->isActive) {
            return false;
        }

        if (client->isBlocked) {
            return false;
        }

        for (int i = 0; i < 4; ++i) {
            if (client->pinCode[i] != pinCode[i]) {
                return false;
            }
        }

        return true;
    }

    bool withdraw(Client* client, long long amount) {
        if (client == nullptr || !client->isActive || client->isBlocked) {
            return false;
        }

        if (amount <= 0) {
            return false;
        }

        if (client->balance < amount) {
            return false;
        }

        client->balance -= amount;
        return true;
    }

    bool deposit(Client* client, long long amount) {
        if (client == nullptr || !client->isActive || client->isBlocked) {
            return false;
        }

        if (amount <= 0) {
            return false;
        }

        client->balance += amount;
        return true;
    }

    void blockCard(Client* client) {
        if (client != nullptr && client->isActive) {
            client->isBlocked = true;
        }
    }

    long long getBalance(const Client* client) const {
        if (client == nullptr || !client->isActive || client->isBlocked) {
            return -1;
        }
        return client->balance;
    }

    bool isBlocked(const Client* client) const {
        if (client == nullptr || !client->isActive) {
            return true;
        }
        return client->isBlocked;
    }

    int getClientCount() const {
        return clientCount;
    }
};

class Bankomat {
private:
    long long cassettes[MAX_CASSETTES];
    int maxBillsPerOperation;
    long long cassetteCapacity;

    ProcessingCenter* processingCenter;

    Client* currentClient;  
    int failedPinAttempts; 
    bool cardInserted;    
    bool pinVerified;    

    bool isValidPinFormat(const char* pin) const {
        if (std::strlen(pin) != 4) return false;
        for (int i = 0; i < 4; ++i) {
            if (pin[i] < '0' || pin[i] > '9') return false;
        }
        return true;
    }

    bool isValidCardNumberFormat(const char* cardNumber) const {
        if (std::strlen(cardNumber) != 4) return false;
        for (int i = 0; i < 4; ++i) {
            if (cardNumber[i] < '0' || cardNumber[i] > '9') return false;
        }
        return true;
    }

    int getCassetteIndex(int billValue) const {
        for (int i = 0; i < MAX_CASSETTES; ++i) {
            if (BILL_VALUES[i] == billValue) {
                return i;
            }
        }
        return -1;
    }

    bool canDispense(long long amount, int billsToUse[MAX_CASSETTES], int& totalBills) const {
        for (int i = 0; i < MAX_CASSETTES; ++i) {
            billsToUse[i] = 0;
        }
        totalBills = 0;

        if (amount <= 0) {
            return false;
        }

        if (amount % BILL_VALUES[0] != 0) {
            return false;
        }

        long long remaining = amount;

        for (int i = MAX_CASSETTES - 1; i >= 0; --i) {
            if (remaining <= 0) break;

            int billValue = BILL_VALUES[i];
            if (billValue > remaining) continue;

            long long needed = remaining / billValue;

            if (needed > cassettes[i]) {
                needed = cassettes[i];
            }

            int remainingBillsLimit = maxBillsPerOperation - totalBills;
            if (needed > remainingBillsLimit) {
                needed = remainingBillsLimit;
            }

            if (needed > 0) {
                billsToUse[i] = static_cast<int>(needed);
                totalBills += static_cast<int>(needed);
                remaining -= needed * billValue;
            }
        }

        if (remaining != 0) {
            return false;
        }

        if (totalBills > maxBillsPerOperation) {
            return false;
        }

        return true;
    }

public:
    Bankomat(ProcessingCenter* pc): maxBillsPerOperation(MAX_BILLS_PER_OPERATION), cassetteCapacity(CASSETTE_CAPACITY), processingCenter(pc), currentClient(nullptr), failedPinAttempts(0), cardInserted(false), pinVerified(false){
        for (int i = 0; i < MAX_CASSETTES; ++i) {
            cassettes[i] = static_cast<long long>(CASSETTE_CAPACITY * INITIAL_LOAD_PERCENT);
        }
    }

    Bankomat(const Bankomat& other): maxBillsPerOperation(other.maxBillsPerOperation), cassetteCapacity(other.cassetteCapacity), processingCenter(other.processingCenter), currentClient(other.currentClient), failedPinAttempts(other.failedPinAttempts), cardInserted(other.cardInserted), pinVerified(other.pinVerified){
        for (int i = 0; i < MAX_CASSETTES; ++i) {
            cassettes[i] = other.cassettes[i];
        }
    }

    Bankomat& operator=(const Bankomat& other) {
        if (this == &other) {
            return *this;
        }

        maxBillsPerOperation = other.maxBillsPerOperation;
        cassetteCapacity = other.cassetteCapacity;
        processingCenter = other.processingCenter;  
        currentClient = other.currentClient;
        failedPinAttempts = other.failedPinAttempts;
        cardInserted = other.cardInserted;
        pinVerified = other.pinVerified;

        for (int i = 0; i < MAX_CASSETTES; ++i) {
            cassettes[i] = other.cassettes[i];
        }

        return *this;
    }

    ~Bankomat() {}

    bool insertCard(const char* cardNumber) {
        if (cardInserted) {
            std::cout << "Error: card almost in ATM. " << "First, take out the current card\n" << std::endl;
            return false;
        }

        if (!isValidCardNumberFormat(cardNumber)) {
            std::cout << "Error: incorrect card number format.\n" << std::endl;
            return false;
        }

        currentClient = processingCenter->findClient(cardNumber);

        if (currentClient == nullptr) {
            std::cout << "Error: client wit number " << cardNumber << " not found.\n" << std::endl;
            currentClient = nullptr;
            return false;
        }

        if (currentClient->isBlocked) {
            std::cout << "Error: card is blocked.\n" << std::endl;
            currentClient = nullptr;
            return false;
        }

        cardInserted = true;
        failedPinAttempts = 0;
        pinVerified = false;

        std::cout << "Card is verified. Please enter PIN-code\n" << std::endl;
        return true;
    }

    bool findClient(const char* cardNumber) const {
        Client* client = processingCenter->findClient(cardNumber);
        if (client != nullptr) {
            std::cout << "Client: " << client->fullName << std::endl << std::endl;
            return true;
        }
        else {
            std::cout << "Client with number " << cardNumber << " not found.\n" << std::endl;
            return false;
        }
    }

    bool verifyPin(const char* pinCode) {
        if (!cardInserted || currentClient == nullptr) {
            std::cout << "Errror: card is not inserted.\n" << std::endl;
            return false;
        }

        if (currentClient->isBlocked) {
            std::cout << "Error: card is blocked.\n" << std::endl;
            returnCard();
            return false;
        }

        if (!isValidPinFormat(pinCode)) {
            std::cout << "Error: inccorect format of PIN-code.\n" << std::endl;
            return false;
        }

        bool pinCorrect = processingCenter->verifyPinCode(currentClient, pinCode);

        if (pinCorrect) {
            failedPinAttempts = 0;
            pinVerified = true;
            std::cout << "PIN-code is verified.\n" << std::endl;
            return true;
        }
        else {
            ++failedPinAttempts;
            pinVerified = false;
            std::cout << "Inccorect PIN-code. Attempts left: " << (3 - failedPinAttempts) << std::endl << std::endl;

            if (failedPinAttempts >= 3) {
                processingCenter->blockCard(currentClient);
                std::cout << "CARD WAS BLOCKED!!! " << "You entered 3 inccorect attempts of PIN-code.\n" << std::endl;
                currentClient = nullptr;
                cardInserted = false;
                failedPinAttempts = 0;
                pinVerified = false;
            }

            return false;
        }
    }

    void printBalance() const {
        if (!cardInserted || currentClient == nullptr) {
            std::cout << "Error: card is not inserted.\n" << std::endl;
            return;
        }

        if (currentClient->isBlocked) {
            std::cout << "Error: card is blocked.\n" << std::endl;
            return;
        }

        if (!pinVerified) {
            std::cout << "Error: at first enter PIN-code.\n" << std::endl;
            return;
        }

        long long balance = processingCenter->getBalance(currentClient);

        std::cout << "\n========== BALANCE ==========" << std::endl;
        std::cout << "Client: " << currentClient->fullName << std::endl;
        std::cout << "Card: " << currentClient->cardNumber << std::endl;
        std::cout << "Balance: " << balance << " rub." << std::endl;
        std::cout << "==========================\n" << std::endl;
    }

    bool withdrawCash(long long amount) {
        if (!cardInserted || currentClient == nullptr) {
            std::cout << "Error: card is not inserted.\n" << std::endl;
            return false;
        }

        if (currentClient->isBlocked) {
            std::cout << "Error: card is blocked.\n" << std::endl;
            return false;
        }

        if (!pinVerified) {
            std::cout << "Error: at first enter PIN-code.\n" << std::endl;
            return false;
        }

        long long balance = processingCenter->getBalance(currentClient);
        if (balance < amount) {
            std::cout << "Error: not enough money on balance.\n" << "Balance: " << balance << " rub.\n" << std::endl;
            return false;
        }

        int billsToUse[MAX_CASSETTES];
        int totalBills;

        if (!canDispense(amount, billsToUse, totalBills)) {
            std::cout << "Error: inccorect amount or the limit of bills per operation has been exceeded or it is not possible to give this amount in availible bills.\n" << std::endl;
            return false;
        }

        processingCenter->withdraw(currentClient, amount);

        for (int i = 0; i < MAX_CASSETTES; ++i) {
            cassettes[i] -= billsToUse[i];
        }

        std::cout << "Accepted: " << amount << " rub." << std::endl;
        std::cout << "Bills:\n";
        for (int i = MAX_CASSETTES - 1; i >= 0; --i) {
            if (billsToUse[i] > 0) {
                std::cout << "  " << BILL_VALUES[i] << " rub. x" << billsToUse[i] << std::endl;
            }
        }
        std::cout << "Balance: " << processingCenter->getBalance(currentClient) << " rub.\n" << std::endl;

        return true;
    }

    bool depositCash() {
        if (!cardInserted || currentClient == nullptr) {
            std::cout << "Error: card is not inserted.\n." << std::endl;
            return false;
        }

        if (currentClient->isBlocked) {
            std::cout << "Error: card is blocked.\n." << std::endl;
            return false;
        }

        if (!pinVerified) {
            std::cout << "Error: at first enter PIN-code.\n" << std::endl;
            return false;
        }

        int billsDeposited[MAX_CASSETTES] = { 0 };
        int totalBills = 0;
        long long totalAmount = 0;

        std::cout << "Supported denominations: ";
        for (int i = 0; i < MAX_CASSETTES; ++i) {
            std::cout << BILL_VALUES[i];
            if (i < MAX_CASSETTES - 1) std::cout << ", ";
        }
        std::cout << " rub.\n" << std::endl;

        std::cout << "Bills deposite: \n";
        for (int i = 0; i < MAX_CASSETTES; ++i) {
            int count;
            std::cout << "Amount of " << BILL_VALUES[i] << " rub. bills: ";
            std::cin >> count;

            if (count < 0) {
                std::cout << "Error: amount should be positive.\n." << std::endl;
                return false;
            }

            billsDeposited[i] = count;
            totalBills += count;
            totalAmount += static_cast<long long>(count) * BILL_VALUES[i];
        }

        if (totalBills > maxBillsPerOperation) {
            std::cout << "Error: the limit of bills per operation has been exceeded (" << maxBillsPerOperation << "). Was accepted: " << totalBills << std::endl;
            return false;
        }

        if (totalBills == 0) {
            std::cout << "Error: no one bills was deposited.\n" << std::endl;
            return false;
        }

        for (int i = 0; i < MAX_CASSETTES; ++i) {
            if (cassettes[i] + billsDeposited[i] > cassetteCapacity) {
                std::cout << "Error: cassets of " << BILL_VALUES[i] << " rub. bills is full.\n" << std::endl;
                return false;
            }
        }

        processingCenter->deposit(currentClient, totalAmount);

        for (int i = 0; i < MAX_CASSETTES; ++i) {
            cassettes[i] += billsDeposited[i];
        }

        std::cout << "\nAccepted: " << totalAmount << " rub. (" << totalBills << " bills)" << std::endl;
        std::cout << "Balance: " << processingCenter->getBalance(currentClient) << " rub." << std::endl;

        return true;
    }

    void returnCard() {
        if (!cardInserted) {
            std::cout << "Error: card is not inserted.\n" << std::endl;
            return;
        }

        std::cout << "Card " << currentClient->cardNumber << " was returned. Goodbye, " << currentClient->fullName << "!\n" << std::endl;

        currentClient = nullptr;
        cardInserted = false;
        failedPinAttempts = 0;
        pinVerified = false;
    }

    bool isCardInserted() const {
        return cardInserted;
    }

    bool isPinVerified() const {
        return pinVerified;
    }

    const char* getCurrentClientName() const {
        if (currentClient != nullptr) {
            return currentClient->fullName;
        }
        return "no";
    }
};

int main() {
    std::cout << "========== ATM ==========\n";

    ProcessingCenter pc;

    pc.addClient("0001", "Ivanov Ivan Ivanovich", "1234", 50000);
    pc.addClient("0002", "Petrov Petr Petrovich", "5678", 150000);
    pc.addClient("0010", "Sidorova Anna Mikhailovna", "1111", 250000);
    pc.addClient("0100", "John Woods", "9999", 80000);
    pc.addClient("9999", "Jamula Albibeck Albibeckovich", "4321", 350000);

    Bankomat atm(&pc);

    int choice;

    do {
        std::cout << "=== MENU ===\n";

        if (atm.isCardInserted()) {
            std::cout << "[Card: " << atm.getCurrentClientName() << " | " << (atm.isPinVerified() ? "PIN entered" : "PIN not entered")<< "]\n";
        }
        else {
            std::cout << "[Card is not inserted]\n";
        }

        std::cout << "1. Insert card\n";
        std::cout << "2. Find client by number\n";
        std::cout << "3. Enter PIN-code\n";
        std::cout << "4. Show balance\n";
        std::cout << "5. Withdraw cash\n";
        std::cout << "6. Deposite cash\n";
        std::cout << "7. Return card\n";
        std::cout << "0. Exit\n";
        std::cout << "=====================\n";
        std::cout << "Choice: ";
        std::cin >> choice;

        std::cin.ignore(10000, '\n');

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Input error!\n" << std::endl;
            continue;
        }

        switch (choice) {
        case 1: {
            char cardNumber[5];
            std::cout << "Enter card number: ";
            std::cin.getline(cardNumber, 5);
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
            }
            atm.insertCard(cardNumber);
            break;
        }

        case 2: {
            char cardNumber[5];
            std::cout << "Enter card number: ";
            std::cin.getline(cardNumber, 5);
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
            }
            atm.findClient(cardNumber);
            break;
        }

        case 3: {
            char pinCode[5];
            std::cout << "Enter PIN-code: ";
            std::cin.getline(pinCode, 5);
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
            }

            atm.verifyPin(pinCode);
            if (!atm.isCardInserted()) {
                std::cout << "Card was returned.\n" << std::endl;
            }
            break;
        }

        case 4:
            atm.printBalance();
            break;

        case 5: {
            long long amount;
            std::cout << "Enter withdraw amount (multiple of 100): ";
            std::cin >> amount;
            std::cin.ignore(10000, '\n');

            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                std::cout << "Input error!\n" << std::endl;
                break;
            }

            atm.withdrawCash(amount);
            break;
        }

        case 6:
            atm.depositCash();
            break;

        case 7:
            atm.returnCard();
            break;

        case 0:
            if (atm.isCardInserted()) {
                atm.returnCard();
            }
            std::cout << "Goodbye!\n" << std::endl;
            break;

        default:
            std::cout << "Inccorect choice.\n" << std::endl;
        }
    } while (choice != 0);

    return 0;
}