//
// Created by kucer on 21.02.2026.
//

#pragma once

#include "ast.h"
#include <vector>
#include <memory>
#include <stdexcept>
#include <string>
#include <variant>

#include "Token.h"

class Parser {
public:
    explicit Parser(const std::vector<token::Any>& tokens);

    Parser(const Parser&) = delete;
    Parser& operator=(const Parser&) = delete;

    // main function of parser
    std::unique_ptr<ast::Program> parse();

private:
    const std::vector<token::Any>& tokens;
    size_t pos;

    // help methods
    bool isAtEnd() const;
    const token::Any& peek() const;
    const token::Any& previous() const;
    void advance();

    template<typename T>
    bool check() const;

    template<typename T>
    bool check(const T& value) const;

    template<typename T>
    T consume(const std::string& errorMessage);

    bool match(token::Keyword keyword);

    bool lookaheadIsAssign() const;

    [[noreturn]] void error(const std::string& message) const;

    // methods for grammar
    std::unique_ptr<ast::Function> parseFunction();
    std::unique_ptr<ast::Statement> parseStatement();
    std::unique_ptr<ast::ReturnStatement> parseReturnStatement();
    std::unique_ptr<ast::Expression> parseExpression();
    std::unique_ptr<ast::Expression> parseAdditive();
    std::unique_ptr<ast::Expression> parseTerm();
    std::unique_ptr<ast::CallExpr> parseCall(const std::string& callee);
    std::unique_ptr<ast::AssignStmt> parseAssignStmt();
};

template<typename T>
bool Parser::check() const {
    if (isAtEnd()) return false;
    return std::holds_alternative<T>(peek());
}

template<typename T>
bool Parser::check(const T& value) const {
    if (isAtEnd()) return false;
    if (auto* v = std::get_if<T>(&peek())) {
        return *v == value;
    }
    return false;
}

template<typename T>
T Parser::consume(const std::string& errorMessage) {
    if (check<T>()) {
        T value = std::get<T>(peek());
        advance();
        return value;
    }
    error(errorMessage);
}