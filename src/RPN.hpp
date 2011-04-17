#ifndef _RPN_HPP_
#define _RPN_HPP_

#include "tpl/tables.cpp"
#include "script.hpp"

class RPNElem;
class RPNConst;
class RPNStack;
class RPNScript;
class Script;

// ============ RPN Stack ============
class RPNStack {
	struct Item {
		RPNConst * elem;
		Item * next;
	};

	Item * first;
	Script * master;

public:
	RPNStack(Script * s) : first(0), master(s) {}
	~RPNStack();

	void push(RPNConst *);
	void push(RPNConst &);
	RPNConst * pop();

	void clean();

	void print() const;
	bool isEmpty() const;
	Script * getMaster();
};

// ============ RPN Script ============
class RPNScript : public Table<RPNElem> {
	friend class Script;

	int curCmd;
	RPNStack * stack;
	Script * master;

public:
	RPNScript(Script * s) : curCmd(0), master(s) { stack = new RPNStack(s); }
	~RPNScript() {if (stack != 0) delete stack; }

	void nextCmd();
	void goToCmd(int);
	void runStep();
	void run(bool debug = false);

	bool isEnd() const;

	void printElem(const RPNElem &) const;
	int getCount() const;
};

// ============ RPN Elem ============
class RPNElem {
public:
	virtual ~RPNElem() {}

	virtual void evaluate(RPNScript &, RPNStack &) const {}

	virtual void print(Script &) const {}
};

// ============ RPN Const ============
class RPNConst : public RPNElem {
protected:
	virtual RPNConst * clone() const = 0;

public:
	virtual void evaluate(RPNScript &, RPNStack &) const;

	virtual void printValue() const = 0;
};

// ============ RPN Int ============
class RPNInt : public RPNConst {
	int value;

protected:
	virtual RPNInt * clone() const { return new RPNInt(value); }

public:
	RPNInt(int a) : value(a) {}
	virtual ~RPNInt() {}

	int get() const { return value; }
	virtual void print(Script &) const;
	virtual void printValue() const;
};

// ============ RPN String ============
class RPNString : public RPNConst {
	char * str;

protected:
	virtual RPNString * clone() const { return new RPNString(str); }

public:
	RPNString(const char *);
	virtual ~RPNString();

	const char * get() const { return str; }
	virtual void print(Script &) const;
	virtual void printValue() const;
};

// ============ RPN Label ============
class RPNLabel : public RPNConst {
	int link;

protected:
	virtual RPNLabel * clone() const { return new RPNLabel(link); }

public:
	RPNLabel(int a = -1) : link(a) {}
	virtual ~RPNLabel() {}

	int get() const { return link; }
	void set(int l) { link = l; }
	virtual void print(Script &) const;
	virtual void printValue() const;
};

// ============ RPN Var ============
class RPNVar : public RPNConst {
	int link;
	int offset;

protected:
	virtual RPNVar * clone() const { return new RPNVar(link, offset); }

public:
	RPNVar(int a, int o = 0) : link(a), offset(o) {}
	virtual ~RPNVar() {}

	int get() const { return link; }
	int getOffset() const { return offset; }
	virtual void print(Script &) const;
	virtual void printValue() const;
};

// ============ RPN Go function ============
class RPNGo : public RPNElem {
public:
	virtual void evaluate(RPNScript &, RPNStack &) const;
	virtual void print(Script &) const;
};

// ============ RPN GoFalse function ============
class RPNGoFalse : public RPNElem {
public:
	virtual void evaluate(RPNScript &, RPNStack &) const;
	virtual void print(Script &) const;
};

// ============ RPN abstract function ============
class RPNFunction : public RPNElem {
protected:
	virtual RPNConst * evaluateFun(RPNStack &) const = 0;

public:
	virtual void evaluate(RPNScript &, RPNStack &) const;
};

// ============ RPN Print function ============
class RPNPrint : public RPNFunction {
	int pars;

protected:
	virtual RPNPrint * clone() const { return new RPNPrint(pars); }

public:
	RPNPrint(int p = 1) : pars(p) {}

	virtual RPNConst * evaluateFun(RPNStack &) const;
	virtual void print(Script &) const;
};

// ============ RPN Clear function ============
class RPNClear : public RPNFunction {
protected:
	virtual RPNClear * clone() const { return new RPNClear; }

public:
	virtual RPNConst * evaluateFun(RPNStack &) const;
	virtual void print(Script &) const;
};

// ============ RPN Var Value function ============
class RPNVarVal : public RPNFunction {
protected:
	virtual RPNVarVal * clone() const { return new RPNVarVal; }

public:
	virtual RPNConst * evaluateFun(RPNStack &) const;
	virtual void print(Script &) const;
};

// ============ RPN Array Value function ============
class RPNArrayVar : public RPNFunction {
protected:
	virtual RPNArrayVar * clone() const { return new RPNArrayVar; }

public:
	virtual RPNConst * evaluateFun(RPNStack &) const;
	virtual void print(Script &) const;
};

// ============ RPN Or function ============
class RPNOr : public RPNFunction {
protected:
	virtual RPNOr * clone() const { return new RPNOr; }

public:
	virtual RPNConst * evaluateFun(RPNStack &) const;
	virtual void print(Script &) const;
};

