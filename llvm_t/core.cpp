#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdarg.h>
#include <vector>

#include "grammatic.h"
#include "instr_builder.h"
#include "lexer.h"

token_alphabet token_defs;
nodeRef_utils* global_refs = nodeRef_utils::crt();
std::vector<gram_tnode*> temp_vec;

int main() {

    // InstrBuilder.build_instr_tree("src.nem");

    Lexer* lex = Lexer::crt();
    std::vector<LexToken>* vec = lex->tokens_from_file("src.nem");

    grammatical_rule_list* ls = grammatical_rule_list::crt_grammatic();

    instr_builder::build_tree(ls, vec);

    return 0;
}