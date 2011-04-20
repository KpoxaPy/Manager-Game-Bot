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

int Script::declareLabel(Lex & lex, int link)
{
	int id;
	if ((id = labels.findLinkByName(lex.getValue())) == -1)
	{
		id = labels.put(new Label(lex, link));
		labels[id].setID(id);
	}
	else
		throw ParserError("Double declaring label with name \"%s\" (first appear on line %d) on line %d", lex.getValue(),
			labels[id].getLine(), lex.getLine());

	return id;
}

void Script::linkGoto(Lex & lex, RPNLabel & label)
{
	int id;

	if ((id = labels.findLinkByName(lex.getValue())) != -1)
		label.set(labels[id].getLink());
	else
	{
		id = gotos.put(new Goto(lex, label));
		gotos[id].setID(id);
	}
}

void Script::CorrectGotos()
{
	int id;
	Goto * gt;

	while ((gt = gotos.getFirstUncorrected()) != 0)
		if ((id = labels.findLinkByName(gt->getName())) != -1)
			gt->correctOnLabel(labels[id]);
		else	
			throw ParserError("Goto (line %d) on label \"%s\" which unexistent", gt->getLine(), gt->getName());
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
	printf("\n--= TABLE OF LABELS =--\n");
	labels.printPlain();
	printf("\n--= TABLE OF GOTOS =--\n");
	gotos.printPlain();
	printf("\n\n");
}
