//
// Created by kucer on 21.02.2026.
//

#include "ast.h"

namespace ast {

    // Integer
    Integer::Integer(int64_t v) : value(v) {}

    // String
    StringLiteral::StringLiteral(std::string v) : value(std::move(v)) {}

    // Boolean
    Boolean::Boolean(bool v) : value(v) {}

    // Identifier
    Identifier::Identifier(std::string n) : name(std::move(n)) {}

    // BinaryOp
    BinaryOp::BinaryOp(Op o, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
        : op(o), left(std::move(l)), right(std::move(r)) {}

    // CallExpr
    CallExpr::CallExpr(std::string c, std::vector<std::unique_ptr<Expression>> a)
    : callee(std::move(c)), arguments(std::move(a)) {}

    // ReturnStatement
    ReturnStatement::ReturnStatement(std::unique_ptr<Expression> v)
        : value(std::move(v)) {}

    // ExpressionStatement
    ExpressionStatement::ExpressionStatement(std::unique_ptr<Expression> e)
        : expr(std::move(e)) {}

    IfStmt::IfStmt(std::unique_ptr<Expression> cond,
                   std::vector<std::unique_ptr<Statement>> thenStmts,
                   std::vector<std::unique_ptr<Statement>> elseStmts)
        : condition(std::move(cond)), thenBranch(std::move(thenStmts)), elseBranch(std::move(elseStmts)) {}

    // AssignStatement
    AssignStmt::AssignStmt(std::string n, std::unique_ptr<Expression> v)
    : name(std::move(n)), value(std::move(v)) {}

    // Function
    Function::Function(std::string n, std::vector<std::string> p, std::vector<std::unique_ptr<Statement>> b)
        : name(std::move(n)), params(std::move(p)), body(std::move(b)) {}

} // namespace ast