#include "ResourceAllocation.h"
#include "InterferenceGraph.h"
#include "Constants.h"
#include "Instruction.h"

Variables save;

int getColor(Variable* notColoredVariable, InterferenceGraph* ig) {

    // TO DO: Dobaviti promenljive sa kojima je u smetnji neobojena promeljiva sa stack-a,
    //        jer ne smeju dve promenljive u smetnji da dobiju isti registar (boju)
    //Napomena: smetnja je u InterferenceGraph-u oznacena kao __INTERFERENCE__


    // TO DO: Pronaci boju koju mozemo dodeliti neobojenoj promenljivoj u skladu sa dobavljenom listom promenljivih u smetnji
    //Napomena: ukoliko ne postoji boja koju mozemo dodeliti promenljivoj vratiti __UNDEFINE__
    return __UNDEFINE__;
}

bool doResourceAllocation(stack<Variable*>* simplificationStack, InterferenceGraph* ig) {

    Variable* currentVariable, * previusVariable;

    previusVariable = NULL;

    int counter = 0;

    while (simplificationStack->size() > 0) {

        currentVariable = simplificationStack->top();
        simplificationStack->pop();

        save.push_back(currentVariable);

        if (previusVariable == NULL) {
            // bojenje prvog cvora u stack-u
            currentVariable->assigment = (Regs)counter;
        }
        else {
            // dodeliti boju ako nije prvi cvor u stack-u
            int color = getColor(currentVariable, ig);
            if (color == __UNDEFINE__) {
                return false;
            }
            else {
                currentVariable->assigment = (Regs)color;
            }
        }

        previusVariable = currentVariable;
    }

    ig->variables = &save;

    return true;
}

Instructions* removeMove(Instructions* instrs) {
    Instructions* noMoveInstruction = new Instructions();
    return noMoveInstruction;
}