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
    // save all functions as user functions
    for (auto& func : prog->functions) {
        UserFunction uf;
        uf.func = func.get();
        globals[func->name] = uf;
    }

    // Поиск и вызов main
    auto it = globals.find("main");
    if (it == globals.end()) {
        runtimeError("No 'main' function found");
    }
    if (it->second.type() != typeid(UserFunction)) {
        runtimeError("main is not a function");
    }
    UserFunction mainFunc = std::any_cast<UserFunction>(it->second);
    callUserFunction(mainFunc, {});   // вызов без аргументов
}

void Interpreter::visit(ast::Statement* stmt) {
    if (auto* returnStmt = dynamic_cast<ast::ReturnStatement*>(stmt)) {
        visitReturn(returnStmt);
    } else if (auto* exprStmt = dynamic_cast<ast::ExpressionStatement*>(stmt)) {
        visitExpressionStmt(exprStmt);
    } else if (auto* assignStmt = dynamic_cast<ast::AssignStmt*>(stmt)) {  // новый
        visitAssign(assignStmt);
    } else {
        runtimeError("Unknown statement type");
    }
}

void Interpreter::visitReturn(ast::ReturnStatement* node) {
    Value val;
    if (node->value) {
        val = visit(node->value.get());
    }
    throw ReturnException(val);
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
    }
    if (auto* strNode = dynamic_cast<ast::StringLiteral*>(expr)) {  // новый
        return visitString(strNode);
    }
    if (auto* idNode = dynamic_cast<ast::Identifier*>(expr)) {
        return visitIdentifier(idNode);
    }
    if (auto* binOpNode = dynamic_cast<ast::BinaryOp*>(expr)) {
        return visitBinaryOp(binOpNode);
    }
    if (auto* callNode = dynamic_cast<ast::CallExpr*>(expr)) {
        return visitCall(callNode);
    }
    runtimeError("Unknown expression type");
    return {};
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

    if (node->op != ast::BinaryOp::Plus) {
        runtimeError("Unknown binary operator");
    }

    // Строка + строка
    if (left.type() == typeid(std::string) && right.type() == typeid(std::string)) {
        return Value{std::any_cast<std::string>(left) + std::any_cast<std::string>(right)};
    }
    // Число + число
    if (left.type() == typeid(int64_t) && right.type() == typeid(int64_t)) {
        return Value{std::any_cast<int64_t>(left) + std::any_cast<int64_t>(right)};
    }

    runtimeError("Binary '+' only supported for (int, int) or (string, string)");
    return {};
}

Interpreter::Value Interpreter::visitString(ast::StringLiteral* node) {
    return Value{node->value};
}

void Interpreter::visitAssign(ast::AssignStmt* node) {
    Value val = visit(node->value.get());
    setVariable(node->name, val);
}

Interpreter::Value Interpreter::visitCall(ast::CallExpr* node) {
    auto it = globals.find(node->callee);
    if (it == globals.end()) {
        runtimeError("Undefined function: " + node->callee);
    }

    // Вычисляем аргументы
    std::vector<Value> args;
    for (auto& arg : node->arguments) {
        args.push_back(visit(arg.get()));
    }

    if (it->second.type() == typeid(NativeFunction)) {
        NativeFunction func = std::any_cast<NativeFunction>(it->second);
        return func(args);
    }
    if (it->second.type() == typeid(UserFunction)) {
        UserFunction uf = std::any_cast<UserFunction>(it->second);
        return callUserFunction(uf, args);
    }
    runtimeError("Not a callable: " + node->callee);
    return {};
}

Interpreter::Value Interpreter::callUserFunction(UserFunction& uf, const std::vector<Value>& args) {
    // Создаём новое локальное окружение
    Environment newEnv;

    if (args.size() != uf.func->params.size()) {
        runtimeError("Argument count mismatch for function " + uf.func->name);
    }

    for (size_t i = 0; i < args.size(); ++i) {
        newEnv[uf.func->params[i]] = args[i];
    }

    envStack.push(std::move(newEnv));

    Value result;
    try {
        for (auto& stmt : uf.func->body) {
            visit(stmt.get());
        }
        // если функция завершилась без return, возвращаем "пустое" значение
        result = Value{};
    } catch (ReturnException& e) {
        result = e.value;
    }

    envStack.pop();
    return result;
}

Interpreter::Value Interpreter::getVariable(const std::string& name) {
    // ищем в стеке локальных окружений (от вершины к основанию)
    if (!envStack.empty()) {
        auto& env = envStack.top();
        auto it = env.find(name);
        if (it != env.end()) {
            return it->second;
        }
    }
    // не нашли локально – ищем в глобальных
    auto it = globals.find(name);
    if (it == globals.end()) {
        runtimeError("Undefined variable: " + name);
    }
    return it->second;
}

void Interpreter::setVariable(const std::string& name, Value value) {
    if (envStack.empty()) {
        // глобальный уровень
        globals[name] = value;
    } else {
        // локальный уровень – кладём в текущее окружение
        envStack.top()[name] = value;
    }
}

void Interpreter::runtimeError(const std::string& msg) {
    throw std::runtime_error("Runtime error: " + msg);
}