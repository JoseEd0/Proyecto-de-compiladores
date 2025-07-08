#ifndef VISITOR_H
#define VISITOR_H
#include<iostream>
#include <unordered_map>
#include "environment.h"
using namespace std;

class Body;
class Exp;
class Stm;
class Include;
class IncludeList;
class Type;
class IFExp;
class BinaryExp;
class AssignExp;
class UnaryExp;
class NumberExp;
class BoolExp;
class CharExp;
class StringExp;
class BoolExp;
class IdentifierExp;
class FunctionCallExp;
class ArrayAccessExp;
class MemberAccessExp;
class ParenExp;
class StructInitializerExp;

class PrintfStatement;
class IfStatement;
class ElseIfStatement;
class WhileStatement;
class ForStatement;
class ExpressionStatement;
class ReturnStatement;
class VarDec;
class VarDecList;
class GlobalVarDec;
class GlobalVarDecList;
class Parameter;
class ParameterList;
class StatementList;
class Function;
class FunctionList;
class MainFunction;
class StructDeclaration;
class StructDeclarationList;
class ArrayInitializerExp;
class Program;


class Visitor {
public:

    virtual ~Visitor() = default;

    virtual void visit(Include* inc) = 0;
    virtual void visit(IncludeList* incList) = 0;
    virtual void visit(Type* type) = 0;
    
    virtual int visit(BinaryExp* exp) = 0;
    virtual int visit(AssignExp* exp) = 0;
    virtual int visit(UnaryExp* exp) = 0;
    virtual int visit(NumberExp* exp) = 0;
    virtual int visit(BoolExp* exp) = 0;
    virtual int visit(CharExp* exp) = 0;
    virtual int visit(StringExp* exp) = 0;
    virtual int visit(IdentifierExp* exp) = 0;
    virtual int visit(FunctionCallExp* exp) = 0;
    virtual int visit(ArrayAccessExp* exp) = 0;
    virtual int visit(MemberAccessExp* exp) = 0;
    virtual int visit(ParenExp* exp) = 0;
    virtual int visit(ArrayInitializerExp* exp) = 0;
    virtual int visit(StructInitializerExp* exp) = 0;
    virtual void visit(PrintfStatement* stm) = 0;
    virtual void visit(IfStatement* stm) = 0;
    virtual void visit(ElseIfStatement* stm) = 0;
    virtual void visit(WhileStatement* stm) = 0;
    virtual void visit(ForStatement* stm) = 0;
    virtual void visit(ExpressionStatement* stm) = 0;
    virtual void visit(ReturnStatement* stm) = 0;
    virtual void visit(VarDec* stm) = 0;
    virtual void visit(VarDecList* stm) = 0;
    virtual void visit(GlobalVarDec* dec) = 0;
    virtual void visit(GlobalVarDecList* decList) = 0;
    virtual void visit(Parameter* param) = 0;
    virtual void visit(ParameterList* paramList) = 0;

    virtual void visit(StatementList* stm) = 0;
    virtual void visit(Body* b) = 0;
    virtual void visit(Function* func) = 0;
    virtual void visit(FunctionList* funcList) = 0;
    virtual void visit(MainFunction* mainFunc) = 0;
    virtual void visit(StructDeclaration* structDecl) = 0;
    virtual void visit(StructDeclarationList* structList) = 0;
};

class PrintVisitor : public Visitor {
public:
    void imprimir(Program* program);
    int indent_level = 0;

    void printIndent();
    void visit(Include* inc) ;
    void visit(IncludeList* incList) ;
    void visit(Type* type) ;

    int visit(BinaryExp* exp) ;
    int visit(AssignExp* exp) ;
    int visit(UnaryExp* exp) ;
    int visit(NumberExp* exp) ;
    int visit(BoolExp* exp) ;
    int visit(CharExp* exp) ;
    int visit(StringExp* exp) ;
    int visit(IdentifierExp* exp) ;
    int visit(FunctionCallExp* exp) ;
    int visit(ArrayAccessExp* exp) ;
    int visit(MemberAccessExp* exp) ;
    int visit(ParenExp* exp) ;
    int visit(ArrayInitializerExp* exp) ;
    int visit(StructInitializerExp* exp) ;
    void visit(PrintfStatement* stm) ;
    void visit(IfStatement* stm) ;
    void visit(ElseIfStatement* stm) ;
    void visit(WhileStatement* stm) ;
    void visit(ForStatement* stm) ;
    void visit(ExpressionStatement* stm) ;
    void visit(ReturnStatement* stm) ;
    void visit(VarDec* stm) ;
    void visit(VarDecList* stm) ;
    void visit(GlobalVarDec* dec) ;
    void visit(GlobalVarDecList* decList) ;
    void visit(Parameter* param) ;
    void visit(ParameterList* paramList) ;

