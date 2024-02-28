#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdarg.h>
#include <vector>

#include "grammatic.h"

token_alphabet token_defs;
nodeRef_utils* global_refs = nodeRef_utils::crt();

class LexToken {
    
    public:
        std::string value;
        cnst_TokenType type;
        
        LexToken(cnst_TokenType cnst_type, std::string val) {
            //printf("ctor: char=%s \n", val);
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

void vec_dump_ref(std::vector<LexToken*>* vec) {
    for(LexToken* i : *vec) {
        printf("vec<token*> [val=%s, type=%i] \n", i->value.c_str(), i->type);
    }
}

bool vec_find(std::vector<gram_tnode*>* vec, gram_tnode* tar) {
    for(auto i : *vec) {
        if (i == tar) {
            return true;
        }
    }
    return false;
}


int get_instr_pre_src(
    int offset,
    std::vector<LexToken>* tokens,
    std::vector<LexToken*>* pre_instr_list) {
    
    pre_instr_list->clear();

    // read until [cur] != '{', ';'
    for(int i = offset; i < tokens->size(); i++) {
        
        std::string cur_s = tokens->at(i).value;

        pre_instr_list->push_back(&(tokens->at(i)));

        if (cur_s == "{" || cur_s == ";") {
            return i+1;
        }
    }

    return -1;
}

std::vector<gram_tnode*> temp_vec;

std::vector<gram_tnode*>* deep_getEntries(
    std::vector<gram_tnode*>* ls,
    LexToken* target) {
    
    std::vector<gram_tnode*>* ents = &temp_vec;

    ents->clear();

    for(auto item : *ls) {
        if (item->value != NULL && *item->value == target->value) {
            
            ents->push_back(item);
        }
        else if (item->type == cnst_TokenType::VAR_NAME) {
            ents->push_back(item);
        }
        else if (item->type == cnst_TokenType::VAR_SET) {
            ents->push_back(item);
        }
        else if (item->type == cnst_TokenType::LITERAL_CHAR) {
            ents->push_back(item);
        }
        else if (item->type == cnst_TokenType::LITERAL_NUM) {
            ents->push_back(item);
        }
        else if (item->type == cnst_TokenType::LITERAL_STRING) {
            ents->push_back(item);
        }
        else if (item->type == cnst_TokenType::SYMBOL_END) {
            ents->push_back(item);
        }
        else if (item->type == cnst_TokenType::SYMBOL_BRACKET) {
            ents->push_back(item);
        }
        else if (item->type == cnst_TokenType::SYMBOL) {
            ents->push_back(item);
        }
        else if (item->type == cnst_TokenType::META_REF_RULE) {
            ents->push_back(item);
        }
    }

    return ents;
}



// return - new offset
// 
int make_offset_ref(
    int offset,
    std::vector<LexToken*>* pre_instr_list,
    gram_tnode* deep_root) {

    if (deep_root->rule_name != meta_type::UNDF_META) {
        return offset;
    }

    // int t, char b) 
    if (deep_root->parse_rule == meta_parse_rule::FUNC_PARAM_LIST) {
        for(int i = offset; i < pre_instr_list->size(); i++) {
            
            LexToken* cur = pre_instr_list->at(i);

            //printf("[make_offset_ref] val=%s\n", cur->value.c_str());

            if (cur->value == token_defs._BR_LFIG) {
                return i;
            }
            // todo: parse type, parse varname
        }
    }
    else {
        deep_root = deep_root->childrens->at(0);
        return make_offset_ref(offset, pre_instr_list, deep_root);
    }

    return -1;
    
        // parse_rule
        // rule_nam,e
        // spec type
        // value=value
}


class offset_instr {
    public:
        int instr_offset;
        gram_tnode* instr_node;
};

offset_instr* make_offset(
    int offset,
    std::vector<LexToken*>* pre_instr_list,
    gram_tnode* deep_root) {
    

    if (deep_root->rule_name != meta_type::UNDF_META) {
        offset_instr* ret = (offset_instr*)malloc(sizeof(offset_instr));
        ret->instr_node = deep_root;
        ret->instr_offset = offset;
        return ret;
    }

    // int t, char b) 
    if (deep_root->parse_rule == meta_parse_rule::FUNC_PARAM_LIST) {
        for(int i = offset; i < pre_instr_list->size(); i++) {
            
            LexToken* cur = pre_instr_list->at(i);

            if (cur->value == token_defs._BR_LCIR) {
                return make_offset(offset, pre_instr_list, deep_root->childrens->at(0));
            }
            // todo: parse type, parse varname
        }
    }
    else {
        deep_root = deep_root->childrens->at(0);
        return make_offset(offset, pre_instr_list, deep_root);
    }

    return NULL;
}

// build_instr (instr, vec<lex>)

bool deep_build(
    gram_tnode* root,
    std::vector<gram_tnode*>* vis,
    std::vector<LexToken*>* pre_instr_list,
    int pre_instr_index) {

    vis->push_back(root);

    auto c = pre_instr_list->at(pre_instr_index);
    if (root->type == c->type && c->type == cnst_TokenType::VAR_NAME) {
        pre_instr_index++;
    }
    else if (root->type == c->type && c->type == cnst_TokenType::VAR_SET) {
        pre_instr_index++;
    }
    else if (root->type == c->type && c->type == cnst_TokenType::LITERAL_NUM) {
        pre_instr_index++;
    }
    else if (root->type == c->type && c->type == cnst_TokenType::TYPE) {
        pre_instr_index++;
    }
    else if (root->value != NULL && *root->value == c->value) {
        pre_instr_index++;
    }
    else pre_instr_index--;

    if (pre_instr_index < 0) pre_instr_index = 0;

    //printf("(--->deep instr[%i]) %s parse=%i type=%i rule=%i\n", pre_instr_index, root->value != NULL ? root->value->c_str() : "NULL", root->parse_rule, root->type, root->rule_name);

    // if parse_rule
    if (root->parse_rule != meta_parse_rule::NO_RULE) {
        //printf("[refff name] %i \n", root->ref_rule);
        printf("[parse rule] val=%s\n", root->value->c_str());
    }
    else if (root->ref_rule != meta_type::UNDF_META) {
        //printf("[refff name] %i \n", root->ref_rule);
        gram_tnode* ref = global_refs->get_node_byref(root->ref_rule);
        if (ref != NULL) {
            
            //printf("[REF. offset=%i]\n", pre_instr_index);
            pre_instr_index = make_offset_ref(pre_instr_index, pre_instr_list, ref);

            //deep_build(root->childrens->at(0), vis, pre_instr_list, pre_instr_index);
            if (pre_instr_index != -1) {
                offset_instr* offset_instr = make_offset(pre_instr_index, pre_instr_list, root);
                if (offset_instr != NULL) {
                    printf("[REF_INSTR] offset=%i instr=%i \n", offset_instr->instr_offset, offset_instr->instr_node->rule_name);
                }
            }
        } 
    }
    else if (root->rule_name != meta_type::UNDF_META) {

        if (pre_instr_index == pre_instr_list->size()) {
            printf("[INSTR] %i \n", root->rule_name);
            return true;
        }
    }
    
    if (root == NULL) {
        return false;
    }
    if (pre_instr_index >= pre_instr_list->size()) {
        return false;
    }

    std::vector<gram_tnode*>* ents = deep_getEntries(root->childrens, pre_instr_list->at(pre_instr_index));

    for(gram_tnode* item : *ents) {
        if (vec_find(vis,item) == false) {
            
            if (item != NULL) {
                deep_build(item, vis, pre_instr_list, pre_instr_index);
            }
        }
    }

    return false;
}

gram_tnode* find_entry(grammatical_rule_list* ls, LexToken* token) {
    for(auto i : *ls->roots) {
        if (*(i->value) == token->value) {
            return i;
        }
    }

    return NULL;
} 

void build_instr_tree(
    grammatical_rule_list* gram_list,
    std::vector<LexToken>* tokens) {
    
    // read instr_pre_src
    // parse instr_pre list
    // -> add instr to tree
    // get offset, read now pt.1

    std::vector<LexToken*>* pre_instr_vec = (std::vector<LexToken*>*)malloc(sizeof(std::vector<LexToken*>));
    std::vector<gram_tnode*>* visited = (std::vector<gram_tnode*>*)malloc(sizeof(std::vector<gram_tnode*>));

    int offset = 0;
    offset = get_instr_pre_src(offset, tokens, pre_instr_vec);

    while(pre_instr_vec->size() > 0) {
        
        printf("-----------\n");
        vec_dump_ref(pre_instr_vec);

        gram_tnode* entry = find_entry(gram_list, pre_instr_vec->at(0)); 

        if (entry != NULL) {
            printf("--> entry=%s\n", entry->value->c_str());

            visited->clear();
            deep_build(entry, visited, pre_instr_vec, 0);
        }

       offset = get_instr_pre_src(offset, tokens, pre_instr_vec);
    }
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

    //vec_dump(&vec);

    grammatical_rule_list* ls = grammatical_rule_list::crt_filled_rulelist();

    ls->make_rules(ls);

    //dump_rules(ls);

    build_instr_tree(ls, &vec);

    //std::cout << ls->roots->at(0)->childrens->at(0)->value->c_str() << std::endl;

    return 0;
}