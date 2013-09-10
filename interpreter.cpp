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
#define GLOB_MAX 1024
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



void initjit(dasm_State **state, const void *actionlist) {
    dasm_init(state, 1);
    dasm_setup(state, actionlist);
}

void *jitcode(dasm_State **state) {

    int dasm_status;
    do {

        size_t size;
        if((dasm_status = dasm_link(state, &size))) break;
        //assert(dasm_status == DASM_S_OK);

        // Allocate memory readable and writable so we can
        // write the encoded instructions there.
        char *mem = mmap(NULL, size + sizeof(size_t),
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

int Interpreter::run() {

    Instruction *opcode = (Instruction*)(&opcodeList[0]);
    int size = (int)opcodeList->size();

    dasm_State *state;
    initjit(&state, gmh_acntionlist);

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

    free_jitcode(fptr);

    return ret;

}
