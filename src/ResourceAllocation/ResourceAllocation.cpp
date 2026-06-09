#include "ResourceAllocation.h"
#include "InterferenceGraph.h"
#include "../LexicalAnalysis/Constants.h"
#include "Instruction.h"
#include "Simplification.h"
#include <stack>

using namespace std;

Variables save;

int getColor(Variable* notColoredVariable, InterferenceGraph* ig) {

    // TO DO: Dobaviti promenljive sa kojima je u smetnji neobojena promeljiva sa stack-a,
    //        jer ne smeju dve promenljive u smetnji da dobiju isti registar (boju)
    //Napomena: smetnja je u InterferenceGraph-u oznacena kao __INTERFERENCE__

    int pos = notColoredVariable->m_position;
    bool usedColors[4] = { 0,0,0,0 };

    for (int i = 0; i < ig->size; i++) {
        if (ig->values[pos][i] == __INTERFERENCE__) {
            for (auto it = ig->variables->begin(); it != ig->variables->end();++it) {
                Variable* var = *it;
                if (var->m_position == i && var->m_assignment != __UNDEFINE__) {
                    usedColors[var->m_assignment] = 1;
                }
            }
        }
    }

    // TO DO: Pronaci boju koju mozemo dodeliti neobojenoj promenljivoj u skladu sa dobavljenom listom promenljivih u smetnji
    //Napomena: ukoliko ne postoji boja koju mozemo dodeliti promenljivoj vratiti 


    for (int color = t0; color <= t3; color++) {
        if (usedColors[color] == 0) {
            return color;
        }
    }

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
            currentVariable->m_assignment = t0;
        }
        else {
            // dodeliti boju ako nije prvi cvor u stack-u
            int color = getColor(currentVariable, ig);
            if (color == __UNDEFINE__) {
                return false;
            }
            else {
                currentVariable->m_assignment = (Regs)color;
            }
        }

        previusVariable = currentVariable;
    }

    ig->variables = &save;

    return true;
}

/*
Реализовати брисање Т_MOVE инструкција у оквиру функције removeMove у датотеци
ResourceAllocation.cpp. Ова функција, након завршетка доделе ресурса, елиминише све инструкције које
представљају доделу регистра самом себи.
Instructions * removeMove(Instructions * instrs);
*/
/*
Instructions* removeMove(Instructions* instrs) {
    Instructions* noMoveInstruction = new Instructions();

    for (auto instr : *instrs) {
        if (instr->m_type == T_MOVE) {
            if (instr->dst->assigment == instr->src1->assigment) {
                continue;
            }

        }
        else {
            noMoveInstruction->push_back(instr);
        }
    }

    return noMoveInstruction;
}
*/
InterferenceGraph* doInterferenceGraph(Instructions* instructions) {
    InterferenceGraph* ig = new InterferenceGraph();
    ig->variables = new Variables();

    // Prikupi varijable i nadji maksimalnu poziciju
    int maxPos = 0;
    for (auto instr : *instructions) {
        for (Variable* var : instr->m_def) {
            bool found = false;
            for (Variable* v : *(ig->variables))
                if (v->m_position == var->m_position) { found = true; break; }
            if (!found) {
                ig->variables->push_back(var);
                if (var->m_position > maxPos) maxPos = var->m_position;
            }
        }
    }

    // Alociraj matricu po maxPos+1, ne po broju varijabli
    ig->size = maxPos + 1;
    ig->values = new char* [ig->size];
    for (int i = 0; i < ig->size; i++) {
        ig->values[i] = new char[ig->size]();
    }

    // Popuni interferenciju iz m_out
    for (auto instr : *instructions) {
        for (Variable* v1 : instr->m_out) {
            for (Variable* v2 : instr->m_out) {
                if (v1->m_position != v2->m_position) {
                    ig->values[v1->m_position][v2->m_position] = __INTERFERENCE__;
                    ig->values[v2->m_position][v1->m_position] = __INTERFERENCE__;
                }
            }
        }
    }

    return ig;
}

stack<Variable*>* doSimplification(InterferenceGraph* ig, int degree) {

    stack<Variable*>* simplificationStack = new stack<Variable*>();

    // Kopija stepena svake varijable (broj smetnji)
    vector<int> degrees(ig->size, 0);
    vector<bool> removed(ig->size, false);

    // Izracunaj pocetni stepen za svaku varijablu
    for (int i = 0; i < ig->size; i++) {
        for (int j = 0; j < ig->size; j++) {
            if (ig->values[i][j] == __INTERFERENCE__) {
                degrees[i]++;
            }
        }
    }

    int removedCount = 0;

    while (removedCount < (int)ig->variables->size()){

        // Nadji varijablu sa stepenom manjim od degree koja nije uklonjena
        bool found = false;

        for (auto it = ig->variables->begin(); it != ig->variables->end(); ++it) {
            Variable* var = *it;
            int pos = var->m_position;

            if (!removed[pos] && degrees[pos] < degree) {
                // Stavi na stack
                simplificationStack->push(var);
                removed[pos] = true;
                removedCount++;
                found = true;

                // Umanji stepen susedima
                for (int j = 0; j < ig->size; j++) {
                    if (ig->values[pos][j] == __INTERFERENCE__ && !removed[j]) {
                        degrees[j]--;
                    }
                }

                break; // Kreni ponovo od pocetka
            }
        }

        // Ako nije nadjena ni jedna - spill
        if (!found) {
            return NULL;
        }
    }

    return simplificationStack;
}

void freeInterferenceGraph(InterferenceGraph* ig) {
    if (ig == NULL) return;

    if (ig->values != NULL) {
        for (int i = 0; i < ig->size; i++) {
            delete[] ig->values[i];
        }
        delete[] ig->values;
        ig->values = NULL;
    }

    // NE deletuj ig->variables - nije uvek heap alociran
    ig->variables = NULL;

    delete ig;
}