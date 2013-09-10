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
static const unsigned char gmh_acntionlist[7] = {
  184,2,0,0,0,195,255
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
//|.define TMP,  r15
//|.define TMP2, r14
//|.define TMP3, r13
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

// stack heap and label tables
typedef struct {
  int* stacktop;

  int* stacklimit;

} context_t;

//|.type Cxt, context_t, rbx
#define Dt1(_V) (int)(ptrdiff_t)&(((context_t *)0)_V)
# 36 "gmhjit.dasc"

#define MAX_NESTING 4092


int gmhjit(dasm_State *state, Instruction *opcode, int size) {

    int step;

    unsigned int maxpc = 0;
    //int gmhstack[MAX_NESTING];
    //int *stacktop = gmhstack, *stacklimit = gmhstack + MAX_NESTING;

    assert(sizeof(Instruction) == 4);

//    |->start:
//    |  push  PTR
//
//    for(step = 0; step < size; step++, opcode++) {
//        switch(*opcode) {
//        // opcode t_C
//        case PUSH: {
//            // push next instruction to stack
//            // top pointer++
//            |  mov  eax,    *(opcode + 1)
//            |  mov  TMP,    Cxt->stacktop
//            |  mov  TMP2,   [TMP]
//            |  mov  [TMP2], eax
//            |  mov  TMP3,   Cxt->stacktop
//            |  add  qword   [TMP3], 4
//            break;
//        }
//        default: {
//            break;
//        }
//        }
//    }
//
//    //return SUC_RETURN;
//    |  mov  eax,  SUC_RETURN
//    |  pop PTR
//    |  ret

    //|  mov  eax,  2
    //|  ret
    dasm_put(Dst, 0);
# 80 "gmhjit.dasc"
    return SUC_RETURN;
}



#endif // GMHJIT_DASM_INCLUDED
