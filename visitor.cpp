#include <stdexcept>
#include "visitor.h"
#include <iostream>
#include "expression.h"
#include <unordered_map>
#include <algorithm>
#include <iterator>
using namespace std;

int Include::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int IncludeList::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int Type::accept(Visitor *visitor) {
     visitor->visit(this);
    return 0;
}

int BinaryExp::accept(Visitor *visitor) {
    return visitor->visit(this);
}


int AssignExp::accept(Visitor *visitor) {
    return visitor->visit(this);
}

int UnaryExp::accept(Visitor *visitor) {
    return visitor->visit(this);
}

int NumberExp::accept(Visitor *visitor) {
    return visitor->visit(this);
}

int BoolExp::accept(Visitor *visitor) {
    return visitor->visit(this);
}

int CharExp::accept(Visitor *visitor) {
    return visitor->visit(this);
}

int StringExp::accept(Visitor *visitor) {
    return visitor->visit(this);
}

int IdentifierExp::accept(Visitor *visitor) {
    return visitor->visit(this);
}

int FunctionCallExp::accept(Visitor *visitor) {
    return visitor->visit(this);
}

int ArrayAccessExp::accept(Visitor *visitor) {
    return visitor->visit(this);
}

int MemberAccessExp::accept(Visitor *visitor) {
    return visitor->visit(this);
}

int ParenExp::accept(Visitor *visitor) {
    return visitor->visit(this);
}

int PrintfStatement::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int IfStatement::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int ElseIfStatement::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int WhileStatement::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int ForStatement::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int ExpressionStatement::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}
int ArrayInitializerExp::accept(Visitor *visitor) {
    return visitor->visit(this);
    
}

int ReturnStatement::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int VarDec::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int VarDecList::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int GlobalVarDec::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int GlobalVarDecList::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}


int Parameter::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int ParameterList::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int StatementList::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int Body::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int Function::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int FunctionList::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int MainFunction::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}


int StructDeclaration::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int StructDeclarationList::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}
int StructInitializerExp::accept(Visitor *visitor) {
    return visitor->visit(this);
}


void PrintVisitor::visit(Include* inc) {
    cout << "#include "
         << inc->header_name << endl;
}

void PrintVisitor::visit(IncludeList* inc_list) {
    for (auto& inc : inc_list->includes) {
        inc->accept(this);
    }
}



int PrintVisitor::visit(BinaryExp* exp) {
    exp->left->accept(this);
    cout << " " << Exp::binopToChar(exp->op) << " ";
    exp->right->accept(this);
    return 0;
}

int PrintVisitor::visit(ArrayInitializerExp* exp) {
    cout << "{";
    for (size_t i = 0; i < exp->elements.size(); ++i) {
        exp->elements[i]->accept(this);
        if (i < exp->elements.size() - 1) {
            cout << ", ";
        }
    }
    cout << "}";
    return 0;
}

int PrintVisitor::visit(StructInitializerExp* exp) {
    cout << "{";
    for (size_t i = 0; i < exp->members.size(); ++i) {
        exp->members[i].second->accept(this);
        if (i < exp->members.size() - 1) {
            cout << ", ";
        }
    }
    cout << "}";
    return 0;
}

int PrintVisitor::visit(AssignExp* exp) {
    exp->left->accept(this);
    cout << " " << Exp::binopToChar(exp->assign_op) << " ";
    exp->right->accept(this);
    return 0;
}

int PrintVisitor::visit(UnaryExp* exp) {
    if (exp->is_prefix) {
        cout << Exp::unaryToChar(exp->op);
        exp->uexp->accept(this);
    } else {
        exp->uexp->accept(this);
        cout << Exp::unaryToChar(exp->op);
    }
    return 0;
}

int PrintVisitor::visit(NumberExp* exp) {
    cout << exp->value;
    return 0;
}

int PrintVisitor::visit(BoolExp* exp) {
    cout << (exp->value ? "true" : "false");
    return 0;
}

int PrintVisitor::visit(CharExp* exp) {
    cout << "'" << exp->value << "'";
    return 0;
}

int PrintVisitor::visit(StringExp* exp) {
    cout  << exp->value;
    return 0;
}

int PrintVisitor::visit(IdentifierExp* exp) {
    cout << exp->name;
    return 0;
}

