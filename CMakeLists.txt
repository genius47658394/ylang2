cmake_minimum_required(VERSION 4.0)
project(ylang2)

set(CMAKE_CXX_STANDARD 20)

add_executable(ylang2 main.cpp
        Token.h
        Lexer.cpp
        Lexer.h
        AST.h
        Parser.cpp
        Parser.h
        AST.cpp
        Interpreter.cpp
        Interpreter.h
        stdlib.cpp
        stdlib.h)
