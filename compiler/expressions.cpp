#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdarg.h>
#include <vector>

#include "expressions.h"

Literal_String* Literal_String::crt(std::string val) {
    Literal_String* lit = (Literal_String*)malloc(sizeof(Literal_String));
    lit->value = val;
    return lit;
}

Literal_Num* Literal_Num::crt(NumTypes type, std::string val) {
    Literal_Num* lit = (Literal_Num*)malloc(sizeof(Literal_Num));
    lit->type = type;
    lit->value = val;
    return lit;
}

Literal_Char* Literal_Char::crt(std::string val) {
    Literal_Char* lit = (Literal_Char*)malloc(sizeof(Literal_Char));
    lit->value = val;
    return lit;
}


Expr_Type* Expr_Type::crt(TypeDefs type, bool user_def) {
    Expr_Type* tp = (Expr_Type*)malloc(sizeof(Expr_Type));

    tp->type_def = type;
    tp->user_deftype = user_def;
}


Expr_Value* Expr_Value::crt(LiteralBase* _literal) {
    Expr_Value* expr = (Expr_Value*)malloc(sizeof(Expr_Value));
    expr->literal = _literal;
    expr->member_path = NULL;
}

Expr_Value* Expr_Value::crt(std::string* _member_path) {
    Expr_Value* expr = (Expr_Value*)malloc(sizeof(Expr_Value));
    expr->literal = NULL;
    expr->member_path = _member_path;
}

// ___________ VARS

Expr_VarDecl* Expr_VarDecl::crt(Expr_Type* type, std::string* name) {
    Expr_VarDecl* ex = (Expr_VarDecl*)malloc(sizeof(Expr_VarDecl));
    ex->type = type;
    ex->var_name = name;
}

Expr_VarInit* Expr_VarInit::crt(Expr_Type* type, std::string* name, Expr_Value* val) {
    Expr_VarInit *ex = (Expr_VarInit*)malloc(sizeof(Expr_VarInit));
    ex->type - type;
    ex->value = val;
    ex->var_name = name;

    return ex;
}

Expr_VarSet* Expr_VarSet::crt(std::string* name, Expr_Value* val) {
    Expr_VarSet* ex = (Expr_VarSet*)malloc(sizeof(Expr_VarSet));

    ex->value = val;
    ex->var_name = name;
}



// ___________ FUNCS

Expr_FParamListCall* Expr_FParamListCall::crt(std::vector<Expr_Value*>* vec) {
    Expr_FParamListCall* ex = (Expr_FParamListCall*)malloc(sizeof(Expr_FParamListCall));
    ex->params = vec;
}

Expr_FParamListDecl* Expr_FParamListDecl::crt(std::vector<Expr_VarDecl*>* _params) {
    Expr_FParamListDecl* ex = (Expr_FParamListDecl*)malloc(sizeof(Expr_FParamListDecl));
    ex->params = _params;

    return ex;
}

Expr_FuncDecl* Expr_FuncDecl::crt(Expr_Type* type, Expr_FParamListDecl* params) {
    Expr_FuncDecl* ex = (Expr_FuncDecl*)malloc(sizeof(Expr_FuncDecl));
    ex->body = (std::vector<ExprBase*>*)malloc(sizeof(std::vector<ExprBase*>));
    ex->params = params;
    ex->type = type;

    return ex;
};

Expr_FuncCall* crt(std::string* name, Expr_FParamListCall* call_params) {
    Expr_FuncCall* ex = (Expr_FuncCall*)malloc(sizeof(Expr_FuncCall));
    ex->func_name = name;
    ex->call_params = call_params;
    return ex;
};




