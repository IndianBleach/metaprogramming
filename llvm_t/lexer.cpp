#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdarg.h>
#include <vector>

#include "grammatic.h"

#include "lexer.h"

/*
    TODO_1:
        Utils
            - dump

        Lexer
            - makeTokensFromFile
            - crt

        --------------------- ast_builder.h

        instr_node
        instr_tree


        InstrBuilder
            - build_from_file()

        --------------------- expressions.h
        добавить классы и наследование для построения дерева

*/

#ifndef _LEXER_H_
#define _LEXER_H_

LexToken::LexToken(cnst_TokenType cnst_type, std::string val) {
    //printf("ctor: char=%s \n", val);
    value = val;
    type = cnst_type;
};


Lexer* Lexer::crt() {
    Lexer* lex = (Lexer*)malloc(sizeof(Lexer));
    return lex;
};

void Lexer::make_tokens(std::string* src, std::vector<LexToken>* tokens) {

    size_t stlen = src->length();
    std::cout << stlen << std::endl;

    std::ostringstream bld;
    size_t cur = 0;
    size_t temp = 0;

    // TODO_3: fix char substr,. control memory


    while (cur < stlen && src->at(cur) != '\0') {

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
        else if (cur + 1 < stlen) {

            // 2 symbol tokens
            cnst_TokenType token_type = parse_two_smb_tk(src->at(cur), src->at(cur + 1));

            if (token_type != UNDF) {
                tokens->push_back(
                    LexToken(
                        token_type,
                        src->substr(cur, 2)));

                cur += 2;
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

                cur += 1;
                temp = cur;
                continue;
            }
        }

        bool is_num_lit = false;
        bool is_str_lit = false;
        bool is_char_lit = false;
        bool is_varname = false;

        while (temp < stlen && src->at(temp) != ' ') {

            // LITERALS.NUMBER
            if (this->is_num_literal(src->at(temp)) && bld.tellp() == 0) {

                is_num_lit = true;

                while (temp < stlen) {
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
            if (curc == '"' && bld.tellp() == 0) {
                is_str_lit = true;
                temp++;

                while (temp < stlen) {
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
                if ((temp + 2) < stlen &&
                    src->at(temp + 2) == '\'') {
                    bld.put(src->at(temp + 1));
                    is_char_lit = true;

                    cur = temp + 2;
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
            else if (ttype == UNDF) {
                // error
            }
        }

        cur++;
        temp = cur;
        bld.str("");
    }
};

std::vector<LexToken>* Lexer::tokens_from_file(const char* path) {
    std::string* src = new std::string();
    std::vector<LexToken>* vec = new std::vector<LexToken>();

    if (src != NULL) {
        std::ifstream rd(path);

        if (rd.is_open()) {
            while (std::getline(rd, *src, '\0'))
            {
            }
        }

        rd.close();
    }

    make_tokens(src, vec);

    return vec;
}


#endif