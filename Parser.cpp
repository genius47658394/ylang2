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

bool Parser::lookaheadIsAssign() const {
    if (pos + 1 >= tokens.size()) return false;
    return std::holds_alternative<token::Assign>(tokens[pos + 1]);
}

void Parser::error(const std::string& message) const {
    throw std::runtime_error("Parse error at token " + std::to_string(pos) + ": " + message);
}

std::unique_ptr<ast::Program> Parser::parse() {
    auto program = std::make_unique<ast::Program>();

    while (!isAtEnd()) {
        if (check<token::Keyword>() && std::get<token::Keyword>(peek()) == token::Keyword::FN) {
            advance();
            program->functions.push_back(parseFunction());
        } else {
            auto stmt = parseStatement();
            program->globalStatements.push_back(std::move(stmt));
        }
    }

    return program;
}

std::unique_ptr<ast::Function> Parser::parseFunction() {
    auto nameToken = consume<token::Identifier>("Expected function name after 'fn'");
    std::string funcName = nameToken.value;

    consume<token::LPar>("Expected '(' after function name");

    std::vector<std::string> params;
    if (!check<token::RPar>()) {
        do {
            auto param = consume<token::Identifier>("Expected parameter name");
            params.push_back(param.value);
            if (check<token::Comma>()) {
                advance();
            }
        } while (!check<token::RPar>() && !isAtEnd());
    }
    consume<token::RPar>("Expected ')' after parameters");

    consume<token::LBracket>("Expected '{' before function body");
    std::vector<std::unique_ptr<ast::Statement>> body;
    while (!check<token::RBracket>() && !isAtEnd()) {
        body.push_back(parseStatement());
    }
    consume<token::RBracket>("Expected '}' after function body");

    return std::make_unique<ast::Function>(funcName, params, std::move(body));
}

std::unique_ptr<ast::Statement> Parser::parseStatement() {
    if (match(token::Keyword::RETURN)) {
        return parseReturnStatement();
    }
    if (match(token::Keyword::IF)) {
        return parseIfStmt();
    }
    if (check<token::Identifier>() && lookaheadIsAssign()) {
        return parseAssignStmt();
    }
    auto expr = parseExpression();
    consume<token::Semicolon>("Expected ';' after expression");
    return std::make_unique<ast::ExpressionStatement>(std::move(expr));
}

std::unique_ptr<ast::ReturnStatement> Parser::parseReturnStatement() {

    if (check<token::Semicolon>()) {
        consume<token::Semicolon>("Expected ';' after return");
        return std::make_unique<ast::ReturnStatement>(nullptr); //void return
    }
    auto expr = parseExpression();
    consume<token::Semicolon>("Expected ';' after return value");
    return std::make_unique<ast::ReturnStatement>(std::move(expr));
}

