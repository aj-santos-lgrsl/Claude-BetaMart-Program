# 500 Beta Mart Shop

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)

## Description

**500 Beta Mart Shop** is a C++ console-based department store simulation developed by **Group 2 of 1CSIT Inc.** at **Colegio de San Juan de Letran Calamba**. The application simulates a convenience store checkout experience, allowing users to browse six product sections, add items to a cart, apply student discounts, pay via cash or e-wallet, and receive printed receipts. It demonstrates core C++ concepts including object-oriented programming, file I/O, input validation, JSON parsing without third-party libraries, and structured project organization with CMake.

## Features

- 🛒 **Six product sections**: Basic Essentials, Drinks, Foods, School Supplies, Clothing, and Beta Meals
- 🎓 **Student discount**: 15% off for verified student ID holders (7-digit ID required)
- 💵 **Cash payment** with change calculation
- 📱 **E-Wallet payment** with 11-digit number and 4-digit PIN validation
- 🎟️ **Beta Meals 10% coupon system** — earned on purchase, applied to Beta Meals items on next e-wallet transaction
- 🔍 **Product search** by name (case-insensitive)
- 🗑️ **Remove items** from cart with automatic stock restoration
- ✏️ **Update cart item quantity** with stock validation
- 🔐 **Admin restock mode** with password protection
- 🧾 **Timestamped receipts** saved to `receipts/receipts.txt`
- 📦 **JSON product catalog** loaded from `data/products.json` at startup
- ✅ **Robust input validation** — no infinite loops on bad input

## Getting Started

### Prerequisites

- A C++17-compatible compiler (GCC 8+, Clang 7+, MSVC 2017+)
- CMake 3.16 or later

### Clone

```bash
git clone https://github.com/your-org/500-beta-mart.git
cd 500-beta-mart
```

### Build — Linux / macOS

```bash
mkdir build && cd build
cmake ..
make
./BetaMartShop
```

### Build — Windows (MSVC / MinGW)

```bash
mkdir build && cd build
cmake .. -G "MinGW Makefiles"   # or "Visual Studio 17 2022"
cmake --build .
BetaMartShop.exe
```

### Single-command build (no CMake)

```bash
g++ -std=c++17 -Iinclude src/*.cpp -o BetaMartShop
./BetaMartShop
```

> **Note:** On some Linux distros you may need to link `stdc++fs` explicitly:
> `g++ -std=c++17 -Iinclude src/*.cpp -o BetaMartShop -lstdc++fs`

## Usage

1. **Launch** the program. You will be asked whether you are a student.
2. **Enter your 7-digit Student ID** to unlock the 15% discount (optional).
3. **Navigate the main menu** — choose a section number (1–6) to browse products, or use the other options:
   - **7** — Search for an item by name
   - **8** — View wallet balances and coupons
   - **9** — View your cart
   - **10** — Remove an item from the cart
   - **11** — Update the quantity of a cart item
   - **12** — Checkout (cash or e-wallet)
   - **13** — Admin restock mode *(password: see source)*
   - **14** — Exit
4. **Checkout** — choose Cash or E-Wallet. A receipt is printed to the screen and appended to `receipts/receipts.txt`.

## Project Structure

```
500-beta-mart/
├── CMakeLists.txt
├── README.md
├── LICENSE
├── .gitignore
├── data/
│   └── products.json         ← product catalog (loaded at startup)
├── include/
│   ├── Product.h             ← Product and CartItem structs + constants
│   ├── Cart.h                ← Cart class declaration
│   ├── Wallet.h              ← Wallet class declaration
│   └── Store.h               ← Store class declaration
├── src/
│   ├── Cart.cpp              ← Cart implementation
│   ├── Wallet.cpp            ← Wallet implementation
│   ├── Store.cpp             ← Store implementation + JSON loader
│   └── main.cpp              ← Entry point, menu loop, receipt printing
└── receipts/
    └── receipts.txt          ← Appended on each successful checkout
```

## Known Limitations / Future Ideas

- **No persistent user accounts** — wallet balance and coupons reset on each run
- **Plain-text admin password** — `admin123` is hardcoded; replace with hashed credentials for any real deployment
- **Manual JSON parser** — the built-in loader handles the schema defined in `data/products.json`; deeply nested or escaped strings will not parse correctly
- **GUI version** — a Qt or SFML front-end would improve usability
- **Barcode scanning** — integrate a USB HID barcode reader for faster item lookup
- **Database backend** — replace the JSON flat file with SQLite for persistent inventory and sales history
- **Network sync** — multi-terminal inventory synchronization over LAN

## License

MIT License © 2026 Group 2 / 1CSIT Inc., Colegio de San Juan de Letran Calamba

See [LICENSE](LICENSE) for full text.
