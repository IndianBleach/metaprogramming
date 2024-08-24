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

#define _CRT_SECURE_NO_WARNINGS

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

#include "malloc.h"

void heap_used() {
	_HEAPINFO info = { 0 };

	int used = 0;

	while (_heapwalk(&info) == _HEAPOK) {
		if (info._useflag == _USEDENTRY) {
			used += info._size;
		}
	}

	printf("heap.used: %i\n", used);
}

#pragma region Classed, fields, constant - mutable funcs, friends

class mvec {
public:
	int x, y, z;
public:
	mvec(int x, int y, int z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
};

class foo {
	int _x = 0;
	const char* name;
public:

	foo(const char* _name) {
		//name = new char[strlen(_name)];
		//strcpy(name, _name);
		name = _name;
	}

	~foo() {
		printf("foo.dctor\n");
		delete[] name;
	}

	int read() const;
	int sq() const;
	int wr(int b);
};

int foo::read() const {
	return _x;
}

int foo::sq() const {
	//_x++; error
	((foo*)this)->_x++;
	return _x;
}

int foo::wr(int b) {
	_x = 5;
	return _x;
}

void f(foo& mut, const foo& cons) {
	mut.read(); // ok
	mut.wr(5); // ok
	cons.read(); // ok
	//cons.wr(); // error
}

#pragma endregion 

#pragma region Unions

union number {
	float fval;
	int ival;

	number(int _ival) { ival = _ival; };
	number(float val) { fval = val; };
};

#pragma endregion

#pragma region class inherites, virtual fields

class shape {
	int m;
public:
	shape(int _m) { m = _m; };
	virtual void sq() = 0;
	virtual void rot() = 0;
};

class circle : public shape {
private:
	int _rad;
public:
	circle(int rad, int m) : shape(m), _rad(rad) {};
	void sq() override {
		printf("circle.sq()\n");
	}

	void rot() override {
		printf("circle.rot\n");
	}
};

class square : public shape {
private:
	int _w;
	int _h;
public:
	square(int w, int h, int m) : shape(m), _w(w), _h(h) {};
	void sq() override {
		printf("square.sq()\n");
	}

	void rot() override {
		printf("square.rot\n");
	}
};

class window {
private:
	int p = 10;
protected:
	int t = 5;
public:
	virtual void draw() = 0;

	virtual ~window() {};
};

class window_w_input : public  window {
public:
	void draw() {
		printf("window_w_input.draw()\n");
	}

	~window_w_input() {
		printf("window_w_input.dctor\n");
	}
};

class window_w_menu : public  window {
public:
	void draw() {
		int a = t;
		printf("window_w_menu.draw()\n");
	}

	~window_w_menu() {
		printf("window_w_menu.dctor\n");
	}
};

class window_menu_input 
	: public  window_w_input,
	public  window_w_menu
{
public:
	char t[200];
	~window_menu_input() {
		printf("window_menu_input.dctor\n");
	}
};

#pragma endregion

#pragma region ovveriding new

class xo {
	int _x = 0;
public:
	xo(int t) { _x = t; };
	void* operator new (size_t, void* p, int sz) { return p; };
};

char buffer[sizeof(xo)];

xo* crt(int t) {
	xo* ptr = new(buffer, 10) xo(t);
	return ptr;
}

#pragma endregion

#pragma region diff overload operators

class com {
	
	int x;
public:
	
	com(int _x) {
		x = _x;
	}

	com() { x = -1; };

	friend com operator+(com a, com b) {
		com r(a.x + b.x);
		return r;
	}

	com& operator=(const com& ptr) {
		x = ptr.x * 2;
		return *this;
	}

	com* operator&() {
		printf("op:&\n");
		return this;
	}

	com operator++(int) {
		com temp(*this);
		operator++();
		return temp;
	}

	com& operator++() {
		x += 1;
		return *this;
	}



