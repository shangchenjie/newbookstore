cmake_minimum_required(VERSION 3.0)

project(bookstore)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src)

add_executable(bookstore
        cpp/book.cpp
        cpp/diary.cpp
        cpp/finance.cpp
        cpp/main.cpp
        cpp/token.cpp
        cpp/user.cpp
)
target_include_directories(
        code
        PUBLIC
        src/
        cpp/
)
