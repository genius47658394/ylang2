//
// Created by kucer on 21.02.2026.
//

#include "stdlib.h"
#include <iostream>
#include <string>
#include <sstream>

void registerStdLib(Interpreter& interpreter) {
    // println: принимает любое количество аргументов, печатает их через пробел и переводит строку
    interpreter.registerFunction("println", [](const std::vector<Interpreter::Value>& args) -> Interpreter::Value {
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) std::cout << ' ';
            // Печатаем в зависимости от типа
            if (args[i].type() == typeid(int64_t)) {
                std::cout << std::any_cast<int64_t>(args[i]);
            } else if (args[i].type() == typeid(std::string)) {
                std::cout << std::any_cast<std::string>(args[i]);
            } else if (args[i].type() == typeid(const char*)) {
                // на случай строковых литералов
                std::cout << std::any_cast<const char*>(args[i]);
            } else {
                std::cout << "<unknown>";
            }
        }
        std::cout << std::endl;
        return {}; // void
    });

    // print: то же, но без перевода строки
    interpreter.registerFunction("print", [](const std::vector<Interpreter::Value>& args) -> Interpreter::Value {
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) std::cout << ' ';
            if (args[i].type() == typeid(int64_t)) {
                std::cout << std::any_cast<int64_t>(args[i]);
            } else if (args[i].type() == typeid(std::string)) {
                std::cout << std::any_cast<std::string>(args[i]);
            } else if (args[i].type() == typeid(const char*)) {
                std::cout << std::any_cast<const char*>(args[i]);
            } else {
                std::cout << "<unknown>";
            }
        }
        return {};
    });

    // input: читает строку из stdin
    interpreter.registerFunction("input", [](const std::vector<Interpreter::Value>&) -> Interpreter::Value {
        std::string line;
        std::getline(std::cin, line);
        return line;
    });

    // toInt: преобразует строку в целое число
    interpreter.registerFunction("toint", [](const std::vector<Interpreter::Value>& args) -> Interpreter::Value {
        if (args.size() != 1) {
            throw std::runtime_error("toInt expects exactly one argument");
        }
        if (args[0].type() == typeid(std::string)) {
            std::string s = std::any_cast<std::string>(args[0]);
            try {
                size_t pos;
                long long val = std::stoll(s, &pos);
                if (pos != s.size()) {
                    throw std::runtime_error("toInt: invalid integer format");
                }
                return val;
            } catch (...) {
                throw std::runtime_error("toInt: conversion failed");
            }
        } else if (args[0].type() == typeid(int64_t)) {
            return args[0]; // уже число
        } else {
            throw std::runtime_error("toInt: argument must be a string or integer");
        }
    });

    // toString: преобразует целое число в строку
    interpreter.registerFunction("tostring", [](const std::vector<Interpreter::Value>& args) -> Interpreter::Value {
        if (args.size() != 1) {
            throw std::runtime_error("toString expects exactly one argument");
        }
        if (args[0].type() == typeid(int64_t)) {
            return std::to_string(std::any_cast<int64_t>(args[0]));
        }
        throw std::runtime_error("toString: argument must be an integer");
    });

    // exit: завершает программу с кодом
    interpreter.registerFunction("exit", [](const std::vector<Interpreter::Value>& args) -> Interpreter::Value {
        int code = 0;
        if (args.size() > 0 && args[0].type() == typeid(int64_t)) {
            code = static_cast<int>(std::any_cast<int64_t>(args[0]));
        }
        exit(code);
        return {}; // never reached
    });
}