//
// Created by kucer on 21.02.2026.
//

#include "Lexer.h"

#include <map>
#include <stdexcept>

std::vector<token::Any> Lexer::tokenize() {
    std::vector<token::Any> result;
    size_t line{1};

    for (auto it = code.begin(); it != code.end(); ++it) {
        switch (*it) {
            case ' ':
            case '\r':
            case '\t':
                break;
            case '\n':
                line++;
                break;
            case '(':
                result.emplace_back(token::LPar{});
                break;
            case ')':
                result.emplace_back(token::RPar{});
                break;
            case '{':
                result.emplace_back(token::LBracket{});
                break;
            case '}':
                result.emplace_back(token::RBracket{});
                break;
            case '+':
                result.emplace_back(token::Plus{});
                break;
            case ';':
                result.emplace_back(token::Semicolon{});
                break;
            case ',':
                result.emplace_back(token::Comma{});
                break;
            default:
                if (std::isdigit(*it)) {
                    std::string int_buff{};
                    while (std::isdigit(*it)) {
                        int_buff += *it;
                        ++it;
                    }
                    --it;
                    result.emplace_back(token::Integer{std::stoll(int_buff)});
                    break;
                }

                if (std::isalpha(*it)) {
                    std::string word{};
                    while (std::isalpha(*it)) {
                        word += *it;
                        ++it;
                    }
                    static std::map<std::string_view, token::Keyword> keywords{
                        {"func", token::Keyword::FUNC},
                        {"return", token::Keyword::RETURN},
                    };

                    if (auto keyword = keywords.find(word); keyword != keywords.end()) {
                        result.emplace_back(keyword->second);
                        break;
                    }
                    --it;
                    result.emplace_back(token::Identifier{std::string(word)});
                    break;
                }

                throw std::domain_error(std::string{"Unexpected character: " } + std::string{*it} + std::string{" at line: " + std::to_string(line)});
        }
    }

    return result;
}
