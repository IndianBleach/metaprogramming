
// charbuff
// nodebuff
// utils
// ctranslator
#include <cstring>
#include <iostream>
#include <memory>

#define _CRT_SECURE_NO_WARNINGS

class CharBuffer {
private:
	char* _buff;
	int _cursor;
	int _capacity;
public:
	CharBuffer(int size) {
		_buff = new char[size]();
		_cursor = 0;
		_capacity = size;
	}

	~CharBuffer() {
		delete[] _buff;
	}

	inline char* get() { return _buff; }
	inline int size() { return _capacity; }

	CharBuffer* operator << (const char* line) {
		int len = std::strlen(line);
		//mstring::strcpy(&_buff[_cursor], line);
		_cursor += len;

		return this;
	}
};


enum TRANS_TOKEN_TYPE {
	ASSIGN = '=',
	DOT = '.',
	BR_FIG_START = '{',
	BR_FIG_END = '}',
	BR_CIRCLE_START = '(',
	BR_CIRCLE_END = ')',
	BR_QUAD_START = '[',
	BR_QUAD_END = ']',
	HASHTAG = '#',

	FUNC,
	FUNC_PARAM_DEF,
	FUNC_PARAM_CALL,
	LITERAL,
	KEYWORD,
	VARNAME,

	EMPTY=0
};

enum LITERAL_DESC
{
	LIT_CHAR,
	LIT_STRING,

	LIT_INT,
	LIT_FLOAT,

	ARRAY = 1,
	EMPTY_LIT = 0,
};

class TranslateNode{
public:
	char* name;
	TranslateNode* next;
	LITERAL_DESC literal_desc;
	TRANS_TOKEN_TYPE type;
	//TranslateNode(char* from, int len);
};

class NodeParser {
private:
	char* buff_start;
	int buff_capacity;
	TranslateNode* _head = nullptr;

	bool read_word(TranslateNode& in, int& at);
	bool read_literal(TranslateNode& in, int& at);
	bool read_symbol(TranslateNode& in,  int& at);

public:
	void parse(CharBuffer& buffer);
	inline TranslateNode* head() { return _head; }
};

class NodeList {
private:
	TranslateNode* _head = nullptr;
public:
	TranslateNode* next();
	void back();
	void make(CharBuffer& buff);
};


class CTranslator {
public:
	void translate(CharBuffer& in, const char* filepath);
};
