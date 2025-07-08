#include <iostream>
#include <cstring>
#include "token.h"
#include "scanner.h"

using namespace std;

Scanner::Scanner(const char* s):input(s),first(0), current(0) { }

bool is_white_space(char c) {
    return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

Token* Scanner::nextToken() {
    Token* token;
    while (current < input.length() && is_white_space(input[current]))
        current++;

    if (current >= input.length())
        return new Token(Token::END_OF_FILE);

    char c = input[current];
    first = current;

    if (c == '#') {
        current++;
        while (current < input.length() && isalpha(input[current]))
            current++;
        string directive = input.substr(first, current - first);
        if (directive == "#include") {
            token = new Token(Token::INCLUDE, directive);
            return token;
        } else {
            token = new Token(Token::ERROR, directive);
            return token;
        }
    }

    if (c == '/' && current + 1 < input.length()) {
        if (input[current + 1] == '/') {
            current += 2;
            while (current < input.length() && input[current] != '\n') {
                current++;
            }
            if (current < input.length() && input[current] == '\n') {
                current++;
            }
            string comment = input.substr(first, current - first);
            token = new Token(Token::LINE_COMMENT, comment);
            return token;
        }
        else if (input[current + 1] == '*') {
            current += 2;
            bool commentClosed = false;
            while (current + 1 < input.length()) {
                if (input[current] == '*' && input[current + 1] == '/') {
                    current += 2;
                    commentClosed = true;
                    break;
                }
                current++;
            }

            if (commentClosed) {
                string comment = input.substr(first, current - first);
                token = new Token(Token::BLOCK_COMMENT, comment);
            } else {
                token = new Token(Token::ERROR, "Unclosed block comment");
            }
            return token;
        }
    }

    if ((c == '<' || c == '"') && first > 0) {
        int temp = first - 1;
        while (temp >= 0 && is_white_space(input[temp])) temp--;

        bool afterInclude = false;
        if (temp >= 7) {
            string prev = input.substr(temp - 7, 8);
            if (prev == "#include") {
                afterInclude = true;
            }
        }

        if (afterInclude) {
            char end_char = (c == '<') ? '>' : '"';
            current++;
            while (current < input.length() && input[current] != end_char) {
                current++;
            }
            if (current < input.length() && input[current] == end_char) {
                current++;
                string header = input.substr(first, current - first);
                token = new Token(Token::HEADER_NAME, header);
                return token;
            } else {
                token = new Token(Token::ERROR, "Unclosed header");
                return token;
            }
        }
    }

    if (c == '\'') {
        current++;
        if (current < input.length()) {
            if (input[current] == '\\' && current + 1 < input.length()) {
                current += 2;
            } else {
                current++;
            }
        }
        if (current < input.length() && input[current] == '\'') {
            current++;
            string charConst = input.substr(first, current - first);
            token = new Token(Token::CHARACTER_CONSTANT, charConst);
        } else {
            token = new Token(Token::ERROR, "Unclosed character constant");
        }
        return token;
    }

    if (c == '"') {
        current++;
        bool is_format = false;
        while (current < input.length() && input[current] != '"') {
            if (input[current] == '%' && current + 1 < input.length()) {
                is_format = true;
            }
            if (input[current] == '\\' && current + 1 < input.length()) {
                current += 2;
            } else {
                current++;
            }
        }
        if (current < input.length() && input[current] == '"') {
            current++;
            string text = input.substr(first, current - first);
            if (is_format) {
                token = new Token(Token::FORMAT_STRING, text);
            } else {
                token = new Token(Token::STRING_LITERAL, text);
            }
        } else {
            token = new Token(Token::ERROR, "Unclosed string");
        }
        return token;
    }

    if (isdigit(c)) {
        current++;
        while (current < input.length() && isdigit(input[current]))
            current++;
        string number = input.substr(first, current - first);
        token = new Token(Token::NUMBER, number);
        return token;
    }

    if (isalpha(c) || c == '_') {
        current++;
        if (input.substr(first, 8) == "else if ") {
            current = first + 8;
            token = new Token(Token::ELSE_IF, "else if");
            return token;
        }
        while (current < input.length() && (isalnum(input[current]) || input[current] == '_'))
            current++;
        string word = input.substr(first, current - first);
        if (word == "int") {
            token = new Token(Token::INT, word);
        } else if (word == "char") {
            token = new Token(Token::CHAR, word);
        } else if (word == "void") {
            token = new Token(Token::VOID, word);
        }else if (word == "bool") {
            token = new Token(Token::BOOL, word);
        } else if (word == "struct") {
            token = new Token(Token::STRUCT, word);
        }else if (word == "if") {
            token = new Token(Token::IF, word);
        }else if (word == "else") {
            token = new Token(Token::ELSE, word);
        } else if (word == "while") {
            token = new Token(Token::WHILE, word);
        } else if (word == "for") {
            token = new Token(Token::FOR, word);
        } else if (word == "return") {
            token = new Token(Token::RETURN, word);
        } else if (word == "main") {
            token = new Token(Token::MAIN, word);
        } else if (word == "printf") {
            token = new Token(Token::PRINTF, word);
        } else if(word == "true"){
            token = new Token(Token::TRUE);
        }else if (word == "false"){
            token = new Token(Token::FALSE);
        }
        else {
            token = new Token(Token::IDENTIFIER, word);
        }
        return token;
    }

    switch(c) {
        case '+':
            current++;
            if (current < input.length() && input[current] == '+') {
                current++;
                return new Token(Token::INCREMENT, "++");
            } else if (current < input.length() && input[current] == '=') {
                current++;
                return new Token(Token::PLUS_ASSIGN, "+=");
            } else {
                return new Token(Token::PLUS, '+');
            }

        case '-':
            current++;
            if (current < input.length() && input[current] == '-') {
                current++;
                return new Token(Token::DECREMENT, "--");
            } else if (current < input.length() && input[current] == '=') {
                current++;
                return new Token(Token::MINUS_ASSIGN, "-=");
            } else if (current < input.length() && input[current] == '>') {
                current++;
                return new Token(Token::ARROW, "->");
            } else {
                return new Token(Token::MINUS, '-');
            }
        
        case '*':
            current++;
            if (current < input.length() && input[current] == '=') {
                current++;
                return new Token(Token::MULTIPLY_ASSIGN, "*=");
            } else {
                bool isDereference = false;
                if (first > 0) {
                    int temp = first - 1;
                    while (temp >= 0 && is_white_space(input[temp])) temp--;
                    if (temp >= 0) {
                        char prevChar = input[temp];
                        isDereference = !(isalnum(prevChar) || prevChar == ')' || prevChar == ']' || prevChar == '_');
                    } else {
                        isDereference = true;
                    }
                }

                if (isDereference) {
                    return new Token(Token::DEREFERENCE, '*');
                } else {
                    return new Token(Token::MULTIPLY, '*');
                }
            }

        case '/':
            current++;
            if (current < input.length() && input[current] == '=') {
                current++;
                return new Token(Token::DIVIDE_ASSIGN, "/=");
            } else {
                return new Token(Token::DIVIDE, '/');
            }

        case '%':
            current++;
            if (current < input.length() && input[current] == '=') {
                current++;
                return new Token(Token::MODULO_ASSIGN, "%=");
            } else {
                return new Token(Token::MODULO, '%');
            }

        case '=':
            current++;
            if (current < input.length() && input[current] == '=') {
                current++;
                return new Token(Token::EQUAL, "==");
            } else {
                return new Token(Token::ASSIGN, '=');
            }

        case '!':
            current++;
            if (current < input.length() && input[current] == '=') {
                current++;
                return new Token(Token::NOT_EQUAL, "!=");
            } else {
                return new Token(Token::LOGICAL_NOT, '!');
            }

        case '>':
            current++;
            if (current < input.length() && input[current] == '=') {
                current++;
                return new Token(Token::GREATER_EQUAL, ">=");
            } else {
                return new Token(Token::GREATER_THAN, '>');
            }

        case '<':
            current++;
            if (current < input.length() && input[current] == '=') {
                current++;
                return new Token(Token::LESS_EQUAL, "<=");
            } else {
                return new Token(Token::LESS_THAN, '<');
            }

        case '&':
            current++;
            if (current < input.length() && input[current] == '&') {
                current++;
                return new Token(Token::LOGICAL_AND, "&&");
            } else {
                return new Token(Token::ADDRESS_OF, '&');
            }

        case '|':
            current++;
            if (current < input.length() && input[current] == '|') {
                current++;
                return new Token(Token::LOGICAL_OR, "||");
            } else {
                return new Token(Token::ERROR, '|');
            }

        case '(':
            current++;
            return new Token(Token::LEFT_PAREN, '(');
        case ')':
            current++;
            return new Token(Token::RIGHT_PAREN, ')');
        case '{':
            current++;
            return new Token(Token::LEFT_BRACE, '{');
        case '}':
            current++;
            return new Token(Token::RIGHT_BRACE, '}');
        case '[':
            current++;
            return new Token(Token::LEFT_BRACKET, '[');
        case ']':
            current++;
            return new Token(Token::RIGHT_BRACKET, ']');
        case ';':
            current++;
            return new Token(Token::SEMICOLON, ';');
        case ',':
            current++;
            return new Token(Token::COMMA, ',');
        case '.':
            current++;
            return new Token(Token::DOT, '.');

       
        default:
            current++;
            return new Token(Token::ERROR, c);
    }
}

void Scanner::reset() {
    first = 0;
    current = 0;
}

Scanner::~Scanner() { }

void test_scanner(Scanner* scanner) {
    Token* current;
    cout << "Iniciando Scanner:" << endl << endl;
    while ((current = scanner->nextToken())->type != Token::END_OF_FILE) {
        if (current->type == Token::ERROR) {
            cout << "Error en scanner - carácter inválido: " << current->text << endl;
            break;
        } else {
            cout << *current << endl;
        }
        delete current;
    }
    cout << "TOKEN(END_OF_FILE)" << endl;
    delete current;
}