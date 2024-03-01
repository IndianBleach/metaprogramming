#include <vector>
#include "lexer.h"


class instr_builder {
    public:
        static void build_tree(grammatical_rule_list* gram_list, std::vector<LexToken>* tokens);
};