//
// Created by kucer on 21.02.2026.
//

#pragma once
#include <string_view>
#include <vector>

#include "Token.h"

class Lexer {
private:
    std::string_view code;
public:
    explicit Lexer(std::string_view code) : code(code) {}
    std::vector<token::Any> tokenize();
};
