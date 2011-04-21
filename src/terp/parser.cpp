#include <stdlib.h>
#include "../stuff/exceptions.hpp"
#include "parser.hpp"

void Parser::checkOnVar()
{
	int link = rpn->getCount() - 1;
	RPNElem * elem = &(*rpn)[link];

	RPNVarVal * varval = dynamic_cast<RPNVarVal *>(elem);
	if (varval == 0)
		throw ParserError(LEX_IDENTIFICATOR, *lex, rstack.getStack());
	else
		rpn->deleteLast();
}

void Parser::getLex()
{
	if (lex != 0)
		delete lex;

	lex = lexer->getLex();
	//lex->print();
}

Parser::Parser(Lexer * lexer)
{
	if (lexer == 0)
		this->lexer = new Lexer;
	else
		this->lexer = lexer;

	lex = 0;
	scr = 0;
}

Parser::~Parser()
{
	if (lexer != 0)
		delete lexer;
	if (lex != 0)
		delete lex;
	if (scr != 0)
		delete scr;
}

Script * Parser::checkSyntax()
{
	getLex();
	scr = new Script();
	rpn =& scr->getRPN();

	P();

	scr->CorrectGotos();

	return scr;
}

void Parser::P()
{
	rstack.push(rP);
	while (*lex != LEX_EOI)
	{
		L();
		Cg();
	}
	getLex();
	rstack.pop();
}

void Parser::L()
{
	rstack.push(rL);
	if (*lex == LEX_LABEL)
	{
		getLex();

		if (*lex != LEX_IDENTIFICATOR)
			throw ParserError(LEX_IDENTIFICATOR, *lex, rstack.getStack());
		scr->declareLabel(*lex, rpn->getCount());
		getLex();

		if (*lex != LEX_COLON)
			throw ParserError(LEX_COLON, *lex, rstack.getStack());
		getLex();
	}
	rstack.pop();
}

void Parser::Cg()
{
	rstack.push(rCg);
	if (*lex == LEX_LBRACE)
	{
		getLex();

		while (*lex != LEX_RBRACE)
		{
			L();
			Cg();
		}
		getLex();
	}
	else if (*lex == LEX_IF)
	{
		RPNLabel * iflink;
		getLex();

		lparen();
		E1();
		rparen();
		rpn->put(iflink = new RPNLabel);
		rpn->put(new RPNGoFalse);
		Cg();
		if (*lex == LEX_ELSE)
		{
			RPNLabel * elselink;
			getLex();

			rpn->put(elselink = new RPNLabel);
			rpn->put(new RPNGo);
			iflink->set(rpn->getCount());

			Cg();

			elselink->set(rpn->getCount());
		}
		else
			iflink->set(rpn->getCount());
	}
	else if (*lex == LEX_WHILE)
	{
		RPNLabel * whilelink;
		int whilenum;
		getLex();

		whilenum = rpn->getCount();
		lparen();
		E1();
		rparen();
		rpn->put(whilelink = new RPNLabel);
		rpn->put(new RPNGoFalse);
		Cg();
		rpn->put(new RPNLabel(whilenum));
		rpn->put(new RPNGo);
		whilelink->set(rpn->getCount());
	}
	else if (*lex == LEX_FOR)
	{
		int link1, link2;
		RPNLabel * lbl1, * lbl2;
		getLex();

		lparen();
		E1();
		rpn->put(new RPNClear);
		semicolon();
		link1 = rpn->getCount();
		E1();
		rpn->put(lbl1 = new RPNLabel);
		rpn->put(new RPNGoFalse);
		rpn->put(lbl2 = new RPNLabel);
		rpn->put(new RPNGo);
		semicolon();
		link2 = rpn->getCount();
		E1();
		rpn->put(new RPNClear);
		rpn->put(new RPNLabel(link1));
		rpn->put(new RPNGo);
		rparen();
		lbl2->set(rpn->getCount());
		Cg();
		rpn->put(new RPNLabel(link2));
		rpn->put(new RPNGo);
		lbl1->set(rpn->getCount());
	}
	else
	{
		C();
		semicolon();
		rpn->put(new RPNClear);
	}
	rstack.pop();
}

void Parser::C()
{
	rstack.push(rC);
	if (*lex == LEX_GOTO)
	{
		RPNLabel * label;
		getLex();

		if (*lex != LEX_IDENTIFICATOR)
			throw ParserError(LEX_IDENTIFICATOR, *lex, rstack.getStack());

		rpn->put(label = new RPNLabel);
		scr->linkGoto(*lex, *label);

		getLex();

		rpn->put(new RPNGo);
	}
	else if (*lex == LEX_SEMICOLON)
	{
	}
	else
		E1();
	rstack.pop();
}

