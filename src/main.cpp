// FILE: src/main.cpp

#include <iostream>
#include <iomanip>
#include <fstream>
#include <limits>
#include <string>
#include <ctime>
#include <filesystem>

#include "Product.h"
#include "Cart.h"
#include "Wallet.h"
#include "Store.h"

using std::cin;
using std::cout;
using std::cerr;
using std::string;
using std::fixed;
using std::setprecision;
using std::left;
using std::right;
using std::setw;
using std::numeric_limits;
using std::streamsize;

// ── Scoped menu enum ──────────────────────────────────────────────────────────
enum class MenuChoice {
    BasicEssentials  = 1,
    Drinks           = 2,
    Foods            = 3,
    SchoolSupplies   = 4,
    Clothing         = 5,
    BetaMeals        = 6,
    Search           = 7,
    Wallet           = 8,
    ViewCart         = 9,
    RemoveItem       = 10,
    UpdateQuantity   = 11,
    Checkout         = 12,
    AdminRestock     = 13,
    Exit             = 14
};

// ── Input helpers ─────────────────────────────────────────────────────────────

// Print prompt, read a valid integer, clear fail-state on bad input, loop until valid.
int getIntInput(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            return value;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "[ERROR] Invalid input. Please enter a number.\n";
    }
}

// Read a full line, trimming leading whitespace left after an earlier cin >>.
string getLineInput(const string& prompt) {
    string value;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << prompt;
    std::getline(cin, value);
    return value;
}

// ── Receipt printing ──────────────────────────────────────────────────────────

void printReceipt(const Cart& cart, double subtotal, double discount,
                  double tax, double total, double cashPaid) {

    // Build real timestamp
    time_t now = time(nullptr);
    char   buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));

    // Ensure receipts directory exists
    std::filesystem::create_directories("receipts");

    std::ofstream file(RECEIPT_FILE, std::ios::app);

    // Lambda writes to both cout and the file
    auto out = [&](std::ostream& os) {
        os << fixed << setprecision(2);
        os << "\n------------------------------------- RECEIPT -------------------------------------\n";
        os << "\n------------------------------ 500 BETA MART SHOP ---------------------------------\n";
        os << "--------------------------- Owned and Operated by Group 2 -------------------------\n";
        os << "------------------ 1CSIT Inc., Colegio de San Juan de Letran Calamba ---------------\n\n";
        os << "Date: " << buf << "\n\n";

        os << left << setw(32) << "Item & Qty."
           << right << setw(10) << "Price\n";
        os << "-----------------------------------------------------------\n";

        for (const auto& c : cart.getItems()) {
            string label = c.product.name + " x" + std::to_string(c.quantity);
            os << left  << setw(32) << label
               << right << setw(10) << c.product.price * c.quantity << " PHP\n";
        }

        os << "-----------------------------------------------------------\n";
        os << "Subtotal: "  << right << setw(10) << subtotal << " PHP\n";
        os << "Discount: -" << right << setw(9)  << discount << " PHP\n";
        os << "Tax:      +" << right << setw(9)  << tax      << " PHP\n";
        os << "-----------------------------------------------------------\n";
        os << "TOTAL:    "  << right << setw(10) << total    << " PHP\n";

        if (cashPaid > 0) {
            os << "Cash Paid:" << right << setw(10) << cashPaid        << " PHP\n";
            os << "Change:   " << right << setw(10) << cashPaid - total << " PHP\n";
        }

        os << "\n----------------- Thank you for shopping at 500 BETA MART SHOP! -----------------\n";
    };

    out(cout);
    if (file.is_open()) {
        out(file);
        file.close();
    } else {
        cerr << "[WARNING] Could not write receipt to " << RECEIPT_FILE << "\n";
    }
}

// ── Section shopping ──────────────────────────────────────────────────────────

void browseSection(Store& store, Cart& cart, const string& section) {
    store.displaySection(section);
    std::vector<int> indices = store.getSectionIndices(section);

    if (indices.empty()) {
        cout << "No items in this section.\n";
        return;
    }

    int pick = getIntInput("Enter item number (0 to cancel): ");
    if (pick <= 0 || pick > static_cast<int>(indices.size())) {
        cout << "Cancelled.\n";
        return;
    }

    int masterIdx = indices[pick - 1];
    Product& prod = store.getProduct(masterIdx);

    int qty = getIntInput("Quantity: ");
    if (qty < 1) {
        cout << "Quantity must be at least 1.\n";
        return;
    }
    if (qty > prod.stock) {
        cout << "Not enough stock. Available: " << prod.stock << "\n";
        return;
    }

    prod.stock -= qty;
    cart.addItem(prod, qty);
    cout << qty << "x " << prod.name << " added to cart.\n";
}

