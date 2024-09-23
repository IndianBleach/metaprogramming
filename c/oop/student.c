
#include "./user_p.h"
#include "./user.h"
#include "./student.h"

typedef struct student {
    user usr;
    int number;
} student;

int student_get_number(student* student) {
    return student->number;
}
student* student_new(const char* name, int number) {
    student* s = (student*)malloc(sizeof(student));
    s->number = number;
    s->usr.name = name;
}