#include "grammatic.h"

class LexToken {
    
    public:
        std::string value;
        cnst_TokenType type;
        
        LexToken(cnst_TokenType cnst_type, std::string val);
};

extern token_alphabet token_defs;

class Lexer {
    private:
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

        static Lexer* crt();
        void make_tokens(std::string* src, std::vector<LexToken>* tokens);
};