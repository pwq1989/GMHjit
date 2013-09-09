#ifndef INTERPRETER_H_INCLUDED
#define INTERPRETER_H_INCLUDED

#include "type.h"

#include <stddef.h>

#include <list>
#include <map>
#include <vector>
#include <iostream>

using namespace std;

// class declare
class Interpreter {
public:
    Interpreter(InstrList& p) {
        this->opcodeList = &p;
    }

    int run(Instruction *opcode, size_t size);


private:
    InstrList *opcodeList; // Contains instructions
    std::vector<int> heap;
    std::list<int> stack; // To store values
    std::vector<int> callStack; // To remember where to return to
    std::map<int, unsigned> labels; // Lookup table for labels
};


#endif // INTERPRETER_H_INCLUDED
