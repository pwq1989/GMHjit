#include "interpreter.h"

#include <malloc.h>
#include <stdio.h>
#include <stddef.h>
#include <assert.h>


#define ERR_RETURN 0
#define SUC_RETURN 1

#ifndef DEBUG
#define DEBUG
#endif

/* General DynASM definitions */
#define GLOB_MAX 1024
#define DASM_MAXSECTION 1024
#define DASM_ALIGNED_WRITES 1 /* aligned writing */
#ifdef DEBUG
#define DASM_CHECKS 1         /* sanity checks */
#endif

struct dasm_State;
typedef struct gmh_State {
    struct dasm_State *D;
} gmh_State;

typedef int (*dasm_gmh_func)(Instruction*, size_t);


#define Dst		    state
#define Dst_DECL	gmh_State *Dst
#define Dst_REF		(state->D)
#define DASM_FDEF	static

#include "dynasm/dasm_proto.h"
#include "dynasm/dasm_x86.h"



// include gmh jit code
#include "gmhjit.h"


int Interpreter::run(Instruction *opcode, size_t size) {

    void *glob[GLOB_MAX];
    gmh_State state_buf;
    gmh_State *state = &state_buf;
    void *mcode = NULL;
    size_t sz;
    int status = 0;
    int ret    = 0;

    dasm_init(Dst, DASM_MAXSECTION);
    dasm_setupglobal(Dst, glob, GLOB_MAX);
    dasm_setup(Dst, gmh_acntionlist);
    //dasm_growpc(Dst, 1);

    (void)gmhjit(Dst, opcode, size);


    do {
#ifdef DASM_CHECKS
        if ((status = dasm_checkstep(Dst, -1))) break;
#endif

        if ((status = dasm_link(Dst, &sz))) break;

        mcode = malloc(sz);

        if ((status = dasm_encode(Dst, mcode))) break;

        {
            FILE *fp = fopen("debug.dump", "wb");
            fwrite(mcode, 1, sz, fp);
            fclose(fp);
            int sysret = system("objdump -D -b binary -mi386 -Mx86-64 debug.dump; \
                                rm debug.dump");
            (void)sysret;
        }
    } while (0);

    if (status) fprintf(stderr, "DynASM error: %08x\n", status);

    // run jit code
    assert(mcode != NULL);
    dasm_gmh_func fn = (dasm_gmh_func)mcode;
    ret = fn(opcode, size);

    dasm_free(Dst);

    if (mcode) free(mcode);

    return ret;

}
