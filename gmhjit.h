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
static const unsigned char gmh_acntionlist[27] = {
  248,10,83,72,139,15,255,184,237,76,141,185,233,73,129,199,239,65,137,7,255,
  72,137,200,91,195,255
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
//|
//|.macro incptr, ptraddr, size
//|  mov64  TMP3,  ptraddr
//|  add    qword  TMP3,  size
//|  mov64  ptraddr, TMP
//|.endmacro

// stack heap and label tables
typedef struct {
  int* stacktop;

  int* stacklimit;

} context_t;

//|.type Cxt, context_t, rcx
#define Dt1(_V) (int)(ptrdiff_t)&(((context_t *)0)_V)
# 42 "gmhjit.dasc"

#define MAX_NESTING 4092


int gmhjit(dasm_State *state, Instruction *opcode, int size) {

    int step;
    // struct member's offset
    int cxtos_1 = offsetof(context_t, stacktop);
    int cxtos_2 = offsetof(context_t, stacklimit);

    (void)cxtos_2;
    //unsigned int maxpc = 0;

    // stack top ptr offset
    int s_offset = 0;
    assert(sizeof(Instruction) == 4);

    // rdi save the value of first param
    //|->start:
    //|  push  PTR
    //|  mov   rcx,  [rdi]
    dasm_put(Dst, 0);
# 64 "gmhjit.dasc"

    for(step = 0; step < 2/*size*/; step++, opcode++) {
        switch(*opcode) {
        // opcode t_C
        case PUSH: {
            // push next instruction to stack
            //|  mov  eax,    *(opcode + 1)
            //|  lea  TMP,    [rcx+cxtos_1]
            //|  add  TMP,    s_offset
            //|  mov  [TMP],  eax
            dasm_put(Dst, 7, *(opcode + 1), cxtos_1, s_offset);
# 74 "gmhjit.dasc"
            // stack top pointer++
            s_offset++;
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

    //return SUC_RETURN;
    //|  mov  rax,  Cxt
    //|  pop PTR
    //|  ret
    dasm_put(Dst, 21);
# 208 "gmhjit.dasc"
    return SUC_RETURN;
}



#endif // GMHJIT_DASM_INCLUDED
