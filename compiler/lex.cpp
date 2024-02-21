#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <stdarg.h>

#include <vector>

/*  TODO
        - типы и обьявления в грамматику
        - проверить типы в грамматике и попробовать собрать AST
*/

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
        gram_tnode(std::string* _val, cnst_TokenType _type) {
            value = _val;
            type = _type;
            meta_inited = true;
            childrens = new std::vector<gram_tnode*>();
            parse_rule = meta_parse_rule::NO_RULE;
        };

        gram_tnode(std::string* _val, cnst_TokenType _type, meta_type _rule_name) {
            
            value = _val;
            type = _type;
            rule_name = _rule_name;
            meta_inited = true;
            childrens = new std::vector<gram_tnode*>();
            parse_rule = meta_parse_rule::NO_RULE;
        };

        // / ctr()
};

        gram_tnode* gram_tnode_crt(const std::string* _val, cnst_TokenType _type) {
            gram_tnode* nd = (gram_tnode*)malloc(sizeof(gram_tnode));
            
            nd->value = _val;
            nd->type = _type;
            nd->meta_inited = true;
            nd->childrens = new std::vector<gram_tnode*>();

            nd->rule_name = meta_type::UNDF_META;
            nd->ref_rule = meta_type::UNDF_META;
            nd->parse_rule = meta_parse_rule::NO_RULE;

            return nd;
        }

        gram_tnode* gram_tnode_crt(const std::string* _val, cnst_TokenType _type, meta_type rule_name) {
            gram_tnode* nd = (gram_tnode*)malloc(sizeof(gram_tnode));
            
            nd->value = _val;
            nd->type = _type;
            nd->meta_inited = true;
            nd->childrens = new std::vector<gram_tnode*>();

            nd->rule_name = rule_name;
            nd->ref_rule = meta_type::UNDF_META;

            nd->parse_rule = meta_parse_rule::NO_RULE;

            return nd;
        }

        gram_tnode* gram_tnode_crt(const std::string* _val, cnst_TokenType _type, meta_type rule_name, meta_parse_rule parse_rule) {
            gram_tnode* nd = (gram_tnode*)malloc(sizeof(gram_tnode));
            
            nd->value = _val;
            nd->type = _type;
            nd->meta_inited = true;
            nd->childrens = new std::vector<gram_tnode*>();

            nd->rule_name = rule_name;
            nd->ref_rule = meta_type::UNDF_META;

            nd->parse_rule = parse_rule;

            return nd;
        }


        gram_tnode* gram_tnode_crt_REF_RULE(const std::string* _val, cnst_TokenType _type, meta_type ref_rule) {
            gram_tnode* nd = (gram_tnode*)malloc(sizeof(gram_tnode));
            
            nd->value = _val;
            nd->type = _type;
            nd->meta_inited = true;
            nd->childrens = new std::vector<gram_tnode*>();

            nd->ref_rule = ref_rule;
            nd->rule_name = UNDF_META;

            nd->parse_rule = meta_parse_rule::NO_RULE;

            return nd;
        }

// add variable_set
/*
    VAR_SETS,
*/



class grammatical_rule_list {
    public:
        std::vector<gram_tnode*>* roots;

        static grammatical_rule_list* crt_filled_rulelist() {
            grammatical_rule_list* ls = (grammatical_rule_list*)malloc(sizeof(grammatical_rule_list));

            ls->roots = new std::vector<gram_tnode*>();

            return ls;
        }
};

int vec_find_node(
    gram_tnode* target,
    std::vector<gram_tnode*>* vec) {
    int i = 0;
    for(gram_tnode* node : *vec) {
        if (node->value == target->value &&
            node->type == target->type) {
                return i;
            }
        i++;
    }

    return -1;
}





void add_gram_rule(
    std::vector<gram_tnode*>* ls,
    std::initializer_list<gram_tnode*> nodes,
    meta_type rule_type)
{
    std::vector<gram_tnode*>* t = (ls);

    gram_tnode* last = NULL;

    for(auto item : nodes) {
        t->push_back(item);
        t = (item->childrens); 
        last = item;
    }

    last->rule_name = rule_type;
}






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

token_alphabet token_defs;

class LexToken {
    
    public:
        std::string value;
        cnst_TokenType type;
        
