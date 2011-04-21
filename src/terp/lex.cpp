#include <string.h>
#include <stdio.h>
#include "lex.hpp"

const char * Lex::lexTypeNames[] = {
	"LEX_NULL",
	"LEX_EOI",

	"LEX_IDENTIFICATOR",
	"LEX_NUMBER",
	"LEX_STRING",
	"LEX_FUNC",

	"LEX_LPAREN",
	"LEX_RPAREN",
	"LEX_LBRACE",
	"LEX_RBRACE",
	"LEX_LBRACKET",
	"LEX_RBRACKET",
	"LEX_SEMICOLON",
	"LEX_MUL",
	"LEX_MODULO",
	"LEX_PLUS",
	"LEX_MINUS",
	"LEX_LESS",
	"LEX_GREATER",
	"LEX_NOT",
	"LEX_ASSIGN",
	"LEX_DIV",
	"LEX_COLON",
	"LEX_COMMA",

	"LEX_INC",
	"LEX_DEC",
	"LEX_LEQ",
	"LEX_GEQ",
	"LEX_NEQ",
	"LEX_EQUAL",
	"LEX_AND",
	"LEX_OR",

	"LEX_FOR",
	"LEX_WHILE",
	"LEX_IF",
	"LEX_ELSE",
	"LEX_GOTO",
	"LEX_LABEL",
	"LEX_TRUE",
	"LEX_FALSE"
};

const char * Lex::funcTypeNames[] = {
	"FUNC_NULL",
	"FUNC_PRINT",
	"FUNC_FUNC0",
	"FUNC_FUNC1",
	"FUNC_FUNC2",
	"FUNC_FUNC"
};

const Lex::Delim1LexTypePair Lex::tDelim1[] = {
	{'(', LEX_LPAREN},
	{')', LEX_RPAREN},
	{'{', LEX_LBRACE},
	{'}', LEX_RBRACE},
	{'[', LEX_LBRACKET},
	{']', LEX_RBRACKET},
	{';', LEX_SEMICOLON},
	{'*', LEX_MUL},
	{'%', LEX_MODULO},
	{'+', LEX_PLUS},
	{'-', LEX_MINUS},
	{'<', LEX_LESS},
	{'>', LEX_GREATER},
	{'!', LEX_NOT},
	{'=', LEX_ASSIGN},
	{'/', LEX_DIV},
	{':', LEX_COLON},
	{',', LEX_COMMA},
	{0, LEX_NULL}
};

const Lex::KeywordLexTypePair Lex::tKeywords[] = {
	{"for", LEX_FOR},
	{"while", LEX_WHILE},
	{"if", LEX_IF},
	{"else", LEX_ELSE},
	{"goto", LEX_GOTO},
	{"label", LEX_LABEL},
	{"true", LEX_TRUE},
	{"false", LEX_FALSE},
	{0, LEX_NULL}
};

const Lex::FuncLexTypePair Lex::tFuncs[] = {
	{"func0", FUNC_FUNC0, 0},
	{"func1", FUNC_FUNC1, 1},
	{"func2", FUNC_FUNC2, 2},
	{"func3", FUNC_FUNC3, 3},
	{"print", FUNC_PRINT, -1},
	{0, FUNC_NULL, 0}
};

const Lex::Delim2LexTypePair Lex::tDelim2[] = {
	{"++", LEX_INC},
	{"--", LEX_DEC},
	{"<=", LEX_LEQ},
	{">=", LEX_GEQ},
	{"!=", LEX_NEQ},
	{"==", LEX_EQUAL},
	{"&&", LEX_AND},
	{"||", LEX_OR},
	{0, LEX_NULL}
};

Lex::~Lex()
{
	if (value != 0)
		delete [] value;
}

Lex::Lex(const Lex & lex)
{
	type = lex.type;
	ftype = lex.ftype;
	line = lex.line;
	funcParams = lex.funcParams;

	if (lex.value != 0)
	{
		value = new char[strlen(lex.value)+1];
		strcpy(value, lex.value);
	}
	else
		value = 0;
}

