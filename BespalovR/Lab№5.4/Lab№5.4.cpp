/*5.4. Касса в магазине
Разработать классы Касса и Склад.
Класс Касса должен имитировать работу кассового аппарата по сканированию товаров и
формированию чека за покупку. Каждый товар идентифицируется штрих-кодом (для
упрощения – строка из четырех цифр от «0001» до «9999»). Один и тот же товар может
сканироваться несколько раз, но в чек информация о каждом товаре входит в виде
«наименование – стоимость за единицу (для упрощения в рублях без копеек) – количество –
общая стоимость за товар». Чек состоит не менее чем из одной записи указанного вида. Чек
дополнительно включает общую стоимость товаров в покупке, суммарную скидку и итоговую
сумму к оплате (все в рублях).
База товаров хранится в классе Склад. Товар описывается штрих-кодом, наименованием,
стоимостью за единицу товара, скидкой в процентах от стоимости. Скидки устанавливаются на
каждый товар независимо (в диапазоне от 1 до 50%).
Класс Касса должен предоставлять следующие операции: 1) «сканировать» очередной товар, 2)
получить описание товара со склада, 3) добавить данные о товаре в чек, 4) сформировать чек за
покупку, 5) рассчитать итоговую сумму к оплате, 6) удалить выбранный товар из покупки.
Класс Склад должен использоваться для поддержки работы класса Касса и может быть
разработан в минимально-необходимом варианте.*/
#include <stdexcept>
#include <iostream>
#include <vector>
#include <string>
using std::ostream;
using std::vector;
using std::string;
using std::cout;

//Продукт с 4 необходимыми полями, проверками корректности в конструкторе и геттерами
class Product {
    string barcode;
    string name;
    size_t price;
    unsigned short discount;
public:
    Product(string barcode_ = "0001", string name_ = "", size_t price_ = 1, unsigned short discount_ = 1): barcode(barcode_), name(name_), price(price_), discount(discount_) {
        if (discount < 1 || discount > 50) {
            throw std::out_of_range("Discount out of range");
        }
        if (barcode.length() != 4) {
            throw std::out_of_range("Incorrect barcode length");
        }
        for (unsigned short i = 0; i < 4; ++i) {
            if (barcode[i] < '0' || barcode[i] > '9') {
                throw std::out_of_range("Incorrect barcode characters");
            }
        }
        if (barcode == "0000") {
            throw std::out_of_range("Incorrect barcode");
        }
    }

    string getBarcode() const { return barcode; }
    string getName() const { return name; }
    size_t getPrice() const { return price; }
    unsigned short getDiscount() const { return discount; }
    size_t getDiscountedPrice() const { return (price * (100 - discount)) / 100; }
};

//Склад - вектор продуктов, как в прошой лабораторной отсортированно хранится и новые продукты вставляются бинарным поиском
//Два метода для поиска продукта на складе и добавления. оба с обработкой ошибок на отсутствие/наличие товара на складе
class Warehouse {
    vector<Product> stock;

    size_t binarySearch(const string& barcode) const {
        size_t left = 0;
        size_t right = stock.size();
        while (left < right) {
            size_t mid = left + (right - left) / 2;
            if (stock[mid].getBarcode() < barcode) {
                left = mid + 1;
            }
            else {
                right = mid;
            }
        }
        return left;
    }
public:
    Product& findByBarcode(const string& barcode) {
        size_t index = binarySearch(barcode);
        if (index < stock.size() && stock[index].getBarcode() == barcode) {
            return stock[index];
        }
        throw std::logic_error("Product doesn't exist");
    }

    void addProduct(const Product& p) {
        try {
            findByBarcode(p.getBarcode());
            throw std::out_of_range("Product with such barcode is already in stock");
        }
        catch (std::logic_error&) {
            size_t pos = binarySearch(p.getBarcode());
            stock.insert(stock.begin() + pos, p);
        }
    }
};

//Структура для упрощения ведения количества товаров в чеке(пытался без неё, вышло страшно)
struct ReceiptEntry {
    Product product;
    size_t quantity = 1;

    size_t getTotalPrice() const {
        return product.getPrice() * quantity;
    }
    size_t getTotalDiscountedPrice() const {
        return product.getDiscountedPrice() * quantity;
    }
    size_t getDiscountAmount() const {
        return getTotalPrice() - getTotalDiscountedPrice();
    }

    friend ostream& operator<<(ostream& os, const ReceiptEntry& entry) {
        os << entry.product.getName() << " - " << entry.product.getPrice() << " rub. - " << entry.quantity << " - " << entry.getTotalDiscountedPrice() << " rub.";
        return os;
    }
};

