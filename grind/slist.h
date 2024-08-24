#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vec.h"
#include <iostream>
#include <fstream>
#include <string>
#include <ctype.h>
#include <windows.h>


struct slink {
	slink* next;
	slink() { next = 0; }
	slink(slink* p) { next = p; }
};

class slist_base {
	slink* _start = NULL;
	slink* _end = NULL;
public:
	int insert(slink* a) {
		slink* old = _start;

		if (_start == NULL) {
			_end = a;
		}

		_start = a;
		a->next = old;

		return 1;
	}
	slink* get() {// удалить и возратить начало списка
		slink* next = _start->next;
		slink* old = _start;
		_start = next;

		return old;
	}
	slink* end() {
		return _end;
	}
};

// ITERATOR
class slist_iter_base {
	slink* cur; // current elem
	slist_base* ls;
public:
	slist_iter_base(slist_base* s) {
		ls = s;
		cur = ls->end();
	}
	slink* operator()() {

		if (cur != NULL) {
			slink* ret = cur;
			cur = cur->next;
			return ret;
		}
		else {
			return NULL;
		}
	}
};


template<class T>
struct Tlink : public slink {
	T info;
	Tlink(const T& a) : info(a) {};
};

template<class T>
class slist2 : private slist_base {
public:
	void ins(const T& a) {
		slist_base::insert(new Tlink<T>(a));
	}

	T get() {
		Tlink<T>* ptr = (Tlink<T>*) slist_base::get();
		T i = ptr->info;
		delete ptr;
		return i;
	}
};

template<class T>
class slist_iter : private slist_iter_base {
public:
	slist_iter(slist2<T>* s) : slist_iter_base((slist_base*)s) {};
	T* operator()() {
		return &((Tlink<T>*)slist_iter_base::operator() ())->info;
	}
};





template<class T>
class slist : private slist_base {
public:
	void insert(T* a) { slist_base::insert(a); }
	T* get() {
		return (T*)slist_base::get();
	}
};