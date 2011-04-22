#include <stdio.h>
#include <string.h>
#include "stuff/exceptions.hpp"
#include "stuff/program.hpp"
#include "terp/parser.hpp"

int main(int argc, char * argv[], char * envp[])
{
	Program & prog = Program::get();

	try
	{
		prog.init(argc, argv, envp);

		//Parser parser(new FileInput("testfile"));
		//Parser parser;
		//Script * s;

		//s = parser.checkSyntax();

		//printf("\n Result RPN:\n");
		//s->getRPN().printPlain();
		//printf("\n\n");
		//s->getRPN().run();
		//printf("\n\n");
		//s->print();
	}
	catch(ParserError & err)
	{
		const char * mes = err.getMessage();

		if (mes != 0)
			printf("\nParserError: %s\n",  mes);

		Program::endWork(EXIT_FAILURE);
	}
	catch(LexerError & err)
	{
		const char * mes = err.getMessage();

		if (mes != 0)
			printf("\nLexerError: %s\n",  mes);

		Program::endWork(EXIT_FAILURE);
	}
	catch(UserError & err)
	{
		const char * mes = err.getMessage();

		if (mes != 0)
			printf("%s: %s\n", prog.getRunName(), mes);
		prog.printUsage();

		Program::endWork(EXIT_FAILURE);
	}
	catch(Error & err)
	{
		const char * mes = err.getMessage();

		if (mes != 0)
			printf("%s: %s\n", prog.getRunName(), mes);
		else
			printf("%s: catched exception without description\n", prog.getRunName());

		Program::endWork(EXIT_FAILURE, false);
	}
	catch(...)
	{
		printf("%s: catched undefined exception\n", prog.getRunName());

		Program::endWork(EXIT_FAILURE, false);
	}

	Program::endWork(EXIT_SUCCESS);
}
