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
#include <stack>

struct UserFunction {
    ast::Function* func;
    // in future can add nested function
};

class Interpreter {
public:
    using Value = std::any;  // in future can be std::variant
    using NativeFunction = std::function<Value(const std::vector<Value>&)>;

    explicit Interpreter(ast::Program* program);
    void interpret();

    // register of standart function
    void registerFunction(const std::string& name, NativeFunction func);

private:
    ast::Program* program;
    std::unordered_map<std::string, Value> globals;  // variables or functions
    using Environment = std::unordered_map<std::string, Value>;
    std::stack<Environment> envStack;   // stack of local environments

    // help methods
    Value visit(ast::Expression* expr);
    void visit(ast::Statement* stmt);
    void visit(ast::Function* func);      // if need
    void visit(ast::Program* prog);

    // nodes
    Value visitInteger(ast::Integer* node);
    Value visitIdentifier(ast::Identifier* node);
    Value visitBinaryOp(ast::BinaryOp* node);
    Value visitCall(ast::CallExpr* node);  // new
    void visitReturn(ast::ReturnStatement* node);
    void visitExpressionStmt(ast::ExpressionStatement* node);
    Value visitString(ast::StringLiteral* node);
    void visitAssign(ast::AssignStmt* node);
    Value visitBoolean(ast::Boolean* node);
    void visitIf(ast::IfStmt* node);

    Value callUserFunction(UserFunction& uf, const std::vector<Value>& args);

    // variable controll
    Value getVariable(const std::string& name);
    void setVariable(const std::string& name, Value value);

    // Errors
    [[noreturn]] void runtimeError(const std::string& msg);

    struct ReturnException : std::exception {
        Value value;
        ReturnException(Value v) : value(v) {}
    };
};
