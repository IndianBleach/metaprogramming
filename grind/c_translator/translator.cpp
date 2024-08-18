#include "translator.h"

#include <iostream>
#include <fstream>
#include <string>


void readFile(CharBuffer& in, const char* file) {
	std::ifstream str(file);

	std::string line;
	while (std::getline(str, line)) {
		in << line.c_str();
		in << "\n";
	}

}

#define TRANSLATE_BUFF_CAP 1024


void CTranslator::translate(CharBuffer& in, const char* filepath)
{
	// read from file to buffer
	CharBuffer readbuff(TRANSLATE_BUFF_CAP);
	readFile(readbuff, filepath);

	// create nodeList
	NodeList ls;
	//ls.makeNodes(,)
}

TranslateNode* NodeList::next()
{
	return nullptr;
}

void NodeList::back()
{
}

void NodeList::make(CharBuffer& buff)
{
	// NodeParser:parse()

	/*
		int t = 5;
		char b[1024];
		char b[] = "apple";
		char t = 'b';
		
		#define
		#include
		bar(int t) {}
		bar(5);
	*/

	// от меньшего к большему
	
	// word
	// literal
	// symbol
}

bool NodeParser::read_word(TranslateNode& in, int& at)
{
	return false;
}

bool NodeParser::read_literal(TranslateNode& in, int& at)
{
	return false;
}

bool NodeParser::read_symbol(TranslateNode& in, int& at)
{
	return false;
}

void NodeParser::parse(CharBuffer& buffer)
{
	char* begin = buffer.get();
	int buff_len = buffer.size();
	int cursor = 0;

	TranslateNode* current = _head;
	bool head_ex = false;

	auto addNode = [&](TranslateNode node) -> void {
		TranslateNode* crt = new TranslateNode(node);
		if (!head_ex) {
			_head = crt;
			current = _head; // fix
		}
		else {
			current->next = crt;
			current = current->next;
		}
		};

	TranslateNode node;
	while (cursor < buff_len) {
		
		if (read_word(node, cursor))
			addNode(node);

		if (read_literal(node, cursor))
			addNode(node);
		
		if (read_symbol(node, cursor))
			addNode(node);

	}
}

TranslateNode* NodeParser::head()
{
	return nullptr;
}
