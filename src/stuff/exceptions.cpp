#include <string.h>
#include <stdio.h>
#include "exceptions.hpp"

/*
 * Error exception class definition
 */

Error::Error()
{
	message = 0;
}

Error::Error(const char * mes, ...)
{
	va_list ap;

	va_start(ap, mes);
	setMessage(mes, ap);
	va_end(ap);
}

Error::Error(const Error & img)
{
	message = strdup(img.message);
}

Error::~Error()
{
	delete [] message;
}

char * Error::strdup(const char * str)
{
	char * res = new char[strlen(str) + 1];
	strcpy(res, str);
	return res;
}

void Error::setMessage(const char * fmt, va_list ap)
{
	va_list aq;

	if (message != 0)
		delete [] message;

	va_copy(aq, ap);
	message = makeMessage(fmt, aq);
	va_end(aq);
}

char * Error::makeMessage(const char * fmt, va_list ap)
{
	int n, size = 100;
	char *p;
	va_list aq;

	if ((p = new char[size]) == NULL)
		return NULL;

	while (1) {
		va_copy(aq, ap);
		n = vsnprintf(p, size, fmt, aq);
		va_end(aq);

		if (n == size - 1)
			return p;
		else
			size = n+1;

		delete [] p;
		if ((p = new char[size]) == NULL)
			return NULL;
	}
}

char * Error::makeMessageV(const char * fmt, ...)
{
	va_list ap;
	char * str;

	va_start(ap, fmt);
	str = makeMessage(fmt, ap);
	va_end(ap);

	return str;
}

const char * Error::getMessage() const
{
	return message;
}

/*
 * User error exception class definition
 */

UserError::UserError()
{
}

UserError::UserError(const char * mes, ...)
{
	va_list ap;

	va_start(ap, mes);
	setMessage(mes, ap);
	va_end(ap);
}

UserError::UserError(const UserError & img)
{
}

UserError::~UserError()
{
}

/*
 * External error exception class definition
 */

ExternalError::ExternalError()
{
}

ExternalError::ExternalError(const char * mes, ...)
{
	va_list ap;

	va_start(ap, mes);
	setMessage(mes, ap);
	va_end(ap);
}

ExternalError::ExternalError(const ExternalError & img)
{
}

ExternalError::~ExternalError()
{
}

/*
 * Bug exception class definition
 */

Bug::Bug()
{
}

Bug::Bug(const char * mes, ...)
{
	va_list ap;

	va_start(ap, mes);
	setMessage(mes, ap);
	va_end(ap);
}

Bug::Bug(const Bug & img)
{
}

Bug::~Bug()
{
}

/*
 * Lexer error exception class definition
 */

LexerError::LexerError()
{
}

LexerError::LexerError(const char * mes, ...)
{
	va_list ap;

	va_start(ap, mes);
	setMessage(mes, ap);
	va_end(ap);
}

LexerError::LexerError(const LexerError & img)
{
}

LexerError::~LexerError()
{
}

/*
 * Parser error exception class definition
 */

ParserError::ParserError()
{
	isJustMessage = true;
	stack = 0;
	tmpMessage = 0;
}

ParserError::ParserError(const char * mes, ...)
{
	va_list ap;

	isJustMessage = true;
	stack = 0;
	tmpMessage = 0;

	va_start(ap, mes);
	setMessage(mes, ap);
	va_end(ap);
}

ParserError::ParserError(const ParserError & img)
{
	lex = img.lex;
	expectedLex = img.expectedLex;
	tmpMessage = 0;

	if (img.stack != 0)
	{
		stack = new char[strlen(img.stack)+1];
		strcpy(stack, img.stack);
	}
	else
		stack = 0;
}

ParserError::~ParserError()
{
	if (stack != 0)
		delete [] stack;
	
	if (tmpMessage != 0)
		delete [] tmpMessage;
}

ParserError::ParserError(lexType expectedLex, Lex lex, char const * stack)
{
	isJustMessage = false;
	tmpMessage = 0;

	this->lex = lex;
	this->expectedLex = expectedLex;

	if (stack != 0)
	{
		this->stack = new char[strlen(stack)+1];
		strcpy(this->stack, stack);
	}
	else
		this->stack = 0;
}

const char * ParserError::getMessage()
{
	if (isJustMessage)
		return Error::getMessage();
	else
	{
		if (expectedLex == LEX_NULL)
			tmpMessage = makeMessageV("line %d: unexpected lexem\n\tstack: %s\n\tgot %s",
				lex.getLine(), stack, lex.getStr());
		else
			tmpMessage = makeMessageV("line %d: expected %s\n\tstack: %s\n\tgot %s",
				lex.getLine(), Lex::getLexTypeStr(expectedLex), stack, lex.getStr());

		return tmpMessage;
	}
}

/*
 * RPN error exception class definition
 */

RPNError::RPNError()
{
}

RPNError::RPNError(const char * mes, ...)
{
	va_list ap;

	va_start(ap, mes);
	setMessage(mes, ap);
	va_end(ap);
}

RPNError::RPNError(const RPNError & img)
{
}

RPNError::~RPNError()
{
}