// ── Remove item from cart ─────────────────────────────────────────────────────

void removeFromCart(Cart& cart, Store& store) {
    if (cart.isEmpty()) {
        cout << "\nYour cart is empty. Nothing to remove.\n";
        return;
    }

    cout << "\n--- Remove Item from Cart ---\n";
    cout << fixed << setprecision(2);
    const auto& items = cart.getItems();
    for (size_t i = 0; i < items.size(); ++i) {
        cout << (i + 1) << ". " << items[i].product.name
             << " x" << items[i].quantity
             << " — " << items[i].product.price * items[i].quantity << " PHP\n";
    }

    int rem = getIntInput("Enter item number to remove (0 to cancel): ");
    if (rem == 0) {
        cout << "Cancelled.\n";
        return;
    }
    if (rem < 1 || rem > cart.size()) {
        cout << "Invalid selection.\n";
        return;
    }

    // Restore stock before removing
    const CartItem& ci = cart.getItems()[rem - 1];
    bool restored = store.restock(ci.product.name, ci.quantity);
    // restock() prints its own confirmation; suppress it here by checking return value
    (void)restored;

    cout << "Removed: " << ci.product.name << "\n";
    cart.removeItem(rem - 1);
}

// ── Update cart item quantity ─────────────────────────────────────────────────

void updateCartQuantity(Cart& cart, Store& store) {
    if (cart.isEmpty()) {
        cout << "\nYour cart is empty. Nothing to update.\n";
        return;
    }

    cout << "\n--- Update Cart Item Quantity ---\n";
    cout << fixed << setprecision(2);
    const auto& items = cart.getItems();
    for (size_t i = 0; i < items.size(); ++i) {
        cout << (i + 1) << ". " << items[i].product.name
             << " x" << items[i].quantity
             << " — " << items[i].product.price * items[i].quantity << " PHP\n";
    }

    int pick = getIntInput("Enter item number to update (0 to cancel): ");
    if (pick == 0) {
        cout << "Cancelled.\n";
        return;
    }
    if (pick < 1 || pick > cart.size()) {
        cout << "Invalid selection.\n";
        return;
    }

    int idx = pick - 1;
    const CartItem& ci = cart.getItems()[idx];

    // How much stock is held by the cart for this item
    int heldQty = ci.quantity;

    // Look up remaining stock in the store (which has already been decremented by heldQty)
    int availableInStore = 0;
    for (const auto& p : store.getProducts()) {
        if (p.name == ci.product.name && p.section == ci.product.section) {
            availableInStore = p.stock;
            break;
        }
    }

    int maxAllowed = heldQty + availableInStore;

    int newQty = getIntInput("New quantity (0 to remove item): ");

    if (newQty < 0) {
        cout << "Quantity cannot be negative.\n";
        return;
    }
    if (newQty > maxAllowed) {
        cout << "Cannot set quantity to " << newQty
             << ". Only " << maxAllowed << " available in total.\n";
        return;
    }

    // Adjust store stock: difference between old held and new held
    int stockDelta = heldQty - newQty; // positive → return stock; negative → take more

    // Apply the stock change to the store's master product via getProduct(index)
    for (int i = 0; i < static_cast<int>(store.getProducts().size()); ++i) {
        const Product& sp = store.getProducts()[i];
        if (sp.name == ci.product.name && sp.section == ci.product.section) {
            store.getProduct(i).stock += stockDelta; // stockDelta > 0 returns stock, < 0 takes more
            break;
        }
    }

    if (newQty == 0) {
        cout << "Item removed from cart.\n";
        cart.removeItem(idx);
    } else {
        cart.updateQuantity(idx, newQty);
        cout << "Quantity updated to " << newQty << ".\n";
    }
}

// ── Admin restock ─────────────────────────────────────────────────────────────

void adminRestock(Store& store) {
    // NOTE: Plain-text password comparison is for demonstration only.
    //       Replace with a hashed credential check before any production deployment.
    string password = getLineInput("Enter admin password: ");
    if (password != ADMIN_PASSWORD) {
        cout << "Incorrect password. Access denied.\n";
        return;
    }

    string productName = getLineInput("Enter product name to restock: ");
    int qty = getIntInput("Enter quantity to add: ");
    if (qty < 1) {
        cout << "Quantity must be at least 1.\n";
        return;
    }

    if (!store.restock(productName, qty)) {
        cout << "Product '" << productName << "' not found.\n";
    }
}

