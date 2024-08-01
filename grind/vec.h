#pragma once

template<class T>
class vec
{
	class expOutOfRange
	{};

private:
	T* arr;
	int capacity;

public:
	vec(int cap)
	{
		arr = new T[cap];
		capacity = cap;
	};

	T& operator[](int index);


	~vec()
	{
		capacity = 0;
		delete[] arr;
	}
};

template<class T> T& vec<T>::operator[](int index)
{
	if (index >= capacity) throw expOutOfRange{};
	return arr[index];
}

