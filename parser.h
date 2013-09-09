#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include "type.h"
#include "exceptions.h"

#include <vector>
#include <string>

class Parser {
public:
    TokenList parserRaw2Token(const std::string&);
    InstrList tokensToIntructions(const TokenList&);


private:

    Mode determineMode(const Token, const Token);
    long tokensToNumber(const TokenList&, int&);
    void parseNumber(const TokenList&, InstrList&, int&);
    void processStackManip(const TokenList&, InstrList&, int&);
    void processArith(const TokenList&, InstrList&, int&);
    void processHeapAcc(const TokenList&, InstrList&, int&);
    void processFlowCont(const TokenList&, InstrList&, int&);
    void processIO(const TokenList&, InstrList&, int&);

};



#endif // PARSER_H_INCLUDED
