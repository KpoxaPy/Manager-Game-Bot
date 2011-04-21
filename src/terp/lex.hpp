#ifndef _LEX_HPP_
#define _LEX_HPP_

enum lexType {
	LEX_NULL,
	LEX_EOI,

	LEX_IDENTIFICATOR,
	LEX_NUMBER,
	LEX_STRING,
	LEX_FUNC,

	LEX_LPAREN,
	LEX_RPAREN,
	LEX_LBRACE,
	LEX_RBRACE,
	LEX_LBRACKET,
	LEX_RBRACKET,
	LEX_SEMICOLON,
	LEX_MUL,
	LEX_MODULO,
	LEX_PLUS,
	LEX_MINUS,
	LEX_LESS,
	LEX_GREATER,
	LEX_NOT,
	LEX_ASSIGN,
	LEX_DIV,
	LEX_COLON,
	LEX_COMMA,

	LEX_INC,
	LEX_DEC,
	LEX_LEQ,
	LEX_GEQ,
	LEX_NEQ,
	LEX_EQUAL,
	LEX_AND,
	LEX_OR,

	LEX_FOR,
	LEX_WHILE,
	LEX_IF,
	LEX_ELSE,
	LEX_GOTO,
	LEX_LABEL,
	LEX_TRUE,
	LEX_FALSE
};

enum funcDef {
	FUNC_NULL,
	FUNC_PRINT,
	FUNC_FUNC0,
	FUNC_FUNC1,
	FUNC_FUNC2,
	FUNC_FUNC3
};

class Lex {
	struct Delim1LexTypePair {
		char symbol;
		lexType type;
	};

	struct KeywordLexTypePair {
		const char * str;
		lexType type;
	};

	struct FuncLexTypePair {
		const char * str;
		funcDef ftype;
		int params;
	};

	struct Delim2LexTypePair {
		const char * str;
		lexType type;
	};

	static const char * lexTypeNames[];
	static const char * funcTypeNames[];
	static const Delim1LexTypePair tDelim1[];
	static const KeywordLexTypePair tKeywords[];
	static const Delim2LexTypePair tDelim2[];
	static const FuncLexTypePair tFuncs[];

	lexType type;
	char * value;
	int line;
	int funcParams;
	funcDef ftype;

public:
	Lex(const Lex &);
	Lex(lexType = LEX_NULL, int = -1, const char * = 0);
	~Lex();

	const Lex & operator=(const Lex &);
	bool operator==(lexType) const;
	bool operator==(const Lex &) const;
	bool operator!=(lexType) const;
	bool operator!=(const Lex &) const;

	operator const char * () const { return getStr(); }
	operator lexType () const { return type; }

	static const char * getLexTypeStr(lexType);
	static const char * getFuncTypeStr(funcDef);

	const char * getStr() const;
	const char * getFType() const;
	const char * getValue() const;
	int getFuncParams() const;
	int getLine() const;
	funcDef getFuncType() const;

	void print() const;

	static Lex * makeDelim1(int, char);
	static Lex * makeIdentificator(int, char *);
	static Lex * makeString(int, char *);
	static Lex * makeNumber(int, char *);
	static Lex * makeDelim2(int, char *);
};

class LexOrder {
	struct OrderElem {
		Lex * elem;
		OrderElem * next, * prev;
	};

	OrderElem * first;
	OrderElem * last;

public:
	LexOrder();
	~LexOrder();

	void put(Lex *);
	Lex * get();

	bool isEmpty() const;
};

#endif
