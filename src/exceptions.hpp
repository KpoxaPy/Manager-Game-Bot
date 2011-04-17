#ifndef _ERRORS_HPP_
#define _ERRORS_HPP_

#include <stdarg.h>
#include "lex.hpp"

class Error {
protected:
	char * message;

	static char * strdup(const char *);
	static char * makeMessage(const char *, va_list);
	static char * makeMessageV(const char *, ...);

public:
	Error();
	Error(const char * mes, ...);
	Error(const Error & img);
	virtual ~Error();

	virtual void setMessage(const char *, va_list);
	virtual const char * getMessage() const;
};

class UserError : public Error {
public:
	UserError();
	UserError(const char * mes, ...);
	UserError(const UserError & img);
	~UserError();
};

class ExternalError : public Error {
public:
	ExternalError();
	ExternalError(const char * mes, ...);
	ExternalError(const ExternalError & img);
	~ExternalError();
};

class Bug : public Error {
public:
	Bug();
	Bug(const char * mes, ...);
	Bug(const Bug & img);
	~Bug();
};

class LexerError : public UserError {
public:
	LexerError();
	LexerError(const char * mes, ...);
	LexerError(const LexerError & img);
	~LexerError();
};

class ParserError : public UserError {
	bool isJustMessage;
	Lex lex;
	lexType expectedLex;
	char * stack;
	char * tmpMessage;

public:
	ParserError();
	ParserError(const char * mes, ...);
	ParserError(const ParserError & img);
	ParserError(lexType, Lex, const char *);
	~ParserError();

	const char * getMessage();
};

class RPNError : public UserError {
public:
	RPNError();
	RPNError(const char * mes, ...);
	RPNError(const RPNError & img);
	~RPNError();
};

#endif
