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
    // save functions
    for (auto& func : prog->functions) {
        UserFunction uf{};
        uf.func = func.get();
        globals[func->name] = uf;
    }

    for (auto& stmt : prog->globalStatements) {
        visit(stmt.get());
    }

    auto it = globals.find("main");
    if (it != globals.end() && it->second.type() == typeid(UserFunction)) {
        auto mainFunc = std::any_cast<UserFunction>(it->second);
        callUserFunction(mainFunc, {});
    }
}

void Interpreter::visit(ast::Statement* stmt) {
    if (auto* returnStmt = dynamic_cast<ast::ReturnStatement*>(stmt)) {
        visitReturn(returnStmt);
    } else if (auto* exprStmt = dynamic_cast<ast::ExpressionStatement*>(stmt)) {
        visitExpressionStmt(exprStmt);
    } else if (auto* assignStmt = dynamic_cast<ast::AssignStmt*>(stmt)) {
        visitAssign(assignStmt);
    } else if (auto* ifStmt = dynamic_cast<ast::IfStmt*>(stmt)) {
        visitIf(ifStmt);
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
    if (val.has_value()) {
        if (val.type() == typeid(int64_t)) {
            std::cout << std::any_cast<int64_t>(val) << std::endl;
        } else if (val.type() == typeid(std::string)) {
            std::cout << std::any_cast<std::string>(val) << std::endl;
        } else if (val.type() == typeid(bool)) {
            std::cout << (std::any_cast<bool>(val) ? "true" : "false") << std::endl;
        }
    }
}

Interpreter::Value Interpreter::visit(ast::Expression* expr) {
    if (auto* intNode = dynamic_cast<ast::Integer*>(expr)) {
        return visitInteger(intNode);
    }
    if (auto* strNode = dynamic_cast<ast::StringLiteral*>(expr)) {
        return visitString(strNode);
    }
    if (auto* boolNode = dynamic_cast<ast::Boolean*>(expr)) {
        return visitBoolean(boolNode);
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

    if (node->op >= ast::BinaryOp::Equal && node->op <= ast::BinaryOp::GreaterEqual) {
        if (left.type() != right.type()) {
            runtimeError("Cannot compare values of different types");
        }

        if (left.type() == typeid(int64_t)) {
            int64_t l = std::any_cast<int64_t>(left);
            int64_t r = std::any_cast<int64_t>(right);

            switch (node->op) {
                case ast::BinaryOp::Equal:        return Value{l == r};
                case ast::BinaryOp::NotEqual:     return Value{l != r};
                case ast::BinaryOp::Less:         return Value{l < r};
                case ast::BinaryOp::LessEqual:    return Value{l <= r};
                case ast::BinaryOp::Greater:      return Value{l > r};
                case ast::BinaryOp::GreaterEqual: return Value{l >= r};
                default: break;
            }
        }
        else if (left.type() == typeid(std::string)) {
            std::string l = std::any_cast<std::string>(left);
            std::string r = std::any_cast<std::string>(right);

            switch (node->op) {
                case ast::BinaryOp::Equal:        return Value{l == r};
                case ast::BinaryOp::NotEqual:     return Value{l != r};
                case ast::BinaryOp::Less:         return Value{l < r};
                case ast::BinaryOp::LessEqual:    return Value{l <= r};
                case ast::BinaryOp::Greater:      return Value{l > r};
                case ast::BinaryOp::GreaterEqual: return Value{l >= r};
                default: break;
            }
        }
        runtimeError("Comparison not supported for this type");
    }

    switch (node->op) {
        case ast::BinaryOp::Plus:
            if (left.type() == typeid(int64_t) && right.type() == typeid(int64_t))
                return Value{std::any_cast<int64_t>(left) + std::any_cast<int64_t>(right)};
            if (left.type() == typeid(std::string) && right.type() == typeid(std::string))
                return Value{std::any_cast<std::string>(left) + std::any_cast<std::string>(right)};
            runtimeError("'+' only works on numbers or strings");

        case ast::BinaryOp::Minus:
            if (left.type() == typeid(int64_t) && right.type() == typeid(int64_t))
                return Value{std::any_cast<int64_t>(left) - std::any_cast<int64_t>(right)};
            runtimeError("'-' only works on numbers");

        case ast::BinaryOp::Asterisk:
            if (left.type() == typeid(int64_t) && right.type() == typeid(int64_t))
                return Value{std::any_cast<int64_t>(left) * std::any_cast<int64_t>(right)};
            runtimeError("'*' only works on numbers");

        case ast::BinaryOp::Slash:
            if (left.type() == typeid(int64_t) && right.type() == typeid(int64_t)) {
                int64_t r = std::any_cast<int64_t>(right);
                if (r == 0) runtimeError("Division by zero");
                return Value{std::any_cast<int64_t>(left) / r};
            }
            runtimeError("'/' only works on numbers");

        default:
            runtimeError("Unknown binary operator");
    }
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

    std::vector<Value> args;
    for (auto& arg : node->arguments) {
        args.push_back(visit(arg.get()));
    }

    if (it->second.type() == typeid(NativeFunction)) {
        auto func = std::any_cast<NativeFunction>(it->second);
        return func(args);
    }
    if (it->second.type() == typeid(UserFunction)) {
        auto uf = std::any_cast<UserFunction>(it->second);
        return callUserFunction(uf, args);
    }
    runtimeError("Not a callable: " + node->callee);
}

Interpreter::Value Interpreter::visitBoolean(ast::Boolean* node) {
    return Value{node->value};
}

void Interpreter::visitIf(ast::IfStmt* node) {
    Value condVal = visit(node->condition.get());

    bool condition = false;
    if (condVal.type() == typeid(bool)) {
        condition = std::any_cast<bool>(condVal);
    } else if (condVal.type() == typeid(int64_t)) {
        condition = (std::any_cast<int64_t>(condVal) != 0);
    } else if (condVal.type() == typeid(std::string)) {
        condition = !std::any_cast<std::string>(condVal).empty();
    } else {
        runtimeError("Condition must evaluate to a boolean or convertible type");
    }

    if (condition) {
        for (auto& stmt : node->thenBranch) {
            visit(stmt.get());
        }
    } else {
        for (auto& stmt : node->elseBranch) {
            visit(stmt.get());
        }
    }
}

Interpreter::Value Interpreter::callUserFunction(UserFunction& uf, const std::vector<Value>& args) {
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
        result = Value{};
    } catch (ReturnException& e) {
        result = e.value;
    }

    envStack.pop();
    return result;
}

Interpreter::Value Interpreter::getVariable(const std::string& name) {
    // find in stack
    if (!envStack.empty()) {
        auto& env = envStack.top();
        auto it = env.find(name);
        if (it != env.end()) {
            return it->second;
        }
    }
    // find in globals
    auto it = globals.find(name);
    if (it == globals.end()) {
        runtimeError("Undefined variable: " + name);
    }
    return it->second;
}

void Interpreter::setVariable(const std::string& name, Value value) {
    if (envStack.empty()) {
        // global
        globals[name] = value;
    } else {
        // local
        envStack.top()[name] = value;
    }
}

void Interpreter::runtimeError(const std::string& msg) {
    throw std::runtime_error("Runtime error: " + msg);
}