    void visit(StatementList* stm) ;
    void visit(Body* b) ;
    void visit(Function* func) ;
    void visit(FunctionList* funcList) ;
    void visit(MainFunction* mainFunc) ;
    void visit(StructDeclaration* structDecl) ;
    void visit(StructDeclarationList* structList) ;
};

class GenCodeVisitor : public Visitor {
public:
     std::ostream& out;
    Environment* env;
    GenCodeVisitor(std::ostream& out) : out(out) {
        env = new Environment();
        env->add_level(); 
    }

    int cantidad = 1;
    void gencode(Program* program);
    void visit(Include* inc) ;
    void visit(IncludeList* incList) ;
    void visit(Type* type) ;
    int visit(BinaryExp* exp) ;
    int visit(AssignExp* exp) ;
    int visit(UnaryExp* exp) ;
    int visit(NumberExp* exp) ;
    int visit(CharExp* exp) ;
    int visit(StringExp* exp) ;
    int visit(IdentifierExp* exp) ;
    int visit(FunctionCallExp* exp) ;
    int visit(ArrayAccessExp* exp) ;
    int visit(MemberAccessExp* exp) ;
    int visit(ParenExp* exp) ;
    int visit(BoolExp* exp) ;
    int visit(ArrayInitializerExp* exp) ;
    int visit(StructInitializerExp* exp, int base_offset = 0); ;
    int visit(StructInitializerExp* exp) override;

    void visit(PrintfStatement* stm) ;
    void visit(IfStatement* stm) ;
    void visit(ElseIfStatement* stm) ;
    void visit(WhileStatement* stm) ;
    void visit(ForStatement*  stm);
    void visit(ExpressionStatement* stm) ;
    void visit(ReturnStatement* stm) ;
    void visit(VarDec* stm) ;
    void visit(VarDecList* stm) ;
    void visit(GlobalVarDec* dec) ;
    void visit(GlobalVarDecList* decList) ;
    void visit(Parameter* param) ;
    void visit(ParameterList* paramList) ;
    void get_member_address(MemberAccessExp* exp);
    void visit(StatementList* stm) ;
    void visit(Body* b) ;
    void visit(Function* func) ;
    void visit(FunctionList* funcList) ;
    void visit(MainFunction* mainFunc) ;
    void visit(StructDeclaration* structDecl) ;
    void visit(StructDeclarationList* structList) ;
    int calcular_stack_body(Body* body);
    };  


/*

class TypeChecker : public Visitor {

    Environment* env;
public:
    TypeChecker();
    ~TypeChecker();
    void visit(Include* inc) ;
    void visit(IncludeList* incList) ;
    void visit(Type* type) ;
    int visit(BinaryExp* exp) ;
    int visit(AssignExp* exp) ;
    int visit(UnaryExp* exp) ;
    int visit(NumberExp* exp) ;
    int visit(BoolExp* exp) ;
    int visit(CharExp* exp) ;
    int visit(StringExp* exp) ;
    int visit(IdentifierExp* exp) ;
    int visit(FunctionCallExp* exp) ;
    int visit(ArrayAccessExp* exp) ;
    int visit(MemberAccessExp* exp) ;
    int visit(ParenExp* exp) ;

    void visit(PrintfStatement* stm) ;
    void visit(IfStatement* stm) ;
    void visit(ElseIfStatement* stm) ;
    void visit(WhileStatement* stm) ;
    void visit(ForStatement* stm) ;
    void visit(ExpressionStatement* stm) ;
    void visit(ReturnStatement* stm) ;

    void visit(VarDec* stm) ;
    void visit(VarDecList* stm) ;
    void visit(GlobalVarDec* dec) ;
    void visit(GlobalVarDecList* decList) ;
    void visit(Parameter* param) ;
    void visit(ParameterList* paramList) ;

    void visit(StatementList* stm) ;
    void visit(Body* b) ;
    void visit(Function* func) ;
    void visit(FunctionList* funcList) ;
    void visit(MainFunction* mainFunc) ;
    void visit(StructDeclaration* structDecl) ;
    void visit(StructDeclarationList* structList) ;

    void check(Program* program);

};
*/
#endif //VISITOR_H
