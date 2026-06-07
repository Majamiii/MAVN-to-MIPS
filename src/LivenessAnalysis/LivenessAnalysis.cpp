#include "LivenessAnalysis.h"
#include "Instruction.h"
#include "Variable.h"
#include "../LexicalAnalysis/Types.h"

void livenessAnalysis(Instructions instructions)
{
	bool changed;
	do {
		changed = false;
		Instructions::reverse_iterator rit;
		for (rit = instructions.rbegin(); rit != instructions.rend(); rit++) {
			{
				Variables& out = (*rit)->m_out;
				Variables& in = (*rit)->m_in;
				Variables out_new;
				Variables in_new;

				// 1. TO DO:
				// out_new = U succIn;
				for (Instruction* succ : (*rit)->m_succ) {
					for (Variable* var : succ->m_in) {
						bool found = false;
						for (Variable* existing : out_new) {
							if (existing == var) {
								found = true;
								break;
							}
						}
						if (!found) {
							out_new.push_back(var);
						}
					}
				}


				// 2. TO DO:
				// newIn = use U (out / def)

				for (Variable* var : (*rit)->m_use) {
					bool found = false;
					for (Variable* existing : in_new) {
						if (existing == var) {
							found = true;
							break;
						}
					}
					if (!found) {
						in_new.push_back(var);
					}
				}

				for (Variable* var : out_new) {
					bool inDef = false;
					for (Variable* defVar : (*rit)->m_def) {
						if (defVar == var) {
							inDef = true;
							break;
						}
					}
					if (!inDef) {
						bool found = false;
						for (Variable* existing : in_new) {
							if (existing == var) {
								found = true;
								break;
							}
						}
						if (!found) {
							in_new.push_back(var);
						}
					}
				}

				if (out != out_new || in != in_new)
					changed = true;

				out = out_new;
				in = in_new;

			}
		}
	} while (changed);
			

}
