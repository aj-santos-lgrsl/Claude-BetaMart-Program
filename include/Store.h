#pragma once
// FILE: include/Store.h

#include <string>
#include <vector>
#include "Product.h"

using std::string;
using std::vector;

class Store {
public:
    explicit Store(const string& name = "500 Beta Mart Shop");

    // Load products from a JSON file.
    // Falls back to a hardcoded default list if the file is not found.
    void loadProducts(const string& jsonPath);

    // Print all products in the given section, numbered from 1.
    void displaySection(const string& section) const;

    // Return the master-list indices of all products in a section.
    vector<int> getSectionIndices(const string& section) const;

    // Case-insensitive name search; prints result to stdout.
    void searchProduct(const string& name) const;

    // Admin: add quantity to the named product's stock.
    // Returns true if the product was found, false otherwise.
    bool restock(const string& productName, int quantity);

    // Direct access to a product by master-list index (for stock adjustments).
    Product& getProduct(int index);

    const vector<Product>& getProducts() const;

    const string& getName() const;

private:
    vector<Product> products_;
    string          storeName_;

    // Used by loadProducts() when the file is missing
    void loadDefaults();

    // Helper: trim leading/trailing whitespace from a string
    static string trim(const string& s);

    // Helper: extract the string value after a JSON key in a single-line record
    static string extractString(const string& line, const string& key);

    // Helper: extract the numeric value after a JSON key
    static double extractNumber(const string& line, const string& key);
};