        LexToken(cnst_TokenType cnst_type, std::string val) {
            printf("ctor: char=%s \n", val);
            value = val;
            type = cnst_type;
        }

};


void vec_dump(std::vector<gram_tnode*>* vec) {
    for(gram_tnode* i : *vec) {
        if (i->value != NULL) {
            printf("vec<token> [val=%s, type=%i] \n", i->value->c_str(), i->type);
        }
        else {
            printf("vec<token> [val=NULL, type=%i] \n", i->type);
        }
    }
}

// varsets
// funcs
// cycles
// 

class nodeRef_utils {
    public:
        std::vector<gram_tnode*>* refs;
        std::vector<meta_type>* ref_types;
    
    public:
        gram_tnode* get_node_byref(meta_type type) {
            int i = 0;
            
            // fix
            for(int i = 0; i < ref_types->size(); i++) {
                if (ref_types->at(i) == type) {
                    return refs->at(i);
                }
            }

            return NULL;
        }

        void add_ref_node(gram_tnode* node, meta_type ref_type) {
            refs->push_back(node);
            ref_types->push_back(ref_type);
        }
};

// to ctor
nodeRef_utils* nodeRef_utils_crt(){
            
    nodeRef_utils* ref = (nodeRef_utils*)malloc(sizeof(nodeRef_utils));

    ref->refs = new std::vector<gram_tnode*>();
    ref->ref_types = new std::vector<meta_type>();

    return ref;
}

nodeRef_utils* global_refs = nodeRef_utils_crt();

gram_tnode* tree_getEntry(
    std::vector<gram_tnode*>* vec,
    std::string* str) {

    printf("tree_getEntry: str=%s child_size=%i\n", str->c_str(), vec->size());
    
    if (vec->size() == 0) return NULL;

    vec_dump(vec);

    for(auto i : *vec) {
        if (i->value != NULL && i->value->compare(*str)) {
            return i;
        }
        else if (i->type == cnst_TokenType::VAR_NAME) {
            return i;
        }
        else if (i->type == cnst_TokenType::META_REF_RULE) {
            return i;
        }
        else if (i->parse_rule != meta_parse_rule::NO_RULE) {
            return i;
        }
        else if (i->type == cnst_TokenType::LITERAL_NUM) {
            return i;
        }
        else if (i->type == cnst_TokenType::SYMBOL_END) {
            return i;
        }
    }

    return NULL;
}


void build_ast(
    grammatical_rule_list* ls,
    std::vector<LexToken>* tokens) {
    
    int token_cap =  tokens->size();

    bool func_block_flag = false;

    for(int i = 0; i < token_cap; i++) {

        gram_tnode* entry = tree_getEntry(ls->roots, &(tokens->at(i).value));

        if (entry != NULL) {
            printf("ENTRY: token=%s i=%i\n", entry->value->c_str(), i);
            
            int j = i + 1;
            
            gram_tnode* temp = entry;
            gram_tnode* next = NULL;

            while(j < token_cap && (temp = tree_getEntry(temp->childrens, &(tokens->at(j).value))) != NULL) {
                
                //printf("INDEX=%i\n", j);

                printf("DEEP: token=%s rule=%i ref=%i parse=%i\n", temp->value != NULL ? temp->value->c_str() : "NULL", temp->ref_rule, temp->ref_rule, temp->parse_rule);
                
                // refs
                if (temp->ref_rule != meta_type::UNDF_META) {
                    temp = global_refs->get_node_byref(temp->ref_rule);
                }

                if (temp->rule_name != meta_type::UNDF_META) {
                    printf("defined (grammatic): %i\n", temp->rule_name);
                    break;
                }

                // rules
                if (temp->parse_rule != meta_parse_rule::NO_RULE) {
                    //printf("rule=%i\n", temp->parse_rule);

                    if (temp->parse_rule == meta_parse_rule::FUNC_PARAM_LIST) {
                        int cur = j;
                        while(cur < token_cap) {
                                printf("CUR = %s\n", tokens->at(cur).value.c_str());
                                auto token = tokens->at(cur);
                                if (token.type == cnst_TokenType::TYPE) {
                                    // type
                                }
                                else if (token.type == cnst_TokenType::VAR_NAME) {
                                    // var
                                }
                                else if (token.type == cnst_TokenType::SYMBOL) {
                                    // ,
                                }
                                else  {
                                    // токен - следующий
                                    // темп - текущий
                                    j = cur;
                                    temp = temp->childrens->at(0);
                                    printf("UPD: token[%i]=%s temp=%s \n", j, tokens->at(j).value.c_str(), temp->value->c_str());
                                    break;
                                }
                                cur++;
                        }
                    }
                    else if (temp->parse_rule == meta_parse_rule::FUNC_BLOCK_CODE) {
                        
                        //temp = temp->childrens->at(0);
                        //j--;
                        func_block_flag = true;
                        i = j-1;
                        printf("_______temp=%s\n", tokens->at(i).value.c_str()); 
                        //temp = tree_getEntry(ls->roots, &(tokens->at(i).value));
                        break;
                    }
                }

                // literals
                if (temp->type == cnst_TokenType::LITERAL_NUM) {
                    //temp = temp->childrens->at(0);
                }


                j++;
            }

            //i = j;
        }

    }
}





