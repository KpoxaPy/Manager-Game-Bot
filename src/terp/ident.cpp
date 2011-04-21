#include <string.h>
#include <stdio.h>
#include "ident.hpp"

Ident::Ident()
{
	link = -1;
	id = -1;
	defineLine = -1;
	Var = false;
	Array = false;
	name = 0;
}

Ident::Ident(const char * name, int line)
{
	link = -1;
	id = -1;
	Var = false;
	Array = false;
	this->defineLine = line;

	if (name != 0)
	{
		this->name = new char[strlen(name) + 1];
		strcpy(this->name, name);
	}
	else
		this->name = 0;
}

Ident::Ident(const Ident & img)
{
	defineLine = img.defineLine;
	link = img.link;
	id = img.id;
	Var = img.Var;
	Array = img.Array;

	if (img.name != 0)
	{
		name = new char[strlen(img.name) + 1];
		strcpy(name, img.name);
	}
	else
		name = 0;
}

Ident::Ident(const Lex & lex)
{
	const char * str;
	defineLine = lex.getLine();
	link = -1;
	id = -1;
	Var = false;
	Array = false;

	if ((str = lex.getValue()) != 0)
	{
		name = new char[strlen(str) + 1];
		strcpy(name, str);
	}
	else
		name = 0;
}

Ident::~Ident()
{
	if (name != 0)
		delete [] name;
}

const Ident & Ident::operator=(const Ident & img)
{
	defineLine = img.defineLine;
	link = img.link;
	Var = img.Var;
	Array = img.Array;

	if (img.name != 0)
	{
		name = new char[strlen(img.name) + 1];
		strcpy(name, img.name);
	}
	else
		name = 0;

	return *this;
}

void Ident::makeVar()
{
	Var = true;
}

void Ident::makeArray()
{
	Array = true;
}

void Ident::setLink(int l)
{
	link = l;
}

int Ident::getLink() const
{
	return link;
}

const char * Ident::getName() const
{
	return name;
}

void Ident::setID(int id)
{
	this->id = id;
}

int Ident::getID() const
{
	return id;
}

bool Ident::isVar() const
{
	return Var;
}

bool Ident::isArray() const
{
	return Array;
}

void Ident::printraw() const
{
	printf("Ident(%s)%s%s id: %d link: %d line: %d", name,
		(Var ? " var" : ""),
		(Array ? " array" : ""),
		id, link, defineLine);
}

void Ident::print() const
{
	printraw();
	printf("\n");
}

Ident * IdentTable::findIdentByName(const char * str) const
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

int IdentTable::findLinkByName(const char * str) const
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

void IdentTable::printElem(const Ident & id) const
{
	id.printraw();
}
