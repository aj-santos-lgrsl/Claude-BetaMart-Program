// FILE: src/Wallet.cpp

#include "Wallet.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

using std::cout;
using std::fixed;
using std::setprecision;
using std::find;

Wallet::Wallet(double cash, double ewallet)
    : cash_(cash), ewallet_(ewallet) {}

void Wallet::display() const {
    cout << "\n--- WALLET ---\n";
    cout << fixed << setprecision(2);
    cout << "Cash Balance:     " << cash_    << " PHP\n";
    cout << "E-Wallet Balance: " << ewallet_ << " PHP\n";

    if (coupons_.empty()) {
        cout << "Coupons: none\n";
    } else {
        cout << "Coupons:\n";
        for (const auto& c : coupons_) {
            cout << "  - " << c << "\n";
        }
    }
}

bool Wallet::deductCash(double amount) {
    if (cash_ < amount) return false;
    cash_ -= amount;
    return true;
}

bool Wallet::deductEwallet(double amount) {
    if (ewallet_ < amount) return false;
    ewallet_ -= amount;
    return true;
}

void Wallet::addCoupon(const string& code) {
    coupons_.push_back(code);
}

bool Wallet::useCoupon(const string& code) {
    auto it = find(coupons_.begin(), coupons_.end(), code);
    if (it == coupons_.end()) return false;
    coupons_.erase(it);
    return true;
}

bool Wallet::hasCoupon(const string& code) const {
    return find(coupons_.begin(), coupons_.end(), code) != coupons_.end();
}

double Wallet::getCash() const    { return cash_;    }
double Wallet::getEwallet() const { return ewallet_; }
const vector<string>& Wallet::getCoupons() const { return coupons_; }
