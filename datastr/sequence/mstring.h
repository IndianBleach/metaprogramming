#include <initializer_list> 

class mstring {
private:
	size_t m_count_refs;
	size_t m_current_length;
	char* m_ptr;

	// operators
	// methods
	// ctores
	// dctors
public:
	mstring();
	mstring(const char* str);
	mstring(const mstring& ptr);
	mstring(const char* charptr, size_t count);
	mstring(const mstring& ptr, size_t count);
	mstring(std::initializer_list<char> list);
};