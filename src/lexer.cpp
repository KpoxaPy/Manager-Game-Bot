#include <ctype.h>
#include "exceptions.hpp"
#include "lexer.hpp"

Lexer::Lexer(Input * src)
{
	CS = H;

	if (src == 0)
		raw = new FileInput;
	else
		raw = src;
}

Lexer::~Lexer()
{
	if (raw != 0)
		delete raw;
}

Lex * Lexer::getLex()
{
	bool isEOF = false;
	int c;
	Lex * lex;

	while (lexes.isEmpty() && !isEOF)
	{
		c = raw->getChar();

		if (c == EOF)
		{
			step('\n');
			isEOF = true;
		}
		else
			step(c);
	}

	if (isEOF)
	{
		if (CS == Q || CS == Q_BS)
			throw LexerError("Unexpected end of input: string isn't unfinished at line %d", raw->getLine());
		else if (CS != H)
			throw LexerError("Unexpected end of input at line %d", raw->getLine());
	}

	//if (isEOF)
		//printf("Lexer is empty? %s\n", (lexes.isEmpty() ? "Yes" : "No"));

	if ((lex = lexes.get()) == 0)
		lex = new Lex(LEX_EOI, raw->getLine());

	return lex;
}

void Lexer::step(char c)
{
	//printf("Step %d started with '%c' ", CS, c);
	switch (CS)
	{
		case H:
			state_H(c);
			break;
		case D2:
			state_D2(c);
			break;
		case D1_2:
			state_D1_2(c);
			break;
		case N:
			state_N(c);
			break;
		case I:
			state_I(c);
			break;
		case C1_2S:
			state_C1_2S(c);
			break;
		case C1L:
			state_C1L(c);
			break;
		case CML:
			state_CML(c);
			break;
		case CMLE:
			state_CMLE(c);
			break;
		case Q:
			state_Q(c);
			break;
		case Q_BS:
			state_Q_BS(c);
			break;
	}
	//printf("and end on %d\n", CS);
}

void Lexer::state_H(char c)
{
	if (c == ' '  ||
	    c == '\n' ||
			c == '\r' ||
			c == '\t')
		return;
	else if (c == '(' ||
		       c == ')' ||
					 c == '{' ||
					 c == '}' ||
					 c == '[' ||
					 c == ']' ||
					 c == ';' ||
					 c == '*' ||
					 c == '%' ||
					 c == ':' ||
					 c == ',')
		lexes.put(Lex::makeDelim1(raw->getLine(), c));
	else if (c == '&' ||
			     c == '|')
	{
		CS = D2;
		buf.putChar(c);
	}
	else if (c == '+' ||
			     c == '-' ||
					 c == '<' ||
					 c == '>' ||
					 c == '!' ||
					 c == '=')
	{
		CS = D1_2;
		buf.putChar(c);
	}
	else if (c == '/')
		CS = C1_2S;
	else if (isalpha(c) || c == '_')
	{
		buf.putChar(c);
		CS = I;
	}
	else if (isdigit(c))
	{
		buf.putChar(c);
		CS = N;
	}
	else if (c == '"')
		CS = Q;
	else
		throw LexerError("Unexpected character '%c' at line %d", c, raw->getLine());
}

void Lexer::state_D2(char c)
{

	if (c == buf[0])
	{
		buf.putChar(c);
		lexes.put(Lex::makeDelim2(raw->getLine(), buf.getStr()));
		CS = H;
	}
	else
		throw LexerError("Unexpected character '%c' at line %d", c, raw->getLine());
}

void Lexer::state_D1_2(char c)
{
	static const struct charPairs { char c1; char c2; } cps[] = {
		{'+', '+'},
		{'-', '-'},
		{'<', '='},
		{'>', '='},
		{'!', '='},
		{'=', '='},
		{0, 0}
	};
	const charPairs * cp = cps;

	while (cp->c1 != 0 && (
	       cp->c1 != buf[0] ||
				 cp->c2 != c))
		cp++;

	if (cp->c1 != 0)
	{
		buf.putChar(c);
		lexes.put(Lex::makeDelim2(raw->getLine(), buf.getStr()));
		CS = H;
	}
	else
	{
		lexes.put(Lex::makeDelim1(raw->getLine(), buf[0]));
		buf.erase();
		CS = H;
		state_H(c);
	}
}

void Lexer::state_N(char c)
{
	if (isdigit(c))
		buf.putChar(c);
	else if (isalpha(c) || c == '_')
		throw LexerError("Unexpected character '%c' at line %d", c, raw->getLine());
	else
	{
		lexes.put(Lex::makeNumber(raw->getLine(), buf.getStr()));
		CS = H;
		state_H(c);
	}
}

void Lexer::state_I(char c)
{
	if (isalpha(c) || isdigit(c) || c == '_')
		buf.putChar(c);
	else
	{
		lexes.put(Lex::makeIdentificator(raw->getLine(), buf.getStr()));
		CS = H;
		state_H(c);
	}
}

void Lexer::state_C1_2S(char c)
{
	if (c == '*')
		CS = CML;
	else if (c == '/')
		CS = C1L;
	else
	{
		lexes.put(new Lex(LEX_DIV, raw->getLine()));
		CS = H;
		state_H(c);
	}
}

void Lexer::state_C1L(char c)
{
	if (c == '\n')
		CS = H;
}

void Lexer::state_CML(char c)
{
	if (c == '*')
		CS = CMLE;
}

void Lexer::state_CMLE(char c)
{
	if (c == '/')
		CS = H;
	else
		CS = CML;
}

void Lexer::state_Q(char c)
{
	if (c == '"')
	{
		lexes.put(Lex::makeString(raw->getLine(), buf.getStr()));
		CS = H;
	}
	else if (c == '\\')
		CS = Q_BS;
	else
		buf.putChar(c);
}

void Lexer::state_Q_BS(char c)
{
	if (c == 'n')
		buf.putChar('\n');
	else if (c == 't')
		buf.putChar('\t');
	else if (c == 'r')
		buf.putChar('\r');
	else
		buf.putChar(c);
	CS = Q;
}