const Lex & Lex::operator=(const Lex & lex)
{
	type = lex.type;
	ftype = lex.ftype;
	line = lex.line;
	funcParams = lex.funcParams;

	if (value != 0)
		delete [] value;
	if (lex.value != 0)
	{
		value = new char[strlen(lex.value)+1];
		strcpy(value, lex.value);
	}
	else
		value = 0;

	return *this;
}

Lex::Lex(lexType t, int l, const char * str)
{
	type = t;
	ftype = FUNC_NULL;
	line = l;
	funcParams = 0;

	if (str != 0)
	{
		value = new char[strlen(str)+1];
		strcpy(value, str);
	}
	else
		value = 0;
}

bool Lex::operator==(lexType vtype) const
{
	return (type == vtype);
}

bool Lex::operator==(const Lex & vlex) const
{
	return (type == vlex.type);
}

bool Lex::operator!=(lexType vtype) const
{
	return (type != vtype);
}

bool Lex::operator!=(const Lex & vlex) const
{
	return (type != vlex.type);
}

const char * Lex::getLexTypeStr(lexType t)
{
	return lexTypeNames[t];
}

const char * Lex::getFuncTypeStr(funcDef t)
{
	return funcTypeNames[t];
}

const char * Lex::getFType() const
{
	return getFuncTypeStr(ftype);
}

const char * Lex::getStr() const
{
	return getLexTypeStr(type);
}

const char * Lex::getValue() const
{
	return value;
}

int Lex::getLine() const
{
	return line;
}

int Lex::getFuncParams() const
{
	return funcParams;
}

funcDef Lex::getFuncType() const
{
	return ftype;
}

void Lex::print() const
{
	printf("%s (%s) at line %d\n", getStr(), (value != 0 ? value : ""), line);
}

LexOrder::LexOrder()
{
	first = 0;
	last = 0;
}

LexOrder::~LexOrder()
{
	OrderElem * sep;

	while (first != 0)
	{
		sep = first;
		delete first->elem;
		first = first->next;
		delete sep;
	}
}

void LexOrder::put(Lex * lex)
{
	OrderElem * sep = new OrderElem;

	sep->elem = lex;
	sep->next = first;
	sep->prev = 0;

	if (last == 0)
		last = sep;
	else
		first->prev = sep;

	first = sep;
}

Lex * LexOrder::get()
{
	OrderElem * sep = last;
	Lex * lex;

	if (last == 0)
		return 0;
	else
		lex = last->elem;

	last = sep->prev;
	if (last != 0)
		last->next = 0;
	else
		first = 0;
	delete sep;

	return lex;
}

bool LexOrder::isEmpty() const
{
	return (first == 0);
}

Lex * Lex::makeDelim1(int l, char c)
{
	const Delim1LexTypePair * t = tDelim1;

	while (t->type != LEX_NULL && t->symbol != c)
		t++;

	if (t == NULL)
		return 0;
	else
		return new Lex(t->type, l);
}

Lex * Lex::makeIdentificator(int l, char * str)
{
	const KeywordLexTypePair * t = tKeywords;
	const FuncLexTypePair * f = tFuncs;
	Lex * lex;

	while (t->type != LEX_NULL && strcmp(t->str, str) != 0)
		t++;

	if (t->type != LEX_NULL)
		lex = new Lex(t->type, l);
	else
	{
		while (f->str != 0 && strcmp(f->str, str) != 0)
			f++;

		if (f->str != 0)
		{
			lex = new Lex(LEX_FUNC, l);
			lex->funcParams = f->params;
			lex->ftype = f->ftype;
		}
		else
			lex = new Lex(LEX_IDENTIFICATOR, l, str);
	}

	delete [] str;
	return lex;
}

Lex * Lex::makeString(int l, char * str)
{
	Lex * lex = new Lex(LEX_STRING, l, str);

	delete [] str;
	return lex;
}

Lex * Lex::makeNumber(int l, char * str)
{
	Lex * lex = new Lex(LEX_NUMBER, l, str);

	delete [] str;
	return lex;
}

Lex * Lex::makeDelim2(int l, char * str)
{
	const Delim2LexTypePair * t = tDelim2;

	while (t->type != LEX_NULL && strcmp(t->str, str) != 0)
		t++;

	delete [] str;

	if (t == NULL)
		return 0;
	else
		return new Lex(t->type, l);
}
