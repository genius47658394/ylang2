#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"
#include "stdlib.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <source file>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << argv[1] << std::endl;
        return 1;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    try {
        // Лексический анализ
        Lexer lexer(source);
        auto tokens = lexer.tokenize();

        // Синтаксический анализ
        Parser parser(tokens);
        auto program = parser.parse();

        // Интерпретация
        Interpreter interpreter(program.get());
        registerStdLib(interpreter);  // <- регистрируем стандартные функции
        interpreter.interpret();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}