
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#pragma region operators

class complex
{
public:
	double val;
	complex(double v) { val = v; };

	friend complex operator- (complex, complex);

};

#pragma endregion




class shape {


public:
	int type{ 0 };

	shape(int x) : type{x}
	{}

	virtual int draw()  = 0;
	virtual int rot()  = 0;


};

class square : shape
{
public:

	square(int type) : shape{ type }
	{ }


	int draw()  override
	{
		return type;
	}

	int rot()   override
	{
		return 2;
	}
};

template<class T>
class vector
{
public:
	class range
	{
		int t = 3;
	};

private:
	int _size;
	T* arr;
public:
	vector(int size);
	~vector();
	T& operator[](int index);
};

template<class T> vector<T>::vector(int size)
{
	arr = (T*)malloc(sizeof(T) * size);
	_size = size;
}

template<class T> vector<T>::~vector()
{
	delete[]arr;
}

template<class T> T& vector<T>::operator[](int index)
{
	if (index >= _size) throw range();

	return arr[index];
}

int main()
{
	complex tr{ 45 };

	printf("tr: %f\n", tr.val);

	vector<complex> vec{ 5 };
	vec[3] = { 999 };

	try
	{
		complex t1 = vec[10];
	}
	catch (vector <complex> ::range)
	{
		printf("out of range\n");
	}

	printf("vec[3]: %f\n", vec[3].val);
	printf("res2: %f\n", vec[4].val);

	vec.~vector();

	return 0;
}

complex operator-(complex x, complex y)
{
	return x.val - y.val;
}
