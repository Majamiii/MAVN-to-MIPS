#include "Instruction.h"

#include "Variable.h"


static void printInstructionsPos(Instructions instrs)
{
	Instructions::iterator iter;

	for (iter = instrs.begin(); iter != instrs.end(); iter++)
	{
		printf("%d ", (*iter)->pos);
	}
}


static void printInstruction(Instruction* instr)
{
	printf("%d\n\n", instr->pos);

	printf("Type: %d\n\n", instr->type);

	printf("\nPRED\t");
	printInstructionsPos(instr->pred);
	printf("\n");

	printf("\nSUCC:\t");
	printInstructionsPos(instr->succ);
	printf("\n");

	printf("\nUSE:\t");
	printVariablesNames(instr->use);
	printf("\n");
	
	printf("DEF:\t");
	printVariablesNames(instr->def);
	printf("\n");
	
	printf("IN:\t");
	printVariablesNames(instr->in);
	printf("\n");

	printf("OUT:\t");
	printVariablesNames(instr->out);
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
