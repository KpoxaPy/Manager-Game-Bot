#include <string.h>
#include "../stuff/exceptions.hpp"
#include "RPN.hpp"

void RPNScript::nextCmd()
{
	curCmd++;
}

void RPNScript::goToCmd(int n)
{
	if (n < 0 || n > count)
		throw RPNError("trying to jump to nonexist command");

	curCmd = n;
}

void RPNScript::runStep()
{
	if (isEnd())
		throw RPNError("trying to evaluate nonexist command");

	(*this)[curCmd].evaluate(*this, *stack);
}

void RPNScript::run(bool debug)
{
	curCmd = 0;
	if (debug) printf("RPN running started from %d/%d command\n", curCmd, count - 1);
	while (!isEnd())
	{
		if (debug) printf("RPN running %d/%d command ", curCmd, count - 1);
		if (debug) (*this)[curCmd].print(*master);
		if (debug) printf("\nRPN stack on begin:\n");
		if (debug) stack->print();
		runStep();
		if (debug) printf("\n");
	}
}

bool RPNScript::isEnd() const
{
	return (curCmd >= count);
}

void RPNScript::printElem(const RPNElem & elem) const
{
	elem.print(*master);
}

int RPNScript::getCount() const
{
	return count;
}

RPNStack::~RPNStack()
{
	Item * tmp;

	while (first != 0)
	{
		tmp = first->next;
		if (first->elem != 0)
			delete first->elem;
		delete first;
		first = tmp;
	}
}

void RPNStack::push(RPNConst * t)
{
	Item * tmp = new Item;
	tmp->elem = t;
	tmp->next = first;
	first = tmp;
}

void RPNStack::push(RPNConst & t)
{
	push(&t);
}

RPNConst * RPNStack::pop()
{
	Item * tmp = first;
	RPNConst * elem = 0;

	if (first != 0)
	{
		elem = tmp->elem;
		first = tmp->next;
		delete tmp;
	}
	else
		throw RPNError("Popping form empty operands stack");

	return elem;
}

void RPNStack::clean()
{
	Item * tmp;

	while (first != 0)
	{
		tmp = first->next;
		if (first->elem != 0)
			delete first->elem;
		delete first;
		first = tmp;
	}
}

void RPNStack::print() const
{
	Item * tmp = first;
	int i = 0;

	while (tmp != 0)
	{
		if (tmp->elem != 0)
		{
			printf("[%d]: ", i);
			tmp->elem->print(*master);
			printf("\n");
			i++;
		}
		tmp = tmp->next;
	}
}

bool RPNStack::isEmpty() const
{
	return (first == 0);
}

Script * RPNStack::getMaster()
{
	return master;
}

void RPNConst::evaluate(RPNScript & scr, RPNStack & st) const
{
	st.push(clone());
	scr.nextCmd();
}

void RPNGo::evaluate(RPNScript & scr, RPNStack & st) const
{
	RPNElem * op = st.pop();
	RPNLabel * label = dynamic_cast<RPNLabel *>(op);

	if (label == 0)
		throw RPNError("Operand is not label");
	scr.goToCmd(label->get());
	delete op;
}

void RPNGoFalse::evaluate(RPNScript & scr, RPNStack & st) const
{
	RPNConst * opl = st.pop();
	RPNConst * ope = st.pop();
	RPNLabel * label = dynamic_cast<RPNLabel *>(opl);
	RPNInt * exp = dynamic_cast<RPNInt *>(ope);

	if (exp == 0)
		throw RPNError("Operand 1 is not integer");
	if (label == 0)
		throw RPNError("Operand 2 is not label");


	if (exp->get() == 0)
		scr.goToCmd(label->get());
	else
		scr.nextCmd();

	delete opl;
	delete ope;
}

void RPNFunction::evaluate(RPNScript & scr, RPNStack & st) const
{
	RPNConst * res = evaluateFun(st);
	if (res != 0)
		st.push(res);
	scr.nextCmd();
}

RPNConst * RPNPrint::evaluateFun(RPNStack & st) const
{
	RPNConst ** ops = new RPNConst*[pars];
	int i;

	for (i = 0; i < pars; i++)
		ops[i] = st.pop();

	for (i = pars - 1; i >= 0; i--)
		ops[i]->printValue();

	for (i = 0; i < pars; i++)
		delete ops[i];

	delete [] ops;

	return 0;
}

RPNConst * RPNClear::evaluateFun(RPNStack & st) const
{
	if (!st.isEmpty())
	{
		RPNConst * op = st.pop();
		delete op;
	}
	return 0;
}

