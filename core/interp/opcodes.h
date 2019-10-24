
#ifndef KN_INTERP_OPCODES_H
#define KN_INTERP_OPCODES_H 1

enum KN_OPCODE {
  OP_NONE = 0,
  OP_LAND,
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
  OP_ADD,
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
//   OP_TAILCALL, /* TODO */
//   OP_RETURN,
//   OP_PROTO,  /* define a method */
//   OP_CLASS,
//   OP_DEBUG
};

#if defined(__GNUC__)
#pragma pack(1)
#else
#pragma pack(push, 1)
#endif

/// KN_OP - a compressed three-address op (as 32bit int bitfield)
/// TODO: expand to 64bit, check jit then
// typedef struct {
//   enum KN_OPCODE code:8; ///< the op. See vm.c http://www.lua.org/doc/jucs05.pdf
//   int a:12;  ///< the data (i.e the register)
//   int b:12;  ///< optional arg, the message
// } KN_OP;

typedef struct {
  enum KN_OPCODE code:8; ///< the op. See vm.c http://www.lua.org/doc/jucs05.pdf
  int a:12;  ///< the data (i.e the register)
  int b:12;  ///< optional arg, the message
} KN_OP;

#if defined(__GNUC__)
#pragma pack()
#else
#pragma pack(pop)
#endif

#define KN_OP_AT(asmb, n) ((KN_OP *)((PNFlex *)asmb)->ptr)[n]
#define KN_OP_LEN(asmb)   (KN_FLEX_SIZE(asmb) / sizeof(KN_OP))

#endif
