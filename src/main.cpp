#include <stdio.h>
#include <string.h>
#include "stuff/exceptions.hpp"
//#include "terp/buffer.hpp"
#include "terp/parser.hpp"
//#include "terp/ident.hpp"
//#include "terp/script.hpp"
//#include "terp/RPN.hpp"
//#include "terp/tables.hpp"

int main()
{
	try
	{
		//=================================================
		// Testing of Script class
		//Script scr;
		//scr.print();
		//Lex l1(LEX_IDENTIFICATOR, 1, "line1var");
		//Lex l2(LEX_IDENTIFICATOR, 2, "line2array");
		//Lex l3(LEX_IDENTIFICATOR, 3, "line3var");
		//Lex l4(LEX_IDENTIFICATOR, 4, "line4var");

		//scr.declareVar(l1);
		//scr.declareArray(l2);
		//scr.declareVar(l3);
		//scr.declareVar(l4);

		//scr.print();

		//=================================================
		// Testing of IdentTable class
		//Ident * x = new Ident("x", 7);
		//Ident * abc = new Ident("abc", 2);
		//Ident * def = new Ident("def", 10);
		//IdentTable t;

		//printf("Putting 'x' variable at %d\n", t.put(x));
		//printf("Putting 'abc' variable at %d\n", t.put(abc));
		//printf("Putting 'def' variable at %d\n", t.put(def));
		//t.findByName("def")->print();
		//t.findByName("abc")->print();
		//t.findByName("x")->print();

		//t.print();

		//=================================================
		// Testing of Parser class
		Parser parser;
		Script * s;

		s = parser.checkSyntax();

		//printf("\n === Syntax OK! === \n");
		printf("\n Result RPN:\n");
		s->getRPN().printPlain();
		printf("\n\n");
		s->getRPN().run();
		printf("\n\n");
		s->print();

		//=================================================
		// Testing of Buffer class
		//Buffer buf;

		//for (int i = 0; i < 100; i++)
			//buf.putChar(' ');

		//=================================================
		// Testing of RPN system
		//Script scr;
		//Lex l(LEX_IDENTIFICATOR, 1, "var");
		//Lex k(LEX_IDENTIFICATOR, 7, "arr");
		//RPNScript & rpn = scr.getRPN();
		//char * str = new char[2];
		//strcpy(str, " ");

		//int p = scr.declareVar(l);
		//int q = scr.declareArray(k);

		//rpn.put(new RPNVar(q));
		//rpn.put(new RPNInt(10));
		//rpn.put(new RPNArrayVar);
		//rpn.put(new RPNInt(3));
		//rpn.put(new RPNAssign);
		//rpn.put(new RPNUnMinus);
		//rpn.put(new RPNClear);
		//rpn.put(new RPNVar(p));
		//rpn.put(new RPNVar(p));
		//rpn.put(new RPNVarVal);
		//rpn.put(new RPNInt(3));
		//rpn.put(new RPNPlus);
		//rpn.put(new RPNAssign);
		//rpn.put(new RPNClear);
		//rpn.put(new RPNInt(0));
		//rpn.put(new RPNLabel(0));
		//rpn.put(new RPNGoFalse);
		//rpn.put(new RPNString(str));
		//rpn.put(new RPNInt(20));
		//rpn.put(new RPNPrint(3));
		//rpn.put(new RPNPrint);
		//rpn.put(new RPNGo());

		//rpn.printPlain();
		//rpn.run();
		//scr.print();
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