// ── Checkout ──────────────────────────────────────────────────────────────────

void checkout(Cart& cart, Wallet& wallet, Store& /*store*/, bool isStudent) {
    if (cart.isEmpty()) {
        cout << "\nYour cart is empty. Nothing to checkout.\n";
        return;
    }

    double sub      = cart.subtotal();
    double discount = isStudent ? sub * STUDENT_DISCOUNT : 0.0;
    double tax      = (sub - discount) * TAX_RATE;
    double total    = sub - discount + tax;

    cout << fixed << setprecision(2);
    cout << "\n--- Checkout Summary ---\n";
    cout << "Subtotal: " << sub      << " PHP\n";
    cout << "Discount: " << discount << " PHP\n";
    cout << "Tax:      " << tax      << " PHP\n";
    cout << "TOTAL:    " << total    << " PHP\n";

    cout << "\n1. Cash\n2. E-Wallet\n";
    int mode = getIntInput("Payment method (1 or 2): ");

    if (mode == 1) {
        // ── Cash payment ──────────────────────────────────────────────────────
        if (wallet.getCash() < total) {
            cout << "Insufficient cash balance (" << wallet.getCash() << " PHP).\n";
            return;
        }

        double paid = 0.0;
        while (true) {
            paid = static_cast<double>(getIntInput("Enter cash amount: "));
            if (paid >= total) break;
            cout << "Amount is less than total (" << total << " PHP). Try again.\n";
        }

        wallet.deductCash(total);

        cout << fixed << setprecision(2);
        cout << "Cash payment successful!\n";
        cout << "Change: " << paid - total << " PHP\n";
        cout << "Enjoy your Beta Meals Coupon!\n";

        // Bug fix: coupon is added ONLY on successful payment
        wallet.addCoupon(COUPON_CODE);

        printReceipt(cart, sub, discount, tax, total, paid);
        cart.clear();

    } else if (mode == 2) {
        // ── E-Wallet payment ──────────────────────────────────────────────────

        // Check if coupon applies BEFORE validation, so we can recompute total
        bool   couponApplied  = false;
        double couponDiscount = 0.0;

        if (wallet.hasCoupon(COUPON_CODE)) {
            double betaSub = 0.0;
            for (const auto& ci : cart.getItems()) {
                if (ci.product.section == "Beta Meals")
                    betaSub += ci.product.price * ci.quantity;
            }
            if (betaSub > 0.0) {
                couponDiscount = betaSub * COUPON_DISCOUNT;
                discount      += couponDiscount;
                tax            = (sub - discount) * TAX_RATE;
                total          = sub - discount + tax;
                couponApplied  = true;
                cout << fixed << setprecision(2);
                cout << "Coupon applied: " << COUPON_CODE
                     << " (-" << couponDiscount << " PHP)\n";
                cout << "Updated TOTAL: " << total << " PHP\n";
            } else {
                cout << "Coupon available but no Beta Meals items in cart. "
                        "Coupon not applied.\n";
            }
        }

        if (wallet.getEwallet() < total) {
            cout << "Insufficient e-wallet balance (" << wallet.getEwallet() << " PHP).\n";
            return;
        }

        // Validate e-wallet number (11 numeric digits)
        string number;
        while (true) {
            number = getLineInput("Enter " + std::to_string(EWALLET_NUM_LENGTH)
                                  + "-digit E-Wallet number: ");
            bool valid = (number.size() == static_cast<size_t>(EWALLET_NUM_LENGTH));
            if (valid) {
                for (char c : number) {
                    if (!isdigit(static_cast<unsigned char>(c))) { valid = false; break; }
                }
            }
            if (valid) break;
            cout << "Invalid. Must be exactly " << EWALLET_NUM_LENGTH
                 << " numeric digits.\n";
        }

        // Validate PIN (4 numeric digits)
        string pin;
        while (true) {
            pin = getLineInput("Enter " + std::to_string(EWALLET_PIN_LENGTH) + "-digit PIN: ");
            bool valid = (pin.size() == static_cast<size_t>(EWALLET_PIN_LENGTH));
            if (valid) {
                for (char c : pin) {
                    if (!isdigit(static_cast<unsigned char>(c))) { valid = false; break; }
                }
            }
            if (valid) break;
            cout << "Invalid. Must be exactly " << EWALLET_PIN_LENGTH
                 << " numeric digits.\n";
        }

        wallet.deductEwallet(total);

        if (couponApplied)
            wallet.useCoupon(COUPON_CODE);

        cout << "E-Payment confirmed! Thank you for your purchase.\n";
        cout << "Enjoy your Beta Meals Coupon!\n";

        // Bug fix: coupon is added ONLY on successful payment
        wallet.addCoupon(COUPON_CODE);

        printReceipt(cart, sub, discount, tax, total, 0.0);
        cart.clear();

    } else {
        cout << "Invalid payment method.\n";
    }
}

