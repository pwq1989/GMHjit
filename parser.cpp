#include "type.h"
#include "parser.h"
#include "exceptions.h"

#include <iostream>
#include <string>
#include "math.h"

using namespace std;


TokenList Parser::parserRaw2Token(const string &data) {
    TokenList tokens;

    for(auto index = data.begin(); index != data.end(); index++) {
        //register int8_t val = (int8_t)*index;
        switch((int8_t)*index) {
        // get 草
        case -78:
            index++;
            if ((int8_t)*index == -35) {
                tokens.push_back(t_C);
                break;
            }
            index--;
            break;
        // get 泥
        case -60:
            index++;
            if ((int8_t)*index == -32) {
                tokens.push_back(t_N);
                break;
            }
            index--;
            break;
        // get 马
        case -62:
            index++;
            if ((int8_t)*index == -19) {
                tokens.push_back(t_M);
                break;
            }
            index--;
            break;
        }
    }
    return tokens;
}


// Note: could be implemented using some sort of lookup t_Nle
// data structure indexed by a pair of enums.
Mode Parser::determineMode(const Token t1, const Token t2) {
    switch(t1) {
    case t_M:
        return FLOWCONT;

    case t_C:
        return STACKMANIP;

    case t_N:
        switch(t2) {
        case t_M:
            return IO;

        case t_C:
            return ARITH;

        case t_N:
            return HEAPACC;

        default:
            throw UnreachableTokenException();
        }

    default:
        throw UnreachableTokenException();
    }
}

InstrList Parser::tokensToIntructions(const TokenList &tokens) {
    int amount = tokens.size();

    if(amount < 3) {
        exit(0); // Empty program
    }

    Mode m;

    try {
        m = determineMode(tokens[0], tokens[1]);
    } catch (exception& e) { // Unreachable token exception
        throw e;
    }

    InstrList p;

    // FLOWCONT and STACKMANIP are only 1 token long, the rest is 2 tokens long
    int start = ((m == FLOWCONT || m == STACKMANIP) ? 1 : 2);

    for(int k = start; k < amount; k++) {
        if(k == amount) {
            cout << "k has reached max" << endl; // We might want to use an exception here...
        }

        if(m == STACKMANIP) {
            processStackManip(tokens, p, k);
        } else if(m == ARITH) {
            processArith(tokens, p, k);
        } else if(m == HEAPACC) {
            processHeapAcc(tokens, p, k);
        } else if(m == FLOWCONT) {
            processFlowCont(tokens, p, k);
        } else if(m == IO) {
            processIO(tokens, p, k);
        } else {
            throw UnreachableTokenException();
        }

        k++; // Proceed to next instruction

        if(k != amount) {
            m = determineMode(tokens[k], tokens[k + 1]);

            if(!(m == STACKMANIP || m == FLOWCONT)) {
                k++;
            }
        }
    }

    return p;
}

// Side-effect: mutates the index from the for-loop in tokensToProgram
// Labels are also represented as numbers, so labels will be handled as well.
long Parser::tokensToNumber(const TokenList &tokens, int &index) {
    TokenList binNum;
    int amount = tokens.size();
    int sign;
    long sum = 0;

    while(tokens[index] != t_M) { // A number is terminated by a t_M
        binNum.push_back(tokens[index++]);

        if(index == amount) {
            throw PrematureEndException();
        }
    }

    if(binNum.front() == t_C) {
        sign = 1;
    } else if(binNum.front() == t_N) {
        sign = -1;
    } else {
        throw UndefinedSignException();
    }

    binNum.erase(binNum.begin()); // Pop the sign bit

    for(int k = binNum.size() - 1; k >= 0; k--) {
        //cout << "Sum = " << sum << endl;
        sum += ((binNum[k] == t_N) ? pow(2, k) : 0);
    }
#ifdef DEBUG
    cout << "Sum = " << sum << endl;
    cout << "sign*Sum = " << sign * sum << endl;
#endif
    return sign * sum;
}

void Parser::parseNumber(const TokenList &tokens, InstrList &p, int &k) {
    if(tokens[++k] == t_M) { // No label as argument
        throw NoLabelArgumentException();
    } else { // We're going to parse the label now
        p.push_back((Instruction)tokensToNumber(tokens, k));
    }
}

