
#include "parser.h"
#include "interpreter.h"
#include "type.h"
#include "exceptions.h"

#include <iostream>
#include <stdio.h>
#include <fstream>

#ifndef DEBUG
#define DEBUG 1
#endif

using namespace std;

void printTokens(const vector<Token> &tokens) {
    int length = tokens.size();

    for(int k = 0; k < length; k++) {
        switch(tokens[k]) {
        case t_C:
            cout << "C " << endl;
            break;
        case t_N:
            cout << "N ";
            break;
        case t_M:
            cout << "M ";
            break;
        }
    }
}


const string readFile(const string filename) {
    string line, fileContents;
    ifstream input;

    input.open(filename.c_str());
    if (input.is_open()) {
        while (!input.eof()) {
            getline(input, line);
            fileContents.append(line);
            fileContents.append("\n");
        }
    }
    return fileContents;
}


int main() {

    //char p[7] = "²ÝÄàÂí";

    //printf("%d\n",p[0]); // -78
    //printf("%d\n",p[1]); // -35
    //printf("%d\n",p[2]); // -60
    //printf("%d\n",p[3]); // -32
    //printf("%d\n",p[4]); // -62
    //printf("%d\n",p[5]); // -19


    Parser parser;

    string fileContents = readFile("hworld.gmh");
    auto tokens = parser.parserRaw2Token(fileContents);

    InstrList bytecode = parser.tokensToIntructions(tokens);

    //printTokens(tokens);

    Interpreter jit(bytecode);
    int result = jit.run();
    if(result) {
        cout << "gmh is successed worked" << endl;
    } else {
        cout << "wocao! something wrong" << endl;
    }

    return 0;
}
