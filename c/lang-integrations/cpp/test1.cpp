#include <iostream>
#include <string>
#include <cstring>
#include "libstack.h"

template<typename T>
value_t CreateValue(const T& pv);

template<typename T>
T ExtractValue(const value_t& pv);

template<>
value_t CreateValue(const std::string & pval) {
    value_t value;
    value.len = pval.size() + 1;
    value.data = new char[value.len];
    strcpy(value.data, pval.c_str());
    return value;
}

template<>
std::string ExtractValue(const value_t& pv) {
    return std::string(pv.data, pv.len);
} 

template<typename T>
class Stack {

    private:
    cstack_t* m_stack;

    public:
        Stack(int pmax) {
            m_stack = cstack_new();
            cstack_ctor(m_stack, pmax);
        }

        ~Stack() {
            cstack_dtor(m_stack, free_value);
            cstack_delete(m_stack);
        }

        size_t Size() {
            return cstack_size(m_stack);
        }

        void Push(const T& pv) {
            value_t val = CreateValue(pv);
            if (!cstack_push(m_stack, val)) {
                throw "Stack if full!";
            }
        }

        const T Pop() {
            value_t value;
            if (!cstack_pop(m_stack, &value)) {
                throw "Stack is empty";
            }

            return ExtractValue<T>(value);
        }

        void Clear() {
            cstack_clear(m_stack, free_value);
        }
};

int main() {
    Stack<std::string> str_st(100);
    str_st.Push("Hello");
    str_st.Push("World");
    
    while(str_st.Size() > 0) {
        printf("Poppped: %s\n", str_st.Pop().c_str());
    }

    printf("Size1=%i\n", str_st.Size());
    str_st.Push("a1");
    printf("Size2=%i\n", str_st.Size());
    str_st.Clear();
    printf("Size3=%i\n", str_st.Size());
}
