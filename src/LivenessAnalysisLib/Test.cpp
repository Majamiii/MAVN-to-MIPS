#include "Test.h"
#include "Instruction.h"


static Instructions instructions;

static Variables variables;


Instructions& getInstructions()
{
	return instructions;
}


Variables& getVariables()
{
	return variables;
}


static Instruction* makeInstruction(unsigned int pPos, InstructionType tType, Variable* dDst, Variable* sSrc1, Variable* sSrc2)
{
	Instruction* p = new Instruction();
	p->pos = pPos;
	p->type = tType;

	if (dDst != nullptr)
		p->def.push_back(dDst);

	if (sSrc1 != nullptr)
		p->use.push_back(sSrc1);

	if (sSrc2 != nullptr)
		p->use.push_back(sSrc2);

	return p;
}


void makeExample()
{
	Variable *a, *b, *c;
	a = new Variable("a", 0);
	b = new Variable("b", 1);
	c = new Variable("c", 2);

	variables.push_back(a);
	variables.push_back(b);
	variables.push_back(c);

	Instruction *instr1, *instr2, *instr3, *instr4, *instr5, *instr6;
	instr1 = makeInstruction(1, equ, a, nullptr, nullptr);
	instr2 = makeInstruction(2, add, b, a, nullptr);
	instr3 = makeInstruction(3, sub, c, b, nullptr);
	instr4 = makeInstruction(4, mul, a, b, nullptr);
	instr5 = makeInstruction(5, cond, nullptr, a, nullptr);
	instr6 = makeInstruction(6, ret, nullptr, c, nullptr);

	// set successive and previous instruction for each one
	instr1->succ.push_back(instr2);
	instr2->pred.push_back(instr1);
	instr2->pred.push_back(instr5);
	instr2->succ.push_back(instr3);
	instr3->pred.push_back(instr2);
	instr3->succ.push_back(instr4);
	instr4->pred.push_back(instr3);
	instr4->succ.push_back(instr5);
	instr5->pred.push_back(instr4);
	instr5->succ.push_back(instr2);
	instr5->succ.push_back(instr6);
	instr6->pred.push_back(instr5);

	instructions.push_back(instr1);
	instructions.push_back(instr2);
	instructions.push_back(instr3);
	instructions.push_back(instr4);
	instructions.push_back(instr5);
	instructions.push_back(instr6);

	return;
}
