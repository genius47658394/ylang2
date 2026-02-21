//
// Created by kucer on 21.02.2026.
//

#include "interpreter.h"
#include <iostream>
#include <cassert>

Interpreter::Interpreter(ast::Program* prog) : program(prog) {}

void Interpreter::registerFunction(const std::string& name, NativeFunction func) {
    globals[name] = Value(func);
}

void Interpreter::interpret() {
    visit(program);
}

void Interpreter::visit(ast::Program* prog) {
    // Сохраняем пользовательские функции в глобальной таблице
    for (auto& func : prog->functions) {
        globals[func->name] = func.get();  // сырой указатель, будьте осторожны!
    }
    // Ищем функцию main и вызываем её
    if (globals.find("main") != globals.end()) {
        auto* mainFunc = std::any_cast<ast::Function*>(globals["main"]);
        for (auto& stmt : mainFunc->body) {
            visit(stmt.get());
        }
    } else {
        runtimeError("No 'main' function found");
    }
}

void Interpreter::visit(ast::Statement* stmt) {
    if (auto* returnStmt = dynamic_cast<ast::ReturnStatement*>(stmt)) {
        visitReturn(returnStmt);
    } else if (auto* exprStmt = dynamic_cast<ast::ExpressionStatement*>(stmt)) {
        visitExpressionStmt(exprStmt);
    } else {
        runtimeError("Unknown statement type");
    }
}

void Interpreter::visitReturn(ast::ReturnStatement* node) {
    if (node->value) {
        Value val = visit(node->value.get());
        // Пока просто печатаем, позже нужно будет организовать возврат из функции
        std::cout << "Return value: " << std::any_cast<int64_t>(val) << std::endl;
    } else {
        std::cout << "Return (void)" << std::endl;
    }
}

void Interpreter::visitExpressionStmt(ast::ExpressionStatement* node) {
    Value val = visit(node->expr.get());
    // Можно игнорировать или выводить для отладки
    // Например, если это не void, можно напечатать
    if (val.has_value()) {
        if (val.type() == typeid(int64_t)) {
            //std::cout << std::any_cast<int64_t>(val) << std::endl;
        }
    }
}

Interpreter::Value Interpreter::visit(ast::Expression* expr) {
    if (auto* intNode = dynamic_cast<ast::Integer*>(expr)) {
        return visitInteger(intNode);
    } else if (auto* idNode = dynamic_cast<ast::Identifier*>(expr)) {
        return visitIdentifier(idNode);
    } else if (auto* binOpNode = dynamic_cast<ast::BinaryOp*>(expr)) {
        return visitBinaryOp(binOpNode);
    } else if (auto* callNode = dynamic_cast<ast::CallExpr*>(expr)) {
        return visitCall(callNode);
    } else {
        runtimeError("Unknown expression type");
        return {};
    }
}

Interpreter::Value Interpreter::visitInteger(ast::Integer* node) {
    return Value{node->value};
}

Interpreter::Value Interpreter::visitIdentifier(ast::Identifier* node) {
    return getVariable(node->name);
}

Interpreter::Value Interpreter::visitBinaryOp(ast::BinaryOp* node) {
    Value left = visit(node->left.get());
    Value right = visit(node->right.get());

    if (left.type() != typeid(int64_t) || right.type() != typeid(int64_t)) {
        runtimeError("Binary operation only supported for integers");
    }

    int64_t l = std::any_cast<int64_t>(left);
    int64_t r = std::any_cast<int64_t>(right);

    switch (node->op) {
        case ast::BinaryOp::Plus:
            return Value{l + r};
        default:
            runtimeError("Unknown binary operator");
            return {};
    }
}

Interpreter::Value Interpreter::visitCall(ast::CallExpr* node) {
    auto it = globals.find(node->callee);
    if (it == globals.end()) {
        runtimeError("Undefined function: " + node->callee);
    }

    // Проверяем, является ли значение нативной функцией
    if (it->second.type() == typeid(NativeFunction)) {
        NativeFunction func = std::any_cast<NativeFunction>(it->second);
        std::vector<Value> args;
        for (auto& arg : node->arguments) {
            args.push_back(visit(arg.get()));
        }
        return func(args);
    }
    // Если это пользовательская функция (указатель на ast::Function)
    if (it->second.type() == typeid(ast::Function*)) {
        // Здесь нужно реализовать вызов пользовательской функции
        // Пока просто заглушка
        runtimeError("User function calls not implemented yet");
        return {};
    }
    runtimeError("Not a callable: " + node->callee);
    return {};
}

Interpreter::Value Interpreter::getVariable(const std::string& name) {
    auto it = globals.find(name);
    if (it == globals.end()) {
        runtimeError("Undefined variable: " + name);
    }
    return it->second;
}

void Interpreter::setVariable(const std::string& name, Value value) {
    globals[name] = value;
}

void Interpreter::runtimeError(const std::string& msg) {
    throw std::runtime_error("Runtime error: " + msg);
}