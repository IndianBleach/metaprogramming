
// charbuff
// nodebuff
// utils
// ctranslator
#include <cstring>
#include <iostream>
#include <memory>

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
		std::strcpy(&_buff[_cursor], line);
		_cursor += len;

		return this;
	}
};


enum TOKEN_TYPE {
	ASSIGN,
	FUNC,
	FUNC_PARAM_DEF,
	FUNC_PARAM_CALL,
	LITERAL,
	KEYWORD,
	VARNAME,
	BR_START = '{',
	BR_END = '}',
	EMPTY=0
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

class TranslateNode{
public:
	char* name;
	TranslateNode* next;
	LITERAL_DESC literal_desc;
	TOKEN_TYPE type;
	//TranslateNode(char* from, int len);
};

class NodeParser {
private:
	char* buff_start;
	int buff_capacity;
	TranslateNode* _head = nullptr;

	bool read_word(TranslateNode& in, int& at);
	bool read_literal(TranslateNode& in, int& at);
	bool read_symbol(TranslateNode& in, int& at);

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