void make_rules(grammatical_rule_list* ls) {
    


    // generic.def_func
    // (<paramList>) {<func_block>}
    
    auto utils_root = gram_tnode_crt(&(token_defs.META_UTILS_RULE),  cnst_TokenType::UNDF);
    
    add_gram_rule(
        ls->roots, {
            utils_root
        },
        meta_type::UNDF_META);

    // refs.func_decl
    auto refs_funcDecl = gram_tnode_crt(&(token_defs._BR_LCIR),  cnst_TokenType::SYMBOL_BRACKET);
    add_gram_rule(
        utils_root->childrens, {
            refs_funcDecl,
            gram_tnode_crt(NULL,  cnst_TokenType::UNDF, meta_type::UNDF_META, meta_parse_rule::FUNC_PARAM_LIST),
            gram_tnode_crt(&(token_defs._BR_RCIR),  cnst_TokenType::SYMBOL_BRACKET),
            gram_tnode_crt(&(token_defs._BR_LFIG),  cnst_TokenType::SYMBOL_BRACKET),
            gram_tnode_crt(NULL,  cnst_TokenType::UNDF, meta_type::UNDF_META, meta_parse_rule::FUNC_BLOCK_CODE),
            gram_tnode_crt(&(token_defs._BR_RFIG),  cnst_TokenType::SYMBOL_BRACKET),
        },
        meta_type::FUNC_DECL);

    global_refs->add_ref_node(refs_funcDecl, meta_type::FUNC_DECL);

    //printf("GLOBAL REFS=%s\n", global_refs->get_node_byref(meta_type::FUNC_DECL)->value->c_str());

    // VARS.INT
    auto int_root = gram_tnode_crt(NULL,                    cnst_TokenType::VAR_NAME);

    // int t = 5;
    add_gram_rule(
        ls->roots, {
            gram_tnode_crt(&(token_defs.TYPE_INT),  cnst_TokenType::TYPE),
            int_root,
            gram_tnode_crt(&(token_defs._VAR_SET),  cnst_TokenType::VAR_SET),
            gram_tnode_crt(NULL,                    cnst_TokenType::LITERAL_NUM),
            gram_tnode_crt(&(token_defs._SMB_END),  cnst_TokenType::SYMBOL_END),
        },
        meta_type::VAR_INIT_INT);
    
    //int var(...) {}
    add_gram_rule(
        int_root->childrens, {
            gram_tnode_crt_REF_RULE(NULL, cnst_TokenType::META_REF_RULE, meta_type::FUNC_DECL)
        },
        meta_type::FUNC_DEF_INT);

    //printf("DYUMP %i\n", int_root->childrens->size());
    //vec_dump(int_root->childrens);

    // VARS.FLOAT
    add_gram_rule(
        ls->roots, {
            gram_tnode_crt(&(token_defs.TYPE_FLOAT),  cnst_TokenType::TYPE),
            gram_tnode_crt(NULL,                    cnst_TokenType::VAR_NAME),
            gram_tnode_crt(&(token_defs._VAR_SET),  cnst_TokenType::VAR_SET),
            gram_tnode_crt(NULL,                    cnst_TokenType::LITERAL_NUM),
            gram_tnode_crt(&(token_defs._SMB_END),  cnst_TokenType::SYMBOL_END),
        },
        meta_type::VAR_INIT_FLOAT);


    // VARS.DOUBLE
    add_gram_rule(
        ls->roots, {
            gram_tnode_crt(&(token_defs.TYPE_DOUBLE),  cnst_TokenType::TYPE),
            gram_tnode_crt(NULL,                    cnst_TokenType::VAR_NAME),
            gram_tnode_crt(&(token_defs._VAR_SET),  cnst_TokenType::VAR_SET),
            gram_tnode_crt(NULL,                    cnst_TokenType::LITERAL_NUM),
            gram_tnode_crt(&(token_defs._SMB_END),  cnst_TokenType::SYMBOL_END),
        },
        meta_type::VAR_INIT_DOUBLE);

    // VARS.BYTE
    add_gram_rule(
        ls->roots, {
            gram_tnode_crt(&(token_defs.TYPE_BYTE),  cnst_TokenType::TYPE),
            gram_tnode_crt(NULL,                    cnst_TokenType::VAR_NAME),
            gram_tnode_crt(&(token_defs._VAR_SET),  cnst_TokenType::VAR_SET),
            gram_tnode_crt(NULL,                    cnst_TokenType::LITERAL_NUM),
            gram_tnode_crt(&(token_defs._SMB_END),  cnst_TokenType::SYMBOL_END),
        },
        meta_type::VAR_INIT_BYTE);

    // VARS.LONG
    add_gram_rule(
        ls->roots, {
            gram_tnode_crt(&(token_defs.TYPE_LONG),  cnst_TokenType::TYPE),
            gram_tnode_crt(NULL,                    cnst_TokenType::VAR_NAME),
            gram_tnode_crt(&(token_defs._VAR_SET),  cnst_TokenType::VAR_SET),
            gram_tnode_crt(NULL,                    cnst_TokenType::LITERAL_NUM),
            gram_tnode_crt(&(token_defs._SMB_END),  cnst_TokenType::SYMBOL_END),
        },
        meta_type::VAR_INIT_LONG);

    // ------------------------- VARS.CHAR
    // char t = 't';
    gram_tnode* char_root = gram_tnode_crt(&(token_defs.TYPE_CHAR),  cnst_TokenType::TYPE);
    add_gram_rule(
        ls->roots, {
            char_root,
            gram_tnode_crt(NULL,                    cnst_TokenType::VAR_NAME),
            gram_tnode_crt(&(token_defs._VAR_SET),  cnst_TokenType::VAR_SET),
            gram_tnode_crt(NULL,                    cnst_TokenType::LITERAL_CHAR),
            gram_tnode_crt(&(token_defs._SMB_END),  cnst_TokenType::SYMBOL_END),
        },
        meta_type::VAR_INIT_CHAR);

    // char[] t;
    gram_tnode* char_var = gram_tnode_crt(NULL, cnst_TokenType::VAR_NAME);
    gram_tnode* char_close_br_ref = gram_tnode_crt(&(token_defs._BR_RSQUARE),  cnst_TokenType::SYMBOL_BRACKET);
    add_gram_rule(
        char_root->childrens, {
            gram_tnode_crt(&(token_defs._BR_LSQUARE),  cnst_TokenType::SYMBOL_BRACKET),
            char_close_br_ref,
            char_var,
            gram_tnode_crt(&(token_defs._SMB_END),  cnst_TokenType::SYMBOL_END),
        },
        meta_type::VAR_DEF_CHAR_ARR);
    
    // char[] t = "QWD";
    add_gram_rule(
        char_var->childrens, {
            gram_tnode_crt(&(token_defs._VAR_SET),  cnst_TokenType::VAR_SET),
            gram_tnode_crt(NULL,  cnst_TokenType::LITERAL_STRING),
            gram_tnode_crt(&(token_defs._SMB_END),  cnst_TokenType::SYMBOL_END),
        },
        meta_type::VAR_INIT_CHAR_ARR);

    // char[] "abc";
    add_gram_rule(
        char_close_br_ref->childrens, {
            gram_tnode_crt(NULL,  cnst_TokenType::LITERAL_STRING),
            gram_tnode_crt(&(token_defs._SMB_END),  cnst_TokenType::SYMBOL_END),
        },
        meta_type::INIT_CHAR_ARR);

    // ------------------------------- VARS.STRING
    //string s = "123"
    gram_tnode* str_node = gram_tnode_crt(&(token_defs._VAR_SET),  cnst_TokenType::VAR_SET);
    add_gram_rule(
        ls->roots, {
            gram_tnode_crt(&(token_defs.TYPE_STRING),  cnst_TokenType::TYPE),
            gram_tnode_crt(NULL,                    cnst_TokenType::VAR_NAME),
            str_node,
            gram_tnode_crt(NULL,                    cnst_TokenType::LITERAL_STRING),
            gram_tnode_crt(&(token_defs._SMB_END),  cnst_TokenType::SYMBOL_END),
        },
        meta_type::VAR_INIT_STRING);

    //string x = char[] "abc";
    add_gram_rule(
        str_node->childrens, {
            gram_tnode_crt_REF_RULE(NULL,           cnst_TokenType::META_REF_RULE, meta_type::INIT_CHAR_ARR)
        },
        meta_type::VAR_INIT_STRING);

}






