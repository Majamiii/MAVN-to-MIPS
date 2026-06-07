#include "LivenessAnalysis.h"
#include "Instruction.h"
#include "Variable.h"

// Ne menjati postavku zadatka! Popuniti kodom mesta oznacena sa TO DO

void livenessAnalysis(Instructions instructions)
{
	Instructions::reverse_iterator rit;
	for (rit = instructions.rbegin(); rit != instructions.rend(); rit++) {
		{
			Variables& out = (*rit)->out;
			Variables& in = (*rit)->in;
			Variables out_new;
			Variables in_new;

			// 1. TO DO:
			// out_new = U succIn;
			for (Instruction* succ : (*rit)->succ) {
				for (Variable* var : succ->in) {
					// Dodajemo samo ako vec nije u out_new (simuliramo skup)
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

			for (Variable* var : (*rit)->use) {
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
				for (Variable* defVar : (*rit)->def) {
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

			out = out_new;
			in = in_new;
	
		}
	}
			

}
