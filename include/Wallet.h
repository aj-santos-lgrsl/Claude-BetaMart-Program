#pragma once
// FILE: include/Wallet.h

#include <string>
#include <vector>
#include "Product.h"

using std::string;
using std::vector;

class Wallet {
public:
    // Constructor — default balances come from named constants in Product.h
    explicit Wallet(double cash    = STARTING_CASH,
                    double ewallet = STARTING_EWALLET);

    // Display wallet info to stdout
    void display() const;

    // Deduct from cash balance; returns true on success, false if insufficient
    bool deductCash(double amount);

    // Deduct from e-wallet balance; returns true on success, false if insufficient
    bool deductEwallet(double amount);

    // Add a coupon string to the wallet
    void addCoupon(const string& code);

    // Remove a coupon by exact string; returns true if it existed and was removed
    bool useCoupon(const string& code);

    // Check whether a coupon exists without consuming it
    bool hasCoupon(const string& code) const;

    // Accessors
    double getCash()    const;
    double getEwallet() const;
    const vector<string>& getCoupons() const;

private:
    double         cash_;
    double         ewallet_;
    vector<string> coupons_;
};
