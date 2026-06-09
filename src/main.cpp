#include "./LexicalAnalysis/LexicalAnalysis.h"
#include "./SyntaxAnalysis/SyntaxAnalysis.h"

#include "./LexicalAnalysis/Types.h"
#include "./LivenessAnalysis/Instruction.h"
#include "./LivenessAnalysis/Test.h"
#include "./LivenessAnalysis/Variable.h"
#include "./LivenessAnalysis/LivenessAnalysis.h"

#include "./ResourceAllocation/ResourceAllocation.h"
#include "./ResourceAllocation/libResourceAllocation.h"
#include "./ResourceAllocation/InterferenceGraph.h"
#include "./ResourceAllocation/Simplification.h"
#include "./ResourceAllocation/libLivnessAnalysis.h"

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


		// -----------------------------------------------------------------------------


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


		// -----------------------------------------------------------------------------


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



		// -----------------------------------------------------------------------------


		cout << endl << "---------------------------------------------" << endl;
		cout << "Starting resource allocation..." << endl << endl;


		InterferenceGraph* ig;
		ig = doInterferenceGraph(&instructions);
		stack<Variable*>* simplificationStack;


		printInterferenceGraph(ig);

		printVariables(*(ig->variables));
		cout << "\nInstruction list:\n";
		printInstructions(instructions);
		cout << "*******************************************************************\n\n\n";

		// 2) simplify
		simplificationStack = doSimplification(ig, __REG_NUMBER__);

		// 3) spill
		if (simplificationStack == NULL)
		{
			cout << "Spill detected!\n";
		}
		else
		{
			// 4) select
			if (doResourceAllocation(simplificationStack, ig) == true)
			{
				if (checkResourceAllocation(ig) == true)
				{

					// 5) remove unnecessary move operation
					// Instructions* noMoveInstr = removeMove(&instructions);

					// 6) final print
					cout << "Resource allocation finished successfully!" << endl;
					printVariables(*(ig->variables));
					cout << "\nInstruction list:\n";
					printInstructions(instructions);
					cout << "*******************************************************************\n\n\n";
				}
				else
				{
					cout << "Allocation failed!\n";
				}
			}
			else
			{
				cout << "Actual spill!\n";
			}
		}


		// free resources
		if (ig != NULL && ig->values != NULL)
		{
			freeInterferenceGraph(ig);
		}


		cout << endl << endl;



		// -----------------------------------------------------------------------------


	}
	catch (runtime_error e)
	{
		cout << e.what() << endl;
	}
}