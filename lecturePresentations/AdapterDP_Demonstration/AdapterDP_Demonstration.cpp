#include <iostream>
#include <string>

// главный класс
class OldBank {
public:
    double getBalanceOld(std::string acc) {
        double balance = 0;
        // добываем баланс
        return balance;
    }
};

// 1. OBJECT ADAPTER (композиция)
class MobileBank {
    OldBank& old;
public:
    MobileBank(OldBank& o) : old(o) {}

    std::string getBalance(std::string acc) {
        // получаем баланс 
        return std::to_string(old.getBalanceOld(acc));
    }
};

// 2. CLASS ADAPTER (наследование + интерфейс)
class IWebBank {
public:
    virtual std::string getBalance(std::string acc) = 0;
    virtual ~IWebBank() {}
};
// наследуем интерфейс адаптер + главный класс
class WebBank : public IWebBank, public OldBank {
public:
    std::string getBalance(std::string acc) override {
        return std::to_string(getBalanceOld(acc));
    }
};

int main() {
    OldBank old;

    MobileBank mobile(old);
    std::cout << "Object adapter: " << mobile.getBalance("accountNumber") << "\n";

    WebBank web;
    std::cout << "Class adapter: " << web.getBalance("accountNumber") << "\n";

    // Разница:
    // 1. 
    //    - Object adapter содержит OldBank
    //    - Object adapter работает с любым наследником OldBank
    // 2. 
    //    - Class adapter наследует OldBank
    //    - Class adapter фиксирован для OldBank

    return 0;
}