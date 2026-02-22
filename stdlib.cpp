//
// Created by kucer on 21.02.2026.
//

#include "stdlib.h"
#include <iostream>
#include <string>
#include <sstream>

void registerStdLib(Interpreter& interpreter) {
    interpreter.registerFunction("println", [](const std::vector<Interpreter::Value>& args) -> Interpreter::Value {
        for (size_t i = 0; i < args.size(); ++i) {
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
        std::cout << std::endl;
        return {}; // void
    });

    interpreter.registerFunction("print", [](const std::vector<Interpreter::Value>& args) -> Interpreter::Value {
        for (size_t i = 0; i < args.size(); ++i) {
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

    interpreter.registerFunction("input", [](const std::vector<Interpreter::Value>& args) -> Interpreter::Value {
    if (args.size() > 0) {
        if (args[0].type() == typeid(std::string)) {
            std::cout << std::any_cast<std::string>(args[0]);
        } else if (args[0].type() == typeid(int64_t)) {
            std::cout << std::any_cast<int64_t>(args[0]);
        } else {
            std::cout << "<unknown>";
        }
    }
    std::string line;
    std::getline(std::cin, line);
    return line;
});

    interpreter.registerFunction("to_int", [](const std::vector<Interpreter::Value>& args) -> Interpreter::Value {
        if (args.size() != 1) {
            throw std::runtime_error("to_int expects exactly one argument");
        }
        if (args[0].type() == typeid(std::string)) {
            std::string s = std::any_cast<std::string>(args[0]);
            try {
                size_t pos;
                long long val = std::stoll(s, &pos);
                if (pos != s.size()) {
                    throw std::runtime_error("to_int: invalid integer format");
                }
                return val;
            } catch (...) {
                throw std::runtime_error("to_int: conversion failed");
            }
        }
        if (args[0].type() == typeid(int64_t)) {
            return args[0];
        }
        throw std::runtime_error("toInt: argument must be a string or integer");
    });

    interpreter.registerFunction("to_string", [](const std::vector<Interpreter::Value>& args) -> Interpreter::Value {
        if (args.size() != 1) {
            throw std::runtime_error("to_string expects exactly one argument");
        }
        if (args[0].type() == typeid(int64_t)) {
            return std::to_string(std::any_cast<int64_t>(args[0]));
        }
        throw std::runtime_error("to_string: argument must be an integer");
    });

    interpreter.registerFunction("exit", [](const std::vector<Interpreter::Value>& args) -> Interpreter::Value {
        int code = 0;
        if (args.size() > 0 && args[0].type() == typeid(int64_t)) {
            code = static_cast<int>(std::any_cast<int64_t>(args[0]));
        }
        exit(code);
    });

    interpreter.registerFunction("max", [](const std::vector<Interpreter::Value>& args) -> Interpreter::Value {
        if (args.size() != 2) {
            throw std::runtime_error("max expects exactly 2 arguments");
        }

        int64_t a = std::any_cast<int64_t>(args[0]);
        int64_t b = std::any_cast<int64_t>(args[1]);

        return (a > b) ? a : b;
    });

    interpreter.registerFunction("random", [](const std::vector<Interpreter::Value>& args) -> Interpreter::Value {
        if (args.size() != 2) {
            throw std::runtime_error("random expects min and max");
        }
        int64_t min = std::any_cast<int64_t>(args[0]);
        int64_t max = std::any_cast<int64_t>(args[1]);

        return min + (rand() % (max - min + 1));
    });

    interpreter.registerFunction("len", [](const std::vector<Interpreter::Value>& args) -> Interpreter::Value {
        if (args.size() != 1) {
            throw std::runtime_error("len expects exactly 1 argument");
        }

        std::string s = std::any_cast<std::string>(args[0]);
        return static_cast<int64_t>(s.length());
    });
}