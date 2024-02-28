#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <stdarg.h>

#include <vector>


enum cnst_TokenType {
    
    META_REF_RULE,

    ARRAY,

    LIST_PARAMS,
    LIST_VALUES,
    LIST_VARS,

    MATH_COMPARE,
    MATH_OPERATION,
    MATH_LOGIC,

    VAR_NAME,
    VAR_SET,

    LITERAL_NUM,
    LITERAL_STRING,
    LITERAL_CHAR, 

    TYPE,
    RESERVE_WORD,
    OPERAND,
    SYMBOL_BRACKET,
    SYMBOL,
    SYMBOL_END,
    UNDF = -1
};


enum meta_type {
    UNDF_META,

    VAR_INIT_INT,
    VAR_DEF_INT,

    VAR_INIT_FLOAT,
    VAR_INIT_DOUBLE,
    VAR_INIT_BYTE,
    VAR_INIT_LONG,

    VAR_INIT_CHAR,
    VAR_INIT_CHAR_ARR,
    VAR_DEF_CHAR,
    VAR_DEF_CHAR_ARR,
    INIT_CHAR_ARR,

    FUNC_DECL,
    FUNC_DEF_INT,
    
    VAR_INIT_STRING,
    VAR_INIT_BOOL,
};


enum meta_parse_rule {
    NO_RULE,
    FUNC_PARAM_LIST,
    FUNC_BLOCK_CODE,
};

class gram_tnode {
    public:
        std::vector<gram_tnode*>* childrens;
        const std::string* value;
        cnst_TokenType type;

        bool meta_inited = false;
        
        meta_type rule_name;
        meta_type ref_rule;
        
        meta_parse_rule parse_rule;

        bool is_meta_block = false;

        // todo: some metadata? (blockLevel, priority)
        gram_tnode(std::string* _val, cnst_TokenType _type);

        gram_tnode(std::string* _val, cnst_TokenType _type, meta_type _rule_name);

        static gram_tnode* gram_tnode_crt(const std::string* _val, cnst_TokenType _type);

        static gram_tnode* gram_tnode_crt(const std::string* _val, cnst_TokenType _type, meta_type rule_name);

        static gram_tnode* gram_tnode_crt(const std::string* _val, cnst_TokenType _type, meta_type rule_name, meta_parse_rule parse_rule);

        static gram_tnode* gram_tnode_crt_REF_RULE(const std::string* _val, cnst_TokenType _type, meta_type ref_rule);
};

class token_alphabet {
    public:
        // types
        const std::string TYPE_INT = "int";
        const std::string TYPE_DOUBLE = "double";
        const std::string TYPE_CHAR = "char";
        const std::string TYPE_FLOAT = "float";
        const std::string TYPE_BYTE = "byte";
        const std::string TYPE_LONG = "long";
        const std::string TYPE_STRING = "string";

        // brackets
        const char BR_RCIR = ')';
        const char BR_RSQUARE = ']';
        const char BR_RFIG = '}';
        const char BR_LCIR = '(';
        const char BR_LSQUARE = '[';
        const char BR_LFIG = '{';
        
        const std::string  _BR_RCIR = ")";
        const std::string  _BR_RSQUARE = "]";
        const std::string  _BR_RFIG = "}";
        const std::string  _BR_LCIR = "(";
        const std::string  _BR_LSQUARE = "[";
        const std::string  _BR_LFIG = "{";

        // operands
        const char OP_PL = '+';
        const char OP_MIN = '-';
        const char OP_MUL = '*';
        const char OP_PDIV = '%';
        const char OP_DIV = '/';

        const std::string _OP_PL = "+";
        const std::string _OP_MIN = "-";
        const std::string _OP_MUL = "*";
        const std::string _OP_PDIV = "%";
        const std::string _OP_DIV = "/";

        // logic
        const char LOG_AND = '&';
        const char LOG_OR = '|';
        const std::string LOG_DAND = "&&";
        const std::string LOG_DOR = "||";

        const std::string _LOG_AND = "&";
        const std::string _LOG_OR = "|";

        // compare
        const char CMP_LEFT = '<';
        const char CMP_RIGHT = '>';
        const std::string CMP_EQLEFT = "<=";
        const std::string CMP_EQRIGHT = ">=";
        const std::string CMP_EQ = "==";
        const std::string CMP_NEQ = "!=";

        const std::string _CMP_LEFT = "<";
        const std::string _CMP_RIGHT = ">";

        // set
        const char VAR_SET = '='; 
        const std::string _VAR_SET = "="; 

        // symbols
        const char SMB_DOT = '.';
        const char SMB_END = ';';
        const char SMB_CMA = ',';
        const char SMB_VAR = '=';

        const std::string _SMB_DOT = ".";
        const std::string _SMB_END = ";";
        const std::string _SMB_CMA = ",";
        //const std::string _SMB_VAR = "=";

        // reserve words
        const std::string RW_FOR = "for";
        const std::string RW_AUTO = "auto";
        const std::string RW_WHILE = "while";
        const std::string RW_FUNC = "func";
        const std::string RW_CLASS = "class";
        const std::string RW_NEXT = "next";
        const std::string RW_RET= "return";

        const char* _SPEC_VARNAME = "<var>";

        const std::string META_UTILS_RULE = "_";
};


class nodeRef_utils {
    public:
        std::vector<gram_tnode*>* refs;
        std::vector<meta_type>* ref_types;
    
    public:
        gram_tnode* get_node_byref(meta_type type);

        void add_ref_node(gram_tnode* node, meta_type ref_type);

        static nodeRef_utils* crt();
};

class grammatical_rule_list {
    public:
        std::vector<gram_tnode*>* roots;

        static grammatical_rule_list* crt_filled_rulelist();

        void make_rules(grammatical_rule_list* ls);

        void add_gram_rule(std::vector<gram_tnode*>* ls, std::initializer_list<gram_tnode*> nodes, meta_type rule_type);
};