// ── Main ──────────────────────────────────────────────────────────────────────

int main() {
    // Ensure receipts directory exists at startup
    std::filesystem::create_directories("receipts");

    Store  store;
    Cart   cart;
    Wallet wallet;

    // Load product catalog from JSON; falls back to defaults if file not found
    store.loadProducts("data/products.json");

    cout << "\n===========================================\n";
    cout << "   Welcome to " << store.getName() << "!\n";
    cout << "===========================================\n";

    // Student ID check
    bool isStudent = false;
    char sc;
    cout << "Are you a student? (Y/N): ";
    cin >> sc;

    if (sc == 'Y' || sc == 'y') {
        string id = getLineInput("Enter " + std::to_string(STUDENT_ID_LENGTH)
                                 + "-digit Student ID: ");
        bool validId = (id.size() == static_cast<size_t>(STUDENT_ID_LENGTH));
        if (validId) {
            for (char c : id) {
                if (!isdigit(static_cast<unsigned char>(c))) { validId = false; break; }
            }
        }
        if (validId) {
            isStudent = true;
            cout << "Congratulations! 15% Student Discount will be applied at checkout.\n";
        } else {
            cout << "Invalid Student ID. No discount will be applied.\n";
        }
    }

    // ── Main menu loop ────────────────────────────────────────────────────────
    int rawChoice;
    do {
        cout << "\n=================== MAIN MENU ===================\n";
        cout << " SECTION  1. Basic Essentials\n";
        cout << " SECTION  2. Drinks\n";
        cout << " SECTION  3. Foods\n";
        cout << " SECTION  4. School Supplies\n";
        cout << " SECTION  5. Clothing\n";
        cout << " SECTION  6. Beta Meals\n";
        cout << "          7. Search Item\n";
        cout << "          8. Check Wallet\n";
        cout << "          9. View Cart\n";
        cout << "         10. Remove Item from Cart\n";
        cout << "         11. Update Item Quantity\n";
        cout << "         12. Checkout\n";
        cout << "         13. Admin — Restock Item\n";
        cout << "         14. Exit\n";
        cout << "=================================================\n";

        rawChoice = getIntInput("Choice: ");

        // Validate range before casting
        if (rawChoice < static_cast<int>(MenuChoice::BasicEssentials) ||
            rawChoice > static_cast<int>(MenuChoice::Exit)) {
            cout << "Invalid choice. Please enter 1–14.\n";
            continue;
        }

        MenuChoice choice = static_cast<MenuChoice>(rawChoice);

        switch (choice) {
            case MenuChoice::BasicEssentials:
                browseSection(store, cart, "Basic Essentials");
                break;
            case MenuChoice::Drinks:
                browseSection(store, cart, "Drinks");
                break;
            case MenuChoice::Foods:
                browseSection(store, cart, "Foods");
                break;
            case MenuChoice::SchoolSupplies:
                browseSection(store, cart, "School Supplies");
                break;
            case MenuChoice::Clothing:
                browseSection(store, cart, "Clothing");
                break;
            case MenuChoice::BetaMeals:
                browseSection(store, cart, "Beta Meals");
                break;
            case MenuChoice::Search: {
                string name = getLineInput("Enter item name to search: ");
                store.searchProduct(name);
                break;
            }
            case MenuChoice::Wallet:
                wallet.display();
                break;
            case MenuChoice::ViewCart:
                cart.display();
                break;
            case MenuChoice::RemoveItem:
                removeFromCart(cart, store);
                break;
            case MenuChoice::UpdateQuantity:
                updateCartQuantity(cart, store);
                break;
            case MenuChoice::Checkout:
                checkout(cart, wallet, store, isStudent);
                break;
            case MenuChoice::AdminRestock:
                adminRestock(store);
                break;
            case MenuChoice::Exit:
                break;
        }

    } while (static_cast<MenuChoice>(rawChoice) != MenuChoice::Exit);

    cout << "\nThank you for shopping at " << store.getName() << "! Come back soon!\n";
    return 0;
}
