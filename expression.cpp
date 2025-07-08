#include "expression.h"
#include <iostream>
#include <utility>
using namespace  std;

ImpValue::ImpValue(string tipo, int valor, bool bol) : tipo(tipo), valor(valor), bool_value(bol) {}
ImpValue::ImpValue() : tipo(""), valor(0), bool_value(false) {}
ImpValue::~ImpValue() {}

Exp::~Exp() {}

string Exp::binopToChar(BinaryOp op) {
    switch(op) {
        case PLUS_OP: return "+";
        case MINUS_OP: return "-";
        case MULT_OP: return "*";
        case DIV_OP: return "/";
        case MOD_OP: return "%";
        case EQUAL_OP: return "==";
        case NOT_EQUAL_OP: return "!=";
        case LESS_THAN_OP: return "<";
        case GREATER_THAN_OP: return ">";
        case LESS_EQUAL_OP: return "<=";
        case GREATER_EQUAL_OP: return ">=";
        case LOGICAL_AND_OP: return "&&";
        case LOGICAL_OR_OP: return "||";
        case ASSIGN_OP: return "=";
        case PLUS_EQUAL_OP: return "+=";
        case MINUS_EQUAL_OP: return "-=";
        case MULTIPLY_EQUAL_OP: return "*=";
        case DIVIDE_EQUAL_OP: return "/=";
        case MODULO_EQUAL_OP: return "%=";
        case MEMBER_ACCESS_OP: return ".";
        case POINTER_ACCESS_OP: return "->";
        default: return "?";
    }
}

string Exp::unaryToChar(UnaryOp op) {
    switch(op) {
        case NEGACION_OP: return "!";
        case PLUS_PLUS_OP: return "++";
        case MINUS_MINUS_OP: return "--";
        case DEREFERENCE_OP: return "*";
        case UNARY_PLUS_OP: return "+";
        case UNARY_MINUS_OP: return "-";
        case ADDRESS_OF_OP: return "&";
        default: return "?";
    }
}

Include::Include(string header, bool system) :
    header_name(header), is_system_header(system) {}
Include::~Include() {}

IncludeList::IncludeList() {}
void IncludeList::add(Include* inc) { includes.push_back(inc); }
IncludeList::~IncludeList() {
    for (auto inc : includes) delete inc;
}



Type::Type(string name) :
    type_name(name), is_pointer(false), is_array(false), array_size(nullptr), is_reference(false) {}
Type::Type(string name, bool pointer, bool is_reference) :
    type_name(name), is_pointer(pointer), is_array(false), array_size(nullptr), is_reference(is_reference) {}
Type::Type(string name, Exp* size) :
    type_name(name), is_pointer(false), is_array(true), array_size(size), is_reference(false) {}
Type::~Type() { if (array_size) delete array_size; }
Type::Type(string name, bool pointer, bool array, Exp* size, bool is_reference) :
    type_name(name), is_pointer(pointer), is_array(array), array_size(size), is_reference(is_reference) {}


BinaryExp::BinaryExp(Exp* l, Exp* r, BinaryOp op) :
    left(l), right(r), op(op) {}
BinaryExp::~BinaryExp() {
    delete left; delete right;
}

AssignExp::AssignExp(Exp* l, Exp* r, BinaryOp op) :
    left(l), right(r), assign_op(op) {}
AssignExp::~AssignExp() {
    delete left; delete right;
}

UnaryExp::UnaryExp(Exp* uexp, UnaryOp op, bool prefix) :
    uexp(uexp), op(op), is_prefix(prefix) {}
UnaryExp::~UnaryExp() { delete uexp; }

NumberExp::NumberExp(int v) : value(v) {}
NumberExp::~NumberExp() {}

CharExp::CharExp(char v) : value(v) {}
CharExp::~CharExp() {}

StringExp::StringExp(string v) : value(v) {}
StringExp::~StringExp() {}

IdentifierExp::IdentifierExp(const std::string& n) : name(n) {}
IdentifierExp::~IdentifierExp() {}

FunctionCallExp::FunctionCallExp(string name) : function_name(name) {}
void FunctionCallExp::add_argument(Exp* arg) { arguments.push_back(arg); }
FunctionCallExp::~FunctionCallExp() {
    for (auto arg : arguments) delete arg;
}

ArrayAccessExp::ArrayAccessExp(Exp* arr, Exp* idx) : array(arr), index(idx) {}
ArrayAccessExp::~ArrayAccessExp() {
    delete array; delete index;
}

MemberAccessExp::MemberAccessExp(Exp* obj, string member, bool ptr_access) :
    object(obj), member_name(member), is_pointer(ptr_access) {}
MemberAccessExp::~MemberAccessExp() { delete object; }

ParenExp::ParenExp(Exp* exp) : inner(exp) {}
ParenExp::~ParenExp() { delete inner; }

Stm::~Stm() {}



PrintfStatement::PrintfStatement(string format) : format_string(format) {}
void PrintfStatement::add_argument(Exp* arg) { arguments.push_back(arg); }
PrintfStatement::~PrintfStatement() {
    for (auto arg : arguments) delete arg;
}

