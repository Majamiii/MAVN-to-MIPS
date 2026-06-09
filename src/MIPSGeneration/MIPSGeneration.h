#ifndef MIPS_GENERATION_H
#define MIPS_GENERATION_H

#include "../LexicalAnalysis/Types.h"

using namespace std;

Variables filterMemVariables(Variables& variables);

bool generateMIPS(Variables& memVariables, string funcName, Instructions& instructions, string fileName);

string regToString(Regs r);

#endif