RPNConst * RPNVarVal::evaluateFun(RPNStack & st) const
{
	RPNConst * op = st.pop();
	RPNVar * var = dynamic_cast<RPNVar *>(op);
	int link;

	if (var == 0)
		throw RPNError("Operand 1 is not variable");

	if (var->get() == -1)
		throw RPNError("Variable is not declared");

	if ((link = st.getMaster()->getIdents()[var->get()].getLink()) == -1)
		throw RPNError("Variable is not defined");

	RPNInt * ret;
	if (st.getMaster()->getIdents()[var->get()].isVar())
		ret = new RPNInt(st.getMaster()->getIntegers()[link]);
	else
		ret = new RPNInt(st.getMaster()->getArrays()[link][var->getOffset()]);

	delete op;

	return ret;
}

RPNConst * RPNArrayVar::evaluateFun(RPNStack & st) const
{
	RPNConst * op2 = st.pop();
	RPNConst * op1 = st.pop();
	RPNVar * var = dynamic_cast<RPNVar *>(op1);
	RPNInt * offset = dynamic_cast<RPNInt *>(op2);

	if (var == 0)
		throw RPNError("Operand 1 is not variable");

	if (offset == 0)
		throw RPNError("Operand 2 is not integer");

	if (var->get() == -1)
		throw RPNError("Array is not declared");

	RPNVar * ret = new RPNVar(var->get(), offset->get());

	delete op1;
	delete op2;

	return ret;
}

RPNConst * RPNOr::evaluateFun(RPNStack & st) const
{
	RPNConst * op2 = st.pop();
	RPNConst * op1 = st.pop();
	int ret = 0;

	RPNInt * lop = dynamic_cast<RPNInt *>(op1);
	if (lop == 0)
		throw RPNError("Operand 1 is not integer");

	RPNInt * rop = dynamic_cast<RPNInt *>(op2);
	if (rop == 0)
		throw RPNError("Operand 2 is not integer");

	if (lop->get() != 0)
		ret = 1;
	if (rop->get() != 0)
		ret = 1;

	delete op1;
	delete op2;

	return new RPNInt(ret);
}

RPNConst * RPNAnd::evaluateFun(RPNStack & st) const
{
	RPNConst * op2 = st.pop();
	RPNConst * op1 = st.pop();
	int ret = 1;

	RPNInt * lop = dynamic_cast<RPNInt *>(op1);
	if (lop == 0)
		throw RPNError("Operand 1 is not integer");

	RPNInt * rop = dynamic_cast<RPNInt *>(op2);
	if (rop == 0)
		throw RPNError("Operand 2 is not integer");

	if (lop->get() == 0)
		ret = 0;
	if (rop->get() == 0)
		ret = 0;

	delete op1;
	delete op2;

	return new RPNInt(ret);
}

RPNConst * RPNEqual::evaluateFun(RPNStack & st) const
{
	RPNConst * op2 = st.pop();
	RPNConst * op1 = st.pop();

	RPNInt * lop = dynamic_cast<RPNInt *>(op1);
	if (lop == 0)
		throw RPNError("Operand 1 is not integer");

	RPNInt * rop = dynamic_cast<RPNInt *>(op2);
	if (rop == 0)
		throw RPNError("Operand 2 is not integer");

	int ret = (lop->get() == rop->get());

	delete op1;
	delete op2;

	return new RPNInt(ret);
}

RPNConst * RPNNonequal::evaluateFun(RPNStack & st) const
{
	RPNConst * op2 = st.pop();
	RPNConst * op1 = st.pop();

	RPNInt * lop = dynamic_cast<RPNInt *>(op1);
	if (lop == 0)
		throw RPNError("Operand 1 is not integer");

	RPNInt * rop = dynamic_cast<RPNInt *>(op2);
	if (rop == 0)
		throw RPNError("Operand 2 is not integer");

	int ret = (lop->get() != rop->get());

	delete op1;
	delete op2;

	return new RPNInt(ret);
}

RPNConst * RPNLess::evaluateFun(RPNStack & st) const
{
	RPNConst * op2 = st.pop();
	RPNConst * op1 = st.pop();

	RPNInt * lop = dynamic_cast<RPNInt *>(op1);
	if (lop == 0)
		throw RPNError("Operand 1 is not integer");

	RPNInt * rop = dynamic_cast<RPNInt *>(op2);
	if (rop == 0)
		throw RPNError("Operand 2 is not integer");

	int ret = (lop->get() < rop->get());

	delete op1;
	delete op2;

	return new RPNInt(ret);
}

