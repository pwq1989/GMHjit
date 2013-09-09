#ifndef TYPE_H_INCLUDED
#define TYPE_H_INCLUDED

#include <vector>

enum Instruction {
    PUSH, DUP, COPY, SWAP, DISCARD, SLIDE,                 // Stack manipulations
    ADD, SUB, MUL, DIV, MOD,                               // Arithmetic operations
    STORE, RETRIEVE,                                       // Heap access
    MARK, CALL, JUMP, JUMPZERO, JUMPNEG, ENDSUB, ENDPROG,  // Flow control
    WRITEC, WRITEN, READC, READN                           // I/O operations
};

enum Token {
     t_C, t_N, t_M
};

enum Mode {
    STACKMANIP, ARITH, HEAPACC, FLOWCONT, IO
};

typedef std::vector<Instruction> InstrList;
typedef std::vector<Token>       TokenList;


#endif // TYPE_H_INCLUDED
