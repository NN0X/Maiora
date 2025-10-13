#ifndef AST_H
#define AST_H

#include <stdint.h>

enum Ast
{
        // --- KEYWORDS ---

        AST_KEYW_RETURN = 0,

        

        // control flow
        AST_KEYW_WHILE,
        AST_KEYW_FOR,
        AST_KEYW_IF,
        AST_KEYW_ELSE,
        AST_KEYW_SWITCH,
        AST_KEYW_CASE,


        // --- OPERATIONS ---

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


        // --- LITERALS ---


        AST_LIT_BOOL,
        AST_LIT_INT,
        AST_LIT_FLOAT,
        AST_LIT_CHAR,
};

typedef struct AstNode ANode_t;

typedef struct AstNode
{
        uint8_t type;
        uint64_t line;
        uint64_t pos;
        ANode_t* parent;
        ANode_t** children;
        uint16_t numChildren;
} ANode_t;

#endif // AST_H
