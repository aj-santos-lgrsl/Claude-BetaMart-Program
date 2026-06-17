// FILE: src/Cart.cpp

#include "Cart.h"
#include <iostream>
#include <iomanip>

using std::cout;
using std::fixed;
using std::setprecision;
using std::left;
using std::right;
using std::setw;

void Cart::addItem(const Product& product, int quantity) {
    // If the same product is already in the cart, increment its quantity
    for (auto& item : items_) {
        if (item.product.name == product.name &&
            item.product.section == product.section) {
            item.quantity += quantity;
            return;
        }
    }
    items_.push_back({product, quantity});
}

void Cart::removeItem(int index) {
    if (index < 0 || index >= static_cast<int>(items_.size())) return;
    items_.erase(items_.begin() + index);
}

void Cart::updateQuantity(int index, int newQty) {
    if (index < 0 || index >= static_cast<int>(items_.size())) return;
    if (newQty <= 0) {
        items_.erase(items_.begin() + index);
    } else {
        items_[index].quantity = newQty;
    }
}

void Cart::display() const {
    if (items_.empty()) {
        cout << "\nYour cart is empty.\n";
        return;
    }

    cout << "\n--- Your Cart ---\n";
    cout << fixed << setprecision(2);
    for (size_t i = 0; i < items_.size(); ++i) {
        const auto& c = items_[i];
        cout << (i + 1) << ". "
             << left << setw(28)
             << (c.product.name + " x" + std::to_string(c.quantity))
             << right << setw(8)
             << c.product.price * c.quantity << " PHP\n";
    }
    cout << "Subtotal: " << subtotal() << " PHP\n";
}

void Cart::clear() {
    items_.clear();
}

bool Cart::isEmpty() const {
    return items_.empty();
}

double Cart::subtotal() const {
    double total = 0.0;
    for (const auto& c : items_) {
        total += c.product.price * c.quantity;
    }
    return total;
}

const vector<CartItem>& Cart::getItems() const {
    return items_;
}

int Cart::size() const {
    return static_cast<int>(items_.size());
}
