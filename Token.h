//
// Created by kucer on 21.02.2026.
//

#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <iostream>

namespace token {
    enum class Keyword {
        FN,
        RETURN,
        IF,
        ELSE,
    };

    struct Identifier {
        std::string value;
        auto operator<=>(const Identifier &) const = default;
    };

    struct Integer {
        int64_t value;
        auto operator<=>(const Integer &) const = default;
    };

    struct String {
        std::string value;
        auto operator<=>(const String &) const = default;
    };

    struct LPar {
        auto operator<=>(const LPar &) const = default;
    };

    struct RPar {
        auto operator<=>(const RPar &) const = default;
    };

    struct LBracket {
        auto operator<=>(const LBracket &) const = default;
    };

    struct RBracket {
        auto operator<=>(const RBracket &) const = default;
    };

    struct Plus {
        auto operator<=>(const Plus &) const = default;
    };

    struct Minus {
        auto operator<=>(const Minus &) const = default;
    };

    struct Asterisk {
        auto operator<=>(const Asterisk &) const = default;
    };

    struct Slash {
        auto operator<=>(const Slash &) const = default;
    };

    struct Assign {
        auto operator<=>(const Assign &) const = default;
    };

    struct Semicolon {
        auto operator<=>(const Semicolon &) const = default;
    };

    struct Comma {
        auto operator<=>(const Comma &) const = default;
    };

    struct Equal {
        auto operator<=>(const Equal &) const = default;
    };

    struct NotEqual {
        auto operator<=>(const NotEqual &) const = default;
    };

    struct Less {
        auto operator<=>(const Less &) const = default;
    };

    struct LessEqual {
        auto operator<=>(const LessEqual &) const = default;
    };

    struct Greater {
        auto operator<=>(const Greater &) const = default;
    };

    struct GreaterEqual {
        auto operator<=>(const GreaterEqual &) const = default;
    };

    using Any = std::variant<Keyword, Identifier, Integer, LBracket, RBracket, LPar,
    RPar, Plus, Semicolon, Comma, String, Assign, Equal, NotEqual, Less, LessEqual, Greater, GreaterEqual,
    Minus, Asterisk, Slash>;

    inline std::ostream& operator<<(std::ostream& os, const token::LPar&) {
        return os << "'('";
    }

    inline std::ostream& operator<<(std::ostream& os, const token::RPar&) {
        return os << "')'";
    }

    inline std::ostream& operator<<(std::ostream& os, const token::LBracket&) {
        return os << "'{'";
    }

    inline std::ostream& operator<<(std::ostream& os, const token::RBracket&) {
        return os << "'}'";
    }

    inline std::ostream& operator<<(std::ostream& os, const token::Plus&) {
        return os << "'+'";
    }

    inline std::ostream& operator<<(std::ostream& os, const token::Semicolon&) {
        return os << "';'";
    }

    inline std::ostream& operator<<(std::ostream& os, const token::Comma&) {
        return os << ',';
    }

    inline std::ostream& operator<<(std::ostream& os, const token::Integer& i) {
        return os << "Integer(" << i.value << ')';
    }

    inline std::ostream& operator<<(std::ostream& os, token::Keyword k) {
        switch (k) {
            case token::Keyword::FN:   return os << "Keyword(FUNC)";
            case token::Keyword::RETURN: return os << "Keyword(RETURN)";
            default:                     return os << "Keyword(?)";
        }
    }

    inline std::ostream& operator<<(std::ostream& os, token::String s) {
        return os << s.value;
    }

    inline std::ostream& operator<<(std::ostream& os, const token::Identifier& id) {
        return os << "Identifier(\"" << id.value << "\")";
    }
}
