#ifndef _TABLES_HPP_
#define _TABLES_HPP_

#include "tpl/tables.cpp"

class IntTable : public Table<int> {
	void printElem(const int &) const;
};

class IntArrayTable : public Table<IntTable> {
	void printElem(const IntTable &) const;
};

#endif