IfStatement::IfStatement(Exp* cond, Body* statements, Stm* elsChain) :
    condition(cond), statements(statements), elsChain(elsChain) {
    static int counter = 1;
    id = counter++;
}
IfStatement::~IfStatement() {
    delete condition;
    delete statements;
    delete elsChain;
}

ElseIfStatement::ElseIfStatement(Tipo t, Exp* cond, Body* body, Stm* nextChain, int id, int parentId) :
    tipo(t), condition(cond), body(body), nextChain(nextChain), id(id), parentId(parentId) {}

WhileStatement::WhileStatement(Exp* condition, Body* b) :
    condition(condition), b(b) {
    static int counter = 1;
    id = counter++;
}
WhileStatement::~WhileStatement() {
    delete condition;
    delete b;
}

ForStatement::ForStatement(VarDec* init, Exp* condition, Exp* update, Body* b) :
    init(init), condition(condition), update(update), b(b) {
    static int counter = 1;
    id = counter++;
}
ForStatement::~ForStatement() {
    delete init;
    delete condition;
    delete update;
    delete b;
}
BoolExp::BoolExp(bool v) : value(v) {}
BoolExp::~BoolExp() {}

ExpressionStatement::ExpressionStatement(Exp* exp) : expression(exp) {}
ExpressionStatement::~ExpressionStatement() { delete expression; }

ReturnStatement::ReturnStatement(Exp* value) : return_value(value) {}
ReturnStatement::~ReturnStatement() { delete return_value; }

VarDec::VarDec(std::vector<Type*> types, vector<string> vars) :
    types(types), vars(vars) {}

    
void VarDec::add_initializer(Exp* init) { initializers.push_back(init); }
VarDec::~VarDec() {
    for (auto type : types) delete type;
    for (auto init : initializers) delete init;
}

VarDecList::VarDecList() {}
void VarDecList::add(VarDec* vardec) { vardecs.push_back(vardec); }
VarDecList::~VarDecList() {
    for (auto vardec : vardecs) delete vardec;
}

GlobalVarDec::GlobalVarDec(Type* type, string name, Exp* init) :
    type(type), var_name(name), initializer(init) {}
GlobalVarDec::~GlobalVarDec() {
    delete type;
    delete initializer;
}

GlobalVarDecList::GlobalVarDecList() {}
void GlobalVarDecList::add(GlobalVarDec* vardec) { global_vardecs.push_back(vardec); }
GlobalVarDecList::~GlobalVarDecList() {
    for (auto vardec : global_vardecs) delete vardec;
}

Parameter::Parameter(Type* type, string name, bool reference) : type(type), name(name), is_reference(reference) {}
Parameter::~Parameter() { delete type; }

ParameterList::ParameterList() {}
void ParameterList::add(Parameter* param) { parameters.push_back(param); }
ParameterList::~ParameterList() {
    for (auto param : parameters) delete param;
}

StatementList::StatementList() {}
void StatementList::add(Stm* stm) { stms.push_back(stm); }
StatementList::~StatementList() {
    for (auto stm : stms) delete stm;
}

Body::Body(vector<BlockElement*> elements) : elements(std::move(elements)) {}
Body::~Body() {
    for (auto elem : elements) delete elem;
}

Function::Function(Type* ret_type, string name, ParameterList* params, Body* body) :
    return_type(ret_type), name(name), parameters(params), body(body) {}
Function::~Function() {
    delete return_type;
    delete parameters;
    delete body;
}

FunctionList::FunctionList() {}
void FunctionList::add(Function* func) { functions.push_back(func); }
FunctionList::~FunctionList() {
    for (auto func : functions) delete func;
}

MainFunction::MainFunction(Body* body) : body(body) {}
MainFunction::~MainFunction() { delete body; }

StructDeclaration::StructDeclaration(string name, VarDecList* members) :
    struct_name(name), members(members) {}
StructDeclaration::~StructDeclaration() { delete members; }

StructDeclarationList::StructDeclarationList() {}
void StructDeclarationList::add(StructDeclaration* struct_decl) {
    structs.push_back(struct_decl);
}
StructDeclarationList::~StructDeclarationList() {
    for (auto decl : structs) delete decl;
}

StructInitializerExp::StructInitializerExp(const string& name) : struct_name(name) {}
void StructInitializerExp::add_member(const string& member_name, Exp* value) {
    members.emplace_back(member_name, value);
}
StructInitializerExp::~StructInitializerExp() {
    for (auto& member : members) delete member.second; 
}



Program::Program(IncludeList* includes,
                GlobalVarDecList* globals,
                StructDeclarationList* structs,
                FunctionList* functions,
                MainFunction* main) :
    includes(includes),
    global_declarations(globals),
    struct_declarations(structs),
    functions(functions),
    main_function(main) {}

ArrayInitializerExp::ArrayInitializerExp(const std::vector<Exp*>& elems):elements(elems){};
ArrayInitializerExp::~ArrayInitializerExp() {
    for (auto elem : elements) delete elem;
}    

Program::~Program() {
    delete includes;
    delete global_declarations;
    delete struct_declarations;
    delete functions;
    delete main_function;
}

