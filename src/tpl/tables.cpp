#ifndef _TABLES_CPP_
#define _TABLES_CPP_

#include <stdio.h>
#include "../stuff/exceptions.hpp"
#include "tables.hpp"

template <class T>
Table<T>::Sheet::Sheet()
{
	data = new T*[sheetSize];
	next = 0;

	for (int i = 0; i < sheetSize; i++)
		data[i] = 0;
}

template <class T>
Table<T>::Sheet::~Sheet()
{
	for (int i = 0; i < sheetSize; i++)
		if (data[i] != 0)
			delete data[i];

	delete [] data;

	if (next != 0)
		delete next;
}

template <class T>
Table<T>::Table()
{
	table = new Sheet*[tableSize];
	count = 0;

	for (int i = 0; i < tableSize; i++)
		table[i] = 0;
}

template <class T>
Table<T>::~Table()
{
	for (int i = 0; i < tableSize; i++)
		if (table[i] != 0)
			delete table[i];

	delete [] table;
}

template <class T>
T ** Table<T>::getLink(int n) const
{
	int layer = n / (sheetSize*tableSize);
	int sheet = (n  % (sheetSize*tableSize)) / sheetSize;
	int element = n % sheetSize;
	Sheet * sh;

	if (table[sheet] == 0)
		table[sheet] = new Sheet;
	sh = table[sheet];

	for (int i = 0; i < layer; i++)
	{
		if (sh->next == 0)
			sh->next = new Sheet;
		sh = sh->next;
	}

	//if (sh->data[element] == 0)
		//sh->data[element];

	return &(sh->data[element]);
}

template <class T>
T & Table<T>::operator[](int n)
{
	T ** p = getLink(n);
	if (*p == 0)
		*p = new T();
	return **p;
}

template <class T>
int Table<T>::put(T * tp)
{
	int n = count++;
	*getLink(n) = tp;
	return n;
}

template <class T>
void Table<T>::deleteLast()
{
	int link = count - 1;
	if (link >= 0)
	{
		delete *getLink(link);
		*getLink(link) = 0;
		count = link;
	}
}

template <class T>
int Table<T>::getCount()
{
	return count;
}

template <class T>
void Table<T>::printElem(const T & t) const
{
}

template <class T>
void Table<T>::Sheet::print(const Table<T> & t) const
{
	for (int i = 0; i < sheetSize; i++)
		if (data[i] == 0)
			printf("\t\t%02d: null\n", i);
		else
		{
			printf("\t\t%02d: ", i);
			t.printElem(*data[i]);
			printf("\n");
		}
}

template <class T>
void Table<T>::print() const
{
	Sheet * sh;
	int l;

	printf("Count = %d\n", count);
	for (int i = 0; i < tableSize; i++)
	{
		sh = table[i];
		l = 0;
		if (sh == 0)
			printf("Sheet%02d: null\n", i);
		else
		{
			printf("Sheet%02d:\n", i);
			while (sh != 0)
			{
				printf("\tLayer%02d:\n", l);
				sh->print(*this);
				sh = sh->next;
				l++;
			}
		}
	}
}

template <class T>
void Table<T>::printSmall() const
{
	Sheet * sh;
	int l, lp;
	bool layersIsOver = false;

	printf("Count: %d {", count);
	l = 0;
	while (!layersIsOver)
	{
		layersIsOver = true;
		for (int i = 0; i < tableSize; i++)
		{
			sh = table[i];
			lp = 0;
			while (sh != 0 && lp < l)
			{
				sh = sh->next;
				lp++;
			}
			if (lp == l)
				layersIsOver = layersIsOver && false;
			else
				continue;
			if (sh != 0)
				for (int j = 0; j < sheetSize; j++)
					if (sh->data[j] != 0)
					{
						printf("[%d]: ", l*256 + i*16 + j);
						printElem(*sh->data[j]);
						printf(" ");
					}
		}
		l++;
	}
	printf("}");
}

template <class T>
void Table<T>::printPlain() const
{
	Sheet * sh;
	int l, lp;
	bool layersIsOver = false;

	printf("Count = %d\n", count);
	l = 0;
	while (!layersIsOver)
	{
		layersIsOver = true;
		for (int i = 0; i < tableSize; i++)
		{
			sh = table[i];
			lp = 0;
			while (sh != 0 && lp < l)
			{
				sh = sh->next;
				lp++;
			}
			if (lp == l)
				layersIsOver = layersIsOver && false;
			else
				continue;
			if (sh != 0)
				for (int j = 0; j < sheetSize; j++)
					if (sh->data[j] != 0)
					{
						printf("[%d]: ", l*256 + i*16 + j);
						printElem(*sh->data[j]);
						printf("\n");
					}
		}
		l++;
	}
}

#endif
