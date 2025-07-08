#include "parser.h"
#include "expression.h"
#include "scanner.h"
#include "token.h"
using namespace std;
int next_if_id = 1;
void Parser::skipComments() {
    while (check(Token::LINE_COMMENT) || check(Token::BLOCK_COMMENT)) {
        if (check(Token::LINE_COMMENT)) {
            advance();
        }
        else if (check(Token::BLOCK_COMMENT)) {
            advance();
        }
    }
}
bool Parser::match(Token::Type ttype) {
    if (check(ttype)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::isAtEnd() const {
    return current->type == Token::END_OF_FILE;
}

bool Parser::advance() {
    if (!isAtEnd()) {
        previous     = current;
        currentIndex++;
        current      = tokens[currentIndex];
    }
    return true;
}

bool Parser::check(Token::Type ttype) const {
    if (isAtEnd()) return false;
    return current->type == ttype;
}

Token* Parser::peekNext(size_t offset) const {
    size_t idx = currentIndex + offset;
    if (idx < tokens.size()) return tokens[idx];
    return eofToken;
}

Token* Parser::consume(Token::Type ttype, const std::string& message) {
    if (check(ttype)) {
        advance();
        return previous;
    }
    throw std::runtime_error(message);
}

Token* Parser::consumeIdent(const std::string& message) {
    if (check(Token::IDENTIFIER)) {
        advance();
        return previous;
    }
    throw std::runtime_error(message);
}



Parser::Parser(Scanner* scanner)
  : scanner(scanner), currentIndex(0)
{
    Token* tok;
    while ((tok = scanner->nextToken())->type != Token::END_OF_FILE) {
        tokens.push_back(tok);
    }
    tokens.push_back(tok);
    eofToken     = tok;
    current      = tokens[0];
    previous     = nullptr;
}

Parser::~Parser() {
    for (auto t : tokens) delete t;
}



Program* Parser::parseProgram() {
    skipComments();

    IncludeList* includes = parseIncludes();
    skipComments();

    GlobalVarDecList* globals = parseGlobalDeclarations();
    skipComments();
    

    StructDeclarationList* structs = parseStructDeclarations();
    skipComments();
    
    FunctionList* functions = parseFunctions();
    MainFunction* main = parseMainFunction();
    skipComments();

    return new Program(includes, globals, structs, functions, main);
}
IncludeList* Parser::parseIncludes() {
    IncludeList* includes = new IncludeList();
    while (check(Token::INCLUDE)) {
        includes->add(parseInclude());
    }
    return includes;
}

Include* Parser::parseInclude() {
    if(!match(Token::INCLUDE)){
        cout << "Error: se esperaba un 'include'." << endl;
        exit(1);
    }


    if(!match(Token::HEADER_NAME)){
        cout << "Error: se esperaba un nombre de header después de #include." << endl;
        exit(1);
    }

    string headerName = previous->text;



    bool isSystemHeader = (headerName[0] == '<');

    return new Include(headerName, isSystemHeader);
}

GlobalVarDecList* Parser::parseGlobalDeclarations() {
    skipComments();
    GlobalVarDecList* globals = new GlobalVarDecList();

    while (!isAtEnd() && !check(Token::STRUCT) && !check(Token::MAIN)) {
        if ((check(Token::INT) || check(Token::CHAR)) &&
            peekNext(2)->type == Token::LEFT_PAREN) {
            break;
        }

        if (check(Token::INT) || check(Token::CHAR)) {
            globals->add(parseGlobalVarDec());
            if (!match(Token::SEMICOLON)) {
                throw std::runtime_error("Se esperaba ';' después de la declaración de variable global.");
            }
        } else {
            break;
        }
    }

    return globals;
}


GlobalVarDec* Parser::parseGlobalVarDec() {
    skipComments();
    Type* type = parseType();
    skipComments();
    if (!check(Token::IDENTIFIER)) {
        throw runtime_error("Se esperaba un identificador para la variable global.");
    }
    string name = current->text;



    advance();

    Exp* init = nullptr;
    if (match(Token::ASSIGN)) {

        init = parseExpression();
    }
    return new GlobalVarDec(type, name, init);
}

StructDeclarationList* Parser::parseStructDeclarations() {
    skipComments();
    StructDeclarationList* structs = new StructDeclarationList();
    while(match(Token::STRUCT)) {
        skipComments();
        structs->add(parseStructDeclaration());
        skipComments();
    }
    return structs;
}

StructDeclaration* Parser::parseStructDeclaration() {
    if (!check(Token::IDENTIFIER)) {
        throw runtime_error("Se esperaba el nombre del struct.");
    }
    string name = current->text;


    advance();

    if (!match(Token::LEFT_BRACE)) {
        throw runtime_error("Se esperaba '{' para iniciar el cuerpo del struct.");
    }



    VarDecList* members = parseVarDecList();
    if (!match(Token::RIGHT_BRACE)) {
        throw runtime_error("Se esperaba '}' para finalizar el cuerpo del struct.");
    }

    if (!match(Token::SEMICOLON)) {
        throw runtime_error("Se esperaba ';' despues de la declaracion de struct.");
    }
    return new StructDeclaration(name, members);
}

FunctionList* Parser::parseFunctions() {
    FunctionList* functions = new FunctionList();

    while(!isAtEnd() && (check(Token::INT) || check(Token::CHAR) || check(Token::VOID))) {
        Type* return_type = parseType();


        if (check(Token::MAIN)) {
            match(Token::MAIN);
            return functions;
            continue;
        }

        if (check(Token::IDENTIFIER)) {
            string function_name = current->text;
            advance();
            Function* func = parseFunction(return_type, function_name);
            functions->add(func);
        } else {
            throw runtime_error("Se esperaba un nombre de funcion o 'main'");
        }
    }

    if (!check(Token::MAIN)) {
        return functions;
    }

    return functions;
}

Function* Parser::parseFunction(Type* return_type, const string& name) {
    if (!match(Token::LEFT_PAREN)) {
        throw runtime_error("Se esperaba '(' despues del nombre de funcion");
    }
    ParameterList* params = parseParameterList();

    if (!match(Token::RIGHT_PAREN)) {
        throw runtime_error("Se esperaba ')' despues de los parametros de la funcion. Se encontro " + current->text);
    }

    if (!match(Token::LEFT_BRACE)) {
        throw runtime_error("Se esperaba '{' antes del cuerpo de la funcion");
    }

    Body* body = parseBody();

    if (!match(Token::RIGHT_BRACE)) {
        throw runtime_error("Se esperaba '}' despues del cuerpo de la funcion '");
    }


    return new Function(return_type, name, params, body);
}
MainFunction* Parser::parseMainFunction() {
    if (!match(Token::LEFT_PAREN)) {
        throw runtime_error("Se esperaba '(' despues de 'main'");
    }

    if (!match(Token::RIGHT_PAREN)) {
        throw runtime_error("Se esperaba ')' despues de '(' en la funcion main");
    }
    if (!match(Token::LEFT_BRACE)) {
        throw runtime_error("Se esperaba '{' para el cuerpo de main en linea ");
    }
    Body* body = parseBody();
    if (!match(Token::RIGHT_BRACE)) {
        throw runtime_error("Se esperaba '}' para cerrar main en linea)");
    }
    return new MainFunction(body);

}

ParameterList* Parser::parseParameterList() {
    ParameterList* params = new ParameterList();
    if (!check(Token::RIGHT_PAREN)) {
        do {
            params->add(parseParameter());
            if (check(Token::COMMA)) {
            }
        } while (match(Token::COMMA));
    }
    return params;
}

Parameter* Parser::parseParameter() {
    Type* type = parseType();
    if (!check(Token::IDENTIFIER)) {
        throw runtime_error("Se esperaba un nombre para el parametro.");
    }
    string name = current->text;
    advance();

    if (match(Token::LEFT_BRACKET)) {
        Exp* array_size = nullptr;
        if (!check(Token::RIGHT_BRACKET)) {
            array_size = parseExpression();
        }
        if (!match(Token::RIGHT_BRACKET)) {
            throw runtime_error("Se esperaba ']' después del tamaño del array.");
        }
        Type* t = new Type(type->type_name, true, true, array_size, true);
        cout<<"array pasado por "<<t->is_reference<<endl;

    
        return new Parameter(t, name, true);
    }
    bool is_ref = type->is_reference;
    return new Parameter(type, name, is_ref);
}

Body* Parser::parseBody() {
    std::vector<BlockElement*> elements;

    while (!check(Token::RIGHT_BRACE) && !isAtEnd()) {
        if (check(Token::INT) || check(Token::CHAR) || check(Token::STRUCT) || check(Token::BOOL)) {
            VarDec* vardec = parseVarDec();
            if (!match(Token::SEMICOLON)) {
                throw runtime_error("Se esperaba ';' despues de la declaracion de variable.");
            }
            elements.push_back(vardec);
        } else {
            Stm* stm = parseStatement();
            elements.push_back(stm);
        }
    }
    return new Body(elements);
}


VarDecList* Parser::parseVarDecList() {
    VarDecList* vardecs = new VarDecList();

    while (check(Token::INT) || check(Token::CHAR) || check(Token::STRUCT) || check(Token::BOOL)) {

        vardecs->add(parseVarDec());
        if (!match(Token::SEMICOLON)) {
            throw runtime_error("Se esperaba ';' despues de la declaracion de variable.");
        }



    }
    return vardecs;
}
Type* Parser::parseType() {
    string typeName;

    if (check(Token::INT)) {
        typeName = "int";
        advance();
    } else if (check(Token::CHAR)) {
        typeName = "char";
        advance();
    }else if (check(Token::BOOL)) {
        typeName = "bool";
        advance();
    }   
    else if (check(Token::VOID)) {
        typeName = "void";
        advance();
    } else if (check(Token::STRUCT)) {
        advance();
        if (!check(Token::IDENTIFIER)) {
            cout << "Error: se esperaba un nombre de struct después de 'struct'." << endl;
            exit(1);
        }
        typeName = "struct " + current->text;
        advance();
    } else {
        cout << "Error: se esperaba un tipo (int, char, void, struct)." << endl;
        exit(1);
    }

    bool isPointer = false;
    bool isReference = false;
    while (check(Token::MULTIPLY) || check(Token::POINTER_DECL) || check(Token::ADDRESS_OF)) {
          if (check(Token::MULTIPLY) || check(Token::POINTER_DECL)) {
            isPointer = true;
        } else if (check(Token::ADDRESS_OF)) {
            isReference = true;
        }
        advance();
    }


    if (isPointer && isReference) {
        cerr << "Error: un parámetro no puede ser puntero y referencia al mismo tiempo." << endl;
        exit(1);
    }

    return new Type(typeName, isPointer, false, nullptr, isReference);
}


VarDec* Parser::parseVarDec() {
    Type* base_type = parseType();
    vector<string> vars;
    vector<Exp*> initializers;
    vector<Type*> types;

    do {
        if (!check(Token::IDENTIFIER)) {
            throw runtime_error("Se esperaba un identificador.");
        }
        string name = current->text;
        advance();
        vars.push_back(name);

        Type* var_type = new Type(
            base_type->type_name,
            base_type->is_pointer,
            base_type->is_array,
            base_type->array_size,
            base_type->is_reference
        );

        if (match(Token::LEFT_BRACKET)) {
            Exp* array_size = nullptr;
            if (!check(Token::RIGHT_BRACKET)) {
                array_size = parseExpression();
            }
            if (!match(Token::RIGHT_BRACKET)) {
                throw runtime_error("Se esperaba ']' después del tamaño del array.");
            }
            var_type = new Type(base_type->type_name, false, true, array_size, base_type->is_reference);
        }
        types.push_back(var_type);

        if (match(Token::ASSIGN)) {
            if (check(Token::LEFT_BRACE)) {
                advance();
                std::vector<Exp*> inits;
                inits.push_back(parseExpression());
                while (match(Token::COMMA)) {
                    inits.push_back(parseExpression());
                }
                if (!match(Token::RIGHT_BRACE)) throw runtime_error("Se esperaba '}' para cerrar la lista de inicializadores.");
        
                if (var_type->is_array) {
                    initializers.push_back(new ArrayInitializerExp(inits));
                } else if (var_type->type_name.rfind("struct ", 0) == 0) { 
                    std::string struct_name = var_type->type_name.substr(7); 
                    StructInitializerExp* structInit = new StructInitializerExp(struct_name);
                    for (auto* exp : inits)
                        structInit->add_member("", exp); 
                    initializers.push_back(structInit);
                } else {
                    throw runtime_error("Inicialización con llaves no válida para este tipo.");
                }
            } else {
                initializers.push_back(parseExpression());
            }
        } else {
            initializers.push_back(nullptr);
        }
    } while (match(Token::COMMA));

    VarDec* vd = new VarDec(types, vars);
    for (auto init : initializers) {
        vd->add_initializer(init);
    }
    return vd;
}

StatementList* Parser::parseStatementList() {
    StatementList* stms = new StatementList();
    while (!check(Token::RIGHT_BRACE) && !isAtEnd()) {
        stms->add(parseStatement());
    }
    return stms;
}
Stm* Parser::parseStatement() {
    if (match(Token::IF)) return parseIfStatement();
    if (match(Token::WHILE)) return parseWhileStatement();
    if (match(Token::FOR)) return parseForStatement();
    if (match(Token::RETURN)) return parseReturnStatement();
    if (match(Token::PRINTF)) return parsePrintfStatement();
    return parseExpressionStatement();
}

Stm* Parser::parseIfStatement() {

    if (!match(Token::LEFT_PAREN)) throw runtime_error("Se esperaba '(' despues de 'if'.");



    Exp* cond = parseExpression();
    if (!match(Token::RIGHT_PAREN)) throw runtime_error("Se esperaba ')' despues de la condicion del if.");


    if (!match(Token::LEFT_BRACE)) throw runtime_error("Se esperaba '{' para el cuerpo del if.");


    Body* if_body = parseBody();
    if (!match(Token::RIGHT_BRACE)) throw runtime_error("Se esperaba '}' para cerrar el cuerpo del if.");
    int if_id = next_if_id++;
    Stm* else_chain = nullptr;
    if (match(Token::ELSE_IF)) {
        else_chain = parseElseIfStatement(if_id);
    } else if (match(Token::ELSE)) {
        else_chain = parseElseStatement(if_id);
    }



    return new IfStatement(cond, if_body, else_chain);
}

Stm *Parser::parseElseIfStatement(int if_id) {
    if (!match(Token::LEFT_PAREN)) {
        throw runtime_error("Se esperaba '(' despues de 'else if'.");
    }

    Exp* else_if_cond = parseExpression();

    if (!match(Token::RIGHT_PAREN)) {
        throw runtime_error("Se esperaba ')' después de la condición else if");
    }
    if (!match(Token::LEFT_BRACE)) {
        throw runtime_error("Se esperaba '{' para el cuerpo del else if.");
    }

    Body* else_if_body = parseBody();
    if (!match(Token::RIGHT_BRACE)) {
        throw runtime_error("Se esperaba '}' para cerrar el cuerpo del else if.");
    }
    int else_if_id = next_if_id++;
    Stm* next_chain = nullptr;
    if (match(Token::ELSE_IF)) {
        next_chain = parseElseIfStatement(if_id);
    }
    else if (match(Token::ELSE)) {
        next_chain = parseElseStatement(if_id);
    }
    return new ElseIfStatement(ElseIfStatement::ELSE_IF, else_if_cond,
                             else_if_body, next_chain, else_if_id, if_id);
}

Stm* Parser::parseElseStatement(int if_id) {

    if (!match(Token::LEFT_BRACE)) {
        throw runtime_error("Se esperaba '{' para el cuerpo del else.");
    }


    Body* else_body = parseBody();

    if (!match(Token::RIGHT_BRACE)) {
        throw runtime_error("Se esperaba '}' para cerrar el cuerpo del else.");
    }

    int else_id = next_if_id++;
    return new ElseIfStatement(ElseIfStatement::ELSE, nullptr, else_body, nullptr, else_id, if_id);
}

Stm* Parser::parseWhileStatement() {

    if (!match(Token::LEFT_PAREN)) throw runtime_error("Se esperaba '(' despues de 'while'.");


    Exp* cond = parseExpression();
    if (!match(Token::RIGHT_PAREN)) throw runtime_error("Se esperaba ')' despues de la condicion del while.");

    if (!match(Token::LEFT_BRACE)) throw runtime_error("Se esperaba '{' para el cuerpo del while.");


    Body* body = parseBody();
    if (!match(Token::RIGHT_BRACE)) throw runtime_error("Se esperaba '}' para cerrar el cuerpo del while.");

    return new WhileStatement(cond, body);
}
Stm* Parser::parseForStatement() {

    if (!match(Token::LEFT_PAREN)) {
        cout << "Error: se esperaba '('." << endl;
        exit(1);
    }
    VarDec* init = parseVarDec();
    if (!match(Token::SEMICOLON)) {
        cout << "Error: se esperaba ';'." << endl;
        exit(1);
    }
    Exp* e1 = parseExpression();
    if (!match(Token::SEMICOLON)) {
        cout << "Error: se esperaba ';'." << endl;
        exit(1);
    }
    Exp* e2 = parseExpression();
    if (!match(Token::RIGHT_PAREN)) {
        cout << "Error: se esperaba ')'." << endl;
        exit(1);
    }


    if (!match(Token::LEFT_BRACE)) {
        cout << "Error: se esperaba '{'." << endl;
        exit(1);
    }


    Body* body = parseBody();

    if (!match(Token::RIGHT_BRACE)) {
        cout << "Error: se esperaba '('." << endl;
        exit(1);
    }


    return new ForStatement(init,e1,e2,body);}


Stm* Parser::parseReturnStatement() {


    Exp* val = nullptr;
    if (!check(Token::SEMICOLON)) {

        val = parseExpression();
    }
    if (!match(Token::SEMICOLON)) throw runtime_error("Se esperaba ';' despues de la sentencia return.");



    return new ReturnStatement(val);
}

Stm* Parser::parsePrintfStatement() {


    if (!match(Token::LEFT_PAREN)) throw runtime_error("Se esperaba '(' despues de 'printf'.");



    if(!check(Token::FORMAT_STRING) && !check(Token::STRING_LITERAL)) {
        throw runtime_error("Se esperaba una cadena de formato en printf.");
    }
    string format_string = current->text;


    advance();

    PrintfStatement* printf_stm = new PrintfStatement(format_string);
    if(match(Token::COMMA)) {


        list<Exp*> args = parseArguments();
        for(auto arg: args) printf_stm->add_argument(arg);
    }
    if (!match(Token::RIGHT_PAREN)) throw runtime_error("Se esperaba ')' despues de los argumentos de printf.");



    if (!match(Token::SEMICOLON)) throw runtime_error("Se esperaba ';' despues de la llamada a printf.");



    return printf_stm;
}

Stm* Parser::parseExpressionStatement() {
    Exp* exp = parseExpression();
    if (!match(Token::SEMICOLON)) {
        throw runtime_error("Se esperaba ';' despues de la expresion.");
    }

    return new ExpressionStatement(exp);
}

Exp* Parser::parseExpression() {
    return parseAssignment();
}

Exp* Parser::parseAssignment() {
    Exp* left = parseLogicalOr();
    if (match(Token::ASSIGN) || match(Token::PLUS_ASSIGN) || match(Token::MINUS_ASSIGN) ||
        match(Token::MULTIPLY_ASSIGN) || match(Token::DIVIDE_ASSIGN) || match(Token::MODULO_ASSIGN)) {
        Token* op_token = previous;
        BinaryOp op;
        if(op_token->type == Token::ASSIGN) {
            op = ASSIGN_OP;

        }
        else if(op_token->type == Token::PLUS_ASSIGN) {
            op = PLUS_EQUAL_OP;

        }
        else if(op_token->type == Token::MINUS_ASSIGN) {
            op = MINUS_EQUAL_OP;

        }
        else if(op_token->type == Token::MULTIPLY_ASSIGN) {
            op = MULTIPLY_EQUAL_OP;
        }
        else if(op_token->type == Token::DIVIDE_ASSIGN) {
            op = DIVIDE_EQUAL_OP;

        }
        else {
            op = MODULO_EQUAL_OP;

        }
        Exp* right = parseAssignment();
        return new AssignExp(left, right, op);
    }
    return left;
}

Exp* Parser::parseLogicalOr() {
    Exp* expr = parseLogicalAnd();
    while (match(Token::LOGICAL_OR)) {

        expr = new BinaryExp(expr, parseLogicalAnd(), LOGICAL_OR_OP);
    }
    return expr;
}
Exp* Parser::parseLogicalAnd() {
    Exp* expr = parseEquality();
    while (match(Token::LOGICAL_AND)) {

        expr = new BinaryExp(expr, parseEquality(), LOGICAL_AND_OP);
    }
    return expr;
}

Exp* Parser::parseComparison() {
    Exp* expr = parseAdditive();

    while (check(Token::LESS_THAN) || check(Token::GREATER_THAN) ||
           check(Token::LESS_EQUAL) || check(Token::GREATER_EQUAL)) {
        BinaryOp op;
        if (check(Token::LESS_THAN)) {
            op = LESS_THAN_OP;

        } else if (check(Token::GREATER_THAN)) {
            op = GREATER_THAN_OP;
        } else if (check(Token::LESS_EQUAL)) {
            op = LESS_EQUAL_OP;

        } else {
            op = GREATER_EQUAL_OP;

        }

        advance();

        Exp* right = parseAdditive();
        expr = new BinaryExp(expr, right, op);
    }

    return expr;
}

Exp* Parser::parseEquality() {
    Exp* expr = parseComparison();

    while (check(Token::EQUAL) || check(Token::NOT_EQUAL)) {
        BinaryOp op;
        if (check(Token::EQUAL)) {
            op = EQUAL_OP;

        } else {
            op = NOT_EQUAL_OP;

        }

        advance();

        Exp* right = parseComparison();
        expr = new BinaryExp(expr, right, op);
    }

    return expr;
}

Exp* Parser::parseAdditive() {
    Exp* expr = parseMultiplicative();

    while (check(Token::PLUS) || check(Token::MINUS)) {
        BinaryOp op;
        if (check(Token::PLUS)) {
            op = PLUS_OP;

        } else {
            op = MINUS_OP;

        }

        advance();

        Exp* right = parseMultiplicative();
        expr = new BinaryExp(expr, right, op);
    }

    return expr;
}

Exp* Parser::parseMultiplicative() {
    Exp* expr = parseUnary();

    while (check(Token::MULTIPLY) || check(Token::DIVIDE) || check(Token::MODULO)) {
        BinaryOp op;
        if (check(Token::MULTIPLY)) {
            op = MULT_OP;

        } else if (check(Token::DIVIDE)) {
            op = DIV_OP;

        } else {
            op = MOD_OP;

        }

        advance();

        Exp* right = parseUnary();
        expr = new BinaryExp(expr, right, op);
    }

    return expr;
}

Exp* Parser::parseUnary() {
    if (check(Token::LOGICAL_NOT) || check(Token::INCREMENT) || check(Token::DECREMENT) ||
        check(Token::DEREFERENCE) || check(Token::ADDRESS_OF) ||
        check(Token::PLUS) || check(Token::MINUS)) {

        UnaryOp op;
        if (check(Token::LOGICAL_NOT)) {
            op = NEGACION_OP;

        } else if (check(Token::INCREMENT)) {
            op = PLUS_PLUS_OP;

        } else if (check(Token::DECREMENT)) {
            op = MINUS_MINUS_OP;

        } else if (check(Token::DEREFERENCE)) {
            op = DEREFERENCE_OP;

        } else if (check(Token::ADDRESS_OF)) {
            op = ADDRESS_OF_OP;

        } else if (check(Token::PLUS)) {
            op = UNARY_PLUS_OP;

        } else if (check(Token::MINUS)) {
            op = UNARY_MINUS_OP;

        }

        advance();

        Exp* expr = parseUnary();

        return new UnaryExp(expr, op, true);
    }

    return parsePostfix();
}


Exp* Parser::parsePostfix() {
    Exp* expr = parsePrimary();
    while (true) {
        if (match(Token::INCREMENT)) {

            expr = new UnaryExp(expr, PLUS_PLUS_OP, false);
        } else if (match(Token::DECREMENT)) {


            expr = new UnaryExp(expr, MINUS_MINUS_OP, false);
        } else if (match(Token::LEFT_BRACKET)) {

            Exp* index = parseExpression();
            if (!match(Token::RIGHT_BRACKET)) throw runtime_error("Se esperaba ']' despues del indice del array.");


            expr = new ArrayAccessExp(expr, index);
        } else if (match(Token::LEFT_PAREN)) {

            IdentifierExp* id_exp = dynamic_cast<IdentifierExp*>(expr);
            if (!id_exp) throw runtime_error("La llamada a función debe ser sobre un identificador.");
            FunctionCallExp* call = new FunctionCallExp(id_exp->name);
            if(!check(Token::RIGHT_PAREN)){


                list<Exp*> args = parseArguments();
                for(auto arg: args) call->add_argument(arg);
            }

            if (!match(Token::RIGHT_PAREN)) throw runtime_error("Se esperaba ')' despues de los argumentos de la funcion.");

            expr = call;
        } else if (match(Token::DOT)) {
            if (!check(Token::IDENTIFIER)) throw runtime_error("Se esperaba el nombre del miembro.");
            string member_name = current->text;
            advance();
            expr = new MemberAccessExp(expr, member_name, false);
        } else if (match(Token::ARROW)) {

            if (!check(Token::IDENTIFIER)) throw runtime_error("Se esperaba el nombre del miembro.");
            string member_name = current->text;

            advance();

            expr = new MemberAccessExp(expr, member_name, true);
        }
        else {
            break;
        }
    }
    return expr;
}

Exp* Parser::parsePrimary() {

    if (match(Token::NUMBER)){
        return new NumberExp(stoi(previous->text));
    }
    if (match(Token::IDENTIFIER)) {
        return new IdentifierExp(previous->text);
    }
    if (match(Token::TRUE)) {
        return new BoolExp(true);
    }
    if (match(Token::FALSE)) {
        return new BoolExp(false);
    }

    if (match(Token::STRING_LITERAL) || match(Token::FORMAT_STRING)){

        return new StringExp(previous->text);
    }
    if (match(Token::CHARACTER_CONSTANT)) {

        return new CharExp(previous->text[1]);
    }
    if (match(Token::LEFT_PAREN)) {
        Exp* expr = parseExpression();
        if (!match(Token::RIGHT_PAREN)) throw runtime_error("Se esperaba ')' despues de la expresion.");
        return new ParenExp(expr);
    }
    if(match(Token::LEFT_BRACE)){


        list<Exp*> inits = parseInitializerList();
        if (!match(Token::RIGHT_BRACE)) throw runtime_error("Se esperaba '}' para cerrar la lista de inicializadores.");


        if(!inits.empty()) return inits.front();
        return nullptr;
    }
    throw runtime_error("Se esperaba una expresión primaria (numero, identificador, cadena, parentesis). Se obtuvo el token " + current->text);
}

Exp* Parser::parseFunctionCall() {

    return nullptr;
}

Exp* Parser::parseArrayAccess() {

    return nullptr;
}

Exp* Parser::parseMemberAccess() {

    return nullptr;
}

Exp* Parser::parseNumberExp() {
    if (!check(Token::NUMBER)) {
        cout << "Error: se esperaba un número." << endl;
        exit(1);
    }

    string numberText = current->text;
    int value = stoi(numberText);

    advance();

    return new NumberExp(value);
}

list<Exp*> Parser::parseArguments() {
    list<Exp*> args;

    args.push_back(parseExpression());

    while(match(Token::COMMA)){


        args.push_back(parseExpression());
    }
    return args;
}


list<Exp*> Parser::parseInitializerList() {
    list<Exp*> inits;

    inits.push_back(parseExpression());

    while (match(Token::COMMA)){
        inits.push_back(parseExpression());

    }
    return inits;
}
