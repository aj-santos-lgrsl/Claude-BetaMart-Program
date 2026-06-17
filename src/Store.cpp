// FILE: src/Store.cpp

#include "Store.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

using std::cout;
using std::cerr;
using std::fixed;
using std::setprecision;
using std::ifstream;
using std::string;
using std::vector;

Store::Store(const string& name) : storeName_(name) {}

// ── Internal helpers ──────────────────────────────────────────────────────────

string Store::trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

string Store::extractString(const string& line, const string& key) {
    // Looks for: "key": "value"  and returns the inner value string.
    string searchKey = "\"" + key + "\"";
    size_t kpos = line.find(searchKey);
    if (kpos == string::npos) return "";

    size_t colon = line.find(':', kpos + searchKey.size());
    if (colon == string::npos) return "";

    size_t open = line.find('"', colon + 1);
    if (open == string::npos) return "";

    size_t close = line.find('"', open + 1);
    if (close == string::npos) return "";

    return line.substr(open + 1, close - open - 1);
}

double Store::extractNumber(const string& line, const string& key) {
    // Looks for: "key": 123.45  and returns the numeric value.
    string searchKey = "\"" + key + "\"";
    size_t kpos = line.find(searchKey);
    if (kpos == string::npos) return 0.0;

    size_t colon = line.find(':', kpos + searchKey.size());
    if (colon == string::npos) return 0.0;

    // Skip whitespace after the colon
    size_t numStart = colon + 1;
    while (numStart < line.size() && (line[numStart] == ' ' || line[numStart] == '\t'))
        ++numStart;

    // Read until we hit a non-numeric character (comma, space, bracket, newline)
    size_t numEnd = numStart;
    while (numEnd < line.size() &&
           (isdigit(static_cast<unsigned char>(line[numEnd])) || line[numEnd] == '.'))
        ++numEnd;

    if (numEnd == numStart) return 0.0;

    try {
        return std::stod(line.substr(numStart, numEnd - numStart));
    } catch (...) {
        return 0.0;
    }
}

// ── loadProducts ──────────────────────────────────────────────────────────────
//
// Manual JSON parser for the flat array format used in data/products.json.
// Each object is expected to fit on a single line of the form:
//   { "name": "...", "section": "...", "price": N.N, "stock": N }
//
// If the file cannot be opened, loadDefaults() is called instead.

void Store::loadProducts(const string& jsonPath) {
    ifstream file(jsonPath);
    if (!file.is_open()) {
        cerr << "[WARNING] Could not open '" << jsonPath
             << "'. Falling back to built-in product list.\n";
        loadDefaults();
        return;
    }

    products_.clear();

    string line;
    // Accumulate characters between '{' and '}' as a single record
    string record;
    bool   inObject = false;

    while (std::getline(file, line)) {
        for (char ch : line) {
            if (ch == '{') {
                inObject = true;
                record.clear();
                record += ch;
            } else if (ch == '}' && inObject) {
                record += ch;
                inObject = false;

                // Parse the accumulated record
                string name    = extractString(record, "name");
                string section = extractString(record, "section");
                double price   = extractNumber(record, "price");
                int    stock   = static_cast<int>(extractNumber(record, "stock"));

                if (!name.empty() && !section.empty()) {
                    products_.push_back({name, section, price, stock});
                }
                record.clear();
            } else if (inObject) {
                record += ch;
            }
        }
    }

    if (products_.empty()) {
        cerr << "[WARNING] No products parsed from '" << jsonPath
             << "'. Falling back to built-in product list.\n";
        loadDefaults();
    } else {
        cout << "[INFO] Loaded " << products_.size()
             << " products from '" << jsonPath << "'.\n";
    }
}

// ── displaySection ────────────────────────────────────────────────────────────

void Store::displaySection(const string& section) const {
    cout << "\n--- Section: " << section << " ---\n";
    cout << fixed << setprecision(2);

    int num = 1;
    bool found = false;
    for (const auto& p : products_) {
        if (p.section == section) {
            cout << num++ << ". " << p.name
                 << " - " << p.price << " PHP"
                 << " (Stock: " << p.stock << ")\n";
            found = true;
        }
    }
    if (!found) cout << "No items available.\n";
}

// ── getSectionIndices ─────────────────────────────────────────────────────────

vector<int> Store::getSectionIndices(const string& section) const {
    vector<int> indices;
    for (int i = 0; i < static_cast<int>(products_.size()); ++i) {
        if (products_[i].section == section)
            indices.push_back(i);
    }
    return indices;
}

// ── searchProduct ─────────────────────────────────────────────────────────────

void Store::searchProduct(const string& name) const {
    // Case-insensitive comparison helper
    auto toLower = [](string s) {
        std::transform(s.begin(), s.end(), s.begin(),
                       [](unsigned char c){ return std::tolower(c); });
        return s;
    };

    string key = toLower(trim(name));
    cout << fixed << setprecision(2);

    for (const auto& p : products_) {
        if (toLower(p.name) == key) {
            cout << "Found: " << p.name
                 << " | Section: " << p.section
                 << " | Price: "   << p.price << " PHP"
                 << " | Stock: "   << p.stock << "\n";
            return;
        }
    }
    cout << "Item not found.\n";
}

