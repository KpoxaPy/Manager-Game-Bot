#ifndef _INPUT_HPP_
#define _INPUT_HPP_

#include <stdio.h>

class Input {
protected:
	int strNum;

public:
	Input();
	virtual ~Input();

	virtual int getChar() = 0;
	int getLine() const;
};


class StrInput: public Input {
	int pos;
	const char * str;

public:
	StrInput(const char * = 0);
	~StrInput();

	int getChar();
};


class FileInput: public Input {
	FILE * stream;
	bool isStreamOpennedInternally;

public:
	FileInput(FILE * = stdin);
	FileInput(const char *);
	~FileInput();

	int getChar();
};

#endif
