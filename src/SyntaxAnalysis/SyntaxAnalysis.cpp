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
			eat(T_M_ID);
			if (!errorFound && currentToken.getType() == T_NUM) {
				eat(T_NUM);
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
		eat(T_ID);
		if (!errorFound && currentToken.getType() == T_COL) {
			eat(T_COL);
			E();
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
	*/
	if (currentToken.getType() == T_ADD) {
		eat(T_ADD);
		eat(T_R_ID);
		eat(T_COMMA);
		eat(T_R_ID);
		eat(T_COMMA);
		eat(T_R_ID);
	}
	else if (currentToken.getType() == T_ADDI) {
		eat(T_ADDI);
		eat(T_R_ID);
		eat(T_COMMA);
		eat(T_R_ID);
		eat(T_COMMA);
		eat(T_NUM);
	}
	else if (currentToken.getType() == T_SUB) {
		eat(T_SUB);
		eat(T_R_ID);
		eat(T_COMMA);
		eat(T_R_ID);
		eat(T_COMMA);
		eat(T_R_ID);
	}
	else if (currentToken.getType() == T_LA) {
		eat(T_LA);
		eat(T_R_ID);
		eat(T_COMMA);
		eat(T_M_ID);
	}
	else if (currentToken.getType() == T_LW) {
		eat(T_LW);
		eat(T_R_ID);
		eat(T_COMMA);
		eat(T_NUM);
		eat(T_L_PARENT);
		eat(T_R_ID);
		eat(T_R_PARENT);
	}
	else if (currentToken.getType() == T_LI) {
		eat(T_LI);
		eat(T_R_ID);
		eat(T_COMMA);
		eat(T_NUM);
	}
	else if (currentToken.getType() == T_SW) {
		eat(T_SW);
		eat(T_R_ID);
		eat(T_COMMA);
		eat(T_NUM);
		eat(T_L_PARENT);
		eat(T_R_ID);
		eat(T_R_PARENT);
	}
	else if (currentToken.getType() == T_B) {
		eat(T_B);
		eat(T_ID);
	}
	else if (currentToken.getType() == T_BLTZ) {
		eat(T_BLTZ);
		eat(T_R_ID);
		eat(T_COMMA);
		eat(T_ID);
	}
	else if (currentToken.getType() == T_NOP) {
		eat(T_NOP);
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
	else if (currentToken.getType() == T_AND) {
		eat(T_AND);
		eat(T_R_ID);
		eat(T_COMMA);
		eat(T_R_ID);
		eat(T_COMMA);
		eat(T_R_ID);
	}
	else if (currentToken.getType() == T_JR) {
		eat(T_JR);
		eat(T_R_ID);
	}
	else if (currentToken.getType() == T_SLT) {
		eat(T_SLT);
		eat(T_R_ID);
		eat(T_COMMA);
		eat(T_R_ID);
		eat(T_COMMA);
		eat(T_R_ID);
	}
	else {
		printSyntaxError(currentToken);
		errorFound = true;
	}
}