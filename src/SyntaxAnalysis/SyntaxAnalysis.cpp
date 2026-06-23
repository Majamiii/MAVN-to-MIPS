
#include <iostream>
#include <iomanip>

#include "SyntaxAnalysis.h"

using namespace std;


SyntaxAnalysis::SyntaxAnalysis(LexicalAnalysis& lex)
	: lexicalAnalysis(lex), errorFound(false), tokenIterator(lexicalAnalysis.getTokenList().begin())
{
}


bool SyntaxAnalysis::Do()
{
	tokenIterator = lexicalAnalysis.getTokenList().begin();
	currentToken = getNextToken();
	
	//TO DO: Call function for the starting non-terminal symbol

	Q();

	// pozivanje funkcije koja radi kontrolu toka programa
	buildControlFlow();
	return !errorFound;
}


void SyntaxAnalysis::printSyntaxError(Token token)
{
	cout << "Syntax error! Token: " << token.getValue() << " unexpected" << endl;
}


void SyntaxAnalysis::printTokenInfo(Token token)
{
	cout << setw(20) << left << token.tokenTypeToString(token.getType());
	cout << setw(25) << right << token.getValue() << endl;
}


void SyntaxAnalysis::eat(TokenType t)
{
	if (errorFound) return;
	if (errorFound == false)
	{
		if (currentToken.getType() == t)
		{
			printTokenInfo(currentToken);
			currentToken = getNextToken();
		}
		else
		{
			printSyntaxError(currentToken);
			errorFound = true;
		}
	}
}


Token SyntaxAnalysis::getNextToken()
{
	if (tokenIterator == lexicalAnalysis.getTokenList().end()) {
		Token eofToken;
		eofToken.makeEofToken();
		return eofToken;
	}
	return *tokenIterator++;
}


void SyntaxAnalysis::Q()
{
	if (errorFound) return;
	// Q → S ; L 
	S();
	eat(T_SEMI_COL);
	L();
}


void SyntaxAnalysis::S()
{
	if (errorFound) return;

	// S → _mem mid num
	// S → _reg rid
	// S → _func id
	// S → id : E
	// S → E

	if (currentToken.getType() == T_MEM) {
		eat(T_MEM);
		if (!errorFound && currentToken.getType() == T_M_ID) {
			std::string memName = currentToken.getValue();
			eat(T_M_ID);
			if (!errorFound && currentToken.getType() == T_NUM) {
				int memValue = std::stoi(currentToken.getValue());
				eat(T_NUM);
				if (!errorFound) {
					Variable* v = findOrAddVariable(memName, Variable::MEM_VAR);
					v->m_value = memValue;
				}
			}
		}
	}
	else if (currentToken.getType() == T_REG) {
		eat(T_REG);
		if (!errorFound && currentToken.getType() == T_R_ID) {
			eat(T_R_ID);
		}
	}
	else if (currentToken.getType() == T_FUNC) {
		eat(T_FUNC);
		if (!errorFound && currentToken.getType() == T_ID) {
			eat(T_ID);
		}
	}
	else if (currentToken.getType() == T_ID) {
		std::string labelName = currentToken.getValue();
		eat(T_ID);
		if (!errorFound && currentToken.getType() == T_COL) {
			eat(T_COL);
			E();
			// dodaj lalbel poslednjoj dodatoj instrukciji
			if (!m_instructions.empty()) {
				m_instructions.back()->m_label = labelName;
			}
		}
	}
	else {
		E();
	}
}


void SyntaxAnalysis::L()
{
	if (errorFound) return;
	
	// L → EOF
	// L → Q

	if (currentToken.getType() == T_END_OF_FILE) {
		eat(T_END_OF_FILE);
	}
	else {
		Q();
	}
}


