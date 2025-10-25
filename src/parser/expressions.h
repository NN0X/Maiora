#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include <stdbool.h>

#include "ast.h"

typedef struct AstLiteralExpression
{
        LTypes_t type;
        void* value;
} ALitExpr_t;

typedef struct AstArrayLiteralExpression
{
        LTypes_t type;
        ANode_t* values;
} AArrLitExpr_t;

typedef struct AstIdExpression
{
        char* name;
        uint64_t nameLen;

        LTypes_t type;
        bool isFunc;
        bool isInstance;
} AIdExpr_t;

typedef enum UnaryOperationTypes
{
        UOP_NOT,             // !
        UOP_BITWISE_NOT,     // ~
        UOP_INCREMENT,       // ++
        UOP_DECREMENT,       // --
} UOpTypes_t;

typedef struct AstUnaryExpression
{
        UOpTypes_t type;
        ANode_t* target;
} AUnryExpr_t;

typedef enum BinaryOperationTypes
{
        BOP_EQUALS,          // ==
        BOP_LESS,            // <
        BOP_MORE,            // >
        BOP_LESS_EQUALS,     // <=
        BOP_MORE_EQUALS,     // >=
        BOP_NOT_EQUALS,      // !=
        BOP_AND,             // &&
        BOP_OR,              // ||

        BOP_ASSIGN,          // =
        BOP_ADD,             // +
        BOP_SUBTRACT,        // -
        BOP_DIVIDE,          // /
        BOP_MULTIPLY,        // *
        BOP_MODULO,          // %
        BOP_BITWISE_AND,     // &
        BOP_BITWISE_OR,      // |
        BOP_BITWISE_XOR,     // ^
        BOP_SHIFT_LEFT,      // <<
        BOP_SHIFT_RIGHT,     // >>
} BOpTypes_t;

typedef struct AstBinaryExpression
{
        BOpTypes_t type;
        ANode_t* targetLeft;
        ANode_t* targetRight;
} ABinrExpr_t;

typedef struct AstArrayAccessExpression
{
        ANode_t* target;
        ANode_t* index;
} AArrAccExpr_t;

typedef struct AstMemberAccessExpression
{
        ANode_t* target;
        ANode_t* member;
} AMbrAccExpr_t;

typedef struct AstFunctionCallExpression
{
        ANode_t* target;

        ANode_t* parameters;
        uint64_t parametersNum;
} AFCallExpr_t;

typedef struct AstHeapAllocExpression
{
        LTypes_t type;
        uint64_t size;
        ANode_t* initializer;
} AHAllocExpr_t;

typedef struct AstReferenceExpression
{
        ANode_t* target;
} ARefExpr_t;

typedef struct AstInterpolatedStringExpression
{
        LTypes_t type;
        ANode_t* body;     // "num = {x}" -> 'n', 'u', 'm', ' ', '=', ' ', x
        uint64_t bodyNum;
} AIntrStrExpr_t;

typedef struct AstTypeCastExpression
{
        LTypes_t type;
        ANode_t* target;
} ATCastExpr_t;

typedef struct AstGroupExpression
{
        ANode_t* body;
        uint64_t bodyNum;
} AGrpExpr_t;

#endif // EXPRESSIONS_H
