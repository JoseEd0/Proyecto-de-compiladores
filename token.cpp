#include <iostream>
#include "token.h"

using namespace std;

Token::Token(Type type): type(type) { text = ""; }

Token::Token(Type type, char c): type(type) { text = string(1, c); }

Token::Token(Type type, const string& source, int first, int last): type(type) {
    text = source.substr(first, last - first );
}
Token::Token(Type type, const string& source): type(type), text(source){};

std::ostream& operator<<(std::ostream& outs, const Token& tok) {
    switch (tok.type) {
        case Token::IDENTIFIER: outs << "IDENTIFIER"; break;
        case Token::NUMBER: outs << "NUMBER"; break;
        case Token::CHARACTER_CONSTANT: outs << "CHARACTER_CONSTANT"; break;
        case Token::STRING_LITERAL: outs << "STRING_LITERAL"; break;
        case Token::FORMAT_STRING: outs << "FORMAT_STRING"; break;
        case Token::BIT_AND: outs << "REFERENCE"; break;
        case Token::INT: outs << "INT"; break;
        case Token::CHAR: outs << "CHAR"; break;
        case Token::VOID: outs << "VOID"; break;
        case Token::STRUCT: outs << "STRUCT"; break;

        case Token::IF: outs << "IF"; break;
        case Token::ELSE: outs << "ELSE"; break;
        case Token::ELSE_IF: outs << "ELSE_IF"; break;
        case Token::WHILE: outs << "WHILE"; break;
        case Token::FOR: outs << "FOR"; break;
        case Token::RETURN: outs << "RETURN"; break;
        case Token::INCLUDE: outs << "INCLUDE"; break;

        case Token::PLUS: outs << "PLUS"; break;
        case Token::MINUS: outs << "MINUS"; break;
        case Token::MULTIPLY: outs << "MULTIPLY"; break;
        case Token::DIVIDE: outs << "DIVIDE"; break;
        case Token::MODULO: outs << "MODULO"; break;
        case Token::INCREMENT: outs << "INCREMENT"; break;
        case Token::DECREMENT: outs << "DECREMENT"; break;

        case Token::ASSIGN: outs << "ASSIGN"; break;
        case Token::PLUS_ASSIGN: outs << "PLUS_ASSIGN"; break;
        case Token::MINUS_ASSIGN: outs << "MINUS_ASSIGN"; break;
        case Token::MULTIPLY_ASSIGN: outs << "MULTIPLY_ASSIGN"; break;
        case Token::DIVIDE_ASSIGN: outs << "DIVIDE_ASSIGN"; break;
        case Token::MODULO_ASSIGN: outs << "MODULO_ASSIGN"; break;

        case Token::EQUAL: outs << "EQUAL"; break;
        case Token::NOT_EQUAL: outs << "NOT_EQUAL"; break;
        case Token::LESS_THAN: outs << "LESS_THAN"; break;
        case Token::GREATER_THAN: outs << "GREATER_THAN"; break;
        case Token::LESS_EQUAL: outs << "LESS_EQUAL"; break;
        case Token::GREATER_EQUAL: outs << "GREATER_EQUAL"; break;

        case Token::LOGICAL_AND: outs << "LOGICAL_AND"; break;
        case Token::LOGICAL_OR: outs << "LOGICAL_OR"; break;
        case Token::LOGICAL_NOT: outs << "LOGICAL_NOT"; break;

        case Token::DEREFERENCE: outs << "DEREFERENCE"; break;
        case Token::POINTER_DECL: outs << "POINTER_DECL"; break;
        case Token::ADDRESS_OF: outs << "ADDRESS_OF"; break;
        case Token::ARROW: outs << "ARROW"; break;

        case Token::LEFT_PAREN: outs << "LEFT_PAREN"; break;
        case Token::RIGHT_PAREN: outs << "RIGHT_PAREN"; break;
        case Token::LEFT_BRACE: outs << "LEFT_BRACE"; break;
        case Token::RIGHT_BRACE: outs << "RIGHT_BRACE"; break;
        case Token::LEFT_BRACKET: outs << "LEFT_BRACKET"; break;
        case Token::RIGHT_BRACKET: outs << "RIGHT_BRACKET"; break;
        case Token::SEMICOLON: outs << "SEMICOLON"; break;
        case Token::COMMA: outs << "COMMA"; break;
        case Token::COLON: outs << "COLON"; break;
        case Token::DOT: outs << "DOT"; break;
        case Token::HEADER_NAME: outs << "HEADER_NAME"; break;

        case Token::LINE_COMMENT: outs << "LINE_COMMENT"; break;
        case Token::BLOCK_COMMENT: outs << "BLOCK_COMMENT"; break;

        case Token::MAIN: outs << "MAIN"; break;
        case Token::PRINTF: outs << "PRINTF"; break;

        case Token::END_OF_FILE: outs << "END_OF_FILE"; break;
        case Token::ERROR: outs << "ERROR"; break;
        case Token::UNKNOWN: outs << "UNKNOWN"; break;
        case Token::BOOL: outs << "BOOL"; break;
        case Token::TRUE: outs << "TRUE"; break;
        case Token::FALSE: outs << "FALSE"; break;
        default: outs << "UNKNOWN_TOKEN_TYPE"; break;
    }

    outs << "('" << tok.text << "')";
    return outs;
}

std::ostream& operator<<(std::ostream& outs, const Token* tok) {
    return outs << *tok;
}