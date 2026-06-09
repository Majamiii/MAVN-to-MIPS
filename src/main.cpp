#include "./LexicalAnalysis/LexicalAnalysis.h"
#include "./SyntaxAnalysis/SyntaxAnalysis.h"

#include "./LexicalAnalysis/Types.h"
#include "./LivenessAnalysis/Instruction.h"
#include "./LexicalAnalysis/Test.h"
#include "./LexicalAnalysis/Variable.h"
#include "./LivenessAnalysis/LivenessAnalysis.h"

#include "./ResourceAllocation/ResourceAllocation.h"
#include "./ResourceAllocation/libResourceAllocation.h"
#include "./ResourceAllocation/InterferenceGraph.h"
#include "./ResourceAllocation/Simplification.h"
#include "./ResourceAllocation/libLivnessAnalysis.h"

#include "./MIPSGeneration/MIPSGeneration.h"

#include <iostream>
#include <exception>

using namespace std;

void main()
{
	try
	{
		std::string fileName = ".\\..\\examples\\commands.mavn";


		cout << "Input MAVN file: " << endl;

		std::ifstream file(fileName);
		if (!file.is_open()) {
			std::cerr << "Error: Could not open the file!" << std::endl;
		}
		else {

			string line;

			while (getline(file, line)) {
				std::cout << line << '\n';
			}
			file.close();
		}
		cout << endl << endl;

		bool retVal = false;

		LexicalAnalysis lex;
		
		if (!lex.readInputFile(fileName))
			throw runtime_error("\nException! Failed to open input file!\n");
		
		lex.initialize();

		string funcName = "";
		retVal = lex.Do(funcName);
		cout << "Function name: " << funcName << endl;

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


		//printInterferenceGraph(ig);

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
				//if (checkResourceAllocation(ig) == true)
				if(true)
				{

					// 5) remove unnecessary move operation
					// Instructions* noMoveInstr = removeMove(&instructions);

					// 6) final print
					cout << "Resource allocation finished successfully!" << endl;
					printVariables(*(ig->variables));
					cout << "\nInstruction list:\n";
					printInstructions(instructions);


					// -----------------------------------------------------------------------------


					cout << endl << "---------------------------------------------" << endl;
					cout << "Starting writing MIPS file..." << endl << endl;


					Variables memVariables = filterMemVariables(syntax.getVariables());

					cout << "Memory variables list:" << endl;
					printVariables(memVariables);
					cout << endl;

					// funcName is already set by lexical analysis

					size_t dot_pos = fileName.rfind('.');
					fileName.erase(dot_pos);
					fileName.append(".mips");

					if (generateMIPS(memVariables, funcName, instructions, fileName)) {

						cout << "Generated MIPS code: " << endl << endl;

						std::ifstream file(fileName);
						if (!file.is_open()) {
							std::cerr << "Error: Could not open the file!" << std::endl;
						}
						else {

							string line;

							while (getline(file, line)) {
								std::cout << line << '\n';
							}
							file.close();
						}
						cout << endl << endl;
						cout << "MIPS code generation finished successfully!" << endl << endl;
					}
					else
					{
						cout << "MIPS code generation failed!" << endl;
					}


					// -----------------------------------------------------------------------------

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



	}
	catch (runtime_error e)
	{
		cout << e.what() << endl;
	}
}