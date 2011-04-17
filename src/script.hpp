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
	IntTable integers;
	IntArrayTable arrays;
	RPNScript * rpn;

public:
	Script();
	~Script();

	int declareVar(Lex &);
	int declareArray(Lex &);

	IdentTable & getIdents();
	IntTable & getIntegers();
	IntArrayTable & getArrays();
	RPNScript & getRPN();

	void print() const;
};

#endif
