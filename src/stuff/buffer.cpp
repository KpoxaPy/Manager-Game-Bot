#include <string.h>
#include "exceptions.hpp"
#include "buffer.hpp"

void Buffer::expand(int d)
{
	int newsize = size + (d <= 0 ? bufWinSize : d);
	char * newbuf = new char[newsize + 1];

	for (int i = 0; i < size; i++)
		newbuf[i] = buf[i];

	delete [] buf;
	buf = newbuf;
	size = newsize;
}

Buffer::Buffer()
{
	size = bufWinSize;
	charCount = 0;
	buf = new char[size + 1];
	buf[0] = '\0';
}

Buffer::~Buffer()
{
	delete [] buf;
}

void Buffer::putChar(char c)
{
	if (size == charCount)
		expand(1);

	buf[charCount++] = c;
	buf[charCount] = '\0';
}

void Buffer::putStr(const char * str)
{
	int len = strlen(str);

	if (size - charCount < len)
		expand(len - (size - charCount));

	for (int i = 0; i < len; i++)
		buf[charCount++] = *str++;
	buf[charCount] = '\0';
}

char * Buffer::getStr()
{
	char * ret = new char[charCount + 1];

	for (int i = 0; i <= charCount; i++)
		ret[i] = buf[i];

	erase();
	return ret;
}

void Buffer::erase()
{
	charCount = 0;
}

const char * Buffer::lookStr() const
{
	return buf;
}

char & Buffer::operator[](int n)
{
	if (n >= charCount)
		throw Bug("Out of range access to Buffer (n: %d)", n);

	return buf[n];
}

const char & Buffer::operator[](int n) const
{
	if (n >= charCount)
		throw Bug("Out of range access to Buffer (n: %d)", n);

	return buf[n];
}

bool Buffer::isEmpty() const
{
	return (charCount == 0);
}
