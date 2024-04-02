#ifndef _UTILS_H_
#define _UTILS_H

#include "utils.h"

void vec_dump(std::vector<gram_tnode*>* vec) {
    for (gram_tnode* i : *vec) {
        if (i->value != NULL) {
            printf("vec<token> [val=%s, type=%i] \n", i->value->c_str(), i->type);
        }
        else {
            printf("vec<token> [val=NULL, type=%i] \n", i->type);
        }
    }
}

void vec_dump_ref(std::vector<LexToken*>* vec) {
    for (LexToken* i : *vec) {
        printf("vec<token*> [val=%s, type=%i] \n", i->value.c_str(), i->type);
    }
}

void vec_dump_node(gram_tnode* vec, int deep = 1) {

    if (vec->value != NULL) {
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

    for (auto i : *vec->childrens) {
        vec_dump_node(i, deep + 1);
    }

    if (deep == 1)
        printf("\n");
}

void dump_rules(grammatical_rule_list* ls) {
    for (auto i : *ls->roots) {
        vec_dump_node(i);
    }
}

void vec_dump(std::vector<LexToken>* vec) {
    for (LexToken i : *vec) {
        printf("vec<token> [val=%s, type=%i] \n", i.value.c_str(), i.type);
    }
}

#endif