#pragma once
// FILE: include/Product.h

#include <string>

using std::string;

// ── Named constants ────────────────────────────────────────────────────────────
constexpr double TAX_RATE          = 0.10;
constexpr double STUDENT_DISCOUNT  = 0.15;
constexpr double COUPON_DISCOUNT   = 0.10;
constexpr int    STUDENT_ID_LENGTH = 7;
constexpr int    EWALLET_NUM_LENGTH = 11;
constexpr int    EWALLET_PIN_LENGTH = 4;
constexpr double STARTING_CASH    = 500.0;
constexpr double STARTING_EWALLET = 500.0;

// NOTE: ADMIN_PASSWORD is plain-text for demonstration only.
//       Replace with a hashed credential check before any production deployment.
constexpr char ADMIN_PASSWORD[] = "admin123";

const string COUPON_CODE   = "Beta Meals 10% Discount";
const string RECEIPT_FILE  = "receipts/receipts.txt";

// ── Data structures ────────────────────────────────────────────────────────────

struct Product {
    string name;
    string section;
    double price;
    int    stock;
};

struct CartItem {
    Product product;
    int     quantity;
};
