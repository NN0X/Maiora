#ifndef AST_H
#define AST_H

#include <stdint.h>

enum Ast
{
        AST_ROOT = 0,


        // --- KEYWORDS ---

        // modifiers
        AST_KEYW_RETURN,
        AST_KEYW_PUBLIC,
        AST_KEYW_PRIVATE,
        AST_KEYW_ENTRY,

        // functions
        AST_KEYW_INSTANCE,
        AST_KEYW_FUNCTION,

        // memory
        AST_KEYW_HEAP,
        AST_KEYW_ADDRESS,
        AST_KEYW_REFERENCE,
        AST_KEYW_REF,
        AST_KEYW_AT,

        // control flow
        AST_KEYW_WHILE,
        AST_KEYW_FOR,
        AST_KEYW_IF,
        AST_KEYW_ELSE,
        AST_KEYW_SWITCH,
        AST_KEYW_CASE,


        // --- OPERATIONS ---

        // comparators
        AST_OP_EQUALS,
        AST_OP_LESS,
        AST_OP_MORE,
        AST_OP_LESS_EQUALS,
        AST_OP_MORE_EQUALS,
        AST_OP_NOT_EQUALS,
        AST_OP_AND,
        AST_OP_OR,
        AST_OP_NOT,

        // accessors
        AST_OP_FROM,
        AST_OP_IN,
        AST_OP_FROM_MODULE,

        // modifiers

        AST_OP_ASSIGN,
        AST_OP_ADD,
        AST_OP_SUBTRACT,
        AST_OP_DIVIDE,
        AST_OP_MULTIPLY,
        AST_OP_MODULO,
        AST_OP_INCREMENT,
        AST_OP_DECREMENT,
        AST_OP_ADD_ASSIGN,
        AST_OP_SUB_ASSIGN,
        AST_OP_DIV_ASSIGN,
        AST_OP_MUL_ASSIGN,
        AST_OP_BITWISE_AND,


        // -- TYPES ---

        AST_TYPE_NONE,
        AST_TYPE_BOOL,

        // integers
        AST_TYPE_SINT8,
        AST_TYPE_SINT16,
        AST_TYPE_SINT32,
        AST_TYPE_SINT64,
        AST_TYPE_UINT8,
        AST_TYPE_UINT16,
        AST_TYPE_UINT32,
        AST_TYPE_UINT64,

        // floats
        AST_TYPE_FLOAT8,
        AST_TYPE_FLOAT16,
        AST_TYPE_FLOAT32,
        AST_TYPE_FLOAT64,

        // characters
        AST_TYPE_ASCII,
        AST_TYPE_UTF8,


        // --- LITERALS ---

        AST_LIT_INT,
        AST_LIT_SINT,
        AST_LIT_UINT,
        AST_LIT_BOOL,
        AST_LIT_FLOAT,
        AST_LIT_FLOAT32,
        AST_LIT_FLOAT64,
        AST_LIT_CHAR,


        // -- IDENTIFIERS ---

        AST_ID,
};

typedef struct AstNode ANode_t;

typedef struct AstNode
{
        uint8_t type;

        uint64_t line;
        uint64_t pos;

        ANode_t* parent;
        ANode_t** children;
        uint64_t numChildren;

        void* value;
        uint64_t valueLen;
} ANode_t;

#endif // AST_H
