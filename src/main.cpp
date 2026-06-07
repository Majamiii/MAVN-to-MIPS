#include "./LexicalAnalysis/LexicalAnalysis.h"
#include "./SyntaxAnalysis/SyntaxAnalysis.h"

#include "./LexicalAnalysis/Types.h"
#include "./LivenessAnalysis/Instruction.h"
#include "./LivenessAnalysis/Test.h"
#include "./LivenessAnalysis/Variable.h"
#include "./LivenessAnalysis/LivenessAnalysis.h"

#include <iostream>
#include <exception>

using namespace std;

void main()
{
	try
	{
		std::string fileName = ".\\..\\examples\\commands.mavn";
		bool retVal = false;

		LexicalAnalysis lex;
		
		if (!lex.readInputFile(fileName))
			throw runtime_error("\nException! Failed to open input file!\n");
		
		lex.initialize();

		retVal = lex.Do();

		if (retVal)
		{
			cout << "Lexical analysis finished successfully!" << endl;
			lex.printTokens();
		}
		else
		{
			lex.printLexError();
			throw runtime_error("\nException! Lexical analysis failed!\n");
		}

		SyntaxAnalysis syntax(lex);

		cout << endl << "---------------------------------------------" << endl;
		cout<< "Starting syntax analysis..." << endl<<endl;

		if (syntax.Do())
		{
			cout << endl <<  "Syntax analysis finished successfully!" << endl;
		}
		else
		{
			cout << endl << "Syntax analysis failed!" << endl;
		}

		cout << endl << "---------------------------------------------" << endl;
		cout << "Starting liveliness analysis..." << endl << endl;

		Instructions& instructions = syntax.getInstructions();
		Variables& variables = syntax.getVariables();

		cout << "Variables list:" << endl;
		printVariables(variables);
		cout << endl << endl << endl;
		cout << "Instruction list (flow control graph):" << endl;
		printInstructions(instructions);
		cout << endl << endl;

		livenessAnalysis(instructions);
		cout << endl;
		cout << "Instruction list after LIVENESS ANALYSIS:" << endl;
		printInstructions(instructions);
		cout << endl << endl;

		cout << "Liveliness analysis finished successfully!" << endl;

	}
	catch (runtime_error e)
	{
		cout << e.what() << endl;
	}
}