// ============ RPN And function ============
class RPNAnd : public RPNFunction {
protected:
	virtual RPNAnd * clone() const { return new RPNAnd; }

public:
	virtual RPNConst * evaluateFun(RPNStack &) const;
	virtual void print(Script &) const;
};

// ============ RPN Equal function ============
class RPNEqual : public RPNFunction {
protected:
	virtual RPNEqual * clone() const { return new RPNEqual; }

public:
	virtual RPNConst * evaluateFun(RPNStack &) const;
	virtual void print(Script &) const;
};

// ============ RPN Nonequal function ============
class RPNNonequal : public RPNFunction {
protected:
	virtual RPNNonequal * clone() const { return new RPNNonequal; }

public:
	virtual RPNConst * evaluateFun(RPNStack &) const;
	virtual void print(Script &) const;
};

// ============ RPN Less function ============
class RPNLess : public RPNFunction {
protected:
	virtual RPNLess * clone() const { return new RPNLess; }

public:
	virtual RPNConst * evaluateFun(RPNStack &) const;
	virtual void print(Script &) const;
};

// ============ RPN Leq function ============
class RPNLeq : public RPNFunction {
protected:
	virtual RPNLeq * clone() const { return new RPNLeq; }

public:
	virtual RPNConst * evaluateFun(RPNStack &) const;
	virtual void print(Script &) const;
};

// ============ RPN Greater function ============
class RPNGreater : public RPNFunction {
protected:
	virtual RPNGreater * clone() const { return new RPNGreater; }

public:
	virtual RPNConst * evaluateFun(RPNStack &) const;
	virtual void print(Script &) const;
};

// ============ RPN Geq function ============
class RPNGeq : public RPNFunction {
protected:
	virtual RPNGeq * clone() const { return new RPNGeq; }

public:
	virtual RPNConst * evaluateFun(RPNStack &) const;
	virtual void print(Script &) const;
};

// ============ RPN Plus function ============
class RPNPlus : public RPNFunction {
protected:
	virtual RPNPlus * clone() const { return new RPNPlus; }

public:
	virtual RPNConst * evaluateFun(RPNStack &) const;
	virtual void print(Script &) const;
};

// ============ RPN Minus function ============
class RPNMinus : public RPNFunction {
protected:
	virtual RPNMinus * clone() const { return new RPNMinus; }

public:
	virtual RPNConst * evaluateFun(RPNStack &) const;
	virtual void print(Script &) const;
};

// ============ RPN Mul function ============
class RPNMul : public RPNFunction {
protected:
	virtual RPNMul * clone() const { return new RPNMul; }

public:
	virtual RPNConst * evaluateFun(RPNStack &) const;
	virtual void print(Script &) const;
};

// ============ RPN Div function ============
class RPNDiv : public RPNFunction {
protected:
	virtual RPNDiv * clone() const { return new RPNDiv; }

public:
	virtual RPNConst * evaluateFun(RPNStack &) const;
	virtual void print(Script &) const;
};

// ============ RPN Modulo function ============
class RPNModulo : public RPNFunction {
protected:
	virtual RPNModulo * clone() const { return new RPNModulo; }

public:
	virtual RPNConst * evaluateFun(RPNStack &) const;
	virtual void print(Script &) const;
};

// ============ RPN Not function ============
class RPNNot : public RPNFunction {
protected:
	virtual RPNNot * clone() const { return new RPNNot; }

public:
	virtual RPNConst * evaluateFun(RPNStack &) const;
	virtual void print(Script &) const;
};

// ============ RPN Prefix Inc function ============
class RPNPrefixInc : public RPNFunction {
protected:
	virtual RPNPrefixInc * clone() const { return new RPNPrefixInc; }

public:
	virtual RPNConst * evaluateFun(RPNStack &) const;
	virtual void print(Script &) const;
};

// ============ RPN Prefix Dec function ============
class RPNPrefixDec : public RPNFunction {
protected:
	virtual RPNPrefixDec * clone() const { return new RPNPrefixDec; }

public:
	virtual RPNConst * evaluateFun(RPNStack &) const;
	virtual void print(Script &) const;
};

// ============ RPN Un Minus function ============
class RPNUnMinus : public RPNFunction {
protected:
	virtual RPNUnMinus * clone() const { return new RPNUnMinus; }

public:
	virtual RPNConst * evaluateFun(RPNStack &) const;
	virtual void print(Script &) const;
};

// ============ RPN Infix Inc function ============
class RPNInfixInc : public RPNFunction {
protected:
	virtual RPNInfixInc * clone() const { return new RPNInfixInc; }

public:
	virtual RPNConst * evaluateFun(RPNStack &) const;
	virtual void print(Script &) const;
};

// ============ RPN Infix Dec function ============
class RPNInfixDec : public RPNFunction {
protected:
	virtual RPNInfixDec * clone() const { return new RPNInfixDec; }

public:
	virtual RPNConst * evaluateFun(RPNStack &) const;
	virtual void print(Script &) const;
};

// ============ RPN Assign function ============
class RPNAssign : public RPNFunction {
protected:
	virtual RPNAssign * clone() const { return new RPNAssign; }

public:
	virtual RPNConst * evaluateFun(RPNStack &) const;
	virtual void print(Script &) const;
};

#endif
