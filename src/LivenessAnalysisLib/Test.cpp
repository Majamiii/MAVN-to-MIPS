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


static InstructionStruct* makeInstruction(unsigned int pPos, InstructionType tType, Variable* dDst, Variable* sSrc1, Variable* sSrc2)
{
	InstructionStruct* p = new InstructionStruct();
	p->m_position = pPos;
	p->m_type = tType;

	if (dDst != nullptr)
		p->m_def.push_back(dDst);

	if (sSrc1 != nullptr)
		p->m_use.push_back(sSrc1);

	if (sSrc2 != nullptr)
		p->m_use.push_back(sSrc2);

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

	/*
	I_NO_TYPE = 0,
	I_ADD,
	I_ADDI,
	I_SUB,
	I_LA,
	I_LI,
	I_LW,
	I_SW,
	I_BLTZ,
	I_B,
	I_NOP,

	I_AND,
	I_JR,
	I_SLT,
	*/

	InstructionStruct *instr1, *instr2, *instr3, *instr4, *instr5, *instr6, *instr7, * instr8, * instr9, * instr10, * instr11, * instr12, * instr13, * instr14;
	instr1 = makeInstruction(1, I_NO_TYPE, a, nullptr, nullptr);
	instr2 = makeInstruction(2, I_ADD, b, a, nullptr);
	instr3 = makeInstruction(3, I_ADDI, c, b, nullptr);
	instr4 = makeInstruction(4, I_SUB, a, c, nullptr);
	instr5 = makeInstruction(5, I_LA, a, nullptr, nullptr);
	instr6 = makeInstruction(6, I_LI, b, nullptr, nullptr);

	instr7 = makeInstruction(7, I_LW, nullptr, a, nullptr);
	instr8 = makeInstruction(8, I_SW, nullptr, b, nullptr);
	instr9 = makeInstruction(9, I_BLTZ, nullptr, c, nullptr);
	instr10 = makeInstruction(10, I_B, nullptr, nullptr, nullptr);
	instr11 = makeInstruction(11, I_NOP, nullptr, nullptr, nullptr);
	instr12 = makeInstruction(12, I_AND, a, b, c);
	instr13 = makeInstruction(13, I_JR, nullptr, a, nullptr);
	instr14 = makeInstruction(14, I_SLT, b, a, c);

	// set successive and previous instruction for each one
	// set successors and predecessors

	instr1->m_succ.push_back(instr2);
	instr2->m_pred.push_back(instr1);

	instr2->m_succ.push_back(instr3);
	instr3->m_pred.push_back(instr2);

	instr3->m_succ.push_back(instr4);
	instr4->m_pred.push_back(instr3);

	instr4->m_succ.push_back(instr5);
	instr5->m_pred.push_back(instr4);

	// loop
	instr5->m_succ.push_back(instr2);
	instr2->m_pred.push_back(instr5);

	// continue after loop
	instr5->m_succ.push_back(instr6);
	instr6->m_pred.push_back(instr5);

	instr6->m_succ.push_back(instr7);
	instr7->m_pred.push_back(instr6);

	instr7->m_succ.push_back(instr8);
	instr8->m_pred.push_back(instr7);

	instr8->m_succ.push_back(instr9);
	instr9->m_pred.push_back(instr8);

	// BLTZ
	instr9->m_succ.push_back(instr10);
	instr10->m_pred.push_back(instr9);

	instr9->m_succ.push_back(instr11);
	instr11->m_pred.push_back(instr9);

	// B
	instr10->m_succ.push_back(instr14);
	instr14->m_pred.push_back(instr10);

	// fall-through
	instr11->m_succ.push_back(instr12);
	instr12->m_pred.push_back(instr11);

	instr12->m_succ.push_back(instr13);
	instr13->m_pred.push_back(instr12);

	// path from branch target
	instr14->m_succ.push_back(instr13);
	instr13->m_pred.push_back(instr14);

	// JR = exit (no successors)


	instructions.push_back(instr1);
	instructions.push_back(instr2);
	instructions.push_back(instr3);
	instructions.push_back(instr4);
	instructions.push_back(instr5);
	instructions.push_back(instr6);
	instructions.push_back(instr7);
	instructions.push_back(instr8);
	instructions.push_back(instr9);
	instructions.push_back(instr10);
	instructions.push_back(instr11);
	instructions.push_back(instr12);
	instructions.push_back(instr13);
	instructions.push_back(instr14);

	return;
}
