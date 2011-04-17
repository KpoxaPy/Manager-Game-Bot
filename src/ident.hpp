#ifndef _IDENT_HPP_
#define _IDENT_HPP_

#include "tpl/tables.cpp"
#include "lex.hpp"

class Ident {
	friend class IdentTable;

	char * name;

	bool Var;
	bool Array;

	int id;
	int link;

	int defineLine;

public:
	Ident();
	Ident(const char *, int);
	Ident(const Lex &);
	Ident(const Ident &);
	~Ident();

	const Ident & operator=(const Ident &);

	void makeVar();
	void makeArray();
	void setLink(int);
	void setID(int);

	const char * getName() const;
	int getLink() const;
	int getID() const;
	bool isVar() const;
	bool isArray() const;

	void printraw() const;
	void print() const;
};

class IdentTable : public Table<Ident> {
	void printElem(const Ident &) const;

public:
	Ident * findIdentByName(const char *) const;
	int findLinkByName(const char *) const;
};

#endif