void SyntaxAnalysis::E()
{
	if (errorFound) return;
	/*
	   E → add rid, rid, rid
	   E → addi rid, rid, num
	   E → sub rid, rid, rid
	   E → la rid, mid
	   E → lw rid, num(rid)
	   E → li rid, num 
	   E → sw rid, num(rid) 
	   E → b id 
	   E → bltz rid, id 
	   E → nop
	   E → and rid, rid, rid
	   E → slt rid, rid, rid
	   E → jr rid
	*/
	else if (currentToken.getType() == T_ADD) {
		eat(T_ADD);
		// rd (dst)
		std::string rd = currentToken.getValue();
		eat(T_R_ID);
		eat(T_COMMA);
		// rs, rt (src)
		std::string rs = currentToken.getValue();
		eat(T_R_ID);
		eat(T_COMMA);
		std::string rt = currentToken.getValue();
		eat(T_R_ID);

		if (!errorFound) {
			Variables dst, src;
			dst.push_back(findOrAddVariable(rd, Variable::REG_VAR));
			src.push_back(findOrAddVariable(rs, Variable::REG_VAR));
			src.push_back(findOrAddVariable(rt, Variable::REG_VAR));
			buildInstruction(I_ADD, dst, src);
		}
	}
	else if (currentToken.getType() == T_ADDI) {
		eat(T_ADDI);
		std::string rd = currentToken.getValue();
		eat(T_R_ID);
		eat(T_COMMA);
		std::string rs = currentToken.getValue();
		eat(T_R_ID);
		eat(T_COMMA);
		std::string imm = currentToken.getValue();
		eat(T_NUM);

		if (!errorFound) {
			Variables dst, src;
			dst.push_back(findOrAddVariable(rd, Variable::REG_VAR));
			src.push_back(findOrAddVariable(rs, Variable::REG_VAR));
			InstructionStruct* instr = new InstructionStruct(m_position++, I_ADDI, dst, src);
			instr->m_def = dst;
			instr->m_use = src;
			instr->m_immediat = std::stoi(imm);
			m_instructions.push_back(instr);
		}
	}
	else if (currentToken.getType() == T_SUB) {
		eat(T_SUB);
		std::string rd = currentToken.getValue();
		eat(T_R_ID);
		eat(T_COMMA);
		std::string rs = currentToken.getValue();
		eat(T_R_ID);
		eat(T_COMMA);
		std::string rt = currentToken.getValue();
		eat(T_R_ID);

		if (!errorFound) {
			Variables dst, src;
			dst.push_back(findOrAddVariable(rd, Variable::REG_VAR));
			src.push_back(findOrAddVariable(rs, Variable::REG_VAR));
			src.push_back(findOrAddVariable(rt, Variable::REG_VAR));
			buildInstruction(I_SUB, dst, src);
		}
	}
	else if (currentToken.getType() == T_LA) {
		eat(T_LA);
		std::string rd = currentToken.getValue();
		eat(T_R_ID);
		eat(T_COMMA);
		std::string mid = currentToken.getValue();
		eat(T_M_ID);

		if (!errorFound) {
			Variables dst, src;
			dst.push_back(findOrAddVariable(rd, Variable::REG_VAR));
			src.push_back(findOrAddVariable(mid, Variable::MEM_VAR));
			buildInstruction(I_LA, dst, src);
		}
	}
	else if (currentToken.getType() == T_LW) {
		eat(T_LW);
		std::string rd = currentToken.getValue();
		eat(T_R_ID);
		eat(T_COMMA);
		eat(T_NUM);
		eat(T_L_PARENT);
		std::string rs = currentToken.getValue();
		eat(T_R_ID);
		eat(T_R_PARENT);

		if (!errorFound) {
			Variables dst, src;
			dst.push_back(findOrAddVariable(rd, Variable::REG_VAR));
			src.push_back(findOrAddVariable(rs, Variable::REG_VAR));
			buildInstruction(I_LW, dst, src);
		}
	}
	else if (currentToken.getType() == T_LI) {
		eat(T_LI);
		std::string rd = currentToken.getValue();
		eat(T_R_ID);
		eat(T_COMMA);
		std::string imm = currentToken.getValue();
		eat(T_NUM);

		if (!errorFound) {
			Variables dst, src;
			dst.push_back(findOrAddVariable(rd, Variable::REG_VAR));
			InstructionStruct* instr = new InstructionStruct(m_position++, I_LI, dst, src);
			instr->m_def = dst;
			instr->m_immediat = std::stoi(imm);
			m_instructions.push_back(instr);
		}
	}
	else if (currentToken.getType() == T_SW) {
		eat(T_SW);
		std::string rs = currentToken.getValue();
		eat(T_R_ID);
		eat(T_COMMA);
		eat(T_NUM);
		eat(T_L_PARENT);
		std::string rb = currentToken.getValue();
		eat(T_R_ID);
		eat(T_R_PARENT);

		if (!errorFound) {
			Variables dst, src;
			// sw nema dst u smislu definicije, samo koristi registre
			src.push_back(findOrAddVariable(rs, Variable::REG_VAR));
			src.push_back(findOrAddVariable(rb, Variable::REG_VAR));
			buildInstruction(I_SW, dst, src);
		}
	}
	else if (currentToken.getType() == T_BLTZ) {
		eat(T_BLTZ);
		std::string rs = currentToken.getValue();
		eat(T_R_ID);
		eat(T_COMMA);
		std::string label = currentToken.getValue();
		eat(T_ID);

		if (!errorFound) {
			Variables dst, src;
			src.push_back(findOrAddVariable(rs, Variable::REG_VAR));
			InstructionStruct* instr = new InstructionStruct(m_position++, I_BLTZ, dst, src);
			instr->m_use = src;
			instr->m_label = label;
			m_instructions.push_back(instr);
		}
	}
	else if (currentToken.getType() == T_B) {
			eat(T_B);
			std::string label = currentToken.getValue();
			eat(T_ID);

			if (!errorFound) {
				Variables dst, src;
				InstructionStruct* instr = new InstructionStruct(m_position++, I_B, dst, src);
				instr->m_label = label;
				m_instructions.push_back(instr);
			}
	}
	else if (currentToken.getType() == T_NOP) {
		eat(T_NOP);
		if (!errorFound) {
			Variables dst, src;
			buildInstruction(I_NOP, dst, src);
		}
	}
	else if (currentToken.getType() == T_AND) {
		eat(T_AND);
		std::string rd = currentToken.getValue();
		eat(T_R_ID);
		eat(T_COMMA);
		std::string rs = currentToken.getValue();
		eat(T_R_ID);
		eat(T_COMMA);
		std::string rt = currentToken.getValue();
		eat(T_R_ID);

		if (!errorFound) {
			Variables dst, src;
			dst.push_back(findOrAddVariable(rd, Variable::REG_VAR));
			src.push_back(findOrAddVariable(rs, Variable::REG_VAR));
			src.push_back(findOrAddVariable(rt, Variable::REG_VAR));
			buildInstruction(I_AND, dst, src);
		}
	}
	else if (currentToken.getType() == T_JR) {
		eat(T_JR);
		std::string rs = currentToken.getValue();
		eat(T_R_ID);

		if (!errorFound) {
			Variables dst, src;
			src.push_back(findOrAddVariable(rs, Variable::REG_VAR));
			buildInstruction(I_JR, dst, src);
		}
	}
	else if (currentToken.getType() == T_SLT) {
		eat(T_SLT);
		std::string rd = currentToken.getValue();
		eat(T_R_ID);
		eat(T_COMMA);
		std::string rs = currentToken.getValue();
		eat(T_R_ID);
		eat(T_COMMA);
		std::string rt = currentToken.getValue();
		eat(T_R_ID);

		if (!errorFound) {
			Variables dst, src;
			dst.push_back(findOrAddVariable(rd, Variable::REG_VAR));
			src.push_back(findOrAddVariable(rs, Variable::REG_VAR));
			src.push_back(findOrAddVariable(rt, Variable::REG_VAR));
			buildInstruction(I_SLT, dst, src);
		}
	}
	/*
	
		------ DODATE INSTRUKCIJE ---------

		aritmeticko-logicka: and rd, rs, rt
		Put the logical AND of register rs 
		and rt into register rd

		jump register
		jr rs
		Unconditionally jump to the 
		instruction whose address is in 
		register rs

		set less than
		slt rd, rs, rt
		Set register rd to 1 if register rs is 
		less than rt, and to 0 otherwise.
	*/
	else {
		printSyntaxError(currentToken);
		errorFound = true;
	}
}


