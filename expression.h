#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <string>
#include <list>
#include <vector>
#include "visitor.h"
using namespace std;

class VarDec;

enum BinaryOp {
    PLUS_OP,
    MINUS_OP,
    MULT_OP,
    DIV_OP,
    MOD_OP,
    EQUAL_OP,
    NOT_EQUAL_OP,
    LESS_THAN_OP,
    GREATER_THAN_OP,
    LESS_EQUAL_OP,
    GREATER_EQUAL_OP,
    LOGICAL_AND_OP,
    LOGICAL_OR_OP,
    ASSIGN_OP,
    PLUS_EQUAL_OP,
    MINUS_EQUAL_OP,
    MULTIPLY_EQUAL_OP,
    DIVIDE_EQUAL_OP,
    MODULO_EQUAL_OP,
    MEMBER_ACCESS_OP,
    POINTER_ACCESS_OP,
};

enum UnaryOp {
    NEGACION_OP,
    PLUS_PLUS_OP,
    MINUS_MINUS_OP,
    DEREFERENCE_OP,
    ADDRESS_OF_OP,
    UNARY_PLUS_OP,
    UNARY_MINUS_OP,
};

class Body;
class Exp;
class Stm;

class ImpValue {
public:
    string tipo;
    int valor;
    bool bool_value;
    ImpValue(string tipo, int valor, bool bol);
    ImpValue();
    ~ImpValue();
};

class Exp {
public:
    virtual int  accept(Visitor* visitor) = 0;
    virtual ~Exp() = 0;
    static string binopToChar(BinaryOp op);
    static string unaryToChar(UnaryOp op);
};




class Include {
public:
    string header_name;
    bool is_system_header;
    int accept(Visitor* visitor);
    Include(string header, bool system = false);
    ~Include();
};

class IncludeList {
public:
    list<Include*> includes;
    IncludeList();
    int accept(Visitor* visitor);
    void add(Include* inc);
    ~IncludeList();
};



class Type {
public:
    string type_name;
    bool is_pointer;
    bool is_array;
    Exp* array_size;
    bool is_reference = false; 
    int  accept(Visitor* visitor);
    Type(string name);
    Type(string name, bool pointer, bool is_reference=false);
    Type(string name, Exp* size);
    Type(string name, bool pointer, bool array, Exp* size = nullptr, bool is_reference = false);
    ~Type();
};



class BinaryExp : public Exp {
public:
    Exp *left, *right;
    string type;
    BinaryOp op;
    int accept(Visitor *visitor);
    BinaryExp(Exp* l, Exp* r, BinaryOp op);
    ~BinaryExp();
};

class AssignExp : public Exp {
public:
    Exp* left;
    Exp* right;
    BinaryOp assign_op;
    int accept(Visitor *visitor);
    AssignExp(Exp* l, Exp* r, BinaryOp op);
    ~AssignExp();
};

class UnaryExp : public Exp {
public:
    Exp* uexp;
    string type;
    UnaryOp op;
    bool is_prefix;
    int accept(Visitor *visitor) ;
    UnaryExp(Exp* uexp, UnaryOp op, bool prefix = true);
    ~UnaryExp();
};

class NumberExp : public Exp {
public:
    int value;
    NumberExp(int v);
    int accept(Visitor* visitor);
    ~NumberExp();
};

class BoolExp : public Exp {
public:
    int value;
    int accept(Visitor* visitor);
    BoolExp(bool v);
    ~BoolExp();
};

class CharExp : public Exp {
public:
    char value;
    CharExp(char v);
    int accept(Visitor *visitor);
    ~CharExp();
};

class StringExp : public Exp {
public:
    string value;
    int accept(Visitor *visitor);
    StringExp(string v);
    ~StringExp();
};

class IdentifierExp : public Exp {
public:
    std::string name;
    int accept(Visitor *visitor) ;
    IdentifierExp(const std::string& n);
    ~IdentifierExp();
};

class FunctionCallExp : public Exp {
public:
    string function_name;
    
    int accept(Visitor *visitor) ;
    vector<Exp*> arguments;
    FunctionCallExp(string name);
    void add_argument(Exp* arg);
    ~FunctionCallExp();
};

class ArrayAccessExp : public Exp {
public:
    Exp* array;
    Exp* index;
    int accept(Visitor *visitor);
    ArrayAccessExp(Exp* arr, Exp* idx);
    ~ArrayAccessExp();
};

class MemberAccessExp : public Exp {
public:
    Exp* object;
    string member_name;
    bool is_pointer;
    MemberAccessExp(Exp* obj, string member, bool ptr_access = false);
    ~MemberAccessExp();
    int accept(Visitor *visitor) ;
};

class ParenExp : public Exp {
public:
    Exp* inner;
    ParenExp(Exp* exp);
    int accept(Visitor *visitor);
    ~ParenExp();
};
class BlockElement {
public:
    virtual ~BlockElement() {}
    virtual int accept(Visitor* visitor) = 0;
};

class Stm : public BlockElement {
public:
     virtual ~Stm() = 0;
     virtual int accept(Visitor* visitor) = 0;
};




class PrintfStatement : public Stm {
public:
    string format_string;
    int accept(Visitor *visitor) ;
    vector<Exp*> arguments;
    PrintfStatement(string format);
    void add_argument(Exp* arg);
    ~PrintfStatement();
};