int PrintVisitor::visit(FunctionCallExp* exp) {
    cout << exp->function_name << "(";
    for (size_t i = 0; i < exp->arguments.size(); ++i) {
        exp->arguments[i]->accept(this);
        if (i < exp->arguments.size() - 1) {
            cout << ", ";
        }
    }
    cout << ")";
    return 0;
}

int PrintVisitor::visit(ArrayAccessExp* exp) {
    exp->array->accept(this);
    cout << "[";
    exp->index->accept(this);
    cout << "]";
    return 0;
}

int PrintVisitor::visit(MemberAccessExp* exp) {
    exp->object->accept(this);
    cout << (exp->is_pointer ? "->" : ".");
    cout << exp->member_name;
    return 0;
}


int PrintVisitor::visit(ParenExp* exp) {
    cout << "(";
    exp->inner->accept(this);
    cout << ")";
    return 0;
}

void PrintVisitor::visit(PrintfStatement* stm) {
    printIndent();
    cout << "printf(" << stm->format_string;
    if (!stm->arguments.empty()) {
        cout << ", ";
        for (size_t i = 0; i < stm->arguments.size(); ++i) {
            stm->arguments[i]->accept(this);
            if (i < stm->arguments.size() - 1) {
                cout << ", ";
            }
        }
    }
    cout << ");" << endl;
}
void PrintVisitor::visit(IfStatement* stm) {
    if (!stm) return;

    printIndent();

    cout << "if (";
    if (stm->condition) stm->condition->accept(this);
    cout << ") {" << endl;

    indent_level++;
    if (stm->statements) stm->statements->accept(this);
    indent_level--;

    printIndent();

    cout << "}";
    if (stm->elsChain) {
        stm->elsChain->accept(this);
    }


    cout << endl;
}

void PrintVisitor::visit(ElseIfStatement* stm) {
    if (!stm) return;

    if (stm->tipo == ElseIfStatement::ELSE_IF) {
        cout << "else if (";
        if (stm->condition)
            stm->condition->accept(this);
        cout << ")";
        cout << " {" << endl;
        indent_level++;
        if (stm->body) stm->body->accept(this);
        indent_level--;
        printIndent();
        cout << "}";
        if (stm->nextChain) {
            stm->nextChain->accept(this);
        }
    }


    if (stm->tipo == ElseIfStatement::ELSE ) {
        cout << " else {" << endl;
        indent_level++;
        if (stm->body) stm->body->accept(this);
        indent_level--;
        printIndent();
        cout << "}";
    }
}

void PrintVisitor::visit(WhileStatement* stm) {
    printIndent();
    cout << "while (";
    stm->condition->accept(this);
    cout << ") {" << endl;
    indent_level++;
    stm->b->accept(this);
    indent_level--;
    printIndent();
    cout << "}" << endl;
}

void PrintVisitor::visit(ForStatement* stm) {
    printIndent();
    cout << "for (";
    if (stm->init) {
        stm->init->types[0]->accept(this);
        cout << " ";
        for (size_t i = 0; i < stm->init->vars.size(); ++i) {
            cout << stm->init->vars[i];
            if (!stm->init->initializers.empty() && i < stm->init->initializers.size()) {
                cout << " = ";
                stm->init->initializers[i]->accept(this);
            }
            if (i < stm->init->vars.size() - 1) {
                cout << ", ";
            }
        }
    }
    cout << "; ";
    if (stm->condition) {
        stm->condition->accept(this);
    }
    cout << "; ";
    if (stm->update) {
        stm->update->accept(this);
    }
    cout << ") {" << endl;
    indent_level++;
    stm->b->accept(this);
    indent_level--;
    printIndent();
    cout << "}" << endl;
}

void PrintVisitor::visit(ExpressionStatement* stm) {
    printIndent();
    stm->expression->accept(this);
    cout << ";" << endl;
}

void PrintVisitor::visit(ReturnStatement* stm) {
    printIndent();
    cout << "return";
    if (stm->return_value) {
        cout << " ";
        stm->return_value->accept(this);
    }
    cout << ";" << endl;
}

void PrintVisitor::visit(Type* type) {
    cout << type->type_name;
    if (type->is_pointer) {
        cout << "*";
    }
  
    if (type->is_reference) {
        cout << "&";
    }
}