Variable* SyntaxAnalysis::findOrAddVariable(const std::string& name, Variable::VariableType type) {
	for (Variable* v : m_variables) {
		if (v->m_name == name) {
			return v;
		}
	}
	Variable* v = new Variable(name, m_variables.size());
	v->m_type = type;
	m_variables.push_back(v);
	return v;
}

void SyntaxAnalysis::buildInstruction(InstructionType type, Variables& dst, Variables& src) {
	InstructionStruct* instr = new InstructionStruct(m_position++, type, dst, src);
	// def = dst, use = src
	instr->m_def = dst;
	instr->m_use = src;
	m_instructions.push_back(instr);
}

void SyntaxAnalysis::buildControlFlow() {
	std::map<std::string, InstructionStruct*> labelMap;
	for (auto instr : m_instructions) {
		if (!instr->m_label.empty() &&
			instr->m_type != I_BLTZ && instr->m_type != I_B) {
			labelMap[instr->m_label] = instr;
		}
	}

	auto it = m_instructions.begin();
	while (it != m_instructions.end()) {
		auto next = std::next(it);
		InstructionStruct* cur = *it;

		if (cur->m_type == I_B) {
			if (labelMap.count(cur->m_label)) {
				InstructionStruct* target = labelMap[cur->m_label];
				cur->m_succ.push_back(target);
				target->m_pred.push_back(cur);
			}
		}
		else if (cur->m_type == I_BLTZ) {
			if (next != m_instructions.end()) {
				cur->m_succ.push_back(*next);
				(*next)->m_pred.push_back(cur);
			}
			if (labelMap.count(cur->m_label)) {
				InstructionStruct* target = labelMap[cur->m_label];
				cur->m_succ.push_back(target);
				target->m_pred.push_back(cur);
			}
		}
		else {
			if (next != m_instructions.end()) {
				cur->m_succ.push_back(*next);
				(*next)->m_pred.push_back(cur);
			}
		}

		it = next;
	}
}


Instructions& SyntaxAnalysis::getInstructions() { return m_instructions; }
Variables& SyntaxAnalysis::getVariables() { return m_variables; }