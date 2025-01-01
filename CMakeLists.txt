cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_STANDARD 20)
project(code)

add_executable(code
        cpp/main.cpp
        cpp/user.cpp
        cpp/book.cpp
        cpp/diary.cpp
        cpp/token.cpp
        cpp/finance.cpp
)

target_include_directories(
        code
        PUBLIC
        src/
        cpp/
)