void Parser::E1()
{
	rstack.push(rE1);
	E2();

	while (*lex == LEX_OR)
	{
		getLex();

		E2();

		rpn->put(new RPNOr);
	}
	rstack.pop();
}

void Parser::E2()
{
	rstack.push(rE2);
	E3();

	while (*lex == LEX_AND)
	{
		getLex();

		E3();

		rpn->put(new RPNAnd);
	}
	rstack.pop();
}

void Parser::E3()
{
	rstack.push(rE3);
	E4();

	if (*lex == LEX_EQUAL ||
	    *lex == LEX_NEQ)
	{
		Lex l = *lex;
		getLex();

		E4();

		if (l == LEX_EQUAL)
			rpn->put(new RPNEqual);
		else
			rpn->put(new RPNNonequal);
	}
	rstack.pop();
}

void Parser::E4()
{
	rstack.push(rE4);
	E5();

	if (*lex == LEX_LESS    ||
	    *lex == LEX_LEQ     ||
	    *lex == LEX_GREATER ||
	    *lex == LEX_GEQ)
	{
		Lex l = *lex;
		getLex();

		E5();

		if (l == LEX_LESS)
			rpn->put(new RPNLess);
		else if (l == LEX_LEQ)
			rpn->put(new RPNLeq);
		else if (l == LEX_GREATER)
			rpn->put(new RPNGreater);
		else
			rpn->put(new RPNGeq);
	}
	rstack.pop();
}

void Parser::E5()
{
	rstack.push(rE5);
	E6();

	while (*lex == LEX_PLUS  ||
	       *lex == LEX_MINUS)
	{
		Lex l = *lex;
		getLex();

		E6();

		if (l == LEX_PLUS)
			rpn->put(new RPNPlus);
		else
			rpn->put(new RPNMinus);
	}
	rstack.pop();
}

void Parser::E6()
{
	rstack.push(rE6);
	E7();

	while (*lex == LEX_MUL ||
	       *lex == LEX_DIV ||
	       *lex == LEX_MODULO)
	{
		Lex l = *lex;
		getLex();

		E7();

		if (l == LEX_MUL)
			rpn->put(new RPNMul);
		else if (l == LEX_DIV)
			rpn->put(new RPNDiv);
		else
			rpn->put(new RPNModulo);
	}
	rstack.pop();
}

void Parser::E7()
{
	rstack.push(rE7);
	if (*lex == LEX_NOT)
	{
		getLex();

		E8();

		rpn->put(new RPNNot);
	}
	else if (*lex == LEX_INC)
	{
		getLex();

		E8();

		checkOnVar();
		rpn->put(new RPNPrefixInc);
	}
	else if (*lex == LEX_DEC)
	{
		getLex();

		E8();

		checkOnVar();
		rpn->put(new RPNPrefixDec);
	}
	else if (*lex == LEX_MINUS)
	{
		getLex();

		E8();

		rpn->put(new RPNUnMinus);
	}
	else
	{
		E8();

		if (*lex == LEX_INC)
		{
			getLex();
			checkOnVar();
			rpn->put(new RPNInfixInc);
		}
		else if (*lex == LEX_DEC)
		{
			getLex();
			checkOnVar();
			rpn->put(new RPNInfixDec);
		}
	}
	rstack.pop();
}