// ── restock ───────────────────────────────────────────────────────────────────

bool Store::restock(const string& productName, int quantity) {
    auto toLower = [](string s) {
        std::transform(s.begin(), s.end(), s.begin(),
                       [](unsigned char c){ return std::tolower(c); });
        return s;
    };

    string key = toLower(productName);
    for (auto& p : products_) {
        if (toLower(p.name) == key) {
            p.stock += quantity;
            cout << "[RESTOCK] " << p.name << " — new stock: " << p.stock << "\n";
            return true;
        }
    }
    return false;
}

// ── Accessors ─────────────────────────────────────────────────────────────────

Product& Store::getProduct(int index) {
    return products_.at(static_cast<size_t>(index));
}

const vector<Product>& Store::getProducts() const {
    return products_;
}

const string& Store::getName() const {
    return storeName_;
}

// ── loadDefaults ──────────────────────────────────────────────────────────────

void Store::loadDefaults() {
    products_ = {
        {"Toothpaste",              "Basic Essentials", 50.0,  20 },
        {"Shampoo Sachet",          "Basic Essentials", 10.0,  15 },
        {"Mouthwash Bottle",        "Basic Essentials", 100.0, 25 },
        {"Alcohol",                 "Basic Essentials", 45.0,  30 },
        {"Tissue",                  "Basic Essentials", 20.0,  50 },
        {"Cotton Buds",             "Basic Essentials", 10.0,  100},
        {"Sanitary Napkins",        "Basic Essentials", 30.0,  50 },
        {"Bath Soap",               "Basic Essentials", 25.0,  75 },
        {"Cotton Balls",            "Basic Essentials", 15.0,  100},
        {"Face Mask",               "Basic Essentials", 10.0,  100},
        {"Hand Sanitizer",          "Basic Essentials", 50.0,  30 },
        {"Rexona Deodorant Sachet", "Basic Essentials", 12.0,  20 },

        {"Coke 1.5L",                "Drinks", 40.0, 50 },
        {"Sprite 1.5L",              "Drinks", 40.0, 50 },
        {"Royal 1.5L",               "Drinks", 40.0, 50 },
        {"Gatorade No Sugar Citrus", "Drinks", 50.0, 30 },
        {"Yakult",                   "Drinks", 15.0, 20 },
        {"Mineral Water",            "Drinks", 20.0, 100},
        {"Dutch Mill",               "Drinks", 25.0, 50 },
        {"C2 Green Tea",             "Drinks", 30.0, 40 },
        {"C2 Red Tea",               "Drinks", 30.0, 40 },
        {"Cobra",                    "Drinks", 35.0, 30 },
        {"Kopiko Bottle",            "Drinks", 25.0, 50 },

        {"San Marino Corned Tuna",  "Foods", 40.0, 50 },
        {"Century Tuna",            "Foods", 40.0, 25 },
        {"Argentina Beef Loaf",     "Foods", 30.0, 25 },
        {"Mega Sardines",           "Foods", 25.0, 100},
        {"Hekkaido",                "Foods", 40.0, 30 },
        {"Purefoods Luncheon Meat", "Foods", 60.0, 40 },
        {"CDO Carne Norte",         "Foods", 50.0, 30 },
        {"Pancit Canton",           "Foods", 20.0, 50 },
        {"Lucky Me! Instant Noodles","Foods", 15.0, 100},
        {"Piattos",                 "Foods", 18.0, 100},
        {"Chips Ahoy!",             "Foods", 25.0, 100},
        {"Oreo",                    "Foods", 30.0, 100},
        {"Muncher",                 "Foods", 20.0, 100},
        {"Nissin Wafers",           "Foods", 15.0, 100},
        {"Skyflakes",               "Foods", 10.0, 100},
        {"Rebisco",                 "Foods", 10.0, 100},

        {"Notebook",        "School Supplies", 30.0, 50 },
        {"Pen",             "School Supplies", 10.0, 100},
        {"Pencil",          "School Supplies", 8.0,  100},
        {"Yellow Pad",      "School Supplies", 30.0, 100},
        {"Eraser",          "School Supplies", 5.0,  100},
        {"Glue",            "School Supplies", 25.0, 100},
        {"Scissors",        "School Supplies", 35.0, 100},
        {"Tape",            "School Supplies", 15.0, 100},
        {"Correction Tape", "School Supplies", 25.0, 100},

        {"T-Shirt", "Clothing", 150.0, 25 },
        {"Jeans",   "Clothing", 300.0, 10 },
        {"Jacket",  "Clothing", 450.0, 5  },
        {"Towel",   "Clothing", 40.0,  50 },
        {"Socks",   "Clothing", 20.0,  100},
        {"Cap",     "Clothing", 100.0, 20 },

        {"Tipid Siomai Rice", "Beta Meals", 60.0, 20},
        {"Hot Dog Giant",     "Beta Meals", 60.0, 20},
        {"Siopao Sarap",      "Beta Meals", 60.0, 20},
    };
}
