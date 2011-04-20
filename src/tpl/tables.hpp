#ifndef _IDENTTABLE_HPP_
#define _IDENTTABLE_HPP_

template <class T>
class Table {
	friend struct Sheet;

protected:
	static const int tableSize = 16;
	static const int sheetSize = 16;

	struct Sheet {
		T ** data;
		Sheet * next;

		Sheet();
		~Sheet();

		void print(const Table<T> &) const;
	};

	Sheet ** table;
	int count;

	T ** getLink(int) const;

	virtual void printElem(const T &) const;

public:
	Table();
	virtual ~Table();

	T & operator[](int);
	int put(T *);
	void deleteLast();

	int getCount();

	void print() const;
	void printSmall() const;
	void printPlain() const;
};

#endif