void Parser::E8()
{
	rstack.push(rE8);
	if (*lex == LEX_LPAREN)
	{
		getLex();

		E1();

		rparen();
	}
	else if (*lex == LEX_IDENTIFICATOR)
	{
		int link;
		Lex l = *lex;
		getLex();

		if (*lex == LEX_LBRACKET)
		{
			getLex();

			link = scr->declareArray(l);
			rpn->put(new RPNVar(link));

			E1();

			if (*lex != LEX_RBRACKET)
				throw ParserError(LEX_RBRACKET, *lex, rstack.getStack());
			getLex();

			rpn->put(new RPNArrayVar);
		}
		else
		{
			link = scr->declareVar(l);
			rpn->put(new RPNVar(link));
		}

		Eq();
	}
	else if (*lex == LEX_TRUE)
	{
		rpn->put(new RPNInt(0));
		getLex();
	}
	else if (*lex == LEX_FALSE)
	{
		rpn->put(new RPNInt(1));
		getLex();
	}
	else if (*lex == LEX_NUMBER)
	{
		rpn->put(new RPNInt(atoi(lex->getValue())));
		getLex();
	}
	else if (*lex == LEX_STRING)
	{
		Buffer buf;

		buf.putStr(lex->getValue());
		getLex();

		while (*lex == LEX_STRING)
		{
			buf.putStr(lex->getValue());
			getLex();
		}

		char * str = buf.getStr();
		rpn->put(new RPNString(str));
		delete [] str;
	}
	else if (*lex == LEX_FUNC )
	{
		Lex l = *lex;
		int params = l.getFuncParams();
		getLex();

		lparen();
		if (*lex != LEX_RPAREN)
		{
			if (params == -1)
			{
				int i = 1;
				E1();

				while (*lex == LEX_COMMA)
				{
					i++;
					getLex();

					E1();
				}

				rpn->put(new RPNPrint(i));
			}
			else if (params > 0)
			{
				E1();

				for (int i = 2; i <= params; i++)
					if (*lex == LEX_COMMA)
					{
						getLex();

						E1();
					}
					else
						throw ParserError(LEX_COMMA, *lex, rstack.getStack());
			}
		}
		rparen();

		//printf(" P_FUNC(%s) ", l.getFType());
	}
	else
		throw ParserError(LEX_NULL, *lex, rstack.getStack());
	rstack.pop();
}

void Parser::Eq()
{
	rstack.push(rEq);
	if (*lex == LEX_ASSIGN)
	{
		getLex();

		E1();

		//printf(" P_ASSIGN ");
		rpn->put(new RPNAssign);
	}
	else
		rpn->put(new RPNVarVal);
	rstack.pop();
}

void Parser::lparen()
{
	if (*lex != LEX_LPAREN)
		throw ParserError(LEX_LPAREN, *lex, rstack.getStack());
	getLex();
}

void Parser::rparen()
{
	if (*lex != LEX_RPAREN)
		throw ParserError(LEX_RPAREN, *lex, rstack.getStack());
	getLex();
}

void Parser::semicolon()
{
	if (*lex != LEX_SEMICOLON)
		throw ParserError(LEX_SEMICOLON, *lex, rstack.getStack());
	getLex();
}

Parser::RuleStack::RuleStack()
{
	first = 0;
	stackStr = 0;
}

Parser::RuleStack::~RuleStack()
{
	RuleStackElem * p;

	while (first != 0)
	{
		p = first;
		first = first->next;
		delete p;
	}

	if (stackStr != 0)
		delete [] stackStr;
}

void Parser::RuleStack::push(Rule r)
{
	RuleStackElem * p = new RuleStackElem;

	p->rule = r;
	p->next = first;
	first = p;
}

Parser::Rule Parser::RuleStack::pop()
{
	RuleStackElem * p;
	Rule r;

	p = first;
	first = p->next;
	r = p->rule;
	delete p;

	return r;
}

const char * Parser::RuleStack::getStack()
{
	RuleStackElem * p;
	int len = 0;
	char * cp = 0;

	p = first;
	while (p != 0)
	{
		switch (p->rule)
		{
			case rP:
			case rL:
			case rC:
				len += 2;
				break;
			case rCg:
			case rE1:
			case rE2:
			case rE3:
			case rE4:
			case rE5:
			case rE6:
			case rE7:
			case rE8:
			case rEq:
				len += 3;
				break;
		}
		p = p->next;
	}

	if (stackStr != 0)
		delete [] stackStr;
	stackStr = new char[len];
	cp = stackStr;
	p = first;
	while (p != 0)
	{
		switch (p->rule)
		{
			case rP:
				*cp++ = 'P';
				break;
			case rL:
				*cp++ = 'L';
				break;
			case rC:
				*cp++ = 'C';
				break;
			case rCg:
				*cp++ = 'C';
				*cp++ = 'g';
				break;
			case rE1:
				*cp++ = 'E';
				*cp++ = '1';
				break;
			case rE2:
				*cp++ = 'E';
				*cp++ = '2';
				break;
			case rE3:
				*cp++ = 'E';
				*cp++ = '3';
				break;
			case rE4:
				*cp++ = 'E';
				*cp++ = '4';
				break;
			case rE5:
				*cp++ = 'E';
				*cp++ = '5';
				break;
			case rE6:
				*cp++ = 'E';
				*cp++ = '6';
				break;
			case rE7:
				*cp++ = 'E';
				*cp++ = '7';
				break;
			case rE8:
				*cp++ = 'E';
				*cp++ = '8';
				break;
			case rEq:
				*cp++ = 'E';
				*cp++ = 'q';
				break;
		}
		*cp++ = ' ';
		p = p->next;
	}
	stackStr[len - 1] = '\0';

	return stackStr;
}