RPNConst * RPNLeq::evaluateFun(RPNStack & st) const
{
	RPNConst * op2 = st.pop();
	RPNConst * op1 = st.pop();

	RPNInt * lop = dynamic_cast<RPNInt *>(op1);
	if (lop == 0)
		throw RPNError("Operand 1 is not integer");

	RPNInt * rop = dynamic_cast<RPNInt *>(op2);
	if (rop == 0)
		throw RPNError("Operand 2 is not integer");

	int ret = (lop->get() <= rop->get());

	delete op1;
	delete op2;

	return new RPNInt(ret);
}

RPNConst * RPNGreater::evaluateFun(RPNStack & st) const
{
	RPNConst * op2 = st.pop();
	RPNConst * op1 = st.pop();

	RPNInt * lop = dynamic_cast<RPNInt *>(op1);
	if (lop == 0)
		throw RPNError("Operand 1 is not integer");

	RPNInt * rop = dynamic_cast<RPNInt *>(op2);
	if (rop == 0)
		throw RPNError("Operand 2 is not integer");

	int ret = (lop->get() > rop->get());

	delete op1;
	delete op2;

	return new RPNInt(ret);
}

RPNConst * RPNGeq::evaluateFun(RPNStack & st) const
{
	RPNConst * op2 = st.pop();
	RPNConst * op1 = st.pop();

	RPNInt * lop = dynamic_cast<RPNInt *>(op1);
	if (lop == 0)
		throw RPNError("Operand 1 is not integer");

	RPNInt * rop = dynamic_cast<RPNInt *>(op2);
	if (rop == 0)
		throw RPNError("Operand 2 is not integer");

	int ret = (lop->get() >= rop->get());

	delete op1;
	delete op2;

	return new RPNInt(ret);
}

RPNConst * RPNPlus::evaluateFun(RPNStack & st) const
{
	RPNConst * op2 = st.pop();
	RPNConst * op1 = st.pop();

	RPNInt * lop = dynamic_cast<RPNInt *>(op1);
	if (lop == 0)
		throw RPNError("Operand 1 is not integer");

	RPNInt * rop = dynamic_cast<RPNInt *>(op2);
	if (rop == 0)
		throw RPNError("Operand 2 is not integer");

	int ret = (lop->get() + rop->get());

	delete op1;
	delete op2;

	return new RPNInt(ret);
}

RPNConst * RPNMinus::evaluateFun(RPNStack & st) const
{
	RPNConst * op2 = st.pop();
	RPNConst * op1 = st.pop();

	RPNInt * lop = dynamic_cast<RPNInt *>(op1);
	if (lop == 0)
		throw RPNError("Operand 1 is not integer");

	RPNInt * rop = dynamic_cast<RPNInt *>(op2);
	if (rop == 0)
		throw RPNError("Operand 2 is not integer");

	int ret = (lop->get() - rop->get());

	delete op1;
	delete op2;

	return new RPNInt(ret);
}

RPNConst * RPNMul::evaluateFun(RPNStack & st) const
{
	RPNConst * op2 = st.pop();
	RPNConst * op1 = st.pop();

	RPNInt * lop = dynamic_cast<RPNInt *>(op1);
	if (lop == 0)
		throw RPNError("Operand 1 is not integer");

	RPNInt * rop = dynamic_cast<RPNInt *>(op2);
	if (rop == 0)
		throw RPNError("Operand 2 is not integer");

	int ret = (lop->get() * rop->get());

	delete op1;
	delete op2;

	return new RPNInt(ret);
}

RPNConst * RPNDiv::evaluateFun(RPNStack & st) const
{
	RPNConst * op2 = st.pop();
	RPNConst * op1 = st.pop();

	RPNInt * lop = dynamic_cast<RPNInt *>(op1);
	if (lop == 0)
		throw RPNError("Operand 1 is not integer");

	RPNInt * rop = dynamic_cast<RPNInt *>(op2);
	if (rop == 0)
		throw RPNError("Operand 2 is not integer");

	int ret = (lop->get() / rop->get());

	delete op1;
	delete op2;

	return new RPNInt(ret);
}

