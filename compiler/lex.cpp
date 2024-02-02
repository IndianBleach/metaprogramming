#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <vector>

// token (type, val)


enum cnst_TokenType {
    ARRAY,

    LIST_PARAMS,
    LIST_VALUES,
    LIST_VARS,

    TYPE,
    RESERVE_WORD,
    OPERAND,
    SYMBOL_BRACKET,
    SYMBOL,
    UNDF = -1
};


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

        // symbols
        const char SMB_DOT = '.';
        const char SMB_END = ';';
        const char SMB_CMA = ',';

        // reserve words
        const char* RW_FOR = "for";
        const char* RW_AUTO = "auto";
        const char* RW_WHILE = "while";
        const char* RW_FUNC = "func";
        const char* RW_CLASS = "class";
        const char* RW_NEXT = "next";
};


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


class LexToken {
    
    public:
        const char* value;
        cnst_TokenType type;
        
        LexToken(cnst_TokenType cnst_type, const char* val) {
            value = val;
            type = cnst_type;
        }

};

token_alphabet token_defs;

class Lexer {

    private:
        // TODO: to tree?
        cnst_TokenType prse_word(std::string str) {
            
            //const char* s = str.c_str();

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
                str == token_defs.RW_WHILE) {
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

            // TODO: add op
            return UNDF;
        }

    public:
        
        static Lexer* crt() {
            Lexer* lex = (Lexer*)malloc(sizeof(Lexer));
            return lex;
        }

        void make_tokens(std::string* src, std::vector<LexToken>* tokens) {
            /*
                int main() {
                    int t = 5;

                    return 0;
                }
            */

            size_t stlen = src->length();
            std::cout << stlen << std::endl;


            std::ostringstream bld;
            size_t cur = 0;
            size_t temp = 0;

            while(cur < stlen && src->at(cur) != '\0') {
                
                // to next ' '
                while(temp < stlen && src->at(temp) != ' ') {
                    bld.put(src->at(temp));
                    
                    // TODO: add symbols and breaks

                    temp++;
                }

                if (src->at(cur) != ' ') {
                    cnst_TokenType ttype = prse_word(bld.str());
                    if (ttype != UNDF) {

                        const char* cv = bld.str().c_str();
                        std::cout << "token - " << cv << std::endl;
                        
                        // TODO: fix
                        tokens->push_back(
                            LexToken(ttype, cv));
                    }
                    bld.str("");
                }

                cur++;
                temp = cur;
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



int main() {

    Lexer* lex = Lexer::crt();
    //Lexer::token_defs = defs;
    //lex->token_defs = defs;

    std::string src;
    file__readSource("src.nem", &src);

    std::vector<LexToken> vec;
    lex->make_tokens(&src, &vec);

    std::cout << src << std::endl;
    std::cout << token_defs.TYPE_DOUBLE << std::endl;
    std::cout << vec.at(0).value << std::endl;

    return 0;
}