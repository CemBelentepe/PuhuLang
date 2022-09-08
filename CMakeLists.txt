cmake_minimum_required(VERSION 3.23)
project(PuhuLang)

set(CMAKE_CXX_STANDARD 17)

add_executable(PuhuLang src/main.cpp src/Scanner.cpp src/Scanner.h src/Token.cpp src/Token.h)
