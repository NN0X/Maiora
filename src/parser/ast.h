#ifndef AST_H
#define AST_H

#include <stdint.h>

enum Ast
{
        // --- KEYWORDS ---

        // control flow
        AST_WHILE,
        AST_FOR,
        AST_IF,
        AST_ELSE,
        AST_SWITCH,
        AST_CASE,
};

typedef struct AstNode ANode_t;

typedef struct AstNode
{
        uint8_t type;
        ANode_t* parent;
        ANode_t** children;
} ANode_t;

#endif // AST_H
