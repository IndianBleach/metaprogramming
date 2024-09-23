#include "./user.h"
#include "./user_p.h"

const char* user_get_name(user*u) {
    return u->name;
}
user* user_new(const char* name) {
    user* u = (user*)malloc(sizeof(user));
    u->name = name;
}