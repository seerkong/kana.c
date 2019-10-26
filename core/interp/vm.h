#ifndef kn_interp_vm_h
#define kn_interp_vm_h 1

#include "prefix.h"
#include "opcodes.h"

/* Include the interpreter variant header */

#define THREADED
#define DIRECT 1
#define INLINING 0

// #ifdef DIRECT
// #if INLINING
// #include "interp-inlining.h"
// #else
// #include "interp-direct.h"
// #endif /* INLINING */
// #else
// #include "interp-indirect.h"
// #endif /* DIRECT */



#ifdef THREADED
/* Two levels of macros are needed to correctly produce the label
 * from the OPC_xxx macro passed into DEF_OPC as cpp doesn't 
 * prescan when concatenating with ##...
 *
 * On gcc <= 2.95, we also get a space inserted before the :
 * e.g DEF_OPC(OPC_NULL) -> opc0 : - the ##: is a hack to fix
 * this, but this generates warnings on >= 2.96...
 */
#if (__GNUC__ == 2) && (__GNUC_MINOR__ <= 95)
#define label(x)         \
opc##x##:
#else
#define label(x)         \
opc##x:
#endif

// #define DEF_OPC(opcode)  \
// label(opcode)

#define DEF_OPC(opcode, BODY)            \
    label(opcode)                 \
        BODY

#define DISPATCH(pc)     \
    goto *handlers[*pc];
    // goto *(++pc)->handler;
#else
#define DEF_OPC(opcode)  \
    case opcode:

#define DISPATCH(pc)     \
    break;
#endif


#define STACK_MAX 4096
#define JUMPS_MAX 1024

// #define CASE_OP(name, args) case OP_##name: opHandlers[OP_##name]args; break;

#define CASE_OP(name, args) case OPC_##name: {       \
    opHandlers[OPC_##name]args;                      \
    pc++;                                           \
    break;                                          \
}


KN ExecByteCode(KonState* knState, KonEnv* env, KN_OP codeBlock[], int codeLen);

KN ExecByteCode2(KonState* knState, KonEnv* env, KN_OP codeBlock[], int codeLen);


#endif