#include "grammatic.h"
#include "lexer.h"

void vec_dump(std::vector<gram_tnode*>* vec);

void vec_dump_ref(std::vector<LexToken*>* vec);

void vec_dump_node(gram_tnode* vec, int deep);

void dump_rules(grammatical_rule_list* ls);

void vec_dump(std::vector<LexToken>* vec);