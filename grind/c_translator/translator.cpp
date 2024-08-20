#include "translator.h"

#include <iostream>
#include <fstream>
#include <string>

#define _CRT_SECURE_NO_WARNINGS

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




void _initNode(TranslateNode& node,
	char* nameBuff, 
	int len,
	LITERAL_DESC desc, 
	TRANS_TOKEN_TYPE type) {
	node.literal_desc = desc;
	node.name = new char[len];
	node.name[len] = '\0';
	mstring::strcpy(node.name, nameBuff);
	node.type = type;
}

bool NodeParser::read_word(TranslateNode& in, int& at)
{
	char* buff = &buff_start[at];
	bool start_set = 0;
	int local_cursor = at;

	while (char p = *buff++) {
		if (p >= 'a' && p <= 'z') {
			start_set = true;
			local_cursor++;
		}
		else if (p >= '0' && p <= '9') {
			if (!start_set) {
				printf("translator: syntax error. variable name can't start with number\n");
			}
			local_cursor++;
		}
		else if (p == '_') {
			start_set = true;
			local_cursor++;
		}
		else {
			return false;
		}
	}

	int val_len = (local_cursor - at);
	_initNode(in, buff, val_len, EMPTY_LIT, TRANS_TOKEN_TYPE::VARNAME);

	return true;
}



bool NodeParser::read_literal(TranslateNode& in,  int& at)
{
	bool double_open = 0;
	bool single_open = 0;
	bool float_include = 0;
	bool lit_closed = 0;

	char* buff = &buff_start[at];
	int local_cursor = at;

	while (char p = *buff++) {

		if (p == '"') { // "apple"

			if (double_open) { // closed tag
				local_cursor++;
				lit_closed = true;
				break;
			}

			double_open = true;
			local_cursor++;

		}
		else if (p == '\'') { // 'a'
			if (single_open) { // closed tag
				local_cursor++;
				lit_closed = true;
				break;
			}

			single_open = true;
			local_cursor++;
		}
		else // 123.123.2
		{
			if (p >= '0' && p <= '9') {
				local_cursor++;
			}
			else if (p == '.') {

				if (float_include) {
					printf("translator: float/double number syntax error\n");
					return NULL;
				}

				float_include = true;
				local_cursor++;
			}
			else return false;
		}
	}

	// determine type
	LITERAL_DESC desc;
	TRANS_TOKEN_TYPE type = TRANS_TOKEN_TYPE::LITERAL;

	if (single_open && lit_closed) {
		desc = LITERAL_DESC::LIT_CHAR;
	}
	else if (double_open && lit_closed) {
		desc = LITERAL_DESC::LIT_STRING;
	}
	else {
		if (float_include) desc = LITERAL_DESC::LIT_FLOAT;
		else desc = LITERAL_DESC::LIT_INT;
	}

	// validations
	if (desc == LITERAL_DESC::LIT_CHAR) {
		if (at - local_cursor > 2) {
			printf("translator: literal: char too many symbols on define 'ab..'\n");
			return NULL;
		}

		if (at - local_cursor == 1) {
			printf("translator: literal: empty char ''\n");
			return NULL;
		}
	}

	// create
	int val_len = (local_cursor - at);
	_initNode(in, buff, val_len, desc, type);

	return true;
}

bool NodeParser::read_symbol(TranslateNode& in,  int& at)
{
	char* buff = &buff_start[at];
	int local_cursor = at;

	while (char p = *buff++) {
		if (p != ' ') {

			TRANS_TOKEN_TYPE type = TRANS_TOKEN_TYPE(p);

			_initNode(in, buff, 1, EMPTY_LIT, type);

			return true;
		}
	}

	return false;
}

void NodeParser::parse(CharBuffer& buffer)
{
	buff_start = buffer.get();
	buff_capacity = buffer.size();
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
	while (cursor < buff_capacity) {
		
		if (read_word(node, cursor))
			addNode(node);
		else if (read_literal(node, cursor))
			addNode(node);
		else if (read_symbol(node, cursor))
			addNode(node);

	}
}
