#include <stdexcept>
#include "visitor.h"
#include <iostream>
#include "expression.h"
#include <unordered_map>
#include <algorithm>
#include <iterator>
using namespace std;

int GenCodeVisitor::calcular_stack_body(Body* body) {
    int stack = 0;
    for (auto elem : body->elements) {
        if (auto vardec = dynamic_cast<VarDec*>(elem)) {
            for (size_t i = 0; i < vardec->vars.size(); ++i) {
                int var_size = 8;
                std::string tname = vardec->types[i]->type_name;
                
                if (vardec->types[i]->is_pointer) {
                    var_size = 8;
                }
                else if (tname == "char" || tname == "bool")
                    var_size = 1;
                else if (tname == "int")
                    var_size = 8; 
                else if (tname.find("struct") == 0) {
                    std::string struct_name = tname.substr(7);
                    var_size = env->get_struct_size(struct_name);
                }
                
                if (var_size < 8) var_size = 8;
                stack += var_size;
            }
        }
        else if (auto forstm = dynamic_cast<ForStatement*>(elem)) {
            if (forstm->init) {
                if (auto vardec = dynamic_cast<VarDec*>(forstm->init)) {
                    for (size_t i = 0; i < vardec->vars.size(); ++i) {
                        int var_size = 8; 
                        std::string tname = vardec->types[i]->type_name;
                        
                        if (vardec->types[i]->is_pointer) {
                            var_size = 8;  
                        }
                        else if (tname == "char" || tname == "bool")
                            var_size = 1;
                        else if (tname == "int")
                            var_size = 8;  
                        else if (tname.find("struct") == 0) {
                            std::string struct_name = tname.substr(7);
                            var_size = env->get_struct_size(struct_name);
                        }
                        
                        if (var_size < 8) var_size = 8;
                        stack += var_size;
                    }
                }
            }
            if (forstm->b)
                stack += calcular_stack_body(forstm->b);
        }
        else if (auto ifstm = dynamic_cast<IfStatement*>(elem)) {
            if (ifstm->statements)
                stack += calcular_stack_body(ifstm->statements);
            if (ifstm->elsChain) {
                if (auto elseif = dynamic_cast<ElseIfStatement*>(ifstm->elsChain)) {
                    if (elseif->body)
                        stack += calcular_stack_body(elseif->body);
                    if (elseif->nextChain) {
                        if (auto nextElseIf = dynamic_cast<ElseIfStatement*>(elseif->nextChain)) {
                            if (nextElseIf->body)
                                stack += calcular_stack_body(nextElseIf->body);
                        }
                    }
                }
            }
        }
        else if (auto whilestm = dynamic_cast<WhileStatement*>(elem)) {
            if (whilestm->b)
                stack += calcular_stack_body(whilestm->b);
        }
        else if (auto body2 = dynamic_cast<Body*>(elem)) {
            stack += calcular_stack_body(body2);
        }
    }
    return stack;
}

void GenCodeVisitor::gencode(Program* program) {
    out << ".data" << endl;
    out << "print_fmt: .string \"%ld\\n\"" << endl;
    
    if(program->struct_declarations){
        program->struct_declarations->accept(this);
    }
    
    if (program->global_declarations) {
        for (auto dec : program->global_declarations->global_vardecs) {
            out << dec->var_name << ": .quad 0" << endl;
        }
    }
    
    out << ".text" << endl;
    out << ".globl main" << endl;
    
    if (program->functions) {
        program->functions->accept(this);
    }
    
    out << "main:" << endl;
    out << " pushq %rbp" << endl;
    out << " movq %rsp, %rbp" << endl;
    
    int stack_space = 0;
    if (program->main_function && program->main_function->body) {
        stack_space = calcular_stack_body(program->main_function->body);
    }
    
    if (stack_space % 16 != 0) {
        stack_space = ((stack_space + 15) / 16) * 16;
    }
    
    if (stack_space > 0) {
        out << " subq $" << stack_space << ", %rsp" << endl;
    }
    
    if (program->global_declarations) {
    for (auto dec : program->global_declarations->global_vardecs) {
        if (dec->type->is_array) {
            int array_size = 64; 
            if (dec->type->array_size) {
                array_size = dec->type->array_size->accept(this);
            }
            out << dec->var_name << ": .space " << array_size << endl;
        } else {
            out << dec->var_name << ": .quad 0" << endl;
        }
    }
}
    
    env->add_level();
    if (program->main_function) {
        program->main_function->accept(this);
    }
    env->remove_level();
    
    out << ".section .note.GNU-stack,\"\",@progbits" << endl;
}


int GenCodeVisitor::visit(NumberExp* exp) {
    out << " movq $" << exp->value << ", %rax" << endl;
    return exp->value;
}

int GenCodeVisitor::visit(CharExp* exp) {
    out << " movq $" << (int)exp->value << ", %rax" << endl;
    return (int)exp->value;
}

int GenCodeVisitor::visit(BoolExp* exp) {
    int bool_val = exp->value ? 1 : 0;
    out << " movq $" << bool_val << ", %rax" << endl;
    return bool_val;
}

int GenCodeVisitor::visit(StringExp* exp) {
    string label = "string_" + to_string(cantidad++);
    
    out << ".section .rodata" << endl;
    out << label << ": .string " << exp->value << endl;
    
    out << ".text" << endl;
    
    out << " leaq " << label << "(%rip), %rax" << endl;
    
    return 0; 
}

