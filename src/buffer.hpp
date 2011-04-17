#ifndef _BUFFER_HPP_
#define _BUFFER_HPP_

class Buffer {
	static const int bufWinSize = 16;

	char * buf;
	int size;
	int charCount;

	void expand(int d = 0);

public:
	Buffer();
	~Buffer();

	void erase();

	void putChar(char);
	void putStr(const char *);

	/* Also will erase the buffer */
	char * getStr();
	const char * lookStr() const;
	char & operator[](int);
	const char & operator[](int) const;

	bool isEmpty() const;
};

#endif
