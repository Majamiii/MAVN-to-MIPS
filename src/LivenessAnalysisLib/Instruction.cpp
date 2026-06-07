#include "Instruction.h"

#include "Variable.h"


static void printInstructionsPos(Instructions instrs)
{
	Instructions::iterator iter;

	for (iter = instrs.begin(); iter != instrs.end(); iter++)
	{
		printf("%d ", (*iter)->m_position);
	}
}


static void printInstruction(Instruction* instr)
{
	printf("%d\n\n", instr->m_position);

	printf("Type: %d\n\n", instr->m_type);

	printf("\nPRED\t");
	printInstructionsPos(instr->m_pred);
	printf("\n");

	printf("\nSUCC:\t");
	printInstructionsPos(instr->m_succ);
	printf("\n");

	printf("\nUSE:\t");
	printVariablesNames(instr->m_use);
	printf("\n");
	
	printf("DEF:\t");
	printVariablesNames(instr->m_def);
	printf("\n");
	
	printf("IN:\t");
	printVariablesNames(instr->m_in);
	printf("\n");

	printf("OUT:\t");
	printVariablesNames(instr->m_out);
	printf("\n");
}


void printInstructions(Instructions instrs)
{
	Instructions::iterator iter;

	for (iter = instrs.begin(); iter != instrs.end(); iter++)
	{
		Instruction* instr = *iter;

		printf("-----------------------------------------------\n");
		printInstruction(instr);
		printf("-----------------------------------------------\n");
	}
}