	//friend com operator++(com& a) {
	//	a.x += 1;
	//	return a;
	//}
};

#pragma endregion

#pragma region string

class mstr {
	struct strep {
		char* s;
		int n;
		strep() { n = 1; };
	};

private:
	strep* ptr;

public:
	mstr(const char*);
	mstr(const mstr&);
	mstr();
	mstr& operator=(const char*);
	mstr& operator=(const mstr&);
	~mstr();
	char& operator[](int index);
	const char* get() {
		return ptr->s;
	}
	friend std::ostream& operator<<(std::ostream&, const mstr&);
	friend std::istream& operator>>(std::istream&, mstr&);
	friend int operator ==(const mstr& x, const char* s) {
		return std::strcmp(x.ptr->s, s) == 0;
	}
	friend int operator ==(const mstr& x, const mstr& b) {
		return std::strcmp(x.ptr->s, b.ptr->s) == 0;
	}
	friend int operator !=(const mstr& x, const char* s) {
		return std::strcmp(x.ptr->s, s) != 0;
	}
	friend int operator !=(const mstr& x, const mstr b) {
		return std::strcmp(x.ptr->s, b.ptr->s) != 0;
	}
	const char* operator()(int, int); // substring
};

class mstr_iterator {
private:
	mstr* _base;
	int _capacity;
public:
	mstr_iterator(mstr* base) {
		_base = base;
		_capacity = 0;
		while (base++) {
			_capacity++;
		}
	}
	mstr* begin() {
		return _base;
	}
	mstr* end() {
		return _base + _capacity;
	}



	// ++
	// --
	// []
	// ==
	// !=

};

// sub()
// 

const char* mstr::operator()(int start, int end) {
	int len = end - start + 1;
	char* s = new char[len]();

	for (int i = 0; i < len; i++) {
		char p = ptr->s[start + i];
		s[i] = p;
	}

	s[len - 1] = '\0';
	return const_cast<const char*>(s);
}

mstr::mstr(const mstr& s) {
	s.ptr->n++;
	ptr = s.ptr;
}

mstr::mstr(const char* b) {
	ptr = new strep;
	int len = std::strlen(b);
	ptr->s = new char[len + 1];
	strcpy(ptr->s, b);
}

mstr::~mstr() {
	if (--ptr->n == 0) {
		delete[] ptr->s;
		delete ptr;
	}
}

mstr::mstr() {
	ptr = new strep;
	ptr->s = 0;
};

// s = "gold"
mstr& mstr::operator=(const char* s) {
	if (ptr->n > 1) {
		ptr->n--;
		ptr = new strep;
	}
	else delete ptr->s;

	ptr->s = new char[strlen(s) + 1];
	strcpy(ptr->s, s);
	return *this;
}

mstr& mstr::operator=(const mstr& s) {
	s.ptr->n++;
	if (--ptr->n == 0) { // delete old string
		delete[] ptr->s;
		delete ptr;
	}

	ptr = s.ptr;
	return *this;
}

using namespace std;
ostream& operator << (ostream& s, const mstr& str) {
	s << str.ptr->s << "[" << str.ptr->n << "]" << "\n";
	return s;
}

istream& operator >> (istream& s, mstr& x) {
	char buff[256];
	s >> buff;

	x = buff;
	return s;
}

#pragma endregion


#include "./slist.h"

template<class T>
class mstack {
	T* v;
	T* p;
	int size;
public:
	mstack(int s) { v = p = new T[size = s]; }
	~mstack() { delete[] v; }
	void push(T a);
	T pop();
	int sz() const { return p - v; }
};

template<class T> void mstack<T>::push(T a) {
	*p++ = a;
}

template<class T> T mstack<T>::pop() {
	return *--p;
}

class slink_item : public slink
{
};


int main(int argc, char* argv[])
{
	mstr* arr = new mstr[10];
	for (int i = 0; i < 9; i++) {
		std::string st = "item=" + i;
		arr[i] = st.c_str();
	}

	mstr_iterator iter(arr);
	for (mstr& i : iter) {
		cout << i.get();
	}

	return 0; 
}