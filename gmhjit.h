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
static const unsigned char gmh_acntionlist[223] = {
  248,10,83,85,72,137,229,255,199,132,253,36,233,237,255,72,137,252,251,255,
  199,131,233,237,255,139,147,233,255,137,147,233,255,139,147,233,139,139,233,
  137,147,233,137,139,233,255,3,147,233,137,147,233,255,43,147,233,137,147,
  233,255,15,175,147,233,137,131,233,255,49,210,139,139,233,255,139,131,233,
  252,247,252,249,137,131,233,255,139,131,233,252,247,252,249,137,147,233,255,
  139,147,233,139,139,233,255,76,141,172,253,139,233,65,137,85,0,255,139,147,
  233,76,141,172,253,147,233,65,139,85,0,137,147,233,255,249,255,252,233,245,
  255,131,187,233,0,15,132,245,255,131,187,233,0,15,140,245,255,15,182,187,
  233,72,184,237,237,252,255,208,255,139,187,233,72,184,237,237,252,255,208,
  255,139,147,233,76,141,172,253,147,233,72,184,237,237,252,255,208,65,136,
  69,0,255,139,147,233,76,141,172,253,147,233,72,184,237,237,252,255,208,65,
  137,69,0,255,249,93,91,195,255
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

void putNumber(int out) {
    printf("%d", out);
}

void getNumber() {
    int out;
    scanf("%d", &out);
    return out;
}

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
# 92 "gmhjit.dasc"
#ifndef NDEBUG
    // save the stack top ptr in program stack
    //|  setStackp s_offset
    dasm_put(Dst, 8, - 4, s_offset);
# 95 "gmhjit.dasc"
#endif
    //|  mov   PTR,  rdi
    dasm_put(Dst, 15);
# 97 "gmhjit.dasc"

    for(step = 0; step < size; step++, opcode++) {
        switch(*opcode) {
        // opcode t_C
        case PUSH: {
            // stack top pointer++
            s_offset++;
            // push next instruction to stack
            //|  mov  dword [PTR+sp_offsetb], *(opcode + 1)
            dasm_put(Dst, 20, sp_offsetb, *(opcode + 1));
# 106 "gmhjit.dasc"
#ifndef NDEBUG
            //|  setStackp s_offset
            dasm_put(Dst, 8, - 4, s_offset);
# 108 "gmhjit.dasc"
#endif
            step++;
            opcode++;
            break;
        }
        case DUP: {
            // duplicate the top item on the stack
            //|  mov TMP, [PTR+sp_offsetb]
            dasm_put(Dst, 25, sp_offsetb);
# 116 "gmhjit.dasc"
            s_offset++;
            //|  mov [PTR+sp_offsetb], TMP
            dasm_put(Dst, 29, sp_offsetb);
# 118 "gmhjit.dasc"
#ifndef NDEBUG
            //|  setStackp s_offset
            dasm_put(Dst, 8, - 4, s_offset);
# 120 "gmhjit.dasc"
#endif
            break;
        }
        case COPY: {
            // Copy the nth item on the stack (given by the argument)
            // onto the top of the stack
            //|  mov TMP, [PTR+cxtos_1+(*(opcode + 1))*cellsize]
            dasm_put(Dst, 25, cxtos_1+(*(opcode + 1))*cellsize);
# 127 "gmhjit.dasc"
            s_offset++;
            //|  mov [PTR+sp_offsetb], TMP
            dasm_put(Dst, 29, sp_offsetb);
# 129 "gmhjit.dasc"
#ifndef NDEBUG
            //|  setStackp s_offset
            dasm_put(Dst, 8, - 4, s_offset);
# 131 "gmhjit.dasc"
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
# 141 "gmhjit.dasc"
            break;
        }

        case DISCARD: {
            // Discard the top item on the stack
            s_offset--;
#ifndef NDEBUG
            //|  setStackp s_offset
            dasm_put(Dst, 8, - 4, s_offset);
# 149 "gmhjit.dasc"
#endif
            break;
        }

        case SLIDE: {
            // Slide n items off the stack, keeping the top item
            //|  mov TMP,  [PTR+sp_offsetb]
            dasm_put(Dst, 25, sp_offsetb);
# 156 "gmhjit.dasc"
            s_offset = s_offset - *(opcode + 1) + 1;
            //|  mov [PTR+sp_offsetb], TMP
            dasm_put(Dst, 29, sp_offsetb);
# 158 "gmhjit.dasc"
#ifndef NDEBUG
            //|  setStackp s_offset
            dasm_put(Dst, 8, - 4, s_offset);
# 160 "gmhjit.dasc"
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
# 171 "gmhjit.dasc"
            s_offset--;
            //|  add TMP,  [PTR+sp_offsetb]
            //|  mov [PTR+sp_offsetb], TMP
            dasm_put(Dst, 46, sp_offsetb, sp_offsetb);
# 174 "gmhjit.dasc"
#ifndef NDEBUG
            //|  setStackp s_offset
            dasm_put(Dst, 8, - 4, s_offset);
# 176 "gmhjit.dasc"
#endif
            break;
        }

        case SUB: {
            // Subtraction, pop the first and second element from stack, (second - first)
            // sub them and push the result back
            //|  mov TMP,  [PTR+sp_offsetb]
            dasm_put(Dst, 25, sp_offsetb);
# 184 "gmhjit.dasc"
            s_offset--;
            //|  sub TMP,  [PTR+sp_offsetb]
            //|  mov [PTR+sp_offsetb], TMP
            dasm_put(Dst, 53, sp_offsetb, sp_offsetb);
# 187 "gmhjit.dasc"
#ifndef NDEBUG
            //|  setStackp s_offset
            dasm_put(Dst, 8, - 4, s_offset);
# 189 "gmhjit.dasc"
#endif
            break;
        }

        case MUL: {
            // Multiplication. pop the first and second element from stack, (second*first)
            // mul them and push the result back
            //|  mov TMP,  [PTR+sp_offsetb]
            dasm_put(Dst, 25, sp_offsetb);
# 197 "gmhjit.dasc"
            s_offset--;
            //|  imul TMP,  [PTR+sp_offsetb]
            //|  mov [PTR+sp_offsetb], eax    // ignore over 32bits data
            dasm_put(Dst, 60, sp_offsetb, sp_offsetb);
# 200 "gmhjit.dasc"
#ifndef NDEBUG
            //|  setStackp s_offset
            dasm_put(Dst, 8, - 4, s_offset);
# 202 "gmhjit.dasc"
#endif
            break;
        }

        case DIV: {
            // Division. pop the first and second element from stack, (second/first)
            // div them and push the result back
            /// TODO   tranlate to mul operation
            //|  xor edx,  edx
            //|  mov TMP2,  [PTR+sp_offsetb]
            dasm_put(Dst, 68, sp_offsetb);
# 212 "gmhjit.dasc"
            s_offset--;
            //|  mov eax,   [PTR+sp_offsetb]
            //|  idiv TMP2
            //|  mov [PTR+sp_offsetb], eax       //Quotient
            dasm_put(Dst, 74, sp_offsetb, sp_offsetb);
# 216 "gmhjit.dasc"
#ifndef NDEBUG
            //|  setStackp s_offset
            dasm_put(Dst, 8, - 4, s_offset);
# 218 "gmhjit.dasc"
#endif

            break;
        }

        case MOD: {
            // Modulo. pop the first and second element from stack, (second mod first)
            // mod them and push the result back
            //|  xor edx,  edx
            //|  mov TMP2,  [PTR+sp_offsetb]
            dasm_put(Dst, 68, sp_offsetb);
# 228 "gmhjit.dasc"
            s_offset--;
            //|  mov eax,   [PTR+sp_offsetb]
            //|  idiv TMP2
            //|  mov [PTR+sp_offsetb], edx   //remainder
            dasm_put(Dst, 85, sp_offsetb, sp_offsetb);
# 232 "gmhjit.dasc"
#ifndef NDEBUG
            //|  setStackp s_offset
            dasm_put(Dst, 8, - 4, s_offset);
# 234 "gmhjit.dasc"
#endif

            break;
        }

        // Heap access
        case STORE: {
            // Store
            //|  mov TMP,  [PTR+sp_offsetb]   // value
            //|  mov TMP2, [PTR+sp_offsetb-cellsize]  // address
            dasm_put(Dst, 96, sp_offsetb, sp_offsetb-cellsize);
# 244 "gmhjit.dasc"
            s_offset -= 2;

            //|  lea TMP3, [PTR+TMP2*CellSize+cxtos_2]
            //|  mov [TMP3], TMP
            dasm_put(Dst, 103, cxtos_2);
# 248 "gmhjit.dasc"
#ifndef NDEBUG
            //|  setStackp s_offset
            dasm_put(Dst, 8, - 4, s_offset);
# 250 "gmhjit.dasc"
#endif
            break;
        }

        case RETRIEVE: {
            // Retrieve
            //|  mov TMP,  [PTR+sp_offsetb]   // address
            //|  lea TMP3, [PTR+TMP*CellSize+cxtos_2]
            //|  mov TMP,  [TMP3]
            //|  mov [PTR+sp_offsetb], TMP
            dasm_put(Dst, 114, sp_offsetb, cxtos_2, sp_offsetb);
# 260 "gmhjit.dasc"
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
            dasm_put(Dst, 131, label);
# 272 "gmhjit.dasc"
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
            dasm_put(Dst, 133, label);
# 282 "gmhjit.dasc"
            opcode++;
            step++;
            break;
        }

        case JUMP: {
            // Jump unconditionally to a label
            unsigned int label = ReservedLabel + *(opcode + 1);
            //|  jmp =>label
            dasm_put(Dst, 133, label);
# 291 "gmhjit.dasc"
            opcode++;
            step++;
            break;
        }

        case JUMPZERO: {
            // Jump to a label if the top of the stack is zero
            unsigned int label = ReservedLabel + *(opcode + 1);
            //|  cmp dword [PTR+sp_offsetb], 0
            //|  je  =>label
            dasm_put(Dst, 137, sp_offsetb, label);
# 301 "gmhjit.dasc"
            opcode++;
            step++;
            break;
        }

        case JUMPNEG: {
            // Jump to a label if the top of the stack is negative
            unsigned int label = ReservedLabel + *(opcode + 1);
            //|  cmp dword [PTR+sp_offsetb], 0
            //|  jl  =>label
            dasm_put(Dst, 145, sp_offsetb, label);
# 311 "gmhjit.dasc"
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
            dasm_put(Dst, 133, EndProLabel);
# 326 "gmhjit.dasc"
            break;
            //return SUC_RETURN; // this is officially the end of the interpreter session
        }

        // I/O operations
        case WRITEC: {
            // Output the character at the top of the stack
            //|  movzx edi, byte [PTR+sp_offsetb]
            //|  callp putchar
            dasm_put(Dst, 153, sp_offsetb, (unsigned int)((uintptr_t)putchar), (unsigned int)(((uintptr_t)putchar)>>32));
# 335 "gmhjit.dasc"
            break;
        }

        case WRITEN: {
            // Output the number at the top of the stack
            //|  mov edi, dword [PTR+sp_offsetb]
            //|  callp putNumber
            dasm_put(Dst, 165, sp_offsetb, (unsigned int)((uintptr_t)putNumber), (unsigned int)(((uintptr_t)putNumber)>>32));
# 342 "gmhjit.dasc"
            break;
        }

        case READC: {
            // Read a character and place it in the location
            // given by the top of the stack
            //|  mov TMP,  [PTR+sp_offsetb]   // address
            //|  lea TMP3, [PTR+TMP*CellSize+cxtos_2]
            //|  callp getchar
            //|  mov byte [TMP3], al
            dasm_put(Dst, 176, sp_offsetb, cxtos_2, (unsigned int)((uintptr_t)getchar), (unsigned int)(((uintptr_t)getchar)>>32));
# 352 "gmhjit.dasc"
            break;
        }

        case READN: {
            // Read a number and place it in the location
            // given by the top of the stack
            //|  mov TMP,  [PTR+sp_offsetb]   // address
            //|  lea TMP3, [PTR+TMP*CellSize+cxtos_2]
            //|  callp getNumber
            //|  mov dword [TMP3], eax
            dasm_put(Dst, 197, sp_offsetb, cxtos_2, (unsigned int)((uintptr_t)getNumber), (unsigned int)(((uintptr_t)getNumber)>>32));
# 362 "gmhjit.dasc"
            break;
        }
        default: {
            printf("print opcode : %d  ERRER happend:  %x\n", step, *(opcode));
            break;
        }
        }
    }

    //|  mov  rax,  [PTR]
    //|=>EndProLabel:
    //|  pop  rbp
    //|  pop  PTR
    //|  ret
    dasm_put(Dst, 218, EndProLabel);
# 376 "gmhjit.dasc"
    printf("print maxpc : %d \n", maxpc);
    return SUC_RETURN;
}



#endif // GMHJIT_DASM_INCLUDED
