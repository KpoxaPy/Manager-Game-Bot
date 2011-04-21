#ifndef _LABEL_HPP_
#define _LABEL_HPP_

#include "../tpl/tables.cpp"
#include "lex.hpp"

class RPNLabel;

class Label {
	friend class LabelTable;

	int defineLine;
	bool used;
	int id;
	int link;
	char * name;

public:
	Label();
	Label(const char *, int, int);
	Label(const Lex &, int);
	Label(const Label &);
	~Label();

	const Label & operator=(const Label &);

	void setID(int);
	void setLink(int);
	void setAsUsed();

	const char * getName() const;
	int getID() const;
	int getLink() const;
	int getLine() const;
	bool isUsed() const;

	void printraw() const;
	void print() const;
};

class Goto {
	friend class GotoTable;

	int defineLine;
	int id;
	char * name;

	bool corrected;

	RPNLabel * label;

public:
	Goto();
	Goto(const char *, int, RPNLabel &);
	Goto(const Lex &, RPNLabel &);
	Goto(const Goto &);
	~Goto();

	const Goto & operator=(const Goto &);

	void setID(int);
	void setAsCorrected();
	void setLabel(RPNLabel *);

	void correctOnLabel(Label &);

	const char * getName() const;
	int getID() const;
	int getLine() const;
	bool isCorrected() const;

	void printraw() const;
	void print() const;
};

class LabelTable : public Table<Label> {
	void printElem(const Label &) const;

public:
	Label * findLabelByName(const char *) const;
	int findLinkByName(const char *) const;
};

class GotoTable : public Table<Goto> {
	void printElem(const Goto &) const;

public:
	Goto * getFirstUncorrected();
};

#endif
