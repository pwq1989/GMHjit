#ifndef INTERPRETER_H_INCLUDED
#define INTERPRETER_H_INCLUDED

#include "type.h"

#include <stddef.h>


// class declare
class Interpreter {
public:
    Interpreter(InstrList& p) {
        this->opcodeList = &p;
    }

    int run();

private:
    InstrList *opcodeList; // Contains instructions
};


#endif // INTERPRETER_H_INCLUDED