RPNConst * RPNModulo::evaluateFun(RPNStack & st) const
{
	RPNConst * op2 = st.pop();
	RPNConst * op1 = st.pop();

	RPNInt * lop = dynamic_cast<RPNInt *>(op1);
	if (lop == 0)
		throw RPNError("Operand 1 is not integer");

	RPNInt * rop = dynamic_cast<RPNInt *>(op2);
	if (rop == 0)
		throw RPNError("Operand 2 is not integer");

	int ret = (lop->get() % rop->get());

	delete op1;
	delete op2;

	return new RPNInt(ret);
}

RPNConst * RPNNot::evaluateFun(RPNStack & st) const
{
	RPNConst * op = st.pop();
	int ret;

	RPNInt * lop = dynamic_cast<RPNInt *>(op);
	if (lop == 0)
		throw RPNError("Operand is not integer");

	if (lop->get() != 0)
		ret = 0;
	else
		ret = 1;

	delete op;

	return new RPNInt(ret);
}

RPNConst * RPNUnMinus::evaluateFun(RPNStack & st) const
{
	RPNConst * op = st.pop();
	int ret;

	RPNInt * lop = dynamic_cast<RPNInt *>(op);
	if (lop == 0)
		throw RPNError("Operand is not integer");

	ret = -lop->get();

	delete op;

	return new RPNInt(ret);
}

RPNConst * RPNPrefixInc::evaluateFun(RPNStack & st) const
{
	RPNConst * op = st.pop();
	RPNVar * var = dynamic_cast<RPNVar *>(op);
	int link;

	if (var == 0)
		throw RPNError("Operand 1 is not variable");

	if (var->get() == -1)
		throw RPNError("Variable is not declared");

	if ((link = st.getMaster()->getIdents()[var->get()].getLink()) == -1)
		throw RPNError("Variable is not defined");


	RPNInt * ret;
	if (st.getMaster()->getIdents()[var->get()].isVar())
		ret = new RPNInt(++st.getMaster()->getIntegers()[link]);
	else
		ret = new RPNInt(++st.getMaster()->getArrays()[link][var->getOffset()]);

	delete op;

	return ret;
}

RPNConst * RPNPrefixDec::evaluateFun(RPNStack & st) const
{
	RPNConst * op = st.pop();
	RPNVar * var = dynamic_cast<RPNVar *>(op);
	int link;

	if (var == 0)
		throw RPNError("Operand 1 is not variable");

	if (var->get() == -1)
		throw RPNError("Variable is not declared");

	if ((link = st.getMaster()->getIdents()[var->get()].getLink()) == -1)
		throw RPNError("Variable is not defined");

	RPNInt * ret;
	if (st.getMaster()->getIdents()[var->get()].isVar())
		ret = new RPNInt(--st.getMaster()->getIntegers()[link]);
	else
		ret = new RPNInt(--st.getMaster()->getArrays()[link][var->getOffset()]);

	delete op;

	return ret;
}

RPNConst * RPNInfixInc::evaluateFun(RPNStack & st) const
{
	RPNConst * op = st.pop();
	RPNVar * var = dynamic_cast<RPNVar *>(op);
	int link;

	if (var == 0)
		throw RPNError("Operand 1 is not variable");

	if (var->get() == -1)
		throw RPNError("Variable is not declared");

	if ((link = st.getMaster()->getIdents()[var->get()].getLink()) == -1)
		throw RPNError("Variable is not defined");

	RPNInt * ret;
	if (st.getMaster()->getIdents()[var->get()].isVar())
		ret = new RPNInt(st.getMaster()->getIntegers()[link]++);
	else
		ret = new RPNInt(st.getMaster()->getArrays()[link][var->getOffset()]++);

	delete op;

	return ret;
}

RPNConst * RPNInfixDec::evaluateFun(RPNStack & st) const
{
	RPNConst * op = st.pop();
	RPNVar * var = dynamic_cast<RPNVar *>(op);
	int link;

	if (var == 0)
		throw RPNError("Operand 1 is not variable");

	if (var->get() == -1)
		throw RPNError("Variable is not declared");

	if ((link = st.getMaster()->getIdents()[var->get()].getLink()) == -1)
		throw RPNError("Variable is not defined");

	RPNInt * ret;
	if (st.getMaster()->getIdents()[var->get()].isVar())
		ret = new RPNInt(st.getMaster()->getIntegers()[link]--);
	else
		ret = new RPNInt(st.getMaster()->getArrays()[link][var->getOffset()]--);

	delete op;

	return ret;
}

