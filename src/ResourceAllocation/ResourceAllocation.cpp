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
    bool usedColors[6] = { 0,0,0,0,0,0 };

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


    for (int color = t0; color <= t4; color++) {
        if (usedColors[color] == 0) {
            return color;
        }
    }

    return __UNDEFINE__;
}

bool doResourceAllocation(stack<Variable*>* simplificationStack, InterferenceGraph* ig) {

    save.clear();

    Variable* currentVariable, * previusVariable;

    previusVariable = NULL;

    int counter = 0;

    for (auto it = ig->variables->begin(); it != ig->variables->end(); ++it) {
        (*it)->m_assignment = (Regs)__UNDEFINE__;
    }

    while (simplificationStack->size() > 0) {
        currentVariable = simplificationStack->top();
        simplificationStack->pop();
        save.push_back(currentVariable);

        int color = getColor(currentVariable, ig);
        if (color == __UNDEFINE__) {
            printf("COLORING FAILED for: %s pos=%d\n", currentVariable->m_name.c_str(), currentVariable->m_position);
            return false;
        }
        printf("Colored %s -> %d\n", currentVariable->m_name.c_str(), color);
        currentVariable->m_assignment = (Regs)color;

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
    // collect from both def AND out sets,
    // but only actual registers
    int maxPos = 0;

    // Collection loop
    for (auto instr : *instructions) {
        for (Variable* var : instr->m_out) {
            if (var->m_type == Variable::REG_VAR) {  // ← correct enum
                bool found = false;
                for (Variable* v : *(ig->variables))
                    if (v->m_position == var->m_position) { found = true; break; }
                if (!found) {
                    ig->variables->push_back(var);
                    if (var->m_position > maxPos) maxPos = var->m_position;
                }
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
            if (v1->m_type != Variable::REG_VAR) continue;
            for (Variable* v2 : instr->m_out) {
                if (v2->m_type != Variable::REG_VAR) continue;
                if (v1->m_position != v2->m_position) {
                    ig->values[v1->m_position][v2->m_position] = __INTERFERENCE__;
                    ig->values[v2->m_position][v1->m_position] = __INTERFERENCE__;
                }
            }
        }
    }

    // takodje iz m_in
    for (auto instr : *instructions) {
        for (Variable* v1 : instr->m_in) {
            if (v1->m_type != Variable::REG_VAR) continue;
            for (Variable* v2 : instr->m_in) {
                if (v2->m_type != Variable::REG_VAR) continue;
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

                printf("degree param = %d\n", degree);
                printf("ig->variables->size() = %d\n", (int)ig->variables->size());
                for (int i = 0; i < ig->size; i++) {
                    printf("degrees[%d] = %d\n", i, degrees[i]);
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