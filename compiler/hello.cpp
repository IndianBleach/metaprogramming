#include <iostream>
#include <fstream>
#include <string>


void file__readSource(const char* path, std::string *str) {
    if (str != NULL) {
        std::ifstream rd(path);
        
        if (rd.is_open()) {
            while(std::getline(rd, *str, '\0')) 
            {}
        }

        rd.close();
    }
}


int main() {

    std::string str;

    char* pt = (char*)"src.nem";

    file__readSource(pt, &str);

    std::cout << str << std::endl;

    return 0;
}