//Касса, конструктором по ссылке связывается со складом, пункт 1 не понял и сделал аналогично Коробейникову(есть/нет продукт на складе)
class Cashier {
    Warehouse& warehouse;
    //Товары в чеке хронятся хаотично, без сортировки, в требованиях нет, я сам не захотел
    vector<ReceiptEntry> receipt;
public:
    Cashier(Warehouse& warehouse_) : warehouse(warehouse_) {}

    //1) «сканировать» очередной товар,
    bool scanProduct(const string& barcode) const{
        try {
            warehouse.findByBarcode(barcode);
            return true;
        }
        catch (const std::logic_error&) {
            return false;
        }
    }
    //Можно было и вывести но решил просто вернуть продукт
    //2) получить описание товара со склада,
    Product getProductInfo(const string& barcode) const{
        return warehouse.findByBarcode(barcode);
    }
    //Ищет по штрихкоду после пробега по всему чеку, что с длинным чеком в случае несуществующего на складе штрихкода неэффективно
    //3) добавить данные о товаре в чек,
    void addToReceipt(const string& barcode) {
        for (size_t i = 0; i < receipt.size(); ++i) {
            if (receipt[i].product.getBarcode() == barcode) {
                receipt[i].quantity++;
                return;
            }
        }
        ReceiptEntry new_entry;
        new_entry.product = warehouse.findByBarcode(barcode);
        receipt.push_back(new_entry);
    }

    //4) сформировать чек за покупку,
    void printReceipt() const {
        if (receipt.empty()) {
            throw std::logic_error("Empty receipt");
        }

        size_t total_price = 0;
        size_t total_discount = 0;

        cout << "Receipt:\n";
        for (const auto& entry : receipt) {
            cout << entry << "\n";
            total_price += entry.getTotalPrice();
            total_discount += entry.getDiscountAmount();
        }

        size_t final_amount = total_price - total_discount;
        cout << "\nTotal price: " << total_price << " rub.\n";
        cout << "Discounted: " << total_discount << " rub.\n";
        cout << "To pay: " << final_amount << " rub.\n";
    }

    //5) рассчитать итоговую сумму к оплате,
    size_t getTotalCost() const {
        size_t total = 0;
        for (size_t i = 0; i < receipt.size(); ++i) {
            total += receipt[i].getTotalDiscountedPrice();
        }
        return total;
    }

    //6) удалить выбранный товар из покупки.
    void removeProduct(const string& barcode) {
        for (size_t i = 0; i < receipt.size(); ++i) {
            if (receipt[i].product.getBarcode() == barcode) {
                receipt.erase(receipt.begin()+i);
                return;
            }
        }
        throw std::logic_error("Product is not in the receipt");
    }
};


int main() {

    Warehouse warehouse;
    try {
        warehouse.addProduct(Product("0001", "Bread", 50, 5));
        warehouse.addProduct(Product("0002", "Milk", 80, 10));
        warehouse.addProduct(Product("0003", "Cheese", 200, 15));
        warehouse.addProduct(Product("0004", "Apple", 60, 3));
        warehouse.addProduct(Product("0005", "Chocolate bar", 120, 20));
    }
    catch (const std::exception& e) {
        std::cout << "ERR: " << e.what() << std::endl;
        return 1;
    }


    Cashier cashier(warehouse);
    cashier.addToReceipt("0001");
    cashier.addToReceipt("0002");
    cashier.addToReceipt("0001");
    cashier.addToReceipt("0003");
    cashier.addToReceipt("0004");
    cashier.addToReceipt("0005");
    cashier.addToReceipt("0002");


    if (cashier.scanProduct("0003")) {
        std::cout << "0003 Found." << std::endl;
    }
    else {
        std::cout << "0003 Not found." << std::endl;
    }


    try {
        Product product = cashier.getProductInfo("0004");
        std::cout << "Name: " << product.getName() << std::endl;
        std::cout << "Price: " << product.getPrice() << " rub." << std::endl;
        std::cout << "Discount: " << product.getDiscount() << "%" << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "ERR: " << e.what() << std::endl;
        return 1;
    }


    try {
        cashier.removeProduct("0004");
    }
    catch (const std::exception& e) {
        std::cout << "ERR: " << e.what() << std::endl;
        return 1;
    }


    try {
        cashier.printReceipt();
    }
    catch (const std::exception& e) {
        std::cout << "ERR: " << e.what() << std::endl;
        return 1;
    }
    std::cout << "Total cost\n" << cashier.getTotalCost() << " rub." << std::endl;

    return 0;
}
