#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <stdarg.h>

#include <vector>

// token (type, val)

enum cnst_TokenType {
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
    UNDF = -1
};

class gram_tnode {
    public:
        std::vector<gram_tnode*>* childrens;
        std::string* value;
        cnst_TokenType type;
        // todo: some metadata? (blockLevel, priority)
        gram_tnode(std::string* _val, cnst_TokenType _type) {
            
            value = _val;
            type = _type;
            childrens = new std::vector<gram_tnode*>();
        };

        // / ctr()
};

class utils {
    public:
        gram_tnode* gram_tnode_crt(std::string* _val, cnst_TokenType _type) {
            gram_tnode* nd = (gram_tnode*)malloc(sizeof(gram_tnode));
            
            nd->value = _val;
            nd->type = _type;
            nd->childrens = new std::vector<gram_tnode*>();

            return nd;
        }
};

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

void add_rule(std::vector<gram_tnode*>* rootList, int n, gram_tnode* nodes[]) {

    //gram_tnode* t = root;

    printf("ADD %i\n", (int)(rootList->size()));

    std::vector<gram_tnode*>* t = rootList;
    
    for(int i=0;i<n; i++)
    {
        printf("ADD %i\n", (int)(t->size()));
        t->push_back(nodes[i]);

        t = t->at(t->size()-1)->childrens;
    }
}

void rules_add_varsets(grammatical_rule_list* ls) {
    
    /*
    ls->roots->push_back(gram_tnode {
        "int",
        TYPE,
    });

    ls->roots->at(0).childrens->push_back(gram_tnode {
        "int",
        TYPE,
    });
    */


}


class token_alphabet {
    public:
        // types
        const char* TYPE_INT = "int";
        const char* TYPE_DOUBLE = "double";
        const char* TYPE_CHAR = "char";
        const char* TYPE_FLOAT = "float";
        const char* TYPE_BYTE = "byte";
        const char* TYPE_LONG = "long";
        const char* TYPE_STRING = "string";

        // brackets
        const char BR_RCIR = ')';
        const char BR_RSQUARE = ']';
        const char BR_RFIG = '}';
        const char BR_LCIR = '(';
        const char BR_LSQUARE = '[';
        const char BR_LFIG = '{';

        // operands
        const char OP_PL = '+';
        const char OP_MIN = '-';
        const char OP_MUL = '*';
        const char OP_PDIV = '%';
        const char OP_DIV = '/';

        // logic
        const char LOG_AND = '&';
        const char LOG_OR = '|';
        const char* LOG_DAND = "&&";
        const char* LOG_DOR = "||";

        // compare
        const char CMP_LEFT = '<';
        const char CMP_RIGHT = '>';
        const char* CMP_EQLEFT = "<=";
        const char* CMP_EQRIGHT = ">=";
        //const char* CMP_EQLEFT = "<=";
        const char* CMP_EQ = "==";
        const char* CMP_NEQ = "!=";

        // set
        const char VAR_SET = '='; 

        // symbols
        const char SMB_DOT = '.';
        const char SMB_END = ';';
        const char SMB_CMA = ',';
        const char SMB_VAR = '=';

        // reserve words
        const char* RW_FOR = "for";
        const char* RW_AUTO = "auto";
        const char* RW_WHILE = "while";
        const char* RW_FUNC = "func";
        const char* RW_CLASS = "class";
        const char* RW_NEXT = "next";
        const char* RW_RET= "return";
};

/*

class TokenDefTreeNode {
    public:
        std::vector<TokenDefTreeNode*>* childrens;
        char value;
        cnst_TokenType complete_flag;

        TokenDefTreeNode(char val) {
            childrens = (std::vector<TokenDefTreeNode*>*)malloc(sizeof(std::vector<TokenDefTreeNode*>));
            value = val;
        }

        TokenDefTreeNode* crt(char val) {
            TokenDefTreeNode* node = (TokenDefTreeNode*)malloc(sizeof(TokenDefTreeNode));
            node->childrens = (std::vector<TokenDefTreeNode*>*)malloc(sizeof(std::vector<TokenDefTreeNode*>));
            node->value = val;
            node->complete_flag = UNDF;
        }

        void put_word(TokenDefTreeNode* root, char* str) {

        }

        void index_of(TokenDefTreeNode* root)
        {}

        TokenDefTreeNode* crt_fulldef_root() {
            TokenDefTreeNode* root = crt('_');
            // add types
        }
};
*/


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

token_alphabet token_defs;

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

            return UNDF;
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
            else return UNDF;
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
            else return UNDF;
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


int main() {

    //Lexer* lex = Lexer::crt();

    //std::string src;
    //file__readSource("src.nem", &src);

    //std::vector<LexToken> vec;

    //lex->make_tokens(&src, &vec);

    //std::cout << src << std::endl;

    //vec_dump(&vec);

    grammatical_rule_list* ls = grammatical_rule_list::crt_filled_rulelist();

    utils ut = utils();

    printf("deb\n");


    gram_tnode* data[2] = {
        ut.gram_tnode_crt(new std::string("int"), TYPE),
        ut.gram_tnode_crt(new std::string("_"), VAR_SET),
    };

    printf("deb2\n");

    add_rule(ls->roots, 2, data);


    std::cout << ls->roots->at(0)->childrens->at(0)->value->c_str() << std::endl;

    return 0;
}