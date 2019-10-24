#include "vm.h"

typedef void (*OpHandlerRef)(KonState* knState, KonEnv* env, KN* regLastVal, KN_OP op);

void OpHandler_NONE(KonState* knState, KonEnv* env, KN* regLastVal, KN_OP op)
{
}

void OpHandler_LAND(KonState* knState, KonEnv* env, KN* regLastVal, KN_OP op)
{
}

void OpHandler_ADD(KonState* knState, KonEnv* env, KN* regLastVal, KN_OP op)
{
    int a = op.a;
    int b = op.b;
    int res = a + b;
    // (*pc) += 1;
    (*regLastVal) = KN_BOX_INT(res);
}

OpHandlerRef opHandlers[] = {
    OpHandler_NONE, //   OP_NONE,
    OpHandler_LAND, //   OP_LAND,
//   OP_MOVE,
//   OP_LOADK,
//   OP_LOADPN,
//   OP_SELF,
//   OP_NEWTUPLE,
//   OP_GETTUPLE,
//   OP_SETTUPLE,
//   OP_GETLOCAL,
//   OP_SETLOCAL,
//   OP_GETUPVAL,
//   OP_SETUPVAL,
//   OP_GLOBAL,
//   OP_GETTABLE,
//   OP_SETTABLE,
//   OP_NEWLICK,
//   OP_GETPATH,
//   OP_SETPATH,
    OpHandler_ADD,  //   OP_ADD,
//   OP_SUB,
//   OP_MULT,
//   OP_DIV,
//   OP_REM,
//   OP_POW,
//   OP_NOT,
//   OP_CMP,
//   OP_EQ,
//   OP_NEQ,
//   OP_LT,
//   OP_LTE,
//   OP_GT,
//   OP_GTE,
//   OP_BITN,
//   OP_BITL,
//   OP_BITR,
//   OP_DEF,
//   OP_BIND,
//   OP_MSG,
//   OP_JMP,
//   OP_TEST,
//   OP_TESTJMP,
//   OP_NOTJMP,
//   OP_NAMED,
//   OP_CALL,
//   OP_CALLSET,
//   OP_TAILCALL,
//   OP_RETURN,
//   OP_PROTO,
//   OP_CLASS,
//   OP_DEBUG
};

KN ExecByteCode(KonState* knState, KonEnv* env, KN_OP codeBlock[], int codeLen)
{
    KN regLastVal = KN_NIL;

    int pc = 0;
    int contRun = true;
    while (contRun && pc < codeLen) {
        switch (codeBlock[pc].code) {
            CASE_OP(ADD, (knState, env, &regLastVal, codeBlock[pc]))
            case OP_LAND: {
                contRun = false;
                break;
            }
        }
    }

    int d = 1;
    static const void* array[] = {
        &&lab0, &&lab1, &&lab2, &&lab3
    };
    goto *array[d%3];
    lab0: {
        d += 1;
        // goto *array[d%3];
        printf("code in lab0\n");
        return regLastVal;
    }
    lab1: {
        d += 2;
        printf("code in lab1\n");
        goto *array[d];
        
    }
    lab2: {
        d += 1;
        printf("code in lab2\n");
        // goto *array[d%3];
        
    }
    lab3: {
        d += 1;
        // goto *array[d%3];
        printf("code in lab3\n");
        goto *array[0]; // return
        
    }

    return regLastVal;
}


// int foo (std::map<int,int>& m, int d, int x) {
//   static const void* array[] = {&&lab1, &&lab2, &&lab3 };
//   goto *array[d%3];
// lab1: {
//     m[d]= 1;...


// static const int array[] = { &&foo - &&foo, &&bar - &&foo,
//                              &&hack - &&foo };
// goto *(&&foo + array[i]);

// }





/*
KN ExecByteCode2(KonState* knState, KonEnv* env, KN_OP codeBlock[], int codeLen)
{
    KN regLastVal = KN_NIL;

    int pc = 0;
    // volatile unsigned char *pc = mb->code;


#ifdef THREADED
    static void *handlers[] = {
        &&opc0, &&opc1, &&unused};

    DISPATCH(pc)

#else
    while(TRUE) {
        switch(*pc) {
            default:
#endif

    

    DEF_OPC(OPC_NOP)
        pc += 1;
        DISPATCH(pc)

    // DEF_OPC(OPC_INVOKESUPER_QUICK)
    //     new_mb = CLASS_CB(CLASS_CB(mb->class)->super)->method_table[CP_DINDEX(pc)];
    //     arg1 = ostack - (new_mb->args_count);
	// NULL_POINTER_CHECK(*arg1);
	// goto invokeMethod;
    // DEF_OPC(OPC_ATHROW)
    // {
    //     Object *ob = (Object *)ostack[-1];
    //     frame->last_pc = (unsigned char*)pc;
	// NULL_POINTER_CHECK(ob);
		
    //     ee->exception = ob;
    //     goto throwException;
    // }
invokeMethod:
{
    // DISPATCH(pc)
}
methodReturn:
{
    // Set interpreter state to previous frame

    // frame = frame->prev;

    // if(frame->mb == NULL) {
    //     // The previous frame is a dummy frame - this indicates
    //     //    top of this Java invocation.
    //     return ostack;
    // }
    // DISPATCH(pc)
}

throwException:
{
    // DISPATCH(pc)
}

#ifndef THREADED
  }}    // end switch, end while
#endif
    return regLastVal;
}

*/