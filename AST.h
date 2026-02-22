//
// Created by kucer on 21.02.2026.
//

#pragma once

#include <string>
#include <memory>
#include <vector>

namespace ast {

    struct Expression {
        virtual ~Expression() = default;
    };

    struct Integer : Expression {
        int64_t value;
        explicit Integer(int64_t v);
    };

    // identifier for variable/function
    struct Identifier : Expression {
        std::string name;
        explicit Identifier(std::string n);
    };

    struct StringLiteral : Expression {
        std::string value;
        explicit StringLiteral(std::string v);
    };

    struct Boolean : Expression {
        bool value;
        explicit Boolean(bool v);
    };

    struct BinaryOp : Expression {
        enum Op { Plus, Minus, Asterisk, Slash, Equal, NotEqual, Less, LessEqual, Greater, GreaterEqual, };
        Op op;
        std::unique_ptr<Expression> left;
        std::unique_ptr<Expression> right;

        BinaryOp(Op o, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r);
    };

    struct CallExpr : Expression {
        std::string callee;   // name of function
        std::vector<std::unique_ptr<Expression>> arguments;

        CallExpr(std::string c, std::vector<std::unique_ptr<Expression>> a);
    };

    struct Statement {
        virtual ~Statement() = default;
    };

    struct ReturnStatement : Statement {
        std::unique_ptr<Expression> value;
        explicit ReturnStatement(std::unique_ptr<Expression> v = nullptr);
    };

    struct ExpressionStatement : Statement {
        std::unique_ptr<Expression> expr;
        explicit ExpressionStatement(std::unique_ptr<Expression> e);
    };

    struct AssignStmt : Statement {
        std::string name;
        std::unique_ptr<Expression> value;
        AssignStmt(std::string n, std::unique_ptr<Expression> v);
    };

    struct IfStmt : Statement {
        std::unique_ptr<Expression> condition;
        std::vector<std::unique_ptr<Statement>> thenBranch;
        std::vector<std::unique_ptr<Statement>> elseBranch;

        IfStmt(std::unique_ptr<Expression> cond,
               std::vector<std::unique_ptr<Statement>> thenStmts,
               std::vector<std::unique_ptr<Statement>> elseStmts = {});
    };

    struct Function {
        std::string name;
        std::vector<std::string> params;
        std::vector<std::unique_ptr<Statement>> body;

        Function(std::string n, std::vector<std::string> p, std::vector<std::unique_ptr<Statement>> b);
    };

    struct Program {
        std::vector<std::unique_ptr<Function>> functions;
        std::vector<std::unique_ptr<Statement>> globalStatements;
    };

}