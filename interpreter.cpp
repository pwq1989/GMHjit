#include "interpreter.h"

#include <malloc.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stddef.h>
#include <assert.h>
#include <string>
#include <iostream>


#define ERR_RETURN 0
#define SUC_RETURN 1

#ifndef DEBUG
#define DEBUG
#endif

/* General DynASM definitions */
#define GLOB_MAX 4096
//#define DASM_MAXSECTION 1024
#define DASM_ALIGNED_WRITES 1 /* aligned writing */
#ifdef DEBUG
#define DASM_CHECKS 1         /* sanity checks */
#endif

#include "dynasm/dasm_proto.h"
#include "dynasm/dasm_x86.h"

#define Dst  &state


// include gmh jit code
#include "gmhjit.h"


typedef int (*dasm_gmh_func)(context_t*);



void initjit(dasm_State **state, const void *actionlist,
             void** glob) {
    dasm_init(state, 1);
    dasm_setupglobal(state, glob, GLOB_MAX);
    dasm_setup(state, actionlist);
}

void *jitcode(dasm_State **state) {

    int dasm_status;
    do {
#ifdef DASM_CHECKS
        if ((dasm_status = dasm_checkstep(state, -1))) break;
#endif
        size_t size;
        if((dasm_status = dasm_link(state, &size))) break;
        //assert(dasm_status == DASM_S_OK);

        // Allocate memory readable and writable so we can
        // write the encoded instructions there.
        char *mem = (char*)mmap(NULL, size + sizeof(size_t),
                         PROT_READ | PROT_WRITE,
                         MAP_ANON | MAP_PRIVATE, -1, 0);
        assert(mem != MAP_FAILED);

        // Store length at the beginning of the region, so we
        // can free it without additional context.
        *(size_t*)mem = size;
        void *ret = mem + sizeof(size_t);

        if((dasm_status = dasm_encode(state, ret))) break;

        dasm_free(state);

        // Adjust the memory permissions so it is executable
        // but no longer writable.
        int success = mprotect(mem, size, PROT_EXEC | PROT_READ);
        assert(success == 0);

#ifndef NDEBUG
        FILE *fp = fopen("debug.dump", "wb");
        fwrite(ret, 1, size, fp);
        fclose(fp);
        int sysret = system("objdump -D -b binary -mi386 -Mx86-64 debug.dump; \
                    rm debug.dump");
        (void)sysret;
#endif
        return ret;

    } while (0);

    if (dasm_status) fprintf(stderr, "DynASM error: %08x\n", dasm_status);

    return NULL;
}

void free_jitcode(void *code) {
    void *mem = (char*)code - sizeof(size_t);
    int status = munmap(mem, *(size_t*)mem);
    assert(status == 0);
}



void printInstrList(Instruction *opcode, size_t size) {
    using namespace std;

    int step;
    int lineno;
    for(step = 0,lineno = 0; step < (int)size; lineno++, step++, opcode++) {
        string s;
        s.append("line:" + to_string(lineno) + " ");
        //cout << *opcode << endl;
        switch(*opcode) {
        case PUSH:
            s.append("PUSH ");
            break;

        case DUP:
            s.append("DUP ");
            break;

        case COPY:
            s.append("COPY ");
            break;

        case SWAP:
            s.append("SWAP ");
            break;

        case DISCARD:
            s.append("DISCARD ");
            break;

        case SLIDE:
            s.append("SLIDE ");
            break;

        case ADD:
            s.append("ADD ");
            break;

        case SUB:
            s.append("SUB ");
            break;

        case MUL:
            s.append("MUL ");
            break;

        case DIV:
            s.append("DIV ");
            break;

        case MOD:
            s.append("MOD ");
            break;

        case STORE:
            s.append("STORE ");
            break;

        case RETRIEVE:
            s.append("RETRIEVE ");
            break;

        case MARK:
            s.append("MARK ");
            break;

        case CALL:
            s.append("CALL ");
            break;

        case JUMP:
            s.append("JUMP ");
            break;

        case JUMPZERO:
            s.append("JUMPZERO ");
            break;

        case JUMPNEG:
            s.append("JUMPNEG ");
            break;

        case ENDSUB:
            s.append("ENDSUB ");
            break;

        case ENDPROG:
            s.append("ENDPROG ");
            break;

        case WRITEC:
            s.append("WRITEC ");
            break;

        case WRITEN:
            s.append("WRITEN ");
            break;

        case READC:
            s.append("READC ");
            break;

        case READN:
            s.append("READN");
            break;

        default:
            s.append("ERROR!!!!");
            break;
        }

        switch(*opcode) {
        case PUSH: case COPY: case SLIDE: case MARK:
        case CALL: case JUMP: case JUMPZERO: case JUMPNEG:
            ++opcode;
            ++step;
            s.append(to_string(*opcode));
            break;
        default: break;
        }

        s.append("\n");

        cout << s;

    }

}

//void testPrint() {
//    InstrList test;
//    test.push_back(PUSH);
//    test.push_back(2);
//    test.push_back(DISCARD);
//    test.push_back(SWAP);
//    Instruction *opcode;
//    opcode = (Instruction*)(&test[0]);
//    int size = (int)test.size();
//    printInstrList(opcode,size);
//}

int Interpreter::run() {

    Instruction *opcode = (Instruction*)(opcodeList->data());
    int size = (int)opcodeList->size();

    //testPrint();

#ifndef NDEBUG
    printInstrList(opcode, size);
#endif

    dasm_State *state;
    void *glob[GLOB_MAX];

    initjit(&state, gmh_acntionlist, glob);

    int ret = 0;


    (void)gmhjit(state, opcode, size);


    /// TODO translate to C++ style?
    // init context
    context_t *cxt = (context_t*)calloc(1, sizeof(context_t));
    cxt->stacktop = (int*)calloc(4092, sizeof(int));
    cxt->stacklimit = cxt->stacktop + MAX_NESTING;
    //...

    // run jit code
    // mcode can be cached for next call

    dasm_gmh_func fptr = (dasm_gmh_func)jitcode(&state);
    // Call the JIT-ted function.
    ret = fptr(cxt);

    // for debug
    printf("ret = %x\n", (unsigned int)ret);
    printf("cxt = %p\n", (void*)cxt);
    printf("cxt->stacktop = %p\n", (void*)cxt->stacktop);
    free_jitcode((void*)fptr);

    return ret;

}
