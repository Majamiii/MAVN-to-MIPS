#include "Variable.h"


void printVariable(Variable* variable)
{
	printf("(Name: %s, Pos: %d, Assignment: %d) ", variable->name.c_str(), variable->pos, variable->assignment);
}


void printVariableName(Variable* variable)
{
	printf("%s", variable->name.c_str());
}


void printVariables(Variables variables)
{
	Variables::iterator iter;

	for (iter = variables.begin(); iter != variables.end(); iter++)
	{
		printVariable(*iter);
		printf("\n");
	}
	printf("\n");
}


void printVariablesNames(Variables variables)
{
	Variables::iterator iter;

	for (iter = variables.begin(); iter != variables.end(); iter++)
	{
		printVariableName(*iter);
		printf(" ");
	}
	printf("\n");
}


bool variableExists(Variable* variable, Variables variables)
{
	bool ret = false;

	Variables::iterator it;

	for (it = variables.begin(); it != variables.end(); it++)
	{
		if (*it == variable)
		{
			ret = true;
			break;
		}
	}

	return ret;
}
