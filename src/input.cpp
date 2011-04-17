#include "exceptions.hpp"
#include "input.hpp"
#include "errno.h"
#include "string.h"

Input::Input()
{
	strNum = 1;
	//printf("Line #%d!\n", strNum);
}

Input::~Input()
{
}

int Input::getLine() const
{
	//printf("Called getLine() -> %d\n", strNum);
	return strNum;
}

StrInput::StrInput(const char * str)
{
	this->str = str;
	pos = 0;
}

StrInput::~StrInput()
{
}

int StrInput::getChar()
{
	if (str[pos] == '\n')
	{
		strNum++;
		//printf("Line #%d!\n", strNum);
	}
	if (str[pos] != '\0')
	{
		return str[pos++];
	}
	else
		return EOF;
}

FileInput::FileInput(FILE * f)
{
	isStreamOpennedInternally = false;
	stream = f;
}

FileInput::FileInput(const char * str)
{
	if (str == 0)
		throw UserError("Bad string instead of file name in FileInput constructor");

	if ((stream = fopen(str, "r")) == NULL)
		throw UserError("Unable to open file \"%s\" with error \"%s\"", str, strerror(errno));

	isStreamOpennedInternally = true;
}

FileInput::~FileInput()
{
	if (!isStreamOpennedInternally)
		return;

	fclose(stream);
}

int FileInput::getChar()
{
	int c;
	c = fgetc(stream);
	if (c == '\n')
	{
		strNum++;
		//printf("Line #%d!\n", strNum);
	}

	return c;
}
