
#ifndef KN_INTERP_OPCODES_H
#define KN_INTERP_OPCODES_H 1

#define OPC_LAND				  0
#define OPC_NOP                   1
#define OPC_HELLOWORLD            2

#define OPC_ENTER_BOUNCE_MODE     3
#define OPC_ENTER_CODE_SEG_MODE   4

#define OPC_EVAL_EXPR_BY_TYPE     5
#define OPC_EVAL_SENTENCES        6
#define OPC_EVAL_LIST_SENTENCE    7
#define OPC_EVAL_CELL_SENTENCE    8

#define OPC_EVAL_CELL_CLAUSE      9
#define OPC_EVAL_CLAUSE_CORE      10
#define OPC_EVAL_CLAUSE_ARGS      11

#define OPC_LOAD_CONT_RUN_NEXT_NODE   12
#define OPC_RUN_NEXT_CONT             13
#define OPC_RUN_CURRENT_CONT          14
#define OPC_ENV_LOOKUP                15

// native func, composite func
#define OPC_APPLY_PROCEDURE           16


// byte code instructions
#define OPC_SET_KN_REG_FROM_AST         61
#define OPC_SET_NEXT_OP_CODE                62
#define OPC_SET_NEXT_OP_ONE                 63
#define OPC_SET_NEXT_OP_TWO                 64
#define OPC_SET_NEXT_OP_THREE               65
#define OPC_DISPATCH_OP_CODE                     66
#define OPC_END_CODE_SEG                    67


#define OPC_ENV_SET
#define OPC_GET_LVALUE
#define OPC_GET_RVALUE

#define OPC_COMPOSITE_MACRO
#define OPC_NATIVE_MACRO
#define OPC_INFIX_MACRO
#define OPC_PREFIX_MACRO

#define OPC_DISPATCH_SVO
#define OPC_DISPATCH_SV
#define OPC_DISPATCH_SVO_BLOCK



#define OPC_DISPATCH_CELL_ITEM_MACRO
#define OPC_DISPATCH_LIST
#define OPC_DISPATCH_CELL
#define OPC_DISPATCH_TABLE
#define OPC_DISPATCH_VECTOR


#define OPC_EVAL_EXPOSED_LIST     128
#define OPC_EVAL_EXPOSED_CELL     129
#define OPC_EVAL_EXPOSED_TABLE    130
#define OPC_EVAL_EXPOSED_VECTOR   131

#define OPC_EVAL_QUASI_LIST       132
#define OPC_EVAL_QUASI_CELL       133
#define OPC_EVAL_QUASI_TABLE      134
#define OPC_EVAL_QUASI_VECTOR     135

#define OPC_EVAL_SEALED_LIST      136
#define OPC_EVAL_SEALED_CELL      137
#define OPC_EVAL_SEALED_TABLE     138
#define OPC_EVAL_SEALED_VECTOR    139


//   OP_MOVE,
//   OP_LOADK,
//   OP_LOADKN,
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
//   OP_NEWCELL,
//   OP_GETPATH,
//   OP_SETPATH,
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

#endif
