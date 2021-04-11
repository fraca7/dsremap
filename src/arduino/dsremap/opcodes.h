#ifndef _OPCODES_H_
#define _OPCODES_H_
#define ADDR_TYPE_REG 0x00
#define ADDR_TYPE_REGOFF 0x01
#define ADDR_VALTYPE_FLOAT 0x01
#define ADDR_VALTYPE_INT 0x00
#define OPCODE_SUBTYPE_BINARY_ADD 0x00
#define OPCODE_SUBTYPE_BINARY_AND 0x0a
#define OPCODE_SUBTYPE_BINARY_CAST 0x0d
#define OPCODE_SUBTYPE_BINARY_CE 0x08
#define OPCODE_SUBTYPE_BINARY_CGT 0x05
#define OPCODE_SUBTYPE_BINARY_CGTE 0x07
#define OPCODE_SUBTYPE_BINARY_CLT 0x04
#define OPCODE_SUBTYPE_BINARY_CLTE 0x06
#define OPCODE_SUBTYPE_BINARY_CN 0x09
#define OPCODE_SUBTYPE_BINARY_DIV 0x03
#define OPCODE_SUBTYPE_BINARY_LOAD 0x0c
#define OPCODE_SUBTYPE_BINARY_MUL 0x02
#define OPCODE_SUBTYPE_BINARY_OR 0x0b
#define OPCODE_SUBTYPE_BINARY_SUB 0x01
#define OPCODE_SUBTYPE_FLOW_CALL 0x00
#define OPCODE_SUBTYPE_FLOW_JUMP 0x03
#define OPCODE_SUBTYPE_FLOW_JZ 0x04
#define OPCODE_SUBTYPE_FLOW_RET 0x01
#define OPCODE_SUBTYPE_FLOW_YIELD 0x02
#define OPCODE_SUBTYPE_STACK_POP 0x03
#define OPCODE_SUBTYPE_STACK_PUSH 0x02
#define OPCODE_SUBTYPE_STACK_PUSHF 0x01
#define OPCODE_SUBTYPE_STACK_PUSHI 0x00
#define OPCODE_SUBTYPE_UNARY_NEG 0x00
#define OPCODE_SUBTYPE_UNARY_NOT 0x01
#define OPCODE_TYPE_BINARY 0x00
#define OPCODE_TYPE_FLOW 0x02
#define OPCODE_TYPE_STACK 0x03
#define OPCODE_TYPE_UNARY 0x01
#define OPCODE_VARIANT_A 0x00
#define OPCODE_VARIANT_C 0x01
#define OPCODE_VARIANT_CF 0x02
#define OPCODE_VARIANT_CI 0x01
#define REGINDEX_ACCELX 0x1c
#define REGINDEX_ACCELY 0x1d
#define REGINDEX_ACCELZ 0x1e
#define REGINDEX_CIRCLE 0x0a
#define REGINDEX_CROSS 0x09
#define REGINDEX_DELTA 0x1b
#define REGINDEX_HAT 0x07
#define REGINDEX_IMUX 0x18
#define REGINDEX_IMUY 0x19
#define REGINDEX_IMUZ 0x1a
#define REGINDEX_L1 0x0c
#define REGINDEX_L2 0x0d
#define REGINDEX_L2VALUE 0x16
#define REGINDEX_L3 0x12
#define REGINDEX_LPADX 0x03
#define REGINDEX_LPADY 0x04
#define REGINDEX_OPTIONS 0x11
#define REGINDEX_PS 0x14
#define REGINDEX_R1 0x0e
#define REGINDEX_R2 0x0f
#define REGINDEX_R2VALUE 0x17
#define REGINDEX_R3 0x13
#define REGINDEX_RPADX 0x05
#define REGINDEX_RPADY 0x06
#define REGINDEX_SHARE 0x10
#define REGINDEX_SP 0x00
#define REGINDEX_SQUARE 0x08
#define REGINDEX_TH 0x01
#define REGINDEX_TPAD 0x15
#define REGINDEX_TRIANGLE 0x0b
#define REGINDEX_ZR 0x02
#endif /* _REGINDEX_ZR_H_ */
