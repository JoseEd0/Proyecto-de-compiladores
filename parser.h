#ifndef PARSER_H
#define PARSER_H

#include "expression.h"
#include "scanner.h"

class Parser {
private:
    Scanner* scanner;
    std::vector<Token*> tokens;
    size_t currentIndex;
    Token* current;
    Token* previous;
    Token* eofToken;
    Token* peekNext(size_t offset = 1) const;
    void skipComments();
    bool match(Token::Type ttype);
    bool check(Token::Type ttype) const;
    bool advance();
    bool isAtEnd() const;

    Token* consume(Token::Type ttype, const std::string& message);
    Token* consumeIdent(const std::string& message);

    Exp* parseExpression();
    Exp* parseAssignment();
    Exp* parseLogicalOr();
    Exp* parseLogicalAnd();
    Exp* parseEquality();
    Exp* parseComparison();
    Exp* parseAdditive();
    Exp* parseMultiplicative();
    Exp* parseUnary();
    Exp* parsePostfix();
    Exp* parsePrimary();
    Exp* parseFunctionCall();
    Exp* parseArrayAccess();
    Exp* parseMemberAccess();
    Exp* parseNumberExp();
    Type* parseType();

    Stm* parseStatement();
    Stm* parseIfStatement();
    Stm* parseWhileStatement();
    Stm* parseForStatement();
    Stm* parseReturnStatement();
    Stm* parsePrintStatement();
    Stm* parsePrintfStatement();
    Stm* parseExpressionStatement();
    Stm* parseVarDeclaration();
    Stm* parseElseStatement(int parentId);
    Stm* parseElseIfStatement(int parentId);

    VarDec* parseVarDec();
    VarDecList* parseVarDecList();
    GlobalVarDec* parseGlobalVarDec();
    Parameter* parseParameter();
    ParameterList* parseParameterList();

    Body* parseBody();
    StructDeclaration* parseStructDeclaration();
    Include* parseInclude();

public:
    Parser(Scanner* scanner);
    ~Parser();
    Exp* parseCExp();
    Program* parseProgram();
    IncludeList* parseIncludes();
    GlobalVarDecList* parseGlobalDeclarations();
    StructDeclarationList* parseStructDeclarations();
    FunctionList* parseFunctions();
    MainFunction* parseMainFunction();
    Function* parseFunction(Type* return_type, const string& name);

    StatementList* parseStatementList();
    list<Exp*> parseArguments();
    list<Exp*> parseInitializerList();
};

#endif // PARSER_H