RPNConst * RPNAssign::evaluateFun(RPNStack & st) const
{
	RPNConst * op2 = st.pop();
	RPNConst * op1 = st.pop();
	RPNVar * var = dynamic_cast<RPNVar *>(op1);
	RPNInt * exp = dynamic_cast<RPNInt *>(op2);
	int link;

	if (var == 0)
		throw RPNError("Operand 1 is not variable");

	if (exp == 0)
		throw RPNError("Operand 2 is not integer");

	if (var->get() == -1)
		throw RPNError("Variable is not declared");

	if ((link = st.getMaster()->getIdents()[var->get()].getLink()) == -1)
	{
		if (st.getMaster()->getIdents()[var->get()].isVar())
		{
			link = st.getMaster()->getIntegers().put(new int);
			st.getMaster()->getIdents()[var->get()].setLink(link);
		}
		else
		{
			link = st.getMaster()->getArrays().put(new IntTable);
			st.getMaster()->getIdents()[var->get()].setLink(link);
		}
	}

	RPNInt * ret;
	if (st.getMaster()->getIdents()[var->get()].isVar())
		ret = new RPNInt(st.getMaster()->getIntegers()[link] = exp->get());
	else
		ret = new RPNInt(st.getMaster()->getArrays()[link][var->getOffset()] = exp->get());

	delete op1;
	delete op2;

	return ret;
}

void RPNInt::print(Script & scr) const
{
	printf("RPNInt %d", value);
}

void RPNInt::printValue() const
{
	printf("%d", value);
}

RPNString::RPNString(const char * s)
{
	if (s != 0)
	{
		str = new char[strlen(s)+1];
		memmove(str, s, strlen(s) + 1);
	}
	else
		str = 0;
}

RPNString::~RPNString()
{
	if (str != 0)
		delete [] str;
}

void RPNString::print(Script & scr) const
{
	printf("RPNString %s", str);
}

void RPNString::printValue() const
{
	printf("%s", str);
}

void RPNLabel::print(Script & scr) const
{
	printf("RPNLabel %d", link);
}

void RPNLabel::printValue() const
{
	printf("%d", link);
}

void RPNVar::print(Script & scr) const
{
	printf("RPNVar %d[%d]: ", link, offset);
	scr.idents[link].printraw();
}

void RPNVar::printValue() const
{
	printf("%d", link);
}

void RPNGo::print(Script & scr) const
{
	printf("RPNGo");
}

void RPNGoFalse::print(Script & scr) const
{
	printf("RPNGoFalse");
}

void RPNPrint::print(Script & scr) const
{
	printf("RPNPrint with %d pars", pars);
}

void RPNClear::print(Script & scr) const
{
	printf("RPNClear");
}

void RPNVarVal::print(Script & scr) const
{
	printf("RPNVarVal");
}

void RPNArrayVar::print(Script & scr) const
{
	printf("RPNArrayVar");
}

void RPNOr::print(Script & scr) const
{
	printf("RPNOr");
}

void RPNAnd::print(Script & scr) const
{
	printf("RPNAnd");
}

void RPNEqual::print(Script & scr) const
{
	printf("RPNEqual");
}

void RPNNonequal::print(Script & scr) const
{
	printf("RPNNonequal");
}

void RPNLess::print(Script & scr) const
{
	printf("RPNLess");
}

void RPNLeq::print(Script & scr) const
{
	printf("RPNLeq");
}

void RPNGreater::print(Script & scr) const
{
	printf("RPNGreater");
}

void RPNGeq::print(Script & scr) const
{
	printf("RPNGeq");
}

void RPNPlus::print(Script & scr) const
{
	printf("RPNPlus");
}

void RPNMinus::print(Script & scr) const
{
	printf("RPNMinus");
}

void RPNMul::print(Script & scr) const
{
	printf("RPNMul");
}

void RPNDiv::print(Script & scr) const
{
	printf("RPNDiv");
}

void RPNModulo::print(Script & scr) const
{
	printf("RPNModulo");
}

void RPNNot::print(Script & scr) const
{
	printf("RPNNot");
}

void RPNPrefixInc::print(Script & scr) const
{
	printf("RPNPrefixInc");
}

void RPNPrefixDec::print(Script & scr) const
{
	printf("RPNPrefixDec");
}

void RPNUnMinus::print(Script & scr) const
{
	printf("RPNUnMinus");
}

void RPNInfixInc::print(Script & scr) const
{
	printf("RPNInfixInc");
}

void RPNInfixDec::print(Script & scr) const
{
	printf("RPNInfixDec");
}

void RPNAssign::print(Script & scr) const
{
	printf("RPNAssign");
}
