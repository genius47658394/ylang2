//
// Created by kucer on 21.02.2026.
//
#pragma once

#include <string>
#include <memory>
#include <vector>

namespace ast {

    // Базовый класс для выражений
    struct Expression {
        virtual ~Expression() = default;
    };

    // Целочисленный литерал
    struct Integer : Expression {
        int64_t value;
        explicit Integer(int64_t v);
    };

    // Идентификатор (имя переменной/функции)
    struct Identifier : Expression {
        std::string name;
        explicit Identifier(std::string n);
    };

    // Бинарная операция
    struct BinaryOp : Expression {
        enum Op { Plus };
        Op op;
        std::unique_ptr<Expression> left;
        std::unique_ptr<Expression> right;

        BinaryOp(Op o, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r);
    };

    struct CallExpr : Expression {
        std::string callee;   // имя функции
        std::vector<std::unique_ptr<Expression>> arguments;

        CallExpr(std::string c, std::vector<std::unique_ptr<Expression>> a);
    };

    // Базовый класс для инструкций
    struct Statement {
        virtual ~Statement() = default;
    };

    // Инструкция return
    struct ReturnStatement : Statement {
        std::unique_ptr<Expression> value;  // может быть nullptr для "return;"
        explicit ReturnStatement(std::unique_ptr<Expression> v = nullptr);
    };

    // Инструкция-выражение (expression;)
    struct ExpressionStatement : Statement {
        std::unique_ptr<Expression> expr;
        explicit ExpressionStatement(std::unique_ptr<Expression> e);
    };

    // Определение функции
    struct Function {
        std::string name;
        std::vector<std::unique_ptr<Statement>> body;

        Function(std::string n, std::vector<std::unique_ptr<Statement>> b);
    };

    // Программа (корневой узел)
    struct Program {
        std::vector<std::unique_ptr<Function>> functions;
    };

}