#include <stdio.h>
#include <string.h>
#include "stuff/exceptions.hpp"
#include "terp/parser.hpp"

int main()
{
	try
	{
		Parser parser;
		Script * s;

		s = parser.checkSyntax();

		printf("\n Result RPN:\n");
		s->getRPN().printPlain();
		printf("\n\n");
		s->getRPN().run();
		printf("\n\n");
		s->print();
	}
	catch(ParserError & err)
	{
		const char * mes = err.getMessage();

		if (mes != 0)
			printf("\nParserError: %s\n",  mes);
	}
	catch(LexerError & err)
	{
		const char * mes = err.getMessage();

		if (mes != 0)
			printf("\nLexerError: %s\n",  mes);
	}
	catch(UserError & err)
	{
		const char * mes = err.getMessage();

		if (mes != 0)
			printf("\nUserError: %s\n",  mes);
	}
	catch(Error & err)
	{
		const char * mes = err.getMessage();

		if (mes != 0)
			printf("\nError: %s\n",  mes);
	}
	catch(...)
	{
		printf("Catched undefined exception\n");
	}

	return 0;
}
