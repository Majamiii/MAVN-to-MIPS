#include "ResourceAllocation.h"
#include "InterferenceGraph.h"
#include "../LexicalAnalysis/Constants.h"
#include "Instruction.h"
#include "Simplification.h"
#include <stack>
#include <unordered_set>

using namespace std;

Variables save;

int getColor(Variable* notColoredVariable, InterferenceGraph* ig) {

    // TO DO: Dobaviti promenljive sa kojima je u smetnji neobojena promeljiva sa stack-a,
    //        jer ne smeju dve promenljive u smetnji da dobiju isti registar (boju)
    //Napomena: smetnja je u InterferenceGraph-u oznacena kao __INTERFERENCE__

    int pos = notColoredVariable->m_position;
    bool usedColors[5] = { 0,0,0,0,0 };

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


InterferenceGraph* doInterferenceGraph(Instructions* instructions) {
    InterferenceGraph* ig = new InterferenceGraph();
    ig->variables = new Variables();

    int maxPos = 0;
    std::unordered_set<int> visitedPositions;

    for (auto instr : *instructions) {
        
        std::vector<std::list<Variable*>*> allLists = {
            &instr->m_use, &instr->m_def, &instr->m_in, &instr->m_out
        };

        for (auto listPtr : allLists) {
            for (Variable* var : *listPtr) {
                if (var->m_type != Variable::REG_VAR) continue;

                if (visitedPositions.find(var->m_position) == visitedPositions.end()) {
                    visitedPositions.insert(var->m_position);
                    ig->variables->push_back(var);

                    if (var->m_position > maxPos) {
                        maxPos = var->m_position;
                    }
                }
            }
        }
    }

    ig->size = maxPos + 1;
    ig->values = new char* [ig->size];
    for (int i = 0; i < ig->size; i++) {
        ig->values[i] = new char[ig->size]();
    }

    for (auto instr : *instructions) {
        // def je u interferenciji sa out
        for (Variable* d : instr->m_def) {
            if (d->m_type != Variable::REG_VAR) continue;

            for (Variable* o : instr->m_out) {
                if (o->m_type != Variable::REG_VAR) continue;

                if (d->m_position != o->m_position) {
                    ig->values[d->m_position][o->m_position] = __INTERFERENCE__;
                    ig->values[o->m_position][d->m_position] = __INTERFERENCE__;
                }
            }
        }

        // m_out međusobna interferencija
        for (auto it1 = instr->m_out.begin(); it1 != instr->m_out.end(); ++it1) {
            Variable* v1 = *it1;
            if (v1->m_type != Variable::REG_VAR) continue;
            for (auto it2 = std::next(it1); it2 != instr->m_out.end(); ++it2) {
                Variable* v2 = *it2;
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

    // kopija stepena svake varijable (broj smetnji)
    vector<int> degrees(ig->size, 0);
    vector<bool> removed(ig->size, false);

    // izracunaj pocetni stepen za svaku varijablu
    for (int i = 0; i < ig->size; i++) {
        for (int j = 0; j < ig->size; j++) {
            if (ig->values[i][j] == __INTERFERENCE__) {
                degrees[i]++;
            }
        }
    }

    int removedCount = 0;

    while (removedCount < (int)ig->variables->size()){

		// da li je pronadjena varijabla sa stepenom manjem od degree
        bool found = false;

        for (auto it = ig->variables->begin(); it != ig->variables->end(); ++it) {
            Variable* var = *it;
            int pos = var->m_position;

            if (!removed[pos] && degrees[pos] < degree) {
                // stavi na stack
                simplificationStack->push(var);
                removed[pos] = true;
                removedCount++;
                found = true;

                // umanji stepen susedima
                for (int j = 0; j < ig->size; j++) {
                    if (ig->values[pos][j] == __INTERFERENCE__ && !removed[j]) {
                        degrees[j]--;
                    }
                }

                break;
            }
        }

        // ako nije nadjena ni jedna - spill
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

    // nemoj da brises ig->variables, koriste se posle
    ig->variables = NULL;

    delete ig;
}