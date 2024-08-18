#include "c2cpp_translator.h"
#include <fstream>
#include <string>


void _readfile(CharBuffer& in, const char* filepath) {

	std::ifstream st;
	st.open(filepath, std::ios::in);

	if (st.is_open()) {

		std::string line;
		while (std::getline(st, line, '\n'))
		{
			line += '\n';
			in << line.c_str();
		}
	}
}




void CppTranslator::translate(const char* sourceFilePath) {
	
	_readfile(_nodeBuff, sourceFilePath);
	
	int t = 3;
	// read node List

	// parse tokens (nodes) to tokenList
	/*
	* int t= 5;
	* char t[] buff = "qwd";
	* char t[23];
	*
	*   read_expr
	*	read_term 
		read_word()
		read_func
		read_literal :
		read_key :  symbols like .,/ {}
	*/

	// create modified AST

	// create file, write 


}
