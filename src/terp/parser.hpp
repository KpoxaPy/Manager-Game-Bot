#ifndef _PARSER_HPP_
#define _PARSER_HPP_

#include "lexer.hpp"
#include "script.hpp"

class Parser {
	enum Rule {rP, rL, rCg, rC, rE1, rE2, rE3, rE4, rE5, rE6, rE7, rE8, rEq};
	class RuleStack {
		struct RuleStackElem {
			Rule rule;
			struct RuleStackElem * next;

		} *first;

		char * stackStr;

	public:
		RuleStack();
		~RuleStack();

		void push(Rule);
		Rule pop();
		const char * getStack();
	} rstack;

	Lexer * lexer;
	Lex * lex;
	Script * scr;
	RPNScript * rpn;


	void checkOnVar();
	void getLex();

	void P();
	void L();
	void Cg();
	void C();
	void E1();
	void E2();
	void E3();
	void E4();
	void E5();
	void E6();
	void E7();
	void E8();
	void Eq();

	void lparen();
	void rparen();
	void semicolon();

public:
	Parser(Lexer *);
	Parser(Input * = 0);
	~Parser();

	Script * checkSyntax();
};

#endif
