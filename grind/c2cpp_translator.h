#pragma once
#include <vector>


class CharBuffer {
private:
	char* _buff = nullptr;
	size_t _buffSize = 0;

public:
	inline size_t size() {
		return _buffSize;
	}

	int cursor = 0;

	inline char* get() {
		return _buff;
	}

	CharBuffer(size_t size) {
		_buffSize = size;
		_buff = new char[size]();
	}

	~CharBuffer() {
		delete[] _buff;
	}

	void operator <<  (const char* line) {
		while (char p = *line++) {
			_buff[cursor++] = p;
		}
	}
};

enum TRANS_TOKEN_TYPE {
	ASSIGN,
	FUNC,
	FUNC_PARAM_DEF,
	FUNC_PARAM_CALL,
	LITERAL,
	KEYWORD,
	VARNAME,
	BR_START = '{',
	BR_END = '}',
};

enum LITERAL_DESC
{
	LIT_CHAR,
	LIT_STRING,

	LIT_INT,
	LIT_FLOAT,

	ARRAY = 1,
	EMPTY = 0,
};

class TransNode {
public:
	char* name;
	TRANS_TOKEN_TYPE type;
	LITERAL_DESC literal_desc;
	TransNode* next;
};


// read_word apple
// read_lit 'a'
// read_lit "apple"
// read_lit 132.123

class NodeBuffer : public CharBuffer {
private:
	char* start_buff = nullptr;
	int read_cursor = 0;

	TransNode* head;

	TransNode* read_word() {
		int cur = 0;

		bool start_flag = 0;
		int substr_index = read_cursor;
		char* ptr = start_buff;

		while (char p = *ptr++) {
			if (p >= '0' && p <= '9') {
				if (!start_flag) {
					printf("translator: name/keyword can't starts with number\n");
					return NULL;
				}
				substr_index++;
			}
			else if (p >= 'a' && p <= 'z') {
				substr_index++;
				start_flag = true;
			}
			else if (p == '_') {
				start_flag = true;
				substr_index++;
			}
			else break;
		}

		TransNode* node = new TransNode();
		node->name = new char[substr_index - read_cursor]();
		std::strcpy(node->name, &start_buff[read_cursor]);
		node->type = TRANS_TOKEN_TYPE::VARNAME;

		read_cursor = substr_index;

		return node;
	}

	TransNode* read_lit() {

		bool double_open = 0;
		bool single_open = 0;
		bool float_include = 0;
		bool lit_closed = 0;

		int substr_index = read_cursor;
		char* ptr = start_buff;

		while (char p = *ptr++) {
			
			if (p == '"') { // "apple"
				
				if (double_open) { // closed tag
					substr_index++;
					lit_closed = true;
					break;
				}
			
				double_open = true;
				substr_index++;

			}
			else if (p == '\'') { // 'a'
				if (single_open) { // closed tag
					substr_index++;
					lit_closed = true;
					break;
				}

				single_open = true;
				substr_index++;
			}
			else // 123.123.2
			{
				if (p >= '0' && p <= '9') {
					substr_index++;
				}
				else if (p == '.') {

					if (float_include) {
						printf("translator: float/double number syntax error\n");
						return NULL;
					}

					float_include = true;
					substr_index++;
				}
			}
		}

		// determine type
		LITERAL_DESC desc;
		TRANS_TOKEN_TYPE type= TRANS_TOKEN_TYPE::LITERAL;

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
			if (read_cursor - substr_index > 2) {
				printf("translator: literal: char too many symbols on define 'ab..'\n");
				return NULL;
			}

			if (read_cursor - substr_index == 1) {
				printf("translator: literal: empty char ''\n");
				return NULL;
			}
		}

		// create
		TransNode* node = new TransNode();
		node->name = new char[substr_index - read_cursor]();
		std::strcpy(node->name, &start_buff[read_cursor]);
		node->type = type;
		node->literal_desc = desc;

		read_cursor = substr_index;

		return node;
	}

public:
	TransNode* next();
	void back();

	NodeBuffer(CharBuffer& buff)
		: CharBuffer(buff)
	{
		start_buff = get();
		read_cursor = 0;
	}

	NodeBuffer(size_t size) 
		: CharBuffer(size)
	{
		start_buff = get();
		read_cursor = 0;
	}
	
};

// nodeBuffer
// next()
// back();

#define TRANS_BUFF_SIZE 1024

class CppTranslator
{

private:
	NodeBuffer _nodeBuff;

	//CharBuffer _srcBuff = CharBuffer(TRANS_BUFF_SIZE);
	CharBuffer _outBuff = CharBuffer(TRANS_BUFF_SIZE);

public:
	void translate(const char* fileSourcePath);
	char* getOutput();
};

