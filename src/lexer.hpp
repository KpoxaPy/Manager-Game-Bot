#ifndef _LEXER_HPP_
#define _LEXER_HPP_

#include "buffer.hpp"
#include "input.hpp"
#include "lex.hpp"


class Lexer {
	enum State {H, D2, D1_2, N, I, C1_2S, C1L, CML, CMLE, Q, Q_BS};

	LexOrder lexes;
	Input * raw;
	State CS;
	Buffer buf;

	void step(char);

	void state_H(char);
	void state_D2(char);
	void state_D1_2(char);
	void state_N(char);
	void state_I(char);
	void state_C1_2S(char);
	void state_C1L(char);
	void state_CML(char);
	void state_CMLE(char);
	void state_Q(char);
	void state_Q_BS(char);

public:
	Lexer(Input * = 0);
	~Lexer();

	Lex * getLex();
};

#endif
