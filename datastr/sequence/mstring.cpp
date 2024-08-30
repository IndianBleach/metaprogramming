
#include "mstring.h"

mstring::mstring() {
	m_count_refs = 1;
	m_current_length = 0;
}

mstring::mstring(const char* str)
{
	//m_ptr = new 
}

mstring::mstring(const mstring& ptr)
{
}

mstring::mstring(const char* charptr, size_t count)
{
}

mstring::mstring(const mstring& ptr, size_t count)
{
}

mstring::mstring(std::initializer_list<char> list)
{
}


#include <string>

int main() {
	std::string s1("apple");
	std::string s2("apple");
	std::string s3(s2.c_str());
	
	auto r1 = s1.c_str();
	auto r2 = s2.c_str();
	auto r3 = s3.c_str();

	int t = 3;
}