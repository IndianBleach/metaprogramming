#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vec.h"
#include <iostream>
#include <fstream>
#include <string>
#include <ctype.h>
#include <windows.h>

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

#pragma region logging
#define _logErr(msg) \
	logError(msg);

int consoleColorRed = 4;
int consoleColorWrite = 15;

void logError(const char* msg)
{
	SetConsoleTextAttribute(hConsole, consoleColorRed);
	std::cerr << "error: " << msg << std::endl;
	SetConsoleTextAttribute(hConsole, consoleColorWrite);
}

#pragma endregion


#pragma region Streams: ifstream(r),ofstream(w)
// read file
void f1()
{
	std::string f1;
	std::getline(std::cin, f1, '*');
}

// write file
void f2()
{
	std::ofstream ofs("C:/ghrepos/low/grind/data/foo.txt", std::ios::app);
	if (ofs.is_open())
	{
		std::string str;

		for (int i = 0; i < 3; i++)
		{
			std::getline(std::cin, str, '*');

			int len = str.length();

			ofs.write(str.c_str(), len);
			str.clear();
		}
	}
	ofs.close();
}

// read from file. save to buffer. write again new data
void f3(const char* path)
{
	std::ifstream str(path);
	if (str.is_open())
	{
		std::string s;

		while (std::getline(str, s, '\n'))
		{
			//std::strcpy(&buff2[cur], s.c_str());
			//cur += s.length();
			//buff2[++cur] = ' ';

			s.clear();
		}
	}
	else
	{
		std::cerr << "can't open file to READ\n";
	}

	str.close();


	//std::ofstream of(path, std::ios::trunc);

	FILE* f = fopen(path, "rw");

	//if ()
	//{
	//	int i = --cur;
	//	while (i > 0)
	//	{
	//		fwrite(buff2, sizeof(char), cur, of);

	//		const char* t = &buff2[i];
	//		of.write(t, 1);

	//		i--;
	//	}
	//}
	//else
	//{
	//	std::cerr << "can't open file to write\n";
	//}

	//of.close();
}

// overload operator << on struct, to file
class User
{

public:
	const char* _name;
	int Age;
	User(const char* name)
	{
		_name = name;
	};
	User(const char* name, int _age)
	{
		Age = _age;
		_name = name;
	};

	friend std::ofstream& operator << (std::ofstream& in, User& user)
	{
		std::string st = "from << operator: ";
		st += user._name;

		in << st;

		return in;
	};

	friend std::ifstream& operator >> (std::ifstream& in, User& user)
	{
		std::string st = "from >> operator: ";
		st += user._name;

		in >> st;

		return in;
	};
};
void f4()
{
	User u1("Jony");
	printf("name: %s\n", u1._name);

	std::ofstream str("C:/ghrepos/low/grind/data/foo.txt");
	if (str.is_open())
	{

		str << u1;
	}

	str.close();
}

#pragma endregion

#pragma region bitwise operations
/*
	& - 'И'. 1 если оба соотв. разряда = 1
	| - 'ИЛИ'. 1 если один из соотв. разрядов = 1
	~ - 'ИНВЕРСИЯ'. противоположный разряд, 1=0, 0=1
	^ - 'ИСКЛЮЧАЮЩЕЕ ИЛИ'. 1 если ТОЛЬКО один из соотв. разрядов = 1 

	>> сдвиг вправо (деление)
	<< сдвиг влево (умножение)
*/
#pragma endregion

#pragma region typedefs, const_cast, reinterpret_cast, static_cast, dynamic_cast
typedef const char* CPCHAR;

class Bar
{
public:
	int Age = 5;
	Bar(int _age) : Age(_age) {};
	
};

/*
--const_cast-- 
Bar u1(10);
Bar u2(20);
const Bar* r1 = &u1;
Bar* r2 = const_cast<Bar*>(r1);
r1->Age = 11;
r2->Age = 15;*/

//double d1 = 15.3;
//double* pd = &d1;
//double* v1 = static_cast<double*>(pd);
//
//A a1("a1");
//B b1("b1", "a1");
//R r1("r1");
//
//B* b2 = static_cast<B*>(&a1);
//A* a2 = static_cast<A*>(&b1);
//R* r2 = static_cast<R*>(&b1);

