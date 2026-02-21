//
// Created by kucer on 21.02.2026.
//

#include "ast.h"

namespace ast {

    // Integer
    Integer::Integer(int64_t v) : value(v) {}

    // Identifier
    Identifier::Identifier(std::string n) : name(std::move(n)) {}

    // BinaryOp
    BinaryOp::BinaryOp(Op o, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
        : op(o), left(std::move(l)), right(std::move(r)) {}

    CallExpr::CallExpr(std::string c, std::vector<std::unique_ptr<Expression>> a)
    : callee(std::move(c)), arguments(std::move(a)) {}

    // ReturnStatement
    ReturnStatement::ReturnStatement(std::unique_ptr<Expression> v)
        : value(std::move(v)) {}

    // ExpressionStatement
    ExpressionStatement::ExpressionStatement(std::unique_ptr<Expression> e)
        : expr(std::move(e)) {}

    // Function
    Function::Function(std::string n, std::vector<std::unique_ptr<Statement>> b)
        : name(std::move(n)), body(std::move(b)) {}

} // namespace ast