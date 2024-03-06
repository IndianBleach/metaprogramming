#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdarg.h>
#include <vector>



class ExprBase {
};

class LiteralBase {
    public:
        std::string value;
};

enum NumTypes {
    FLOAT_LIT,
    INT_LIT,
    DOUBLE_LIT,
    BYTE_LIT,
};

class Literal_Num : public LiteralBase {
    public:
        NumTypes type;

        static Literal_Num* crt(NumTypes type, std::string val);
};

class Literal_String : public LiteralBase {
    public:
        static Literal_String* crt(std::string val);
};

class Literal_Char : public LiteralBase {
    public:
        static Literal_Char* crt(std::string val);
};

enum TypeDefs {
    INT_TYPE,
    FLOAT_TYPE,
    STRING_TYPE,
    CHAR_TYPE,
};

class Expr_Type : public ExprBase {
    public:
        TypeDefs type_def;
        bool user_deftype;

        static Expr_Type* crt(TypeDefs type, bool user_def);

};

class Expr_Value : public ExprBase{
    public:
        LiteralBase* literal;
        std::string* member_path;

        static Expr_Value* crt(LiteralBase* _literal);
        static Expr_Value* crt(std::string* _member_path);
};


// ___________ VARS

class Expr_VarDecl : public ExprBase {
    public:
        Expr_Type* type;
        std::string* var_name;

        static Expr_VarDecl* crt(Expr_Type* type, std::string* name);
};

class Expr_VarInit : public ExprBase {
    public:
        Expr_Type* type;
        std::string* var_name;
        Expr_Value* value;

        static Expr_VarInit* crt(Expr_Type* type, std::string* name, Expr_Value* val);
};

class Expr_VarSet : public ExprBase {
    public:
        std::string* var_name;
        Expr_Value* value;

        static Expr_VarSet* crt(std::string* name, Expr_Value* val);
};

// ___________ FUNCS

class Expr_FParamListCall : public ExprBase {
    public:
        std::vector<Expr_Value*>* params;

        static Expr_FParamListCall* crt(std::vector<Expr_Value*>* vec);
};

class Expr_FParamListDecl : public ExprBase {
    public:
        std::vector<Expr_VarDecl*>* params;

        static Expr_FParamListDecl* crt(std::vector<Expr_VarDecl*>* _params);
};

class Expr_FuncDecl : public ExprBase {
    public:
        Expr_Type* type;
        Expr_FParamListDecl* params;
        std::vector<ExprBase*>* body;

        static Expr_FuncDecl* crt(Expr_Type* type, Expr_FParamListDecl* params);
};

class Expr_FuncCall : public ExprBase {
    public:
        std::string* func_name;
        Expr_FParamListCall* call_params;

        static Expr_FuncCall* crt(std::string* name, Expr_FParamListCall* call_params);
};


// ___________ ITERATORS

class Expr_IterFor: public ExprBase {
    public:
        ExprBase* input;
        // todo:: add if expr tree
        ExprBase* action;
        std::vector<ExprBase*>* body;
 
        static Expr_IterFor* crt(std::string* name, Expr_FParamListCall* call_params);
};
