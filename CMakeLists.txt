cmake_minimum_required(VERSION 3.23)
project(PuhuLang)

set(CMAKE_CXX_STANDARD 17)

add_executable(PuhuLang src/Main.cpp src/Scanner.cpp src/Scanner.h src/Token.cpp src/Token.h src/Parser.cpp src/Parser.h src/Stmt.h src/Expr.h src/AstVisitor.h src/Stmt.cpp src/Expr.cpp src/Type.cpp src/Type.h)
