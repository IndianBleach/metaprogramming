#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdarg.h>
#include <vector>

#include "grammatic.h"
#include "lexer.h"
#include "utils.h"

#include "instr_builder.h"

extern token_alphabet token_defs;
extern nodeRef_utils* global_refs;
extern std::vector<gram_tnode*> temp_vec;


/* 
    TODO_1
    instr_builder.build_instr_tree( path ) -> file_instr_tree;
        - 
    
    instr_tree

    -> to file
    expressions

*/




gram_tnode* find_entry(grammatical_rule_list* ls, LexToken* token) {
    for(auto i : *ls->roots) {
        if (*(i->value) == token->value) {
            return i;
        }
    }

    return NULL;
} 

std::vector<gram_tnode*>* find_entries(std::vector<gram_tnode*>* ls,LexToken* target) {
    
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


class offset_instr {
    public:
        int instr_offset;
        gram_tnode* instr_node;
};

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

    std::vector<gram_tnode*>* ents = find_entries(root->childrens, pre_instr_list->at(pre_instr_index));

    for(gram_tnode* item : *ents) {
        if (gram_tnode::vec_find_gram(vis,item) == false) {
            
            if (item != NULL) {
                deep_build(item, vis, pre_instr_list, pre_instr_index);
            }
        }
    }

    return false;
};

void instr_builder::build_tree(
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

