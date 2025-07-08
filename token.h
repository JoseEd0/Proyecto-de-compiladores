#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <iostream>
using namespace std;

class Token {
public:
    enum Type {
        IDENTIFIER,
        NUMBER,
        CHARACTER_CONSTANT,
        STRING_LITERAL,
        FORMAT_STRING,

        INT,
        CHAR,
        VOID,
        STRUCT,

        IF,
        ELSE,
        ELSE_IF,
        WHILE,
        FOR,
        RETURN,

        INCLUDE,
        HEADER_NAME,

        PLUS,
        MINUS,
        MULTIPLY,
        DIVIDE,
        MODULO,
        INCREMENT,
        DECREMENT,

        ASSIGN,
        PLUS_ASSIGN,
        MINUS_ASSIGN,
        MULTIPLY_ASSIGN,
        DIVIDE_ASSIGN,
        MODULO_ASSIGN,

        EQUAL,
        NOT_EQUAL,
        LESS_THAN,
        GREATER_THAN,
        LESS_EQUAL,
        GREATER_EQUAL,

        LOGICAL_AND,
        LOGICAL_OR,
        LOGICAL_NOT,

        DEREFERENCE,
        POINTER_DECL,
        ADDRESS_OF,
        ARROW,

        LEFT_PAREN,
        RIGHT_PAREN,
        LEFT_BRACE,
        RIGHT_BRACE,
        LEFT_BRACKET,
        RIGHT_BRACKET,
        SEMICOLON,
        COMMA,
        COLON,
        DOT,

        LINE_COMMENT,
        BLOCK_COMMENT,

        MAIN,
        PRINTF,
        TRUE,
        FALSE,
        BOOL,
        BIT_AND,
        END_OF_FILE,
        ERROR,
        UNKNOWN
    };

    Type type;
    string text;

    Token(Type type);
    Token(Type type, char c);
    Token(Type type, const string& source, int first, int last);
    Token(Type type, const string& source);
    bool isType() const {
        return type == INT || type == CHAR || type == VOID || type == STRUCT;
    }

    bool isOperator() const {
        return (type >= PLUS && type <= LOGICAL_NOT) ||
               (type >= ASSIGN && type <= MODULO_ASSIGN);
    }

    bool isPreprocessor() const {
        return type == INCLUDE || type == HEADER_NAME;
    }

    friend ostream& operator<<(ostream& outs, const Token& tok);
    friend ostream& operator<<(ostream& outs, const Token* tok);
};

#endif // TOKEN_H