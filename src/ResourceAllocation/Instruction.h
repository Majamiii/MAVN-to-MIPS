#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include "../LexicalAnalysis/Types.h"

/**
 * Use this function to print one instruction.
 * @param instr pointer to instruction
 */
void printInstruction(InstructionStruct* instr);

/**
 * Use this function to print all instructions in Instructions list.
 * @param instrs list of instructions
 */
void printInstructions(Instructions* instrs);


#endif
