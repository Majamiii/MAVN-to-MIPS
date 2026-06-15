#include "MIPSGeneration.h"

#include <iostream>
#include <fstream>

using namespace std;

void printFile(const std::string& filename);

Variables filterMemVariables(Variables& variables) {
    Variables memVariables;
    for (Variable* var : variables) {
        if (var->m_type == Variable::MEM_VAR) {
            memVariables.push_back(var);
        }
    }
    return memVariables;
}

/*
Програм на мавн програмском језику:
_mem m1 6;
_mem m2 5;
_reg r1;
_reg r2;
_reg r3;
_reg r4;
_reg r5;
_func main;
 la  r4,m1;
 lw  r1, 0(r4);
 la  r5, m2;
 lw  r2, 0(r5);
 add  r3, r1, r2;

Програм преведен на MIPS 32bit асемблерски језик:
.globl main

.data
m1:    .word 6
m2:    .word 5

.text
main:
 la  $t0, m1
 lw  $t1, 0($t0)
 la  $t0, m2
 lw  $t2, 0($t0)
 add  $t0, $t1, $t2
*/
bool generateMIPS(Variables& memVariables, string funcName, Instructions& instructions, string fileName) {
    ofstream outFile(fileName);

    if (outFile.is_open()) {

		outFile << ".globl " << funcName << "\n\n";
        
		outFile << ".data\n";
		for (Variable* var : memVariables) {
			outFile << var->m_name << ": .word "<<var->m_value<<"\n";
		}

		outFile << "\n.text\n";
        outFile << funcName << ":\n";
        for (InstructionStruct* instr : instructions) {

            if (!instr->m_label.empty() &&
                instr->m_type != I_BLTZ &&
                instr->m_type != I_B) {
                outFile << instr->m_label << ":\n";
            }

            string dst, src1, src2;
            if (instr->m_dst.empty()) {
				dst = "";
            }
            else {
				dst = regToString(instr->m_dst.front()->m_assignment);
            }

            if (instr->m_src.empty()) {
                src1 = "";
            }
            else {
                src1 = regToString(instr->m_src.front()->m_assignment);
            }

			if (instr->m_src.size() > 1) {
				src2 = regToString((*next(instr->m_src.begin()))->m_assignment);
			}
			else {
				src2 = "";
			}


            switch (instr->m_type) {
            case I_ADD:
                outFile << "\tadd\t" << dst << ", " << src1 << ", " << src2 << "\n";
                break;
            case I_ADDI:
                outFile << "\taddi\t" << dst << ", " << src1 << ", " << instr->m_immediat << "\n";
                break;
            case I_SUB:
                outFile << "\tsub\t" << dst << ", " << src1 << ", " << src2 << "\n";
                break;
            case I_AND:
                outFile << "\tand\t" << dst << ", " << src1 << ", " << src2 << "\n";
                break;
            case I_SLT:
                outFile << "\tslt\t" << dst << ", " << src1 << ", " << src2 << "\n";
                break;
            case I_LA:
                outFile << "\tla\t" << dst << ", " << instr->m_src.front()->m_name << "\n";
                break;
            case I_LW:
                outFile << "\tlw\t" << dst << ", 0(" << src1 << ")\n";
                break;
            case I_SW:
                outFile << "\tsw\t" << src1 << ", 0(" << src2 << ")\n";
                break;
            case I_LI:
                outFile << "\tli\t" << dst << ", " << instr->m_immediat << "\n";
                break;
            case I_B:
                outFile << "\tb\t" << instr->m_label << "\n";
                break;
            case I_BLTZ:
                outFile << "\tbltz\t" << src1 << ", " << instr->m_label << "\n";
                break;
            case I_NOP:
                outFile << "\tnop\n";
                break;
            case I_JR:
                outFile << "\tjr\t" << src1 << "\n";
                break;
            default:
                break;
            }

        }

        outFile.close();
    }
    else {
        std::cerr << "Error: Could not open the file for writing.\n";
        return false;
    }

    return true;

}

string regToString(Regs r) {
    switch (r) {
    case t0: return "$t0";
    case t1: return "$t1";
    case t2: return "$t2";
    case t3: return "$t3";
    default: return "$??";
    }
}
