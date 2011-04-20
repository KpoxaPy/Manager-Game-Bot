#ifndef _SCRIPT_HPP_
#define _SCRIPT_HPP_

#include "ident.hpp"
#include "label.hpp"
#include "tables.hpp"
#include "RPN.hpp"

class RPNScript;

class Script {
	friend class RPNVar;

	IdentTable idents;
	LabelTable labels;
	GotoTable gotos;
	IntTable integers;
	IntArrayTable arrays;
	RPNScript * rpn;

public:
	Script();
	~Script();

	int declareVar(Lex &);
	int declareArray(Lex &);
	int declareLabel(Lex &, int);
	void linkGoto(Lex &, RPNLabel &);

	void CorrectGotos();

	IdentTable & getIdents();
	IntTable & getIntegers();
	IntArrayTable & getArrays();
	RPNScript & getRPN();

	void print() const;
};

#endif
