#include <stdio.h>
#include <string.h>
#include "label.hpp"
#include "RPN.hpp"

Label::Label()
{
	defineLine = -1;
	id = -1;
	link = -1;
	used = false;
	name = 0;
}

Label::Label(const char * s, int dl, int l)
{
	defineLine = dl;
	link = l;
	id = -1;
	used = false;

	if (s != 0)
	{
		name = new char[strlen(s) + 1];
		strcpy(name, s);
	}
	else
		name = 0;
}

Label::Label(const Lex & lex, int l)
{
	const char * str;
	defineLine = lex.getLine();
	link = l;
	id = -1;
	used = false;

	if ((str = lex.getValue()) != 0)
	{
		name = new char[strlen(str) + 1];
		strcpy(name, str);
	}
	else
		name = 0;
}

Label::Label(const Label & img)
{
	defineLine = img.defineLine;
	link = img.link;
	id = img.id;
	used = img.used;

	if (img.name != 0)
	{
		name = new char[strlen(img.name) + 1];
		strcpy(name, img.name);
	}
	else
		name = 0;
}

Label::~Label()
{
	if (name != 0)
		delete name;
}

const Label & Label::operator=(const Label & img)
{
	defineLine = img.defineLine;
	link = img.link;
	id = img.id;
	used = img.used;

	if (img.name != 0)
	{
		name = new char[strlen(img.name) + 1];
		strcpy(name, img.name);
	}
	else
		name = 0;

	return *this;
}

void Label::setID(int n)
{
	id = n;
}

void Label::setLink(int n)
{
	link = n;
}

const char * Label::getName() const
{
	return name;
}

int Label::getID() const
{
	return id;
}

int Label::getLink() const
{
	return link;
}

int Label::getLine() const
{
	return defineLine;
}

void Label::printraw() const
{
	printf("Label(%s)%s id: %d line: %d link: %d", name,
		(used ? " USED" : ""),
		id, defineLine, link);
}

void Label::print() const
{
	printraw();
	printf("\n");
}

void Label::setAsUsed()
{
	used = true;
}

bool Label::isUsed() const
{
	return used;
}

Label * LabelTable::findLabelByName(const char * str) const
{
	Sheet * sh;
	for (int i = 0; i < tableSize; i++)
		if (table[i] != 0)
		{
			sh = table[i];
			while (sh != 0)
			{
				for (int j = 0; j < sheetSize; j++)
					if (sh->data[j] != 0)
						if (strcmp(sh->data[j]->name, str) == 0)
							return sh->data[j];
				sh = sh->next;
			}
		}
	return 0;
}

int LabelTable::findLinkByName(const char * str) const
{
	Sheet * sh;
	for (int i = 0; i < tableSize; i++)
		if (table[i] != 0)
		{
			int l = 0;
			sh = table[i];
			while (sh != 0)
			{
				for (int j = 0; j < sheetSize; j++)
					if (sh->data[j] != 0)
						if (strcmp(sh->data[j]->name, str) == 0)
							return l*256 + i*16 + j;
				sh = sh->next;
				l++;
			}
		}
	return -1;
}

void LabelTable::printElem(const Label & id) const
{
	id.printraw();
}

Goto::Goto()
{
	defineLine = -1;
	id = -1;
	name = 0;
	label = 0;
	corrected = false;
}

Goto::Goto(const char * str, int dl, RPNLabel & l)
{
	defineLine = dl;
	id = -1;
	corrected = false;
	label = &l;

	if (str != 0)
	{
		name = new char[strlen(str) + 1];
		strcpy(name, str);
	}
	else
		name = 0;
}

Goto::Goto(const Lex & lex, RPNLabel & l)
{
	const char * str;

	defineLine = lex.getLine();
	id = -1;
	corrected = false;
	label = &l;

	if ((str = lex.getValue()) != 0)
	{
		name = new char[strlen(str) + 1];
		strcpy(name, str);
	}
	else
		name = 0;
}

Goto::Goto(const Goto & img)
{
	defineLine = img.defineLine;
	id = img.id;
	corrected = img.corrected;
	label = img.label;

	if (img.name != 0)
	{
		name = new char[strlen(img.name) + 1];
		strcpy(name, img.name);
	}
	else
		name = 0;
}

Goto::~Goto()
{
	if (name != 0)
		delete [] name;
}


const Goto & Goto::operator=(const Goto & img)
{
	defineLine = img.defineLine;
	id = img.id;
	corrected = img.corrected;
	label = img.label;

	if (img.name != 0)
	{
		name = new char[strlen(img.name) + 1];
		strcpy(name, img.name);
	}
	else
		name = 0;

	return *this;
}

void Goto::setID(int n)
{
	id = n;
}

void Goto::setAsCorrected()
{
	corrected = true;
}

void Goto::setLabel(RPNLabel * l)
{
	label = l;
}

void Goto::correctOnLabel(Label & l)
{
	if (label == 0)
		throw Bug("Trying to correct goto (\"%s\" id: %d line: %d) with undefined RPNLabel", name,
			id, defineLine);

	label->set(l.getLink());
	corrected = true;
}

const char * Goto::getName() const
{
	return name;
}

int Goto::getID() const
{
	return id;
}

int Goto::getLine() const
{
	return defineLine;
}

bool Goto::isCorrected() const
{
	return corrected;
}


void Goto::printraw() const
{
	printf("Goto(%s)%s id: %d line: %d ", name,
		(corrected ? " CORRECTED" : ""),
		id, defineLine);
}

void Goto::print() const
{
	printraw();
	printf("\n");
}

void GotoTable::printElem(const Goto & gt) const
{
	gt.printraw();
}

Goto * GotoTable::getFirstUncorrected()
{
	Sheet * sh;
	int l;

	for (int i = 0; i < tableSize; i++)
	{
		sh = table[i];
		l = 0;
		if (sh != 0)
			while (sh != 0)
			{
				for (int j = 0; j < sheetSize; j++)
					if (sh->data[j] != 0)
						if (!sh->data[j]->isCorrected())
							return sh->data[j];
				sh = sh->next;
				l++;
			}
	}

	return 0;
}