std::unique_ptr<ast::IfStmt> Parser::parseIfStmt() {
    consume<token::LPar>("Expected '(' after if");
    auto condition = parseExpression();
    consume<token::RPar>("Expected ')' after condition");
    consume<token::LBracket>("Expected '{' before if body");

    std::vector<std::unique_ptr<ast::Statement>> thenBranch;
    while (!check<token::RBracket>() && !isAtEnd()) {
        thenBranch.push_back(parseStatement());
    }
    consume<token::RBracket>("Expected '}' after if body");

    std::vector<std::unique_ptr<ast::Statement>> elseBranch;
    if (match(token::Keyword::ELSE)) {
        consume<token::LBracket>("Expected '{' before else body");
        while (!check<token::RBracket>() && !isAtEnd()) {
            elseBranch.push_back(parseStatement());
        }
        consume<token::RBracket>("Expected '}' after else body");
    }

    return std::make_unique<ast::IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<ast::Expression> Parser::parseExpression() {
    return parseComparison();
}

std::unique_ptr<ast::Expression> Parser::parseComparison() {
    auto left = parseAdditive();

    while (true) {
        if (check<token::Equal>()) {
            advance();
            auto right = parseAdditive();
            left = std::make_unique<ast::BinaryOp>(ast::BinaryOp::Equal, std::move(left), std::move(right));
        }
        else if (check<token::NotEqual>()) {
            advance();
            auto right = parseAdditive();
            left = std::make_unique<ast::BinaryOp>(ast::BinaryOp::NotEqual, std::move(left), std::move(right));
        }
        else if (check<token::Less>()) {
            advance();
            auto right = parseAdditive();
            left = std::make_unique<ast::BinaryOp>(ast::BinaryOp::Less, std::move(left), std::move(right));
        }
        else if (check<token::LessEqual>()) {
            advance();
            auto right = parseAdditive();
            left = std::make_unique<ast::BinaryOp>(ast::BinaryOp::LessEqual, std::move(left), std::move(right));
        }
        else if (check<token::Greater>()) {
            advance();
            auto right = parseAdditive();
            left = std::make_unique<ast::BinaryOp>(ast::BinaryOp::Greater, std::move(left), std::move(right));
        }
        else if (check<token::GreaterEqual>()) {
            advance();
            auto right = parseAdditive();
            left = std::make_unique<ast::BinaryOp>(ast::BinaryOp::GreaterEqual, std::move(left), std::move(right));
        }
        else {
            break;
        }
    }

    return left;
}

std::unique_ptr<ast::Expression> Parser::parseAdditive() {
    auto left = parseMultiplicative();

    while (true) {
        if (check<token::Plus>()) {
            advance();
            auto right = parseMultiplicative();
            left = std::make_unique<ast::BinaryOp>(ast::BinaryOp::Plus,
                                                    std::move(left),
                                                    std::move(right));
        } else if (check<token::Minus>()) {
            advance();
            auto right = parseMultiplicative();
            left = std::make_unique<ast::BinaryOp>(ast::BinaryOp::Minus,
                                                    std::move(left),
                                                    std::move(right));
        } else {
            break;
        }
    }
    return left;
}

std::unique_ptr<ast::Expression> Parser::parseMultiplicative() {
    auto left = parseFactor();

    while (true) {
        if (check<token::Asterisk>()) {
            advance();
            auto right = parseFactor();
            left = std::make_unique<ast::BinaryOp>(ast::BinaryOp::Asterisk,
                                                    std::move(left),
                                                    std::move(right));
        } else if (check<token::Slash>()) {
            advance();
            auto right = parseFactor();
            left = std::make_unique<ast::BinaryOp>(ast::BinaryOp::Slash,
                                                    std::move(left),
                                                    std::move(right));
        } else {
            break;
        }
    }
    return left;
}

std::unique_ptr<ast::Expression> Parser::parseFactor() {
    if (auto* intVal = std::get_if<token::Integer>(&peek())) {
        auto value = intVal->value;
        advance();
        return std::make_unique<ast::Integer>(value);
    }

    if (auto* strVal = std::get_if<token::String>(&peek())) {
        std::string val = strVal->value;
        advance();
        return std::make_unique<ast::StringLiteral>(val);
    }

    if (auto* id = std::get_if<token::Identifier>(&peek())) {
        std::string name = id->value;
        advance();
        if (check<token::LPar>()) {
            return parseCall(name);
        }
        return std::make_unique<ast::Identifier>(name);
    }

    if (check<token::LPar>()) {
        advance();
        auto expr = parseExpression();
        consume<token::RPar>("Expected ')' after expression");
        return expr;
    }

    error("Expected expression (integer, identifier, or '(')");
}

std::unique_ptr<ast::CallExpr> Parser::parseCall(const std::string& callee) {
    consume<token::LPar>("Expected '(' after function name");
    std::vector<std::unique_ptr<ast::Expression>> args;

    if (!check<token::RPar>()) {
        do {
            args.push_back(parseExpression());
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

std::unique_ptr<ast::AssignStmt> Parser::parseAssignStmt() {
    auto nameToken = consume<token::Identifier>("Expected variable name in assignment");
    consume<token::Assign>("Expected '=' in assignment");
    auto value = parseExpression();
    consume<token::Semicolon>("Expected ';' after assignment");
    return std::make_unique<ast::AssignStmt>(nameToken.value, std::move(value));
}