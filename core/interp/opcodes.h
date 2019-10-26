
#ifndef KN_INTERP_OPCODES_H
#define KN_INTERP_OPCODES_H 1

#define OPC_LAND				          0
#define OPC_NOP                   1
#define OPC_HELLOWORLD            2

#define OPC_EVAL_EXPR_BY_TYPE     3
#define OPC_EVAL_SENTENCES        4
#define OPC_EVAL_LIST_SENTENCE    5
#define OPC_EVAL_CELL_SENTENCE    6
#define OPC_EVAL_CELL_CLAUSE      7
#define OPC_EVAL_CLAUSE_CORE      8
#define OPC_EVAL_CLAUSE_ARGS      9

#define OPC_ENV_LOOKUP            10



#define OPC_EVAL_EXPOSED_LIST     110
#define OPC_EVAL_EXPOSED_CELL     111
#define OPC_EVAL_EXPOSED_TABLE    112
#define OPC_EVAL_EXPOSED_VECTOR   113

#define OPC_EVAL_QUASI_LIST       114
#define OPC_EVAL_QUASI_CELL       115
#define OPC_EVAL_QUASI_TABLE      116
#define OPC_EVAL_QUASI_VECTOR     117

#define OPC_EVAL_SEALED_LIST      118
#define OPC_EVAL_SEALED_CELL      119
#define OPC_EVAL_SEALED_TABLE     120
#define OPC_EVAL_SEALED_VECTOR    121


/// KN_OP - a compressed three-address op (as 32bit int bitfield)
typedef struct {
  int code:8;
  int a:8;
  int b:8;
  int c:8;
} KN_OP;

#endif
