//
// Created by kucer on 21.02.2026.
//

#pragma once

#include "ast.h"
#include <any>
#include <unordered_map>
#include <functional>
#include <vector>
#include <stdexcept>

class Interpreter {
public:
    using Value = std::any;  // пока так, позже можно заменить на variant
    using NativeFunction = std::function<Value(const std::vector<Value>&)>;

    explicit Interpreter(ast::Program* program);
    void interpret();

    // Регистрация встроенной функции
    void registerFunction(const std::string& name, NativeFunction func);

private:
    ast::Program* program;
    std::unordered_map<std::string, Value> globals;  // может хранить как данные, так и функции

    // Вспомогательные методы
    Value visit(ast::Expression* expr);
    void visit(ast::Statement* stmt);
    void visit(ast::Function* func);      // если понадобится
    void visit(ast::Program* prog);

    // Конкретные узлы
    Value visitInteger(ast::Integer* node);
    Value visitIdentifier(ast::Identifier* node);
    Value visitBinaryOp(ast::BinaryOp* node);
    Value visitCall(ast::CallExpr* node);  // новый
    void visitReturn(ast::ReturnStatement* node);
    void visitExpressionStmt(ast::ExpressionStatement* node);

    // Управление переменными
    Value getVariable(const std::string& name);
    void setVariable(const std::string& name, Value value);

    // Ошибки
    [[noreturn]] void runtimeError(const std::string& msg);
};
