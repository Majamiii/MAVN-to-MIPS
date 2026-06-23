#pragma once

#include "Constants.h"

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <stack>

/**
* Supported token types.
*/
enum TokenType
{
	T_NO_TYPE,

	T_ID,			// abcd...
	T_M_ID,			// m123...
	T_R_ID,			// r123...
	T_NUM,			// 123...
	T_WHITE_SPACE,

	// reserved words
	T_MEM,			// _mem
	T_REG,			// _reg
	T_FUNC,			// _func
	T_ADD,			// add
	T_ADDI,			// addi
	T_SUB,			// sub
	T_LA,			// la
	T_LI,			// li
	T_LW,			// lw
	T_SW,			// sw
	T_BLTZ,			// bltz
	T_B,			// b
	T_NOP,			// nop

	T_AND,			// and
	T_JR,			// jr
	T_SLT,			// slt

	// operators
	T_COMMA,		//,
	T_L_PARENT,		//(
	T_R_PARENT,		//)
	T_COL,			//:
	T_SEMI_COL,		//;

	// utility
	T_COMMENT,
	T_END_OF_FILE,
	T_ERROR,
};


/**
 * Instruction type.
 */
enum InstructionType
{
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
};

/**
 * Reg names.
 */
enum Regs
{
	no_assign = 0,
	t0,
	t1,
	t2,
	t3
};


// CODE FROM IR.H


/**
 * This struct represents one variable from program code.
 */
struct Variable
{
public:
	enum VariableType
	{
		MEM_VAR,
		REG_VAR,
		NO_TYPE
	};

	Variable(int value=0) : m_type(NO_TYPE), m_name(""), m_position(-1), m_assignment(no_assign), m_value(value) {}
	Variable(std::string name, int pos, int value=0) : m_type(NO_TYPE), m_name(name), m_position(pos), m_assignment(no_assign), m_value(value) {}

	VariableType m_type;
	std::string m_name;
	int m_position;
	Regs m_assignment;
	int m_value;
};


/**
 * This type represents list of variables from program code.
 */
typedef std::list<Variable*> Variables;

/**
 * This struct represents one instruction in program code.
 */
struct InstructionStruct {
public:
	InstructionStruct() : m_position(0), m_type(I_NO_TYPE), m_immediat(0), m_label("") {}
	InstructionStruct(int pos, InstructionType type, Variables& dst, Variables& src) :
		m_position(pos), m_type(type), m_dst(dst), m_src(src), m_immediat(0), m_label("") {
	}

	int m_position;
	InstructionType m_type;

	Variables m_dst;
	Variables m_src;
	Variables m_use;
	Variables m_def;
	Variables m_in;
	Variables m_out;
	std::list<InstructionStruct*> m_succ;
	std::list<InstructionStruct*> m_pred;

	int m_immediat;
	std::string m_label;
};

/**
 * This type represents list of instructions from program code.
 */
typedef std::list<InstructionStruct*> Instructions;


typedef struct
{
	Variables* variables;	// all variables
	char** values;			// matrix for interference graph
	int size;				// size of square matrix
} InterferenceGraph;
