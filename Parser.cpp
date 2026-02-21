//
// Created by kucer on 21.02.2026.
//

#include "Parser.h"

Parser::Parser(const std::vector<token::Any>& tokens)
    : tokens(tokens), pos(0) {}

bool Parser::isAtEnd() const {
    return pos >= tokens.size();
}

const token::Any& Parser::peek() const {
    if (isAtEnd()) throw std::runtime_error("Unexpected end of input");
    return tokens[pos];
}

const token::Any& Parser::previous() const {
    if (pos == 0) throw std::runtime_error("No previous token");
    return tokens[pos - 1];
}

void Parser::advance() {
    if (!isAtEnd()) ++pos;
}

bool Parser::match(token::Keyword keyword) {
    if (check(keyword)) {
        advance();
        return true;
    }
    return false;
}

void Parser::error(const std::string& message) const {
    throw std::runtime_error("Parse error at token " + std::to_string(pos) + ": " + message);
}

std::unique_ptr<ast::Program> Parser::parse() {
    auto program = std::make_unique<ast::Program>();

    while (!isAtEnd()) {
        if (match(token::Keyword::FUNC)) {
            program->functions.push_back(parseFunction());
        } else {
            error("Expected function definition");
        }
    }

    return program;
}

std::unique_ptr<ast::Function> Parser::parseFunction() {
    // После match(token::Keyword::FUNC) ключевое слово уже съедено

    auto nameToken = consume<token::Identifier>("Expected function name after 'func'");
    std::string funcName = nameToken.value;

    consume<token::LPar>("Expected '(' after function name");
    consume<token::RPar>("Expected ')' after function parameters");
    consume<token::LBracket>("Expected '{' before function body");

    std::vector<std::unique_ptr<ast::Statement>> body;

    while (!check<token::RBracket>() && !isAtEnd()) {
        body.push_back(parseStatement());
    }

    consume<token::RBracket>("Expected '}' after function body");

    return std::make_unique<ast::Function>(funcName, std::move(body));
}

std::unique_ptr<ast::Statement> Parser::parseStatement() {
    if (match(token::Keyword::RETURN)) {
        return parseReturnStatement();
    }

    // Иначе — expression statement
    auto expr = parseExpression();
    consume<token::Semicolon>("Expected ';' after expression");
    return std::make_unique<ast::ExpressionStatement>(std::move(expr));
}

std::unique_ptr<ast::ReturnStatement> Parser::parseReturnStatement() {
    // Ключевое слово 'return' уже съедено в parseStatement()

    if (check<token::Semicolon>()) {
        // return;
        consume<token::Semicolon>("Expected ';' after return");
        return std::make_unique<ast::ReturnStatement>(nullptr);
    }
    auto expr = parseExpression();
    consume<token::Semicolon>("Expected ';' after return value");
    return std::make_unique<ast::ReturnStatement>(std::move(expr));
}

std::unique_ptr<ast::Expression> Parser::parseExpression() {
    return parseAdditive();
}

std::unique_ptr<ast::Expression> Parser::parseAdditive() {
    auto left = parseTerm();

    while (check<token::Plus>()) {
        advance(); // съедаем '+'
        auto right = parseTerm();
        left = std::make_unique<ast::BinaryOp>(
            ast::BinaryOp::Plus,
            std::move(left),
            std::move(right)
        );
    }

    return left;
}

std::unique_ptr<ast::Expression> Parser::parseTerm() {
    if (auto* intVal = std::get_if<token::Integer>(&peek())) {
        auto value = intVal->value;
        advance();
        return std::make_unique<ast::Integer>(value);
    }

    if (auto* id = std::get_if<token::Identifier>(&peek())) {
        std::string name = id->value;
        advance();
        // Если следующий токен '(', то это вызов функции
        if (check<token::LPar>()) {
            return parseCall(name);
        } else {
            return std::make_unique<ast::Identifier>(name);
        }
    }

    if (check<token::LPar>()) {
        advance(); // '('
        auto expr = parseExpression();
        consume<token::RPar>("Expected ')' after expression");
        return expr;
    }

    error("Expected expression (integer, identifier, or '(')");
    return nullptr;
}

std::unique_ptr<ast::CallExpr> Parser::parseCall(const std::string& callee) {
    consume<token::LPar>("Expected '(' after function name");
    std::vector<std::unique_ptr<ast::Expression>> args;

    if (!check<token::RPar>()) {
        do {
            args.push_back(parseExpression());
            // Если есть запятая, продолжаем
            if (check<token::Comma>()) {
                advance();
            } else {
                break;
            }
        } while (true);
    }
    consume<token::RPar>("Expected ')' after arguments");
    return std::make_unique<ast::CallExpr>(callee, std::move(args));
}