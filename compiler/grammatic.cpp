#include "grammatic.h"


gram_tnode::gram_tnode(std::string* _val, cnst_TokenType _type) {
            value = _val;
            type = _type;
            meta_inited = true;
            childrens = new std::vector<gram_tnode*>();
            parse_rule = meta_parse_rule::NO_RULE;
};

gram_tnode::gram_tnode(std::string* _val, cnst_TokenType _type, meta_type _rule_name) {
            
            value = _val;
            type = _type;
            rule_name = _rule_name;
            meta_inited = true;
            childrens = new std::vector<gram_tnode*>();
            parse_rule = meta_parse_rule::NO_RULE;
};

gram_tnode* gram_tnode::gram_tnode_crt(const std::string* _val, cnst_TokenType _type) {
            gram_tnode* nd = (gram_tnode*)malloc(sizeof(gram_tnode));
            
            nd->value = _val;
            nd->type = _type;
            nd->meta_inited = true;
            nd->childrens = new std::vector<gram_tnode*>();

            nd->rule_name = meta_type::UNDF_META;
            nd->ref_rule = meta_type::UNDF_META;
            nd->parse_rule = meta_parse_rule::NO_RULE;

            return nd;
        };

gram_tnode* gram_tnode::gram_tnode_crt(const std::string* _val, cnst_TokenType _type, meta_type rule_name) {
            gram_tnode* nd = (gram_tnode*)malloc(sizeof(gram_tnode));
            
            nd->value = _val;
            nd->type = _type;
            nd->meta_inited = true;
            nd->childrens = new std::vector<gram_tnode*>();

            nd->rule_name = rule_name;
            nd->ref_rule = meta_type::UNDF_META;

            nd->parse_rule = meta_parse_rule::NO_RULE;

            return nd;
        };

gram_tnode* gram_tnode::gram_tnode_crt(const std::string* _val, cnst_TokenType _type, meta_type rule_name, meta_parse_rule parse_rule) {
            gram_tnode* nd = (gram_tnode*)malloc(sizeof(gram_tnode));
            
            nd->value = _val;
            nd->type = _type;
            nd->meta_inited = true;
            nd->childrens = new std::vector<gram_tnode*>();

            nd->rule_name = rule_name;
            nd->ref_rule = meta_type::UNDF_META;

            nd->parse_rule = parse_rule;

            return nd;
        };

gram_tnode* gram_tnode::gram_tnode_crt_REF_RULE(const std::string* _val, cnst_TokenType _type, meta_type ref_rule) {
            gram_tnode* nd = (gram_tnode*)malloc(sizeof(gram_tnode));
            
            nd->value = _val;
            nd->type = _type;
            nd->meta_inited = true;
            nd->childrens = new std::vector<gram_tnode*>();

            nd->ref_rule = ref_rule;
            nd->rule_name = UNDF_META;

            nd->parse_rule = meta_parse_rule::NO_RULE;

            return nd;
        };



grammatical_rule_list* grammatical_rule_list::crt_filled_rulelist() {
            grammatical_rule_list* ls = (grammatical_rule_list*)malloc(sizeof(grammatical_rule_list));

            ls->roots = new std::vector<gram_tnode*>();

            return ls;
        };



void grammatical_rule_list::add_gram_rule(
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
};

gram_tnode* nodeRef_utils::get_node_byref(meta_type type)  {
            int i = 0;
            
            // fix
            for(int i = 0; i < ref_types->size(); i++) {
                if (ref_types->at(i) == type) {
                    return refs->at(i);
                }
            }

            return NULL;
        };

void nodeRef_utils::add_ref_node(gram_tnode* node, meta_type ref_type) {
            refs->push_back(node);
            ref_types->push_back(ref_type);
        };

nodeRef_utils* nodeRef_utils::crt(){
            
    nodeRef_utils* ref = (nodeRef_utils*)malloc(sizeof(nodeRef_utils));

    ref->refs = new std::vector<gram_tnode*>();
    ref->ref_types = new std::vector<meta_type>();

    return ref;
}

extern nodeRef_utils* global_refs;
extern token_alphabet token_defs;

