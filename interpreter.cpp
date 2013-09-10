#include "interpreter.h"

#include <malloc.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stddef.h>
#include <assert.h>


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
             const void** glob) {
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
    int step;
    for(step = 0; step < size; step++, opcode++) {
        switch(*opcode) {
        // Stack manipulations
        case PUSH: {
            printf("print opcode : %d  PUSH:  %x\n", step, *(opcode));
            break;
        }
        case DUP: {
            printf("print opcode : %d  DUP:  %x\n", step, *(opcode));
            break;
        }
        case COPY: {
            printf("print opcode : %d  COPY:  %x\n", step, *(opcode));
            break;
        }

        case SWAP: {
            printf("print opcode : %d  SWAP:  %x\n", step, *(opcode));
            break;
        }

        case DISCARD: {
            printf("print opcode : %d  DISCARD:  %x\n", step, *(opcode));
            break;
        }

        case SLIDE: {
            printf("print opcode : %d  SLIDE:  %x\n", step, *(opcode));
            break;
        }

        // Arithmetic
        case ADD: {
            printf("print opcode : %d  ADD:  %x\n", step, *(opcode));
            break;
        }

        case SUB: {
            printf("print opcode : %d  SUB:  %x\n", step, *(opcode));
            break;
        }

        case MUL: {
            printf("print opcode : %d  MUL:  %x\n", step, *(opcode));
            break;
        }

        case DIV: {
            printf("print opcode : %d  DIV:  %x\n", step, *(opcode));
            break;
        }

        case MOD: {
            printf("print opcode : %d  MOD:  %x\n", step, *(opcode));
            break;
        }

        // Heap access
        case STORE: {
            printf("print opcode : %d  STORE:  %x\n", step, *(opcode));
            break;
        }

        case RETRIEVE: {
            printf("print opcode : %d  RETRIEVE:  %x\n", step, *(opcode));
            break;
        }

        // Flow control
        case MARK: {
            printf("print opcode : %d  MARK:  %x\n", step, *(opcode));
            break;
        }
        // Not sure what the difference is between CALL and JUMP.
        // I think this CALL function should actually execute a function...
        case CALL: {
            printf("print opcode : %d  CALL:  %x\n", step, *(opcode));
            break;
        }

        case JUMP: {
            printf("print opcode : %d  JUMP:  %x\n", step, *(opcode));
            break;
        }

        case JUMPZERO: {
            printf("print opcode : %d  JUMPZERO:  %x\n", step, *(opcode));
            break;
        }

        case JUMPNEG: {
            printf("print opcode : %d  JUMPNEG:  %x\n", step, *(opcode));
            break;
        }

        case ENDSUB: {
            printf("print opcode : %d  ENDSUB:  %x\n", step, *(opcode));
            break;
        }

        case ENDPROG: {
            printf("print opcode : %d  ENDPROG:  %x\n", step, *(opcode));
            break;
            //return SUC_RETURN; // this is officially the end of the interpreter session
        }

        // I/O operations
        case WRITEC: {
            printf("print opcode : %d  WRITEC:  %x\n", step, *(opcode));
            break;
        }

        case WRITEN: {
            printf("print opcode : %d  WRITEN:  %x\n", step, *(opcode));
            break;
        }

        case READC: {
            printf("print opcode : %d  READC:  %x\n", step, *(opcode));
            break;
        }

        case READN: {
            printf("print opcode : %d  READN:  %x\n", step, *(opcode));
            break;
        }
        default: {
            printf("print opcode : %d  ERRER happend:  %x\n", step, *(opcode));
            break;
        }
        }
    }

}

int Interpreter::run() {

    Instruction *opcode = (Instruction*)(&opcodeList[0]);
    int size = (int)opcodeList->size();

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
    printf("ret = %x\n", ret);
    printf("cxt = %x\n", (void*)cxt);
    printf("cxt->stacktop = %x\n", cxt->stacktop);
    free_jitcode((void*)fptr);

    return ret;

}