class Lexer {

    private:
        // TODO: to tree?
        cnst_TokenType prse_word(std::string str) {
            

            if (str == token_defs.TYPE_INT ||
                str == token_defs.TYPE_DOUBLE ||
                str == token_defs.TYPE_CHAR ||
                str == token_defs.TYPE_FLOAT ||
                str == token_defs.TYPE_BYTE ||
                str == token_defs.TYPE_LONG ||
                str == token_defs.TYPE_STRING) {
                return TYPE;
            }

            if (str == token_defs.RW_AUTO ||
                str == token_defs.RW_CLASS ||
                str == token_defs.RW_FOR ||
                str == token_defs.RW_FUNC ||
                str == token_defs.RW_NEXT ||
                str == token_defs.RW_WHILE ||
                str == token_defs.RW_RET) {
                    return RESERVE_WORD;
                }
            
            char s = str.at(0);
            if (s == token_defs.BR_LCIR ||
                s == token_defs.BR_LFIG ||
                s == token_defs.BR_LSQUARE ||
                s == token_defs.BR_RCIR ||
                s == token_defs.BR_RFIG ||
                s == token_defs.BR_RSQUARE)
                return SYMBOL_BRACKET;

            if (s == token_defs.SMB_CMA ||
                s == token_defs.SMB_DOT ||
                s == token_defs.SMB_END) {
                    return SYMBOL;
                }

            if (str.length() > 0) {
                if ((s >= 'a' && s <= 'z') ||
                    (s >= 'A' && s <= 'Z')) {
                        return VAR_NAME;
                    }
                else {
                    // error varname
                }
            }

            if (s == token_defs.OP_DIV ||
                s == token_defs.OP_MIN ||
                s == token_defs.OP_MUL ||
                s == token_defs.OP_PDIV ||
                s == token_defs.OP_PL)
                return MATH_OPERATION;

            if (str == token_defs.CMP_EQ ||
                str == token_defs.CMP_EQLEFT ||
                str == token_defs.CMP_EQRIGHT ||
                s == token_defs.CMP_LEFT ||
                str == token_defs.CMP_NEQ ||
                s == token_defs.CMP_RIGHT)
                return MATH_COMPARE;

            // TODO: m_compare, m_operation, m_logic, var_set
            if (s == token_defs.LOG_AND ||
                s == token_defs.LOG_OR ||
                str == token_defs.LOG_DAND ||
                str == token_defs.LOG_DOR)
                return MATH_LOGIC;

            if (s == token_defs.VAR_SET)
                return VAR_SET;

            return cnst_TokenType::UNDF;
        }