void PrintVisitor::visit(VarDec* vardec) {
    printIndent();
    for (size_t i = 0; i < vardec->vars.size(); ++i) {
        if (i < vardec->types.size() && vardec->types[i]) {
            vardec->types[i]->accept(this);
          
        } else {
            cout << "/*tipo?*/";
        }
        cout << " " << vardec->vars[i];
        if(vardec->types[i]->is_array) {
            cout << "[";
            if (vardec->types[i]->array_size) {
                vardec->types[i]->array_size->accept(this);
            }
            cout << "]";
        }
        if (!vardec->initializers.empty() && i < vardec->initializers.size() && vardec->initializers[i]) {
            cout << " = ";
            vardec->initializers[i]->accept(this);
        }
        cout << ";" << endl;
    }
}

void PrintVisitor::visit(VarDecList* vardec_list) {
    for (auto& vardec : vardec_list->vardecs) {
        vardec->accept(this);
    }
}

void PrintVisitor::visit(GlobalVarDec* global_vardec) {
    printIndent();
    global_vardec->type->accept(this);
    cout << " " << global_vardec->var_name;
    if (global_vardec->initializer) {
        cout << " = ";
        global_vardec->initializer->accept(this);
    }
    cout << ";" << endl;
}

void PrintVisitor::visit(GlobalVarDecList* global_vardec_list) {
    for (auto& global_vardec : global_vardec_list->global_vardecs) {
        global_vardec->accept(this);
    }
}

void PrintVisitor::visit(Parameter* param) {
    param->type->accept(this);
    if(param->is_reference) {
        cout << "&";
    }
    cout << " " << param->name;
     if(param->type->is_array) {
        cout << "[]";
    }
}

void PrintVisitor::visit(ParameterList* param_list) {
    for (size_t i = 0; i < param_list->parameters.size(); ++i) {
        param_list->parameters[i]->accept(this);
        if (i < param_list->parameters.size() - 1) {
            cout << ", ";
        }
    }
}

void PrintVisitor::visit(StatementList* stm_list) {
    for (auto& stm : stm_list->stms) {
        stm->accept(this);
    }
}



void PrintVisitor::visit(Body* b) {
    if (b->elements.empty()) {
        cout << "/* Empty body */" << endl;
        return;
    }
    for (auto& elem : b->elements) {
        elem->accept(this);
    }
}

void PrintVisitor::visit(Function* func) {
    if (!func) return; 

    printIndent();
    if (func->return_type) {
        func->return_type->accept(this);
        cout << " ";
    }
    cout << func->name << "(";
    if (func->parameters) {
        func->parameters->accept(this);
    }
    cout << ") {" << endl;
    indent_level++;
    if (func->body) {
        func->body->accept(this);
    }
    indent_level--;

    printIndent();
    cout << "}" << endl << endl;
}

void PrintVisitor::visit(FunctionList* func_list) {
    for (auto& func : func_list->functions) {
        func->accept(this);
    }
}

void PrintVisitor::visit(MainFunction* main_func) {
    printIndent();
    cout << "int main() {" << endl;
    indent_level++;
    if (main_func->body) {
        main_func->body->accept(this);
    }
    indent_level--;
    printIndent();
    cout << "}" << endl;
}

void PrintVisitor::visit(StructDeclaration* struct_decl) {
    printIndent();
    cout << "struct " << struct_decl->struct_name << " {" << endl;
    indent_level++;
    if (struct_decl->members) {
        for (auto member : struct_decl->members->vardecs) {
            printIndent();
            member->types[0]->accept(this);
            cout << " ";
            for (size_t i = 0; i < member->vars.size(); ++i) {
                cout << member->vars[i];
                if (i < member->vars.size() - 1) {
                    cout << ", ";
                }
            }
            cout << ";" << endl;
        }
    }
    indent_level--;
    printIndent();
    cout << "};" << endl << endl;
}

void PrintVisitor::visit(StructDeclarationList* struct_list) {
    for (auto& struct_decl : struct_list->structs) {
        struct_decl->accept(this);
    }
}

void PrintVisitor::printIndent() {
    for (int i = 0; i < indent_level; ++i) {
        cout << "    "; 
    }
}

void PrintVisitor::imprimir(Program* program) {
    if (program->includes) {
        program->includes->accept(this);
        cout << endl;
    }

    if (program->global_declarations) {
        program->global_declarations->accept(this);
        cout << endl;
    }

    if (program->struct_declarations) {
        program->struct_declarations->accept(this);
    }

    if (program->functions) {
        program->functions->accept(this);
    }

    if (program->main_function) {
        program->main_function->accept(this);
    }
}
