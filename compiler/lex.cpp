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

    VAR_NAME,
    LITERAL,
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

        // logic
        const char* LOG_EQ = "==";
        const char* LOG_AND = "&";
        const char* LOG_OR = "|";
        const char* LOG_DAND = "&&";
        const char* LOG_DOR = "||";

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
                
                // symbols, 
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

                // todo: +operands (all single values)


                // pull bld, ' '
                while(temp < stlen && src->at(temp) != ' ') {
                    
                    // TODO: парс литерала, вара ДО символа ;({
                    // TODO: +literals, vars
                    bld.put(src->at(temp));

                    temp++;
                }

                if (bld.tellp() > 0) {
                    
                    // TODO: prse_word добавить новые токены

                    cnst_TokenType ttype = prse_word(bld.str());
                    if (ttype != UNDF) {

                        tokens->push_back(
                            LexToken(ttype, bld.str()));
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


void vec_dump(std::vector<LexToken>* vec) {
    for(LexToken i : *vec) {
        printf("vec<token> [val=%s, type=%i] \n", i.value.c_str(), i.type);
    }
}


int main() {

    Lexer* lex = Lexer::crt();
    //Lexer::token_defs = defs;
    //lex->token_defs = defs;

    std::string src;
    file__readSource("src.nem", &src);

    std::vector<LexToken> vec;
    //LexToken* tk = new LexToken(SYMBOL_BRACKET, "test");

    //std::string t = {"test"};
    //LexToken* tk = new LexToken(SYMBOL_BRACKET, t);

    //vec.push_back(LexToken(SYMBOL_BRACKET, "test"));
    lex->make_tokens(&src, &vec);

    std::cout << src << std::endl;
    std::cout << token_defs.TYPE_DOUBLE << std::endl;

    vec_dump(&vec);

    return 0;
}