/*
** This file has been pre-processed with DynASM.
** http://luajit.org/dynasm.html
** DynASM version 1.3.0, DynASM x64 version 1.3.0
** DO NOT EDIT! The original file is in "gmhjit.dasc".
*/

#if DASM_VERSION != 10300
#error "Version mismatch between DynASM and included encoding engine"
#endif

# 1 "gmhjit.dasc"
#ifndef GMHJIT_DASM_INCLUDED
#define GMHJIT_DASM_INCLUDED

//|.arch x64
//|.actionlist gmh_acntionlist
static const unsigned char gmh_acntionlist[19] = {
  139,4,37,237,137,4,37,237,72,141,52,37,237,72,137,52,37,237,255
};

# 6 "gmhjit.dasc"
//|.section jitcode
#define DASM_SECTION_JITCODE	0
#define DASM_MAXSECTION		1
# 7 "gmhjit.dasc"
//|
//|// Use rbx as our cell pointer.
//|// Since rbx is a callee-save register, it will be preserved
//|// across our calls to getchar and putchar.
//|.define PTR, rbx
//|
//|// Macro for calling a function.
//|// In cases where our target is <=2**32 away we can use
//|//   | call &addr
//|// But since we don't know if it will be, we use this safe
//|// sequence instead.
//|.macro callp, addr
//|  mov64  rax, (uintptr_t)addr
//|  call   rax
//|.endmacro


#define MAX_NESTING 4092

int gmhjit(gmh_State *state, Instruction *opcode, size_t size) {

    int step;

    unsigned int maxpc = 0;
    int gmhstack[MAX_NESTING];
    int *stacktop = gmhstack, *stacklimit = gmhstack + MAX_NESTING;

    assert(sizeof(Instruction) == 4);

    for(step = 0; step < size; step++, opcode++) {
        switch(*opcode) {
        // opcode t_C
        case PUSH: {
            //|  mov  eax,  [opcode + 4]
            //|  mov  eax,  [*(opcode + 4)]
            //|  mov  [stacktop],  eax
            //|  lea  rsi,  [stacktop + 4]
            //|  mov  [stacktop],  rsi
            dasm_put(Dst, 0, *(opcode + 4), stacktop, stacktop + 4, stacktop);
# 45 "gmhjit.dasc"
            //stacktop++;
            break;
        }


        }

    }
    return SUC_RETURN;
}



#endif // GMHJIT_DASM_INCLUDED
