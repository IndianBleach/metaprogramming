
#include "./m.h"

#include <stdio.h>

#include "./oop/user.h"
#include "./oop/student.h"

int main() {
    
    struct user* u = user_new("Joghn");
    struct student* s = student_new("John", 15);

    printf("name: %s\n", user_get_name(u));
    printf("name: %s\n", user_get_name((struct user*)s));

    return 1;
}

/*
    OOP
    objects
    attributes - object state
    immutable, muttalbe objects,
    objects relationship
    domen

    инкапсуляция - капсула обьекта с его набором атрибутов - его состоянием
    явная - с помощью методов, описанным в классе
        foo() {this.name = 'John'}
    неявная - с помощью функций вне класса, которые получают ссылку на обьект
        foo(obj*) {obj->name = 'John'}
    сокрытие свойств с помощью заголовка

    Object relations: COMPOSIOTION / AGREGATION
    Композиция - один обьект содержит в себе другой обьект
        car {
            engine* eng
        }
    Агрегация - ВРЕМЕННОЕ отношение обьекта с другим обьектом
        обьекты независимы друг от друга
        car->engine = eng1
        ...
        car->engine = NULL

    8. НАСЛЕДОВАНИЕ, ПОЛИМОРФИЗМ

*/