void Parser::processStackManip(const TokenList &tokens, InstrList &p, int &k) {
    if(tokens[k] == t_C) { // PUSH
        p.push_back(PUSH);
        parseNumber(tokens, p, k);
    } else if(tokens[k] == t_N) {
        k++;

        if(tokens[k] == t_C) { // COPY
            p.push_back(COPY);
        } else if(tokens[k] == t_M) { // SLIDE
            p.push_back(SLIDE);
        } else {
            throw UnreachableTokenException();
        }

        // COPY and SLIDE both require a numeric argument,
        // so we shall try to parse a number now
        parseNumber(tokens, p, k);
    } else if(tokens[k] == t_M) { // DUP, SWAP or DISCARD
        // Could perhaps also be done more concisely
        // with the ?:-operator, but I reckon something will go wrong
        // in the exception branch
        k++;

        if(tokens[k] == t_C) { // DUP
            p.push_back(DUP);
        } else if(tokens[k] == t_N) { // SWAP
            p.push_back(SWAP);
        } else if(tokens[k] == t_M) { // DISCARD
            p.push_back(DISCARD);
        } else {
            throw UnreachableTokenException();
        }
    } else {
        throw UnreachableTokenException();
    }
}

void Parser::processArith(const TokenList &tokens, InstrList &p, int &k) {
    if(tokens[k] == t_C) {
        k++;

        if(tokens[k] == t_C) { // ADD
            p.push_back(ADD);
        } else if(tokens[k] == t_N) { // SUB
            p.push_back(SUB);
        } else if(tokens[k] == t_M) { // MUL
            p.push_back(MUL);
        } else {
            throw UnreachableTokenException();
        }
    } else if(tokens[k] == t_N) {
        k++;

        if(tokens[k] == t_C) { // DIV
            p.push_back(DIV);
        } else if(tokens[k] == t_N) { // MOD
            p.push_back(MOD);
        } else {
            throw UnreachableTokenException();
        }
    } else {
        throw UnreachableTokenException();
    }
}

void Parser::processHeapAcc(const TokenList &tokens, InstrList &p, int &k) {
    if(tokens[k] == t_C) { // STORE
        p.push_back(STORE);
    } else if(tokens[k] == t_N) { // RETRIEVE
        p.push_back(RETRIEVE);
    } else {
        throw UnreachableTokenException();
    }
}

void Parser::processFlowCont(const TokenList &tokens, InstrList &p, int &k) {
    if(tokens[k] == t_C) {
        k++;

        if(tokens[k] == t_C) { // MARK
            p.push_back(MARK);
            parseNumber(tokens, p, k);
        } else if(tokens[k] == t_N) { // CALL
            p.push_back(CALL);
            parseNumber(tokens, p, k);
        } else if(tokens[k] == t_M) { // JUMP
            p.push_back(JUMP);
            parseNumber(tokens, p, k);
        } else {
            throw UnreachableTokenException();
        }
    } else if(tokens[k] == t_N) {
        k++;

        if(tokens[k] == t_C) { // JUMPZERO
            p.push_back(JUMPZERO);
            parseNumber(tokens, p, k);
        } else if(tokens[k] == t_N) { // JUMPNEG
            p.push_back(JUMPNEG);
            parseNumber(tokens, p, k);
        } else if(tokens[k] == t_M) { // ENDSUB
            p.push_back(ENDSUB);
        } else {
            throw UnreachableTokenException();
        }
    } else if(tokens[k] == t_M && tokens[++k] == t_M) { // ENDPROG
        p.push_back(ENDPROG);
    } else {
        throw UnreachableTokenException();
    }
}

void Parser::processIO(const TokenList &tokens, InstrList &p, int &k) {
    if(tokens[k] == t_C) {
        k++;

        if(tokens[k] == t_C) { // WRITEC
            p.push_back(WRITEC);
        } else if(tokens[k] == t_N) { // WRITEN
            p.push_back(WRITEN);
        } else {
            throw UnreachableTokenException();
        }
    } else if(tokens[k] == t_N) {
        k++;

        if(tokens[k] == t_C) { // READC
            p.push_back(READC);
        } else if(tokens[k] == t_N) { // READN
            p.push_back(READN);
        } else {
            throw UnreachableTokenException();
        }
    } else {
        throw UnreachableTokenException();
    }
}

