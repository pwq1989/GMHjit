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
static const unsigned char gmh_acntionlist[163] = {
  248,10,83,85,72,137,229,255,199,132,253,36,233,237,255,72,137,252,251,255,
  199,131,233,237,255,139,147,233,255,137,147,233,255,139,147,233,139,139,233,
  137,147,233,137,139,233,255,3,147,233,137,147,233,255,43,147,233,137,147,
  233,255,15,175,147,233,137,131,233,255,139,147,233,139,139,233,255,76,141,
  172,253,139,233,65,137,85,0,255,139,147,233,76,141,172,253,147,233,65,139,
  85,0,137,147,233,255,249,255,252,233,245,255,131,187,233,0,15,132,245,255,
  131,187,233,0,15,140,245,255,15,182,187,233,72,184,237,237,252,255,208,255,
  139,147,233,76,141,172,253,147,233,72,184,237,237,252,255,208,65,136,69,0,
  255,249,93,91,195,255
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
//|
//|.define TMP,  edx
//|.define TMP2, ecx
//|.define TMP3, r13
//|.define CellSize, 4
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
//|.macro setStackp, value
//|  mov  dword [rsp - 4], value
//|.endmacro
//|
//|.macro incptr, ptraddr, size
//|  mov64  TMP3,  ptraddr
//|  add    qword  TMP3,  size
//|  mov64  ptraddr, TMP
//|.endmacro

// stack heap and label tables
typedef struct {
  int stack[4092];

  int heap[4092];

} context_t;

//|.type Cxt, context_t, r15
#define Dt1(_V) (int)(ptrdiff_t)&(((context_t *)0)_V)
# 48 "gmhjit.dasc"

#define MAX_NESTING 4092

#define cellsize sizeof(int)
#define sp_offsetb cxtos_1+(s_offset-1)*cellsize

#define ReservedLabel 3
// 2 is end of program
#define EndProLabel 2

int gmhjit(dasm_State *state, Instruction *opcode, int size) {

    int step;
    // struct member's offset
    int cxtos_1 = offsetof(context_t, stack);
    int cxtos_2 = offsetof(context_t, heap);

    // init pclabel
    dasm_growpc(&state, 2);

    unsigned int maxpc = 0;

    int pcstack[MAX_NESTING];
    int *top = pcstack, *limit = pcstack + MAX_NESTING;

    // stack top ptr offset
    int s_offset = 0;
    assert(sizeof(Instruction) == 4);

    // rdi save the value of first param
    //|->start:
    //|  push  PTR
    //|  push  rbp
    //|  mov   rbp,  rsp
    dasm_put(Dst, 0);
# 82 "gmhjit.dasc"
#ifndef NDEBUG
    // save the stack top ptr in program stack
    //|  setStackp s_offset
    dasm_put(Dst, 8, - 4, s_offset);
# 85 "gmhjit.dasc"
#endif
    //|  mov   PTR,  rdi
    dasm_put(Dst, 15);
# 87 "gmhjit.dasc"

    for(step = 0; step < size; step++, opcode++) {
        switch(*opcode) {
        // opcode t_C
        case PUSH: {
            // stack top pointer++
            s_offset++;
            // push next instruction to stack
            //|  mov  dword [PTR+sp_offsetb], *(opcode + 1)
            dasm_put(Dst, 20, sp_offsetb, *(opcode + 1));
# 96 "gmhjit.dasc"
#ifndef NDEBUG
            //|  setStackp s_offset
            dasm_put(Dst, 8, - 4, s_offset);
# 98 "gmhjit.dasc"
#endif
            step++;
            opcode++;
            break;
        }
        case DUP: {
            // duplicate the top item on the stack
            //|  mov TMP, [PTR+sp_offsetb]
            dasm_put(Dst, 25, sp_offsetb);
# 106 "gmhjit.dasc"
            s_offset++;
            //|  mov [PTR+sp_offsetb], TMP
            dasm_put(Dst, 29, sp_offsetb);
# 108 "gmhjit.dasc"
#ifndef NDEBUG
            //|  setStackp s_offset
            dasm_put(Dst, 8, - 4, s_offset);
# 110 "gmhjit.dasc"
#endif
            break;
        }
        case COPY: {
            // Copy the nth item on the stack (given by the argument)
            // onto the top of the stack
            //|  mov TMP, [PTR+cxtos_1+(*(opcode + 1))*cellsize]
            dasm_put(Dst, 25, cxtos_1+(*(opcode + 1))*cellsize);
# 117 "gmhjit.dasc"
            s_offset++;
            //|  mov [PTR+sp_offsetb], TMP
            dasm_put(Dst, 29, sp_offsetb);
# 119 "gmhjit.dasc"
#ifndef NDEBUG
            //|  setStackp s_offset
            dasm_put(Dst, 8, - 4, s_offset);
# 121 "gmhjit.dasc"
#endif
            break;
        }

        case SWAP: {
            // Swap the top two items on the stack
            //|  mov TMP,  [PTR+sp_offsetb]
            //|  mov TMP2, [PTR+sp_offsetb-cellsize]
            //|  mov [PTR+sp_offsetb-cellsize],  TMP
            //|  mov [PTR+sp_offsetb],  TMP2
            dasm_put(Dst, 33, sp_offsetb, sp_offsetb-cellsize, sp_offsetb-cellsize, sp_offsetb);
# 131 "gmhjit.dasc"
            break;
        }

        case DISCARD: {
            // Discard the top item on the stack
            s_offset--;
#ifndef NDEBUG
            //|  setStackp s_offset
            dasm_put(Dst, 8, - 4, s_offset);
# 139 "gmhjit.dasc"
#endif
            break;
        }

        case SLIDE: {
            // Slide n items off the stack, keeping the top item
            //|  mov TMP,  [PTR+sp_offsetb]
            dasm_put(Dst, 25, sp_offsetb);
# 146 "gmhjit.dasc"
            s_offset = s_offset - *(opcode + 1) + 1;
            //|  mov [PTR+sp_offsetb], TMP
            dasm_put(Dst, 29, sp_offsetb);
# 148 "gmhjit.dasc"
#ifndef NDEBUG
            //|  setStackp s_offset
            dasm_put(Dst, 8, - 4, s_offset);
# 150 "gmhjit.dasc"
#endif
            step++;
            opcode++;
            break;
        }

        // Arithmetic
        case ADD: {
            // Add pop the first and second element from stack,
            // add them and push the result back
            //|  mov TMP,  [PTR+sp_offsetb]
            dasm_put(Dst, 25, sp_offsetb);
# 161 "gmhjit.dasc"
            s_offset--;
            //|  add TMP,  [PTR+sp_offsetb]
            //|  mov [PTR+sp_offsetb], TMP
            dasm_put(Dst, 46, sp_offsetb, sp_offsetb);
# 164 "gmhjit.dasc"
#ifndef NDEBUG
            //|  setStackp s_offset
            dasm_put(Dst, 8, - 4, s_offset);
# 166 "gmhjit.dasc"
#endif
            break;
        }

        case SUB: {
            // Subtraction, pop the first and second element from stack, (second - first)
            // sub them and push the result back
            //|  mov TMP,  [PTR+sp_offsetb]
            dasm_put(Dst, 25, sp_offsetb);
# 174 "gmhjit.dasc"
            s_offset--;
            //|  sub TMP,  [PTR+sp_offsetb]
            //|  mov [PTR+sp_offsetb], TMP
            dasm_put(Dst, 53, sp_offsetb, sp_offsetb);
# 177 "gmhjit.dasc"
#ifndef NDEBUG
            //|  setStackp s_offset
            dasm_put(Dst, 8, - 4, s_offset);
# 179 "gmhjit.dasc"
#endif
            break;
        }

        case MUL: {
            // Multiplication. pop the first and second element from stack, (second*first)
            // mul them and push the result back
            //|  mov TMP,  [PTR+sp_offsetb]
            dasm_put(Dst, 25, sp_offsetb);
# 187 "gmhjit.dasc"
            s_offset--;
            //|  imul TMP,  [PTR+sp_offsetb]
            //|  mov [PTR+sp_offsetb], eax
            dasm_put(Dst, 60, sp_offsetb, sp_offsetb);
# 190 "gmhjit.dasc"
#ifndef NDEBUG
            //|  setStackp s_offset
            dasm_put(Dst, 8, - 4, s_offset);
# 192 "gmhjit.dasc"
#endif
            break;
        }

        case DIV: {
            // Division. pop the first and second element from stack, (second/first)
            // div them and push the result back
            /// TODO
            break;
        }

        case MOD: {
            // Modulo. pop the first and second element from stack, (second mod first)
            // mod them and push the result back
            /// TODO

            break;
        }

        // Heap access
        case STORE: {
            // Store
            //|  mov TMP,  [PTR+sp_offsetb]   // value
            //|  mov TMP2, [PTR+sp_offsetb-cellsize]  // address
            dasm_put(Dst, 68, sp_offsetb, sp_offsetb-cellsize);
# 216 "gmhjit.dasc"
            s_offset -= 2;

            //|  lea TMP3, [PTR+TMP2*CellSize+cxtos_2]
            //|  mov [TMP3], TMP
            dasm_put(Dst, 75, cxtos_2);
# 220 "gmhjit.dasc"
#ifndef NDEBUG
            //|  setStackp s_offset
            dasm_put(Dst, 8, - 4, s_offset);
# 222 "gmhjit.dasc"
#endif
            break;
        }

        case RETRIEVE: {
            // Retrieve
            //|  mov TMP,  [PTR+sp_offsetb]   // address
            //|  lea TMP3, [PTR+TMP*CellSize+cxtos_2]
            //|  mov TMP,  [TMP3]
            //|  mov [PTR+sp_offsetb], TMP
            dasm_put(Dst, 86, sp_offsetb, cxtos_2, sp_offsetb);
# 232 "gmhjit.dasc"
            break;
        }

        // Flow control
        case MARK: {
            // Mark a location in the program
            unsigned int label = ReservedLabel + *(opcode + 1);
            if (maxpc < label) {
                maxpc = label;
                dasm_growpc(&state, maxpc);
            }
            //|=>label:
            dasm_put(Dst, 103, label);
# 244 "gmhjit.dasc"
            opcode++;
            step++;
            break;
        }
        // Not sure what the difference is between CALL and JUMP.
        // I think this CALL function should actually execute a function...
        case CALL: {
            // Call a subroutine
            unsigned int label = ReservedLabel + *(opcode + 1);
            //|  jmp =>label
            dasm_put(Dst, 105, label);
# 254 "gmhjit.dasc"
            opcode++;
            step++;
            break;
        }

        case JUMP: {
            // Jump unconditionally to a label
            unsigned int label = ReservedLabel + *(opcode + 1);
            //|  jmp =>label
            dasm_put(Dst, 105, label);
# 263 "gmhjit.dasc"
            opcode++;
            step++;
            break;
        }

        case JUMPZERO: {
            // Jump to a label if the top of the stack is zero
            unsigned int label = ReservedLabel + *(opcode + 1);
            //|  cmp dword [PTR+sp_offsetb], 0
            //|  je  =>label
            dasm_put(Dst, 109, sp_offsetb, label);
# 273 "gmhjit.dasc"
            opcode++;
            step++;
            break;
        }

        case JUMPNEG: {
            // Jump to a label if the top of the stack is negative
            unsigned int label = ReservedLabel + *(opcode + 1);
            //|  cmp dword [PTR+sp_offsetb], 0
            //|  jl  =>label
            dasm_put(Dst, 117, sp_offsetb, label);
# 283 "gmhjit.dasc"
            opcode++;
            step++;
            break;
        }

        case ENDSUB: {
            // End a subroutine and transfer control back to the caller
            /// TODO
            // didn't know what's a fuction's beginning
            break;
        }

        case ENDPROG: {
            // End the program
            //|  jmp =>EndProLabel
            dasm_put(Dst, 105, EndProLabel);
# 298 "gmhjit.dasc"
            break;
            //return SUC_RETURN; // this is officially the end of the interpreter session
        }

        // I/O operations
        case WRITEC: {
            // Output the character at the top of the stack
            //|  movzx edi, byte [PTR+sp_offsetb]
            //|  callp putchar
            dasm_put(Dst, 125, sp_offsetb, (unsigned int)((uintptr_t)putchar), (unsigned int)(((uintptr_t)putchar)>>32));
# 307 "gmhjit.dasc"
            break;
        }

        case WRITEN: {
            // Output the number at the top of the stack
            /// TODO

            break;
        }

        case READC: {
            // Read a character and place it in the location
            // given by the top of the stack
            //|  mov TMP,  [PTR+sp_offsetb]   // address
            //|  lea TMP3, [PTR+TMP*CellSize+cxtos_2]
            //|  callp getchar
            //|  mov byte [TMP3], al
            dasm_put(Dst, 137, sp_offsetb, cxtos_2, (unsigned int)((uintptr_t)getchar), (unsigned int)(((uintptr_t)getchar)>>32));
# 324 "gmhjit.dasc"
            break;
        }

        case READN: {
            // Read a number and place it in the location
            // given by the top of the stack
            break;
        }
        default: {
            printf("print opcode : %d  ERRER happend:  %x\n", step, *(opcode));
            break;
        }
        }
    }

    //return SUC_RETURN;
    //|  mov  rax,  [PTR]
    //|=>EndProLabel:
    //|  pop  rbp
    //|  pop  PTR
    //|  ret
    dasm_put(Dst, 158, EndProLabel);
# 345 "gmhjit.dasc"
    printf("print maxpc : %d \n", maxpc);
    return SUC_RETURN;
}



#endif // GMHJIT_DASM_INCLUDED