        bool is_br_tk(char c) {
            return c == token_defs.BR_LCIR ||
                c == token_defs.BR_LFIG ||
                c == token_defs.BR_LSQUARE ||
                c == token_defs.BR_RCIR ||
                c == token_defs.BR_RFIG ||
                c == token_defs.BR_RSQUARE;
        }

        bool is_smb_tk(char c) {
            return c == token_defs.SMB_CMA ||
                c == token_defs.SMB_DOT ||
                c == token_defs.SMB_END;
        }

        bool is_operand_tk(char c) {
            return c == token_defs.OP_PL ||
                c == token_defs.OP_DIV ||
                c == token_defs.OP_MIN ||
                c == token_defs.OP_MUL ||
                c == token_defs.OP_PDIV;
        }

        // fix, parse each literal type in function
        bool is_num_literal(char c) {
            return c >= '0' && c <= '9';
        }

        bool is_varname_char(char c) {
            return (c >= 'a' && c <= 'z') ||
                (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9');
        }

        cnst_TokenType parse_two_smb_tk(char cur, char next) {
            if ((cur == '=' && next == '>') ||
                (cur == '=' && next == '=') ||
                (cur == '<' && next == '=') ||
                (cur == '!' && next == '=') ||
                (cur == '<') || (cur == '>')) {
                return MATH_COMPARE;
            }
            else if ((cur == '&' && next == '&') ||
                (cur == '|' && next == '|') ||
                (cur == '&') || (cur == '|')) {
                    return MATH_LOGIC;
                }
            else if (cur == '=') {
                return VAR_SET;
            }
            else return cnst_TokenType::UNDF;
        }

        cnst_TokenType parse_one_smb_tk(char cur) {
            if ((cur == '<') || (cur == '>')) {
                return MATH_COMPARE;
            }
            else if ((cur == '&') || (cur == '|')) {
                    return MATH_LOGIC;
                }
            else if (cur == '=') {
                return VAR_SET;
            }
            else return cnst_TokenType::UNDF;
        }




    public:
        
        static Lexer* crt() {
            Lexer* lex = (Lexer*)malloc(sizeof(Lexer));
            return lex;
        }

        void make_tokens(std::string* src, std::vector<LexToken>* tokens) {
 
            size_t stlen = src->length();
            std::cout << stlen << std::endl;

            std::ostringstream bld;
            size_t cur = 0;
            size_t temp = 0;

            // TODO_3: fix char substr,. control memory


            while(cur < stlen && src->at(cur) != '\0') {
                
                if (this->is_br_tk(src->at(temp))) {

                    tokens->push_back(
                        LexToken(
                            SYMBOL_BRACKET,
                            src->substr(temp, 1)));

                    cur++;
                    temp = cur;
                    continue;
                }
                else if (this->is_smb_tk(src->at(temp))) {

                    tokens->push_back(
                        LexToken(
                            SYMBOL,
                            src->substr(temp, 1)));

                    cur++;
                    temp = cur;
                    continue;
                }
                else if (this->is_operand_tk(src->at(temp))) {
                    tokens->push_back(
                        LexToken(
                            OPERAND,
                            src->substr(temp, 1)));

                    cur++;
                    temp = cur;
                    continue;
                }
                else if (cur +1 < stlen) {
                    
                    // 2 symbol tokens
                    cnst_TokenType token_type = parse_two_smb_tk(src->at(cur), src->at(cur+1));
                    
                    if (token_type != UNDF) {
                        tokens->push_back(
                            LexToken(
                                token_type,
                                src->substr(cur, 2)));

                        cur+=2;
                        temp = cur;
                        continue;
                    }

                    // 1  symbol tokens
                    token_type = parse_one_smb_tk(src->at(cur));
                    
                    if (token_type != UNDF) {
                        tokens->push_back(
                            LexToken(
                                token_type,
                                src->substr(cur, 1)));

                        cur+=1;
                        temp = cur;
                        continue;
                    }
                }

                bool is_num_lit = false;
                bool is_str_lit = false;
                bool is_char_lit = false;
                bool is_varname = false;

                while(temp < stlen && src->at(temp) != ' ') {
                    
                    // LITERALS.NUMBER
                    if (this->is_num_literal(src->at(temp)) && bld.tellp() == 0) {

                        is_num_lit = true;
                        
                        while(temp < stlen) {
                            if (this->is_num_literal(src->at(temp))) {
                                // add lit_bld
                                bld.put(src->at(temp++));
                            }
                            else if (src->at(temp) == '.') {
                                
                                if (bld.tellp() == 0) {
                                    // error ' .243 '
                                }
                                else {
                                    bld.put(src->at(temp++));
                                }
                            }
                            else {
                                temp--;
                                break;
                            }
                            //temp++;
                        }

                        cur = temp;
                        break;
                    }
                    
                    //  LITERALS.STRING
                    char curc = src->at(temp);
                    if (curc == '"'  && bld.tellp() == 0) {
                        is_str_lit = true;
                        temp++;

                        while(temp < stlen) {
                            if (src->at(temp) == '"') {
                                break;
                            }
                            else {
                                bld.put(src->at(temp++));
                            }
                        }

                        cur = temp;
                        break;
                    }

                    // LITERALS.CHAR
                    if (curc == '\'' && bld.tellp() == 0) {
                        if ((temp+2) < stlen &&
                            src->at(temp+2) == '\'') {
                            bld.put(src->at(temp+1));
                            is_char_lit = true;

                            cur = temp+2;
                            break;
                        }
                        else {
                            // error char
                        }
                    }

                    // VARNAME STOP
                    if (this->is_varname_char(curc) == false) {
                        cur = temp;
                        break;
                    }

                    bld.put(src->at(temp++));
                }

                if (is_num_lit) {
                    tokens->push_back(
                        LexToken(LITERAL_NUM, bld.str()));
                }
                else if (is_str_lit) {
                    tokens->push_back(
                        LexToken(LITERAL_STRING, bld.str()));
                }
                else if (is_char_lit) {
                    tokens->push_back(
                        LexToken(LITERAL_CHAR, bld.str()));
                }
                else if (is_varname) {
                tokens->push_back(
                        LexToken(VAR_NAME, bld.str()));
                }
                else if (bld.tellp() > 0) {
                    
                    cnst_TokenType ttype = prse_word(bld.str());
                    if (ttype != UNDF) {

                        tokens->push_back(
                            LexToken(ttype, bld.str()));

                        cur = temp;
                        bld.str("");
                        continue;
                    }
                    else if(ttype == UNDF) {
                        // error
                    }
                }

                cur++;
                temp = cur;
                bld.str("");
            }
        }
};


void file__readSource(const char* path, std::string *str) {
    if (str != NULL) {
        std::ifstream rd(path);
        
        if (rd.is_open()) {
            while(std::getline(rd, *str, '\0')) 
            {}
        }

        rd.close();
    }
}


void vec_dump(std::vector<LexToken>* vec) {
    for(LexToken i : *vec) {
        printf("vec<token> [val=%s, type=%i] \n", i.value.c_str(), i.type);
    }
}


void vec_dump_node(gram_tnode* vec, int deep = 1) {

    if (vec->value !=  NULL) {
        printf("%s", vec->value->c_str());
    }
        if (vec->type == cnst_TokenType::TYPE) {
            printf("%s ", vec->value == NULL ? "<type>" : "");
        }
        else if (vec->type == cnst_TokenType::VAR_NAME) {
            printf("%s ", vec->value == NULL ? "<var>" : "");
        } 
        else if (vec->type == cnst_TokenType::LITERAL_NUM) {
            printf("%s ", vec->value == NULL ? "<lit_num>" : "");
        }
        else if (vec->type == cnst_TokenType::LITERAL_CHAR) {
            printf("%s ", vec->value == NULL ? "<lit_char>" : "");
        }
        else if (vec->type == cnst_TokenType::LITERAL_STRING) {
            printf("%s ", vec->value == NULL ? "<lit_str>" : "");
        }
        else if (vec->type == cnst_TokenType::META_REF_RULE) {
            printf("@ref=%i ", vec->ref_rule);
        }

    if (vec->rule_name != meta_type::UNDF_META) {
        printf(" | ");
    }

    for(auto i : *vec->childrens) {
        vec_dump_node(i, deep+1);
    }

    if (deep == 1) 
        printf("\n");
}

void dump_rules(grammatical_rule_list* ls) {
    for(auto i : *ls->roots) {
        vec_dump_node(i);
    }
}

int main() {

    Lexer* lex = Lexer::crt();

    std::string src;
    file__readSource("src.nem", &src);

    std::vector<LexToken> vec;

    lex->make_tokens(&src, &vec);

    vec_dump(&vec);

    grammatical_rule_list* ls = grammatical_rule_list::crt_filled_rulelist();

    make_rules(ls);

    //dump_rules(ls);

    build_ast(ls, &vec);

    //std::cout << ls->roots->at(0)->childrens->at(0)->value->c_str() << std::endl;

    return 0;
}