int GenCodeVisitor::visit(IdentifierExp* exp) {
    if (env->check(exp->name) == false) {
        cout << "Variable no declarada: " << exp->name << endl;
        exit(0);
    }
    
    VarInfo info = env->lookup(exp->name);
    
    const char* regs[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    out << "# Cargando valor de " << exp->name << " " << info.is_reference << " " << info.reg_index << endl;
    
    if (info.is_reference && info.reg_index >= 0) {
        out << " movq (" << regs[info.reg_index] << "), %rax # Valor de* " << exp->name << endl;
    } else if (info.reg_index >= 0) {
        out << " movq " << regs[info.reg_index] << ", %rax # Valor de " << exp->name << endl;
    } else if (!info.is_global) {
        out << " movq " << info.offset << "(%rbp), %rax # " << exp->name << endl;
    } else {
        out << " movq " << exp->name << "(%rip), %rax # " << exp->name << endl;
    }
    
    return 0;
}

void GenCodeVisitor::get_member_address(MemberAccessExp* exp) {
    int base_offset = 0;
    std::string struct_type;
    bool is_local_struct = false;
    
    if (auto id = dynamic_cast<IdentifierExp*>(exp->object)) {
        struct_type = env->lookup_type(id->name);
        if (struct_type.rfind("struct ", 0) == 0) {
            struct_type = struct_type.substr(7);
        }
        VarInfo info = env->lookup(id->name);
        base_offset = info.offset;
        is_local_struct = !info.is_global;
    }
    
    int member_offset = 0;
    if (env->has_struct(struct_type)) {
        member_offset = env->get_struct_member_offset(struct_type, exp->member_name);
    } else {
        member_offset = 8;
    }
    
    if (exp->is_pointer) {
        exp->object->accept(this); 
        out << " addq $" << member_offset << ", %rax # dirección de " << exp->member_name << endl;
    } else if (is_local_struct) {
        out << " leaq " << (base_offset + member_offset) << "(%rbp), %rax # dirección de " << exp->member_name << " en struct local" << endl;
    } else {
        exp->object->accept(this);
        out << " addq $" << member_offset << ", %rax # dirección de " << exp->member_name << endl;
    }
}

int GenCodeVisitor::visit(AssignExp* exp) {
    if (exp->assign_op == ASSIGN_OP) {
        exp->right->accept(this);
        out << " pushq %rax" << endl;
        
        if (auto id = dynamic_cast<IdentifierExp*>(exp->left)) {
            if (env->check(id->name) == false) {
                cout << "Variable no declarada: " << id->name << endl;
                exit(0);
            }
            
            VarInfo info = env->lookup(id->name);
            out << " popq %rax" << endl;
            const char* regs[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
            
            if (info.is_reference && info.reg_index >= 0 && info.reg_index < 6) {
                out << " movq %rax, (" << regs[info.reg_index] << ") # guardar en *" << id->name << endl;
            } else if (info.reg_index >= 0 && info.reg_index < 6) {
                out << " movq %rax, " << regs[info.reg_index] << " # guardar en parámetro " << id->name << endl;
            } else if (!info.is_global) {
                out << " movq %rax, " << info.offset << "(%rbp) # guardar en local " << id->name << endl;
            } else {
                out << " movq %rax, " << id->name << "(%rip) # guardar en global " << id->name << endl;
            }
        } else if (auto unary = dynamic_cast<UnaryExp*>(exp->left)) {
            if (unary->op == DEREFERENCE_OP) {
                unary->uexp->accept(this); 
                out << " movq %rax, %rbx # guardar dirección" << endl;
                out << " popq %rax # recuperar valor a asignar" << endl;
                out << " movq %rax, (%rbx) # *ptr = valor" << endl;
            } else {
                cerr << "Error: Operador unario no soportado en asignación" << endl;
                exit(1);
            }
        } else if (auto member = dynamic_cast<MemberAccessExp*>(exp->left)) {
            out << " popq %rax # recuperar valor a asignar" << endl;
            out << " pushq %rax # guardar valor temporalmente" << endl;
            
            get_member_address(member); 
            out << " movq %rax, %rbx # guardar dirección" << endl;
            out << " popq %rax # recuperar valor" << endl;
            out << " movq %rax, (%rbx) # escribir valor en member" << endl;
        } else if (auto array = dynamic_cast<ArrayAccessExp*>(exp->left)) {
            array->array->accept(this); 
            out << " pushq %rax # guardar dirección array" << endl;
            array->index->accept(this); 
            out << " imulq $8, %rax # multiplicar por tamaño del elemento" << endl;
            out << " popq %rbx # recuperar dirección array" << endl;
            out << " addq %rbx, %rax # calcular dirección final" << endl;
            out << " movq %rax, %rbx # guardar dirección final" << endl;
            out << " popq %rax # recuperar valor a asignar" << endl;
            out << " movq %rax, (%rbx) # arr[index] = valor" << endl;
        } else {
            cerr << "Error: Tipo de expresión no soportado en asignación" << endl;
            exit(1);
        }
    } 
    else if (exp->assign_op == PLUS_EQUAL_OP ||
             exp->assign_op == MINUS_EQUAL_OP ||
             exp->assign_op == MULTIPLY_EQUAL_OP ||
             exp->assign_op == DIVIDE_EQUAL_OP ||
             exp->assign_op == MODULO_EQUAL_OP) {
        
        if (auto id = dynamic_cast<IdentifierExp*>(exp->left)) {
            if (env->check(id->name) == false) {
                cout << "Variable no declarada: " << id->name << endl;
                exit(0);
            }
            
            VarInfo info = env->lookup(id->name);
            
            exp->right->accept(this);
            out << " movq %rax, %rbx # guardar operando derecho" << endl;
            
            const char* regs[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
            
            if (info.is_reference && info.reg_index >= 0 && info.reg_index < 6) {
                out << " movq (" << regs[info.reg_index] << "), %rax # cargar *" << id->name << endl;
            } else if (info.reg_index >= 0 && info.reg_index < 6) {
                out << " movq " << regs[info.reg_index] << ", %rax # cargar parámetro " << id->name << endl;
            } else if (!info.is_global) {
                out << " movq " << info.offset << "(%rbp), %rax # cargar local " << id->name << endl;
            } else {
                out << " movq " << id->name << "(%rip), %rax # cargar global " << id->name << endl;
            }
            
            switch (exp->assign_op) {
                case PLUS_EQUAL_OP:
                    out << " addq %rbx, %rax" << endl;
                    break;
                case MINUS_EQUAL_OP:
                    out << " subq %rbx, %rax" << endl;
                    break;
                case MULTIPLY_EQUAL_OP:
                    out << " imulq %rbx, %rax" << endl;
                    break;
                case DIVIDE_EQUAL_OP:
                    out << " cqo" << endl;
                    out << " idivq %rbx" << endl;
                    break;
                case MODULO_EQUAL_OP:
                    out << " cqo" << endl;
                    out << " idivq %rbx" << endl;
                    out << " movq %rdx, %rax" << endl;
                    break;
                default:
                    break;
            }
            
            if (info.is_reference && info.reg_index >= 0 && info.reg_index < 6) {
                out << " movq %rax, (" << regs[info.reg_index] << ") # guardar en *" << id->name << endl;
            } else if (info.reg_index >= 0 && info.reg_index < 6) {
                out << " movq %rax, " << regs[info.reg_index] << " # guardar en parámetro " << id->name << endl;
            } else if (!info.is_global) {
                out << " movq %rax, " << info.offset << "(%rbp) # guardar en local " << id->name << endl;
            } else {
                out << " movq %rax, " << id->name << "(%rip) # guardar en global " << id->name << endl;
            }
        } else if (auto unary = dynamic_cast<UnaryExp*>(exp->left)) {
            if (unary->op == DEREFERENCE_OP) {
                exp->right->accept(this);
                out << " movq %rax, %rbx # guardar operando derecho" << endl;
                
                unary->uexp->accept(this);
                out << " movq %rax, %rcx # guardar dirección del puntero" << endl;
                
                out << " movq (%rcx), %rax # cargar *ptr" << endl;
                
                switch (exp->assign_op) {
                    case PLUS_EQUAL_OP:
                        out << " addq %rbx, %rax" << endl;
                        break;
                    case MINUS_EQUAL_OP:
                        out << " subq %rbx, %rax" << endl;
                        break;
                    case MULTIPLY_EQUAL_OP:
                        out << " imulq %rbx, %rax" << endl;
                        break;
                    case DIVIDE_EQUAL_OP:
                        out << " cqo" << endl;
                        out << " idivq %rbx" << endl;
                        break;
                    case MODULO_EQUAL_OP:
                        out << " cqo" << endl;
                        out << " idivq %rbx" << endl;
                        out << " movq %rdx, %rax" << endl;
                        break;
                    default:
                        break;
                }
                
                out << " movq %rax, (%rcx) # guardar resultado en *ptr" << endl;
            } else {
                cerr << "Error: Operador unario no soportado en asignación compuesta" << endl;
                exit(1);
            }
        } else if (auto member = dynamic_cast<MemberAccessExp*>(exp->left)) {
            exp->right->accept(this);
            out << " pushq %rax # guardar operando derecho" << endl;
            
            get_member_address(member); 
            out << " pushq %rax # guardar dirección del member" << endl;
            
            out << " movq (%rax), %rax # cargar valor actual del member" << endl;
            
            out << " popq %rcx # recuperar dirección" << endl;
            out << " popq %rbx # recuperar operando derecho" << endl;
            
            switch (exp->assign_op) {
                case PLUS_EQUAL_OP:
                    out << " addq %rbx, %rax" << endl;
                    break;
                case MINUS_EQUAL_OP:
                    out << " subq %rbx, %rax" << endl;
                    break;
                case MULTIPLY_EQUAL_OP:
                    out << " imulq %rbx, %rax" << endl;
                    break;
                case DIVIDE_EQUAL_OP:
                    out << " cqo" << endl;
                    out << " idivq %rbx" << endl;
                    break;
                case MODULO_EQUAL_OP:
                    out << " cqo" << endl;
                    out << " idivq %rbx" << endl;
                    out << " movq %rdx, %rax" << endl;
                    break;
                default:
                    break;
            }
            
            out << " movq %rax, (%rcx) # guardar resultado en member" << endl;
            
        } else if (auto array = dynamic_cast<ArrayAccessExp*>(exp->left)) {
            exp->right->accept(this);
            out << " movq %rax, %rbx # guardar operando derecho" << endl;
            
            array->array->accept(this);
            out << " pushq %rax # guardar dirección array" << endl;
            array->index->accept(this);
            out << " imulq $8, %rax # multiplicar por tamaño del elemento" << endl;
            out << " popq %rcx # recuperar dirección array" << endl;
            out << " addq %rcx, %rax # calcular dirección final" << endl;
            out << " movq %rax, %rcx # guardar dirección final" << endl;
            
            out << " movq (%rcx), %rax # cargar arr[index]" << endl;
            
            switch (exp->assign_op) {
                case PLUS_EQUAL_OP:
                    out << " addq %rbx, %rax" << endl;
                    break;
                case MINUS_EQUAL_OP:
                    out << " subq %rbx, %rax" << endl;
                    break;
                case MULTIPLY_EQUAL_OP:
                    out << " imulq %rbx, %rax" << endl;
                    break;
                case DIVIDE_EQUAL_OP:
                    out << " cqo" << endl;
                    out << " idivq %rbx" << endl;
                    break;
                case MODULO_EQUAL_OP:
                    out << " cqo" << endl;
                    out << " idivq %rbx" << endl;
                    out << " movq %rdx, %rax" << endl;
                    break;
                default:
                    break;
            }
            
            out << " movq %rax, (%rcx) # guardar resultado en arr[index]" << endl;
        } else {
            cerr << "Error: Asignación compuesta solo soportada para identificadores, punteros, miembros y arrays" << endl;
            exit(1);
        }
    }
    return 0;
}


int GenCodeVisitor::visit(UnaryExp* exp) {
    exp->uexp->accept(this);

    switch (exp->op) {
        case NEGACION_OP:
            out << "    testq %rax, %rax" << endl;
            out << "    movq $0, %rcx" << endl;
            out << "    sete %cl" << endl;
            out << "    movzbq %cl, %rax" << endl;  
            break;
        case UNARY_MINUS_OP:
            out << "    negq %rax" << endl;
            break;
        case UNARY_PLUS_OP:
            break;
        
        case PLUS_PLUS_OP: {
            if (auto id = dynamic_cast<IdentifierExp*>(exp->uexp)) {
                if (!env->check(id->name)) {
                    cerr << "Error: Variable no declarada: " << id->name << endl;
                    exit(1);
                }
                
                VarInfo info = env->lookup(id->name);
                const char* regs[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};

                if (info.is_reference && info.reg_index >= 0 && info.reg_index < 6) {
                    if (exp->is_prefix) {
                        out << "    incq %rax" << endl;
                        out << "    movq %rax, (" << regs[info.reg_index] << ")  # ++*ref" << endl;
                    } else {
                        out << "    movq %rax, %rcx" << endl;
                        out << "    incq %rcx" << endl;
                        out << "    movq %rcx, (" << regs[info.reg_index] << ")  # *ref++" << endl;
                    }
                } else if (info.reg_index >= 0 && info.reg_index < 6) {
                    if (exp->is_prefix) {
                        out << "    incq %rax" << endl;
                        out << "    movq %rax, " << regs[info.reg_index] << "  # ++param" << endl;
                    } else {
                        out << "    movq %rax, %rcx" << endl;
                        out << "    incq %rcx" << endl;
                        out << "    movq %rcx, " << regs[info.reg_index] << "  # param++" << endl;
                    }
                } else if (!info.is_global) {
                    int offset = info.offset;
                    if (exp->is_prefix) {
                        out << "    incq %rax" << endl;
                        out << "    movq %rax, " << offset << "(%rbp)" << endl;
                    } else {
                        out << "    movq %rax, %rcx" << endl;
                        out << "    incq %rcx" << endl;
                        out << "    movq %rcx, " << offset << "(%rbp)" << endl;
                    }
                } else {
                    if (exp->is_prefix) {
                        out << "    incq %rax" << endl;
                        out << "    movq %rax, " << id->name << "(%rip)" << endl;
                    } else {
                        out << "    movq %rax, %rcx" << endl;
                        out << "    incq %rcx" << endl;
                        out << "    movq %rcx, " << id->name << "(%rip)" << endl;
                    }
                }
            } else if (auto member = dynamic_cast<MemberAccessExp*>(exp->uexp)) {
                get_member_address(member); 
                out << "    movq %rax, %rbx # guardar dirección" << endl;
                out << "    movq (%rbx), %rax # cargar valor actual" << endl;
                
                if (exp->is_prefix) {
                    out << "    incq %rax" << endl;
                    out << "    movq %rax, (%rbx) # ++member" << endl;
                } else {
                    out << "    movq %rax, %rcx # guardar valor original" << endl;
                    out << "    incq %rax" << endl;
                    out << "    movq %rax, (%rbx) # member++" << endl;
                    out << "    movq %rcx, %rax # retornar valor original" << endl;
                }
            } else if (auto unary = dynamic_cast<UnaryExp*>(exp->uexp)) {
                if (unary->op == DEREFERENCE_OP) {
                    unary->uexp->accept(this); 
                    out << "    movq %rax, %rbx # guardar dirección" << endl;
                    out << "    movq (%rbx), %rax # cargar *ptr" << endl;
                    
                    if (exp->is_prefix) {
                        out << "    incq %rax" << endl;
                        out << "    movq %rax, (%rbx) # ++(*ptr)" << endl;
                    } else {
                        out << "    movq %rax, %rcx # guardar valor original" << endl;
                        out << "    incq %rax" << endl;
                        out << "    movq %rax, (%rbx) # (*ptr)++" << endl;
                        out << "    movq %rcx, %rax # retornar valor original" << endl;
                    }
                } else {
                    cerr << "Error: Operador ++ no soportado para esta expresión unaria" << endl;
                    exit(1);
                }
            } else {
                cerr << "Error: Operador ++ solo soportado para identificadores, miembros y punteros" << endl;
                exit(1);
            }
            break;
        }
        
        case MINUS_MINUS_OP: {
            if (auto id = dynamic_cast<IdentifierExp*>(exp->uexp)) {
                if (!env->check(id->name)) {
                    cerr << "Error: Variable no declarada: " << id->name << endl;
                    exit(1);
                }
                
                VarInfo info = env->lookup(id->name);
                const char* regs[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};

                if (info.is_reference && info.reg_index >= 0 && info.reg_index < 6) {
                    if (exp->is_prefix) {
                        out << "    decq %rax" << endl;
                        out << "    movq %rax, (" << regs[info.reg_index] << ")  # --*ref" << endl;
                    } else {
                        out << "    movq %rax, %rcx" << endl;
                        out << "    decq %rcx" << endl;
                        out << "    movq %rcx, (" << regs[info.reg_index] << ")  # *ref--" << endl;
                    }
                } else if (info.reg_index >= 0 && info.reg_index < 6) {
                    if (exp->is_prefix) {
                        out << "    decq %rax" << endl;
                        out << "    movq %rax, " << regs[info.reg_index] << "  # --param" << endl;
                    } else {
                        out << "    movq %rax, %rcx" << endl;
                        out << "    decq %rcx" << endl;
                        out << "    movq %rcx, " << regs[info.reg_index] << "  # param--" << endl;
                    }
                } else if (!info.is_global) {
                    int offset = info.offset;
                    if (exp->is_prefix) {
                        out << "    decq %rax" << endl;
                        out << "    movq %rax, " << offset << "(%rbp)" << endl;
                    } else {
                        out << "    movq %rax, %rcx" << endl;
                        out << "    decq %rcx" << endl;
                        out << "    movq %rcx, " << offset << "(%rbp)" << endl;
                    }
                } else {
                    if (exp->is_prefix) {
                        out << "    decq %rax" << endl;
                        out << "    movq %rax, " << id->name << "(%rip)" << endl;
                    } else {
                        out << "    movq %rax, %rcx" << endl;
                        out << "    decq %rcx" << endl;
                        out << "    movq %rcx, " << id->name << "(%rip)" << endl;
                    }
                }
            } else if (auto member = dynamic_cast<MemberAccessExp*>(exp->uexp)) {
                get_member_address(member); 
                out << "    movq %rax, %rbx # guardar dirección" << endl;
                out << "    movq (%rbx), %rax # cargar valor actual" << endl;
                
                if (exp->is_prefix) {
                    out << "    decq %rax" << endl;
                    out << "    movq %rax, (%rbx) # --member" << endl;
                } else {
                    out << "    movq %rax, %rcx # guardar valor original" << endl;
                    out << "    decq %rax" << endl;
                    out << "    movq %rax, (%rbx) # member--" << endl;
                    out << "    movq %rcx, %rax # retornar valor original" << endl;
                }
            } else if (auto unary = dynamic_cast<UnaryExp*>(exp->uexp)) {
                if (unary->op == DEREFERENCE_OP) {
                    unary->uexp->accept(this); 
                    out << "    movq %rax, %rbx # guardar dirección" << endl;
                    out << "    movq (%rbx), %rax # cargar *ptr" << endl;
                    
                    if (exp->is_prefix) {
                        out << "    decq %rax" << endl;
                        out << "    movq %rax, (%rbx) # --(*ptr)" << endl;
                    } else {
                        out << "    movq %rax, %rcx # guardar valor original" << endl;
                        out << "    decq %rax" << endl;
                        out << "    movq %rax, (%rbx) # (*ptr)--" << endl;
                        out << "    movq %rcx, %rax # retornar valor original" << endl;
                    }
                } else {
                    cerr << "Error: Operador -- no soportado para esta expresión unaria" << endl;
                    exit(1);
                }
            } else {
                cerr << "Error: Operador -- solo soportado para identificadores, miembros y punteros" << endl;
                exit(1);
            }
            break;
        }
        
        case ADDRESS_OF_OP: {
            if (auto id = dynamic_cast<IdentifierExp*>(exp->uexp)) {
                if (!env->check(id->name)) {
                    cerr << "Error: Variable no declarada: " << id->name << endl;
                    exit(1);
                }
                VarInfo info = env->lookup(id->name);
                if (!info.is_global) {
                    out << "    leaq " << info.offset << "(%rbp), %rax  # &" << id->name << endl;
                } else {
                    out << "    leaq " << id->name << "(%rip), %rax  # &" << id->name << endl;
                }
            } else if (auto member = dynamic_cast<MemberAccessExp*>(exp->uexp)) {
                get_member_address(member); 
                out << "    # &member (dirección ya calculada)" << endl;
            } else {
                cerr << "Error: Operador & solo soportado para identificadores y miembros" << endl;
                exit(1);
            }
            break;
        }
        case DEREFERENCE_OP:
            out << "    movq (%rax), %rax  # *ptr" << endl;
            break;
        default:
            out << "    # Operador unario no implementado: " << exp->op << endl;
            break;
    }
    return 0;
}


int GenCodeVisitor::visit(BinaryExp* exp) {
    switch (exp->op) {
        case PLUS_OP:
            exp->left->accept(this);    
            out << "    movq %rax, %rdx" << endl;
            exp->right->accept(this);   
            out << "    addq %rdx, %rax" << endl;
            break;
        case MINUS_OP:
            exp->left->accept(this);
            out << "    movq %rax, %rdx" << endl;
            exp->right->accept(this);
            out << "    subq %rax, %rdx" << endl;
            out << "    movq %rdx, %rax" << endl;
            break;
        case MULT_OP:
            exp->left->accept(this);
            out << "    movq %rax, %rdx" << endl;
            exp->right->accept(this);
            out << "    imulq %rdx, %rax" << endl;
            break;
        case DIV_OP:
            exp->left->accept(this);
            out << "    movq %rax, %rdx" << endl;
            exp->right->accept(this);
            out << "    movq %rax, %rcx" << endl;
            out << "    movq %rdx, %rax" << endl;
            out << "    cqo" << endl;
            out << "    idivq %rcx" << endl;
            break;
        case MOD_OP:
            exp->left->accept(this);
            out << "    movq %rax, %rdx" << endl;
            exp->right->accept(this);
            out << "    movq %rax, %rcx" << endl;
            out << "    movq %rdx, %rax" << endl;
            out << "    cqo" << endl;
            out << "    idivq %rcx" << endl;
            out << "    movq %rdx, %rax" << endl;
            break;
        case EQUAL_OP:
            exp->left->accept(this);
            out << "    movq %rax, %rdx" << endl;
            exp->right->accept(this);
            out << "    cmpq %rax, %rdx" << endl;  
            out << "    movq $0, %rax" << endl;
            out << "    sete %al" << endl;
            out << "    movzbq %al, %rax" << endl;
            break;
        case NOT_EQUAL_OP:
            exp->left->accept(this);
            out << "    movq %rax, %rdx" << endl;
            exp->right->accept(this);
            out << "    cmpq %rax, %rdx" << endl;
            out << "    movq $0, %rax" << endl;
            out << "    setne %al" << endl;
            out << "    movzbq %al, %rax" << endl;
            break;
        case LESS_THAN_OP:
            exp->left->accept(this);
            out << "    movq %rax, %rdx" << endl;
            exp->right->accept(this);
            out << "    cmpq %rax, %rdx" << endl;
            out << "    movq $0, %rax" << endl;
            out << "    setl %al" << endl;
            out << "    movzbq %al, %rax" << endl;
            break;
        case GREATER_THAN_OP:
            exp->left->accept(this);
            out << "    movq %rax, %rdx" << endl;
            exp->right->accept(this);
            out << "    cmpq %rax, %rdx" << endl;
            out << "    movq $0, %rax" << endl;
            out << "    setg %al" << endl;
            out << "    movzbq %al, %rax" << endl;
            break;
        case LESS_EQUAL_OP:
            exp->left->accept(this);
            out << "    movq %rax, %rdx" << endl;
            exp->right->accept(this);
            out << "    cmpq %rax, %rdx" << endl;
            out << "    movq $0, %rax" << endl;
            out << "    setle %al" << endl;
            out << "    movzbq %al, %rax" << endl;
            break;
        case GREATER_EQUAL_OP:
            exp->left->accept(this);
            out << "    movq %rax, %rdx" << endl;
            exp->right->accept(this);
            out << "    cmpq %rax, %rdx" << endl;
            out << "    movq $0, %rax" << endl;
            out << "    setge %al" << endl;
            out << "    movzbq %al, %rax" << endl;
            break;
        case LOGICAL_AND_OP: {
            int label_false = cantidad++;
            int label_end = cantidad++;
            exp->left->accept(this);
            out << "    testq %rax, %rax" << endl;
            out << "    jz .Lfalse" << label_false << endl;
            exp->right->accept(this);
            out << "    testq %rax, %rax" << endl;
            out << "    movq $1, %rax" << endl;
            out << "    jnz .Lend" << label_end << endl;
            out << ".Lfalse" << label_false << ":" << endl;
            out << "    movq $0, %rax" << endl;
            out << ".Lend" << label_end << ":" << endl;
            break;
        }
        case LOGICAL_OR_OP: {
            int label_true = cantidad++;
            int label_end = cantidad++;
            exp->left->accept(this);
            out << "    testq %rax, %rax" << endl;
            out << "    jnz .Ltrue" << label_true << endl;
            exp->right->accept(this);
            out << "    testq %rax, %rax" << endl;
            out << "    movq $0, %rax" << endl;
            out << "    jz .Lend" << label_end << endl;
            out << ".Ltrue" << label_true << ":" << endl;
            out << "    movq $1, %rax" << endl;
            out << ".Lend" << label_end << ":" << endl;
            break;
        }
        case ASSIGN_OP: {
            cerr << "Error: ASSIGN_OP encontrado en BinaryExp - debería ser AssignExp" << endl;
            exit(1);
            break;
        }
        default:
            out << "    # Operador binario no implementado: " << exp->op << endl;
            break;
    }
    return 0;
}

int GenCodeVisitor::visit(ArrayAccessExp* exp) {
    exp->array->accept(this);
    out << " pushq %rax # guardar dirección base del array" << endl;
    
    exp->index->accept(this);
    out << " popq %rcx # recuperar dirección base" << endl;
    
    out << " leaq (%rcx,%rax,8), %rax # calcular dirección del elemento" << endl;
    out << " movq (%rax), %rax # cargar valor del elemento" << endl;
    return 0;
}

int GenCodeVisitor::visit(MemberAccessExp* exp) {
    if (auto id = dynamic_cast<IdentifierExp*>(exp->object)) {
        if (!env->check(id->name)) {
            cerr << "Error: Variable no declarada: " << id->name << endl;
            exit(1);
        }
    }
    
    int base_offset = 0;
    std::string struct_type;
    bool is_local_struct = false;
    
    if (auto id = dynamic_cast<IdentifierExp*>(exp->object)) {
        struct_type = env->lookup_type(id->name);
        if (struct_type.rfind("struct ", 0) == 0) {
            struct_type = struct_type.substr(7);
        }
        VarInfo info = env->lookup(id->name);
        base_offset = info.offset;
        is_local_struct = !info.is_global;
    }
    
    int member_offset = 0;
    if (env->has_struct(struct_type)) {
        member_offset = env->get_struct_member_offset(struct_type, exp->member_name);
    } else {
        member_offset = 8;
    }
    
    if (exp->is_pointer) {
        exp->object->accept(this); 
        out << " addq $" << member_offset << ", %rax # offset del miembro " << exp->member_name << endl;
        out << " movq (%rax), %rax # cargar valor del miembro" << endl;
    } else if (is_local_struct) {
        out << " movq " << (base_offset + member_offset) << "(%rbp), %rax # " << exp->member_name << " de struct local" << endl;
    } else {
        exp->object->accept(this); 
        out << " addq $" << member_offset << ", %rax # offset del miembro " << exp->member_name << endl;
        out << " movq (%rax), %rax # cargar valor del miembro" << endl;
    }
    return 0;
}


int GenCodeVisitor::visit(ParenExp* exp) {
    return exp->inner->accept(this);
}

int GenCodeVisitor::visit(ArrayInitializerExp* exp) {
    out << "    pushq %rax # guardar dirección base del array" << endl;
    
    for (size_t i = 0; i < exp->elements.size(); ++i) {
        exp->elements[i]->accept(this); 
        out << "    popq %rbx # recuperar dirección base" << endl;
        out << "    movq %rax, " << (i * 8) << "(%rbx) # array[" << i << "] = valor" << endl;
        out << "    pushq %rbx # guardar dirección base para siguiente iteración" << endl;
    }
    
    out << "    popq %rax # dirección base final en %rax" << endl;
    return 0;
}

void GenCodeVisitor::visit(VarDec* stm) {
    for (size_t i = 0; i < stm->vars.size(); ++i) {
        string var_name = stm->vars[i];
        Type* var_type = stm->types[i];
        bool is_char = (var_type->type_name == "char");
        bool is_bool = (var_type->type_name == "bool");
        bool is_int = (var_type->type_name == "int");
        bool is_ptr = var_type->is_pointer;
        bool is_array = var_type->is_array;
        bool is_reference = var_type->is_reference;
        bool is_struct = (var_type->type_name.find("struct") == 0);

        int var_size = 8;
        if (is_struct && !is_ptr) {
            string struct_name = var_type->type_name.substr(7);
            var_size = env->get_struct_size(struct_name);
        }
        
        string mov_inst = "movq";
        string reg = "%rax";
        
        int current_offset = env->lookup(var_name).offset;
        
        if (is_array && var_type->array_size) {
            var_type->array_size->accept(this); 
            
            int element_size = 8; 
            if (is_char && !is_ptr) {
                element_size = 1;
            }
            
            out << "    imulq $" << element_size << ", %rax # elementos * tamaño" << endl;
            out << "    movq %rax, %rcx # guardar tamaño total" << endl;
            out << "    subq %rcx, %rsp # reservar espacio en stack" << endl;
            out << "    movq %rsp, " << current_offset << "(%rbp) # guardar puntero al array" << endl;

            if (stm->initializers[i]) {
                if (auto array_init = dynamic_cast<ArrayInitializerExp*>(stm->initializers[i])) {
                    out << "    movq %rsp, %rax # dirección base del array" << endl;
                    array_init->accept(this);
                } else {
                    stm->initializers[i]->accept(this); 
                    out << "    movq %rax, %rbx # guardar valor" << endl;
                    out << "    movq %rsp, %rdi # dirección base" << endl;
                    out << "    movq %rcx, %rcx # contador de bytes" << endl;
                    out << "    shrq $3, %rcx # convertir a número de elementos de 8 bytes" << endl;
                    string loop_label = "init_loop_" + to_string(cantidad++);
                    string end_label = "init_end_" + to_string(cantidad++);
                    out << loop_label << ":" << endl;
                    out << "    testq %rcx, %rcx" << endl;
                    out << "    jz " << end_label << endl;
                    out << "    movq %rbx, (%rdi)" << endl;
                    out << "    addq $8, %rdi" << endl;
                    out << "    decq %rcx" << endl;
                    out << "    jmp " << loop_label << endl;
                    out << end_label << ":" << endl;
                }
            } else {
                out << "    movq %rsp, %rdi # dirección base" << endl;
                out << "    xorq %rax, %rax # valor 0" << endl;
                out << "    rep stosb # llenar con ceros" << endl;
            }
            continue;
        }

        env->add_var(var_name, current_offset, var_type->type_name, is_ptr, is_array, is_reference, -1, false);
        
        if (auto struct_init = dynamic_cast<StructInitializerExp*>(stm->initializers[i])) {
            struct_init->accept(this, current_offset);
        }
        else if (stm->initializers[i]) {
            stm->initializers[i]->accept(this); 
            out << "    movq %rax, " << current_offset << "(%rbp) # " << var_name << endl;
            
            if (is_bool) {
                out << "    andq $1, " << current_offset << "(%rbp) # asegurar 0/1 para bool" << endl;
            }
        } else {
            out << "    movq $0, " << current_offset << "(%rbp) # inicializar " << var_name << " a 0" << endl;
        }
    }
}

void GenCodeVisitor::visit(VarDecList* stm) {
    for (auto vardec : stm->vardecs) {
        vardec->accept(this);
    }
}

void GenCodeVisitor::visit(GlobalVarDec* dec) {
    bool is_ptr = dec->type->is_pointer;
    bool is_array = dec->type->is_array;
    bool is_reference = dec->type->is_reference;
    string type_name = dec->type->type_name;
    
    env->add_var(dec->var_name, 0, type_name, is_ptr, is_array, is_reference, -1, true);
}

void GenCodeVisitor::visit(GlobalVarDecList* decList) {
    for (auto dec : decList->global_vardecs) {
        dec->accept(this);
    }
}

void GenCodeVisitor::visit(PrintfStatement* stm) {
    static int fmt_count = 0;
    string fmt_label = "printf_fmt_" + to_string(fmt_count++);
    
    out << ".section .rodata" << endl;
    out << fmt_label << ": .string " << stm->format_string << endl;
    out << ".text" << endl;
    
    
    out << "    leaq " << fmt_label << "(%rip), %rdi" << endl;
    
    const char* arg_regs[] = {"%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    int reg_index = 0;
    int stack_args = 0;
    
    vector<string> arg_code;
    
    for (auto arg : stm->arguments) {
        arg->accept(this); 
        
        if (reg_index < 5) {
            out << "    movq %rax, " << arg_regs[reg_index] << endl;
            reg_index++;
        } else {
            out << "    pushq %rax" << endl;
            stack_args++;
        }
    }
    
    out << "    xorq %rax, %rax # 0 registros vectoriales usados" << endl;
    
    if (stack_args % 2 != 0) {
        out << "    subq $8, %rsp # alinear stack a 16 bytes" << endl;
        stack_args++; 
    }
    
    out << "    call printf@PLT" << endl;
    
    if (stack_args > 0) {
        out << "    addq $" << (stack_args * 8) << ", %rsp" << endl;
    }
    
}


void GenCodeVisitor::visit(IfStatement* stm) {
    if (!stm) return;
    
    int label_else = cantidad++;
    int label_end = cantidad++;
    
    stm->condition->accept(this);
    out << "    testq %rax, %rax" << endl;
    out << "    jz .Lelse" << label_else << endl; 
    
    if (stm->statements)
        stm->statements->accept(this);
    
    if (stm->elsChain) {
        out << "    jmp .Lendif" << label_end << endl;
    }
    
    out << ".Lelse" << label_else << ":" << endl;
    
    if (stm->elsChain) {
        stm->elsChain->accept(this);
    }
    
    if (stm->elsChain) {
        out << ".Lendif" << label_end << ":" << endl;
    }
}

void GenCodeVisitor::visit(ElseIfStatement* stm) {
    if (!stm) return;
    
    if (stm->tipo == ElseIfStatement::ELSE_IF) {
        int label_next_else = cantidad++;
        int label_end = cantidad++;
        
        stm->condition->accept(this);
        out << "    testq %rax, %rax" << endl;
        out << "    jz .Lelse" << label_next_else << endl; 
        
        if (stm->body)
            stm->body->accept(this);
        
        if (stm->nextChain) {
            out << "    jmp .Lendif" << label_end << endl;
        }
        
        out << ".Lelse" << label_next_else << ":" << endl;
        
        if (stm->nextChain) {
            stm->nextChain->accept(this);
        }
        
        if (stm->nextChain) {
            out << ".Lendif" << label_end << ":" << endl;
        }
    }
    else { 
        if (stm->body) {
            stm->body->accept(this); 
        }
    }
}


void GenCodeVisitor::visit(Body* b) {
    
    int current_offset = env->get_current_offset(); 
    
    for (auto elem : b->elements) {
        if (auto vardec = dynamic_cast<VarDec*>(elem)) {
            for (size_t i = 0; i < vardec->vars.size(); ++i) {
                int var_size = 8; 
                
                if (vardec->types[i]->is_pointer) {
                    var_size = 8;
                } else if (vardec->types[i]->type_name == "char" || vardec->types[i]->type_name == "bool") {
                    var_size = 1;
                    if (var_size < 8) var_size = 8; 
                } else if (vardec->types[i]->type_name == "int") {
                    var_size = 8;
                } else if (vardec->types[i]->type_name.find("struct") == 0) {
                    std::string struct_name = vardec->types[i]->type_name.substr(7);
                    var_size = env->get_struct_size(struct_name);
                    if (var_size < 8) var_size = 8; 
                }
                
                current_offset -= var_size;
                
                env->add_var(
                    vardec->vars[i],
                    current_offset,
                    vardec->types[i]->type_name,
                    vardec->types[i]->is_pointer,
                    vardec->types[i]->is_array,
                    vardec->types[i]->is_reference,
                    -1, 
                    false
                );
                
                out << "# Declarando variable: " << vardec->vars[i] 
                    << " en offset " << current_offset << endl;
            }
        }
        else if (auto forstm = dynamic_cast<ForStatement*>(elem)) {
            if (auto vardec = dynamic_cast<VarDec*>(forstm->init)) {
                for (size_t i = 0; i < vardec->vars.size(); ++i) {
                    int var_size = 8;
                    if (vardec->types[i]->type_name.find("struct") == 0) {
                        std::string struct_name = vardec->types[i]->type_name.substr(7);
                        var_size = env->get_struct_size(struct_name);
                    }
                    if (var_size < 8) var_size = 8;
                    
                    current_offset -= var_size;
                    
                    env->add_var(
                        vardec->vars[i],
                        current_offset,
                        vardec->types[i]->type_name,
                        vardec->types[i]->is_pointer,
                        vardec->types[i]->is_array,
                        vardec->types[i]->is_reference,
                        -1,
                        false
                    );
                    
                    out << "# Declarando variable de for: " << vardec->vars[i] 
                        << " en offset " << current_offset << endl;
                }
            }
        }
    }
    
    env->set_current_offset(current_offset);
    
    for (auto elem : b->elements) {
        elem->accept(this);
    }
}

void GenCodeVisitor::visit(WhileStatement* stm) {
    int label_start = cantidad++;
    int label_end = cantidad++;
    
    out << ".Lwhile" << label_start << ":" << endl;
    
    stm->condition->accept(this);
    out << "    testq %rax, %rax" << endl;
    out << "    jz .Lendwhile" << label_end << endl;
    
    if (stm->b) {
        stm->b->accept(this);
    }
    
    out << "    jmp .Lwhile" << label_start << endl;
    
    out << ".Lendwhile" << label_end << ":" << endl;
}

void GenCodeVisitor::visit(ForStatement* stm) {
    int label_start = cantidad++;
    int label_end = cantidad++;
    
    env->add_level();
    
    if (stm->init) {
        stm->init->accept(this);
    }
    
    out << ".Lfor" << label_start << ":" << endl;
    
    if (stm->condition) {
        stm->condition->accept(this);
        out << "    testq %rax, %rax" << endl;
        out << "    jz .Lendfor" << label_end << endl;
    }
    
    if (stm->b) {
        stm->b->accept(this);
    }
    
    if (stm->update) {
        stm->update->accept(this);
    }
    
    out << "    jmp .Lfor" << label_start << endl;
    
    out << ".Lendfor" << label_end << ":" << endl;
    
    env->remove_level();
}

void GenCodeVisitor::visit(ExpressionStatement* stm) {
    if (stm->expression)
        stm->expression->accept(this);
}

void GenCodeVisitor::visit(StatementList* stm) {
    for (auto s : stm->stms) {
        s->accept(this);
    }
}

void GenCodeVisitor::visit(Parameter* param) {
}

void GenCodeVisitor::visit(ParameterList* paramList) {
}

void GenCodeVisitor::visit(Function* func) {
    out << ".globl " << func->name << endl;
    out << func->name << ":" << endl;
    out << "    pushq %rbp" << endl;
    out << "    movq %rsp, %rbp" << endl;

    int stack_space = 0;
    if (func->body) {
        stack_space = calcular_stack_body(func->body);
    }
    
    if (stack_space % 16 != 0) {
        stack_space = ((stack_space + 15) / 16) * 16;
    }
    
    if (stack_space > 0) {
        out << "    subq $" << stack_space << ", %rsp" << endl;
    }

    FunctionInfo func_info;
    func_info.return_type = func->return_type->type_name;
    func_info.stack_size = stack_space;

    if (func->parameters) {
        for (size_t i = 0; i < func->parameters->parameters.size(); ++i) {
            auto param = func->parameters->parameters[i];
            FunctionParamInfo param_info;
            param_info.name = param->name;
            param_info.type = param->type->type_name;
            param_info.is_pointer = param->type->is_pointer;
            param_info.is_array = param->type->is_array;
            param_info.is_reference = param->is_reference;
            param_info.reg_index = (i < 6) ? i : -1;
            func_info.params.push_back(param_info);
            
            env->add_var(
                param->name, 
                0,  
                param->type->type_name, 
                param->type->is_pointer, 
                param->type->is_array, 
                param->is_reference,
                param_info.reg_index, 
                false  
            );
            
            out << "# Parámetro: " << param->name
                << " reg_index: " << param_info.reg_index
                << " is_reference: " << (param->is_reference ? "true" : "false")
                << endl;
        }
    }

    env->add_function(func->name, func_info);
    env->add_level();
    
    if (func->body) {
        func->body->accept(this);
    }
    
    env->remove_level();
    
    out << "    leave" << endl;
    out << "    ret" << endl;
}

void GenCodeVisitor::visit(ReturnStatement* stm) {
    if (stm->return_value) {
        stm->return_value->accept(this);
    }
    
    out << "    leave" << endl;
    out << "    ret" << endl;
}

int GenCodeVisitor::visit(FunctionCallExp* exp) {
    if (!env->has_function(exp->function_name)) {
        cerr << "Error: Función '" << exp->function_name << "' no declarada" << endl;
        exit(1);
    }
    
    auto func_info = env->get_function(exp->function_name);
    const char* regs[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    int stack_args = 0;
    
    if (exp->arguments.size() != func_info.params.size()) {
        cerr << "Error: Número incorrecto de argumentos para '" << exp->function_name 
             << "'. Esperados: " << func_info.params.size() 
             << ", dados: " << exp->arguments.size() << endl;
        exit(1);
    }
    
    for (size_t i = 0; i < exp->arguments.size(); ++i) {
        auto& param_info = func_info.params[i];
        
        if (param_info.is_reference) {
            if (auto id = dynamic_cast<IdentifierExp*>(exp->arguments[i])) {
                if (!env->check(id->name)) {
                    cerr << "Error: Variable no declarada: " << id->name << endl;
                    exit(1);
                }
                VarInfo var_info = env->lookup(id->name);
                if (!var_info.is_global) {
                    out << "    leaq " << var_info.offset << "(%rbp), %rax # &" << id->name << endl;
                } else {
                    out << "    leaq " << id->name << "(%rip), %rax # &" << id->name << endl;
                }
            } else if (auto member = dynamic_cast<MemberAccessExp*>(exp->arguments[i])) {
                get_member_address(member);
            } else if (auto unary = dynamic_cast<UnaryExp*>(exp->arguments[i])) {
                if (unary->op == DEREFERENCE_OP) {
                    unary->uexp->accept(this); 
                } else {
                    cerr << "Error: Argumento por referencia inválido" << endl;
                    exit(1);
                }
            } else {
                cerr << "Error: Argumento por referencia debe ser variable o member" << endl;
                exit(1);
            }
        } else {
            exp->arguments[i]->accept(this);
        }

        if (i < 6) {
            out << "    movq %rax, " << regs[i] << endl;
        } else {
            out << "    pushq %rax" << endl;
            stack_args++;
        }
    }

    bool needs_padding = (stack_args % 2) != 0;
    if (needs_padding) {
        out << "    subq $8, %rsp # alinear stack a 16 bytes" << endl;
        stack_args++; 
    }

    out << "    call " << exp->function_name << endl;

    if (stack_args > 0) {
        out << "    addq $" << (stack_args * 8) << ", %rsp" << endl;
    }

    return 0;
}

void GenCodeVisitor::visit(FunctionList* funcList) {
    for (auto func : funcList->functions) {
        func->accept(this);
    }
}

void GenCodeVisitor::visit(MainFunction* mainFunc) {
    if (mainFunc->body) {
        mainFunc->body->accept(this);
    }
  
}



void GenCodeVisitor::visit(StructDeclaration* structDecl) {
    StructInfo info;
    int offset = 0;
    
    if (structDecl->members) {
        for (auto member : structDecl->members->vardecs) {
            for (size_t i = 0; i < member->vars.size(); ++i) {
                int member_size = 8; 
                
                if (member->types[i]->is_pointer) {
                    member_size = 8; 
                } else if (member->types[i]->type_name == "char" || member->types[i]->type_name == "bool") {
                    member_size = 1; 
                } else if (member->types[i]->type_name == "int") {
                    member_size = 8; 
                } else if (member->types[i]->type_name.find("struct") == 0) {
                    string struct_name = member->types[i]->type_name.substr(7);
                    if (env->has_struct(struct_name)) {
                        member_size = env->get_struct_size(struct_name);
                    }
                }
                
                int alignment = 8; 
                if (member_size == 1) {
                    alignment = 1; 
                }
                offset = (offset + alignment - 1) & ~(alignment - 1);
                
                info.fields[member->vars[i]] = {
                    member->types[i]->type_name, 
                    member->types[i]->is_pointer, 
                    member->types[i]->is_array
                };
                info.offsets[member->vars[i]] = offset;
                
                out << "# Miembro: " << member->vars[i] 
                    << " tipo: " << member->types[i]->type_name
                    << " offset: " << offset 
                    << " tamaño: " << member_size << endl;
                
                if (member->types[i]->is_array) {
                    if (member->types[i]->array_size) {
                        int array_elements = 10; 
                        member_size = member_size * array_elements;
                    } else {
                        member_size = 8;
                    }
                }
                
                offset += member_size;
            }
        }
    }
    
    info.size = (offset + 7) & ~7;
    env->add_struct(structDecl->struct_name, info);
    
    out << "# Struct " << structDecl->struct_name << " tamaño total: " << info.size << endl;
    
    for (const auto& field : info.offsets) {
        out << "# Campo " << field.first << ": offset " << field.second << endl;
    }
}

void GenCodeVisitor::visit(StructDeclarationList* structList) {
    for (auto structDecl : structList->structs) {
        structDecl->accept(this);
    }
}

int GenCodeVisitor::visit(StructInitializerExp* exp, int base_offset) {
    out << "# Inicializando struct en offset " << base_offset << endl;
    
    if (!env->has_struct(exp->struct_name)) {
        cerr << "Error: Estructura '" << exp->struct_name << "' no declarada" << endl;
        exit(1);
    }
    
    StructInfo info = env->get_struct(exp->struct_name);
    
    std::vector<std::string> field_names;
    for (const auto& kv : info.offsets) {
        field_names.push_back(kv.first);
    }
    std::sort(field_names.begin(), field_names.end(),
        [&](const std::string& a, const std::string& b) {
            return info.offsets.at(a) < info.offsets.at(b);
        });
    
    if (exp->members.size() > field_names.size()) {
        cerr << "Error: Demasiados inicializadores para struct '" << exp->struct_name << "'" << endl;
        exit(1);
    }
    
    for (size_t i = 0; i < exp->members.size() && i < field_names.size(); ++i) {
        int field_offset = info.offsets.at(field_names[i]);
        
        exp->members[i].second->accept(this); 
        
        out << "    movq %rax, " << (base_offset + field_offset) 
            << "(%rbp) # " << field_names[i] << endl;
    }
    
    for (size_t i = exp->members.size(); i < field_names.size(); ++i) {
        int field_offset = info.offsets.at(field_names[i]);
        out << "    movq $0, " << (base_offset + field_offset) 
            << "(%rbp) # " << field_names[i] << " = 0" << endl;
    }
    
    return 0;
}

int GenCodeVisitor::visit(StructInitializerExp* exp) {
    cerr << "Error: StructInitializerExp usado fuera de declaración de variable" << endl;
    exit(1);
}

void GenCodeVisitor::visit(Include* inc) {
}

void GenCodeVisitor::visit(IncludeList* incList) {
}

void GenCodeVisitor::visit(Type* type) {
}

