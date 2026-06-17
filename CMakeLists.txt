cmake_minimum_required(VERSION 3.16)
project(BetaMartShop VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(BetaMartShop
    src/main.cpp
    src/Cart.cpp
    src/Wallet.cpp
    src/Store.cpp
)

# Copy data folder to build directory
file(COPY data DESTINATION ${CMAKE_BINARY_DIR})

# Ensure receipts directory exists in build directory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/receipts)
