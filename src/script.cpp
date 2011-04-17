#include "script.hpp"
#include "exceptions.hpp"

Script::Script()
{
	rpn = new RPNScript(this);
}

Script::~Script()
{
	if (rpn != 0)
		delete rpn;
}

IdentTable & Script::getIdents()
{
	return idents;
}

IntTable & Script::getIntegers()
{
	return integers;
}

IntArrayTable & Script::getArrays()
{
	return arrays;
}

RPNScript & Script::getRPN()
{
	return *rpn;
}

int Script::declareVar(Lex & lex)
{
	int id;
	if ((id = idents.findLinkByName(lex.getValue())) == -1)
	{
		id = idents.put(new Ident(lex));
		Ident & ident = idents[id];
		ident.setID(id);
		ident.makeVar();
		//ident.print();
	}
	else
	{
		Ident & ident = idents[id];
		ident.makeVar();
		if (ident.isVar() && ident.isArray())
			throw ParserError("Identificator '%s' declared both as variable and as array (first appear on line %d)",
				ident.getName(), -1);
	}

	return id;
}

int Script::declareArray(Lex & lex)
{
	int id;
	if ((id = idents.findLinkByName(lex.getValue())) == -1)
	{
		id = idents.put(new Ident(lex));
		Ident & ident = idents[id];
		ident.setID(id);
		ident.makeArray();
		//ident.print();
	}
	else
	{
		Ident & ident = idents[id];
		ident.makeArray();
		if (ident.isVar() && ident.isArray())
			throw ParserError("Identificator '%s' declared both as variable and as array (first appear on line %d)",
				ident.getName(), -1);
	}

	return id;
}

void Script::print() const
{
	printf("======== SCRIPT =======\n");
	printf("--= TABLE OF IDENTS =--\n");
	idents.printPlain();
	printf("\n---= TABLE OF INTS =---\n");
	integers.printPlain();
	printf("\n--= TABLE OF ARRAYS =--\n");
	arrays.printPlain();
	printf("\n\n");
}
