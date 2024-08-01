#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vec.h"
#include <iostream>




const int large = 1024;


// find
struct pair
{
	char* name;
	int val;
};

static pair vec[large+1];

pair* find(const char* name)
{
	int i = 0;

	for (; vec[i].name; i++)
	{
		if (strcmp(vec[i].name, name) == 0)
			return &vec[i];

		if (i == large) return &vec[large - 1];
	}

	return &vec[i];
}

int& val(const char* p)
{
	pair* res = find(p);
	if (res->name == 0) // not found
	{
		res->name = new char[strlen(p) + 1];
		strcpy(res->name, p);
		res->val = 0;
	}
	else
	{
		return res->val;
	}
}

const int max = 256;

#include <fstream>
#include <string>

char buff2[max];
int cur = 0;

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
			std::strcpy(&buff2[cur], s.c_str());
			cur += s.length();
			buff2[++cur] = ' ';

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

	User(const char* name)
	{
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






int main()
{



	char buff[max];
	int i = 0;
	while (i < max && std::cin >> buff[i++]);
	//while(std::cin >> buff) buff[i++];

	std::string foo{};
	while (std::cin >> foo);


	char buf[max];
	while (std::cin >> buf) val(buf)++;

	for (int i = 0; vec[i].name; i++)
	{
		printf("%p: %i", vec[i].name, vec[i].val);
	}


	return 0;
}