#pragma endregion

#pragma region New, set_new_handler
// 
#pragma endregion

#pragma region string funcs

char* _strcat(const char* a, const char* b)
{
	const char* a2 = a;
	const char* b2 = b;

	int len = 0;
	while (*a++ || *b++)
		len++;

	char* line = new char[len + 1]();
	int i = 0;

	while (char p = *a2++)
		line[i++] = p;

	int i2 = i;
	i = 0;
	while (char p = *b2++)
		line[i + i2] = p;

	return line;
}

int _strlen(const char* a)
{
	int i = 0;
	while (*a++) i++;

	return i;
}

void _strcpy(const char* from, char* dest)
{
	while (*dest++ = *from++);
};

int _strcmp(const char* first, const char* second)
{
	while (*first++ | *second++)
	{
		if (first == second)
			continue;

		if (!*second)
			return 1;
		if (!*first)
			return -1;
	}

	return 0;
};

void _strreverse(char* line)
{
	int len = _strlen(line);
	for (int i = 0; i < len / 2; i++)
	{
		char t = line[len - 1 - i];
		line[len - 1 - i] = line[i];
		line[i] = t;
	}

	int t = 3;
}

int _atoi(const char* line) {
	int res = 0;
	while (char p = *line++)
	{
		if (p >= '0' && p <= '9')
		{
			int cur_number = p - '0';
			res *= pow(10, 1);
			res += cur_number;
		}
	}

	return res;
}

void _itoa(int num, char* buff) {
	int buff_len = strlen(buff) - 1;
	int i = 0;
	while (num > 0) {
		int d = num % 10;
		num /= 10;

		buff[buff_len--] = '0' + d;
	}
}

#pragma endregion 

#include <vector>
#include <cassert>


// calculator

// (calc.1) read from cin to buff, parsing, return ans.
// (calc.2) read from console, realtime parsing, realtime result/status
// (calc.3) calc.2 + variables

#include <stdarg.h>

#pragma region va_list

void change_user(User& user, int t ...) {
	user.Age = 123;

	va_list ls;
	va_start(ls, t);
	for (;;) {
		int p = va_arg(ls, int);

		if (p == 0)
			break;

		printf("arg=%i\n", p);
	}
	va_end(ls);
}


void temp_va(int t ...) {
	va_list ls;
	va_start(ls, t);
	for (;;) {
		char* ar = va_arg(ls, char*);
		if (ar == 0) break;
		else printf("arg=%s", ar);
	}
	va_end(ls);
	return;
}


#pragma endregion

#include <signal.h>

#pragma region function ptr typedefs 
typedef int (*SIG_TYP)(int);
typedef void (SIGN_ARG_TYP)(int);
SIG_TYP _signal(int, SIGN_ARG_TYP);

typedef void (*Func)(int);

void MoveLeft(int) {}
void MoveRight(int) {}

Func ops[] = {
	&MoveLeft, &MoveRight
};
#pragma endregion 


// 4.8 

/*
	все функции в хедер файлы
	инклуды и определения

	define заменить const, enum
	malloc, free - заменить на new, delete
	удалить ненужные приведения

*/


char trans_buff[1024];
int trans_cursor = 0;

char out_buff[1024];
int out_cursor = 0;



void _write_buff(char* buff, int& cursor, const char* msg) {
	
	// by buff+cursor
	while (char p = *msg++) {
		buff[cursor++] = p;
	}

	// by buff-stream

	// by strcpy
	//strcpy(trans_buff, line.c_str());
	//trans_cursor += line.size();
}

#include "c2cpp_translator.h"

int main(int argc, char* argv[])
{
	CppTranslator trans;
	

	translate("C:\\ghrepos\\low\\grind\\data\\code.c");

	//Calc calc{};

	//calc.set_context_buff("app=3.33;app*3");
	//calc.parse();

	//calc.get_name("_qwd");
	//int index = calc.look_variable();
	//calc.set_variable(index, 999.23);
	
	//double res = calc.result("123.451*2");

	return 0;
}