void grammatical_rule_list::make_rules(grammatical_rule_list* ls) {
    


    // generic.def_func
    // (<paramList>) {<func_block>}
    
    auto utils_root = gram_tnode::gram_tnode_crt(&(token_defs.META_UTILS_RULE),  cnst_TokenType::UNDF);
    
    add_gram_rule(
        ls->roots, {
            utils_root
        },
        meta_type::UNDF_META);

    // refs.func_decl
    auto refs_funcDecl = gram_tnode::gram_tnode_crt(&(token_defs._BR_LCIR),  cnst_TokenType::SYMBOL_BRACKET);
    add_gram_rule(
        utils_root->childrens, {
            refs_funcDecl,
            gram_tnode::gram_tnode_crt(NULL,  cnst_TokenType::UNDF, meta_type::UNDF_META, meta_parse_rule::FUNC_PARAM_LIST),
            gram_tnode::gram_tnode_crt(&(token_defs._BR_RCIR),  cnst_TokenType::SYMBOL_BRACKET),
            gram_tnode::gram_tnode_crt(&(token_defs._BR_LFIG),  cnst_TokenType::SYMBOL_BRACKET),
        },
        meta_type::FUNC_DECL);

    global_refs->add_ref_node(refs_funcDecl, meta_type::FUNC_DECL);

    //printf("GLOBAL REFS=%s\n", global_refs->get_node_byref(meta_type::FUNC_DECL)->value->c_str());

    // VARS.INT
    auto int_root = gram_tnode::gram_tnode_crt(NULL,                    cnst_TokenType::VAR_NAME);

    // int t = 5;
    add_gram_rule(
        ls->roots, {
            gram_tnode::gram_tnode_crt(&(token_defs.TYPE_INT),  cnst_TokenType::TYPE),
            int_root,
            gram_tnode::gram_tnode_crt(&(token_defs._VAR_SET),  cnst_TokenType::VAR_SET),
            gram_tnode::gram_tnode_crt(NULL,                    cnst_TokenType::LITERAL_NUM),
            gram_tnode::gram_tnode_crt(&(token_defs._SMB_END),  cnst_TokenType::SYMBOL_END),
        },
        meta_type::VAR_INIT_INT);
    
    //int var(...) {}
    add_gram_rule(
        int_root->childrens, {
            gram_tnode::gram_tnode_crt_REF_RULE(NULL, cnst_TokenType::META_REF_RULE, meta_type::FUNC_DECL),
        },
        meta_type::FUNC_DEF_INT);

    // int t; 
    add_gram_rule(
        int_root->childrens, {
            gram_tnode::gram_tnode_crt(&(token_defs._SMB_END),  cnst_TokenType::SYMBOL_END),
        },
        meta_type::VAR_DEF_INT);

    //printf("DYUMP %i\n", int_root->childrens->size());
    //vec_dump(int_root->childrens);

    // VARS.FLOAT
    add_gram_rule(
        ls->roots, {
            gram_tnode::gram_tnode_crt(&(token_defs.TYPE_FLOAT),  cnst_TokenType::TYPE),
            gram_tnode::gram_tnode_crt(NULL,                    cnst_TokenType::VAR_NAME),
            gram_tnode::gram_tnode_crt(&(token_defs._VAR_SET),  cnst_TokenType::VAR_SET),
            gram_tnode::gram_tnode_crt(NULL,                    cnst_TokenType::LITERAL_NUM),
            gram_tnode::gram_tnode_crt(&(token_defs._SMB_END),  cnst_TokenType::SYMBOL_END),
        },
        meta_type::VAR_INIT_FLOAT);


    // VARS.DOUBLE
    add_gram_rule(
        ls->roots, {
            gram_tnode::gram_tnode_crt(&(token_defs.TYPE_DOUBLE),  cnst_TokenType::TYPE),
            gram_tnode::gram_tnode_crt(NULL,                    cnst_TokenType::VAR_NAME),
            gram_tnode::gram_tnode_crt(&(token_defs._VAR_SET),  cnst_TokenType::VAR_SET),
            gram_tnode::gram_tnode_crt(NULL,                    cnst_TokenType::LITERAL_NUM),
            gram_tnode::gram_tnode_crt(&(token_defs._SMB_END),  cnst_TokenType::SYMBOL_END),
        },
        meta_type::VAR_INIT_DOUBLE);

    // VARS.BYTE
    add_gram_rule(
        ls->roots, {
            gram_tnode::gram_tnode_crt(&(token_defs.TYPE_BYTE),  cnst_TokenType::TYPE),
            gram_tnode::gram_tnode_crt(NULL,                    cnst_TokenType::VAR_NAME),
            gram_tnode::gram_tnode_crt(&(token_defs._VAR_SET),  cnst_TokenType::VAR_SET),
            gram_tnode::gram_tnode_crt(NULL,                    cnst_TokenType::LITERAL_NUM),
            gram_tnode::gram_tnode_crt(&(token_defs._SMB_END),  cnst_TokenType::SYMBOL_END),
        },
        meta_type::VAR_INIT_BYTE);

    // VARS.LONG
    add_gram_rule(
        ls->roots, {
            gram_tnode::gram_tnode_crt(&(token_defs.TYPE_LONG),  cnst_TokenType::TYPE),
            gram_tnode::gram_tnode_crt(NULL,                    cnst_TokenType::VAR_NAME),
            gram_tnode::gram_tnode_crt(&(token_defs._VAR_SET),  cnst_TokenType::VAR_SET),
            gram_tnode::gram_tnode_crt(NULL,                    cnst_TokenType::LITERAL_NUM),
            gram_tnode::gram_tnode_crt(&(token_defs._SMB_END),  cnst_TokenType::SYMBOL_END),
        },
        meta_type::VAR_INIT_LONG);

    // ------------------------- VARS.CHAR
    // char t = 't';
    gram_tnode* char_root = gram_tnode::gram_tnode_crt(&(token_defs.TYPE_CHAR),  cnst_TokenType::TYPE);
    add_gram_rule(
        ls->roots, {
            char_root,
            gram_tnode::gram_tnode_crt(NULL,                    cnst_TokenType::VAR_NAME),
            gram_tnode::gram_tnode_crt(&(token_defs._VAR_SET),  cnst_TokenType::VAR_SET),
            gram_tnode::gram_tnode_crt(NULL,                    cnst_TokenType::LITERAL_CHAR),
            gram_tnode::gram_tnode_crt(&(token_defs._SMB_END),  cnst_TokenType::SYMBOL_END),
        },
        meta_type::VAR_INIT_CHAR);

    // char[] t;
    gram_tnode* char_var = gram_tnode::gram_tnode_crt(NULL, cnst_TokenType::VAR_NAME);
    gram_tnode* char_close_br_ref = gram_tnode::gram_tnode_crt(&(token_defs._BR_RSQUARE),  cnst_TokenType::SYMBOL_BRACKET);
    add_gram_rule(
        char_root->childrens, {
            gram_tnode::gram_tnode_crt(&(token_defs._BR_LSQUARE),  cnst_TokenType::SYMBOL_BRACKET),
            char_close_br_ref,
            char_var,
            gram_tnode::gram_tnode_crt(&(token_defs._SMB_END),  cnst_TokenType::SYMBOL_END),
        },
        meta_type::VAR_DEF_CHAR_ARR);
    
    // char[] t = "QWD";
    add_gram_rule(
        char_var->childrens, {
            gram_tnode::gram_tnode_crt(&(token_defs._VAR_SET),  cnst_TokenType::VAR_SET),
            gram_tnode::gram_tnode_crt(NULL,  cnst_TokenType::LITERAL_STRING),
            gram_tnode::gram_tnode_crt(&(token_defs._SMB_END),  cnst_TokenType::SYMBOL_END),
        },
        meta_type::VAR_INIT_CHAR_ARR);

    // char[] "abc";
    add_gram_rule(
        char_close_br_ref->childrens, {
            gram_tnode::gram_tnode_crt(NULL,  cnst_TokenType::LITERAL_STRING),
            gram_tnode::gram_tnode_crt(&(token_defs._SMB_END),  cnst_TokenType::SYMBOL_END),
        },
        meta_type::INIT_CHAR_ARR);

    // ------------------------------- VARS.STRING
    //string s = "123"
    gram_tnode* str_node = gram_tnode::gram_tnode_crt(&(token_defs._VAR_SET),  cnst_TokenType::VAR_SET);
    add_gram_rule(
        ls->roots, {
            gram_tnode::gram_tnode_crt(&(token_defs.TYPE_STRING),  cnst_TokenType::TYPE),
            gram_tnode::gram_tnode_crt(NULL,                    cnst_TokenType::VAR_NAME),
            str_node,
            gram_tnode::gram_tnode_crt(NULL,                    cnst_TokenType::LITERAL_STRING),
            gram_tnode::gram_tnode_crt(&(token_defs._SMB_END),  cnst_TokenType::SYMBOL_END),
        },
        meta_type::VAR_INIT_STRING);

    //string x = char[] "abc";
    add_gram_rule(
        str_node->childrens, {
            gram_tnode::gram_tnode_crt_REF_RULE(NULL,           cnst_TokenType::META_REF_RULE, meta_type::INIT_CHAR_ARR)
        },
        meta_type::VAR_INIT_STRING);

}

