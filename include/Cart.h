#pragma once
// FILE: include/Cart.h

#include <vector>
#include "Product.h"

using std::vector;

class Cart {
public:
    // Add a product with the given quantity
    void addItem(const Product& product, int quantity);

    // Remove item at zero-based index
    void removeItem(int index);

    // Update quantity at zero-based index.
    // If newQty <= 0 the item is removed.
    // The caller is responsible for adjusting store stock before/after calling this.
    void updateQuantity(int index, int newQty);

    // Print all cart items to stdout
    void display() const;

    // Empty the cart (does NOT restore stock — caller must do that before clearing)
    void clear();

    bool isEmpty() const;

    // Sum of (price * quantity) for all items
    double subtotal() const;

    const vector<CartItem>& getItems() const;

    int size() const;

private:
    vector<CartItem> items_;
};
