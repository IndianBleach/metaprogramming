#include "libstack.h"

#include "stdlib.h"
#include "assert.h"

struct cstack_type {
    size_t top;
    size_t max_size;
    value_t* values;
};

value_t copy_value(char* data, size_t len) {
    char* buff = (char*)malloc(sizeof(char)*len);
    for(int i = 0; i < len; i++) {
        buff[i] = data[i];
    }

    return make_value(buff, len);
}

value_t make_value(char* data, size_t len) {
    value_t t;
    t.data = data;
    t.len = len;
    return t;
};

void free_value(value_t *val) {
    if (val) {
        if (val->data) {
            free(val->data);
            val->data = NULL;
        }
    }
};

cstack_t* cstack_new() {
    return (cstack_t*)malloc(sizeof(cstack_t));
};

void cstack_delete(cstack_t* s) {
    free(s);
}

void cstack_ctor(cstack_t* cstack, size_t max_size) {
    cstack->top = 0;
    cstack->max_size = max_size;
    cstack->values = (value_t*)malloc(max_size * sizeof(value_t));
}

void cstack_dtor(cstack_t* s, deleter_t deleter) {
    cstack_clear(s, deleter);
    free(s->values);
}

bool_t cstack_push(cstack_t* s, value_t value) {
    if (s->top < s->max_size) {
    s->values[s->top++] = value;
    return TRUE;
    }
    return FALSE;
}

bool_t cstack_pop(cstack_t* cstack, value_t* value) {
    if (cstack->top > 0) {
        *value = cstack->values[--cstack->top];
        return TRUE;
    }
    return FALSE;
}

size_t cstack_size(const cstack_t* s) {
    return s->top;
}

void cstack_clear(cstack_t* s, deleter_t del) {
    value_t value;
    while(cstack_size(s) > 0) {
        bool_t popped = cstack_pop(s, &value);
        assert(popped);
        if (popped) {
            if (del) {
                del(&value);
            }
        }
    }
}