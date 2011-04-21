#include <stdio.h>
#include "tables.hpp"

void IntTable::printElem(const int & i) const
{
	printf("%d", i);
}

void IntArrayTable::printElem(const IntTable & t) const
{
	t.printSmall();
}