class IfStatement : public Stm {
public:
    int id;
    int counter;
    Exp* condition;
    int accept(Visitor *visitor) ;
    Body* statements;
    Stm* elsChain;
    IfStatement(Exp* cond, Body* statements, Stm* elsChain);
    ~IfStatement();
};

class ElseIfStatement : public Stm {
public:
    int id;
    int parentId;
    enum Tipo { ELSE_IF, ELSE };
    int accept(Visitor *visitor);
    Tipo tipo;
    Exp* condition;
    Body* body;
    Stm* nextChain;
    ElseIfStatement(Tipo, Exp* cond, Body* body, Stm* nextChain, int id, int parentId);
};


class WhileStatement : public Stm {
public:
    Exp* condition;
    int accept(Visitor *visitor) ;
    Body* b;
    int id;
    int counter;
    WhileStatement(Exp* condition, Body* b);
    ~WhileStatement();
};

class ForStatement : public Stm {
public:
    VarDec* init;
    Exp* condition;
    int accept(Visitor *visitor) ;
    Exp* update;
    Body* b;
    int id;
    int counter;
    ForStatement(VarDec* init, Exp* condition, Exp* update, Body* b);
    ~ForStatement();
};

class ExpressionStatement : public Stm {
public:
    Exp* expression;
    ExpressionStatement(Exp* exp);
    int accept(Visitor *visitor) ;
    ~ExpressionStatement();
};

class ReturnStatement : public Stm {
public:
    Exp* return_value;
    ReturnStatement(Exp* value = nullptr);
    ~ReturnStatement();
    int accept(Visitor *visitor) ;
};

class VarDec : public BlockElement {
public:
    std::vector<Type*> types;
    vector<string> vars;
    vector<Exp*> initializers;
    VarDec(vector<Type*> types, vector<string> vars);
    void add_initializer(Exp* init);
    ~VarDec();
    int accept(Visitor* visitor) ;
};

class VarDecList {
public:
    vector<VarDec*> vardecs;
    VarDecList();
    void add(VarDec* vardec);
    int accept(Visitor* visitor) ;
    ~VarDecList();
};


class GlobalVarDec {
public:
    Type* type;
    string var_name;
    Exp* initializer; 
    GlobalVarDec(Type* type, string name, Exp* init = nullptr);
    ~GlobalVarDec();
    int accept(Visitor* visitor) ;
};

class GlobalVarDecList {
public:
    vector<GlobalVarDec*> global_vardecs;
    GlobalVarDecList();
    void add(GlobalVarDec* vardec);
    int accept(Visitor* visitor) ;
    ~GlobalVarDecList();
};

class Parameter {
public:
    Type* type;
    string name;
    bool is_reference;
    Parameter(Type* type, string name, bool reference = false);
    ~Parameter();
    int accept(Visitor* visitor) ;
};

class ParameterList {
public:
    vector<Parameter*> parameters;
    ParameterList();
    void add(Parameter* param);
    ~ParameterList();
    int accept(Visitor* visitor) ;
};

class StatementList {
public:
    vector<Stm*> stms;
    StatementList();
    void add(Stm* stm);
    ~StatementList();
    int accept(Visitor* visitor) ;
};

class Body {
public:
    std::vector<BlockElement*> elements;

    Body(vector<BlockElement*> elements);
    ~Body();
    int accept(Visitor* visitor) ;
};

class Function {
public:
    Type* return_type;
    string name;
    ParameterList* parameters;
    Body* body;
    Function(Type* ret_type, string name, ParameterList* params, Body* body);
    ~Function();
    int accept(Visitor* visitor) ;
};

class FunctionList {
public:
    vector<Function*> functions;
    FunctionList();
    void add(Function* func);
    ~FunctionList();
    int accept(Visitor* visitor) ;
};

class MainFunction {
public:
    Body* body;
    MainFunction(Body* body);

    ~MainFunction();
    int accept(Visitor* visitor) ;
};


class StructDeclaration {
public:
    string struct_name;
    VarDecList* members;
    StructDeclaration(string name, VarDecList* members);
    ~StructDeclaration();
    int accept(Visitor* visitor) ;
};

class StructDeclarationList {
public:
    vector<StructDeclaration*> structs;
    StructDeclarationList();
    void add(StructDeclaration* struct_decl);
    ~StructDeclarationList();
    int accept(Visitor* visitor) ;
};


class StructInitializerExp :public Exp {
public:
    string struct_name;
    vector<pair<string, Exp*>> members; 
    StructInitializerExp(const string& name);
    void add_member(const string& member_name, Exp* value);
    int accept(GenCodeVisitor* visitor, int base_offset){
        return visitor->visit(this, base_offset);
    }

    int accept(Visitor* visitor);
    ~StructInitializerExp();
};

class ArrayInitializerExp: public Exp {
public:
    vector<Exp*> elements;
    ArrayInitializerExp(const std::vector<Exp*>& elems);
    int accept(Visitor* visitor);
    ~ArrayInitializerExp();
};

class Program {
public:
    IncludeList* includes;
    GlobalVarDecList* global_declarations;
    StructDeclarationList* struct_declarations;
    FunctionList* functions;
    MainFunction* main_function;    
        int countVars() const {
        if (main_function && main_function->body && main_function->body->elements.size()>0)
            return main_function->body->elements.size();
        return 0;
    }

    Program(IncludeList* includes,
            GlobalVarDecList* globals,
            StructDeclarationList* structs,
            FunctionList* functions,
            MainFunction* main);
    ~Program();
};

#endif //EXPRESSION_H
