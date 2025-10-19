#ifndef AST_H
#define AST_H

#include <stdint.h>
#include <stdbool.h>

typedef enum AstNodeTypes
{
        AST_ROOT,           // the root of the AST
        AST_FUNC_DECLARE,   // function declaration
        AST_VAR_DECLARE,    // int8 x
        AST_EXPRESSION,     // binary ops, literals
        AST_STATEMENT,      // if, while, return
} ANTypes_t;

typedef struct AstNode ANode_t;

typedef struct AstNode
{
        ANTypes_t type;

        uint64_t line;
        uint64_t pos;

        ANode_t* parent;

        void* astData;
} ANode_t;

typedef struct AstRoot
{
        ANode_t** body;
        uint64_t bodyNum;
} ARoot_t;

typedef enum VisibilityTypes
{
        VIS_PRIVATE,
        VIS_PUBLIC,
} VTypes_t;

typedef enum LiteralTypes
{
        // TODO: enter types
} LTypes_t;

typedef struct AstFunctionDeclare
{
        char* name;
        uint64_t nameLen;

        VTypes_t visibility;
        bool isEntry;
        ANode_t* body;
        LTypes_t returnType;

        ANode_t** params;
        uint64_t numParams;
} AFDec_t;

typedef enum VariableModifiers
{
        VAR_ADDRESS,
        VAR_REFERENCE,
} VMods_t;

typedef struct AstVariableDeclare
{
        char* name;
        uint64_t nameLen;

        VTypes_t visibility;
        VMods_t modifier;
        LTypes_t type;

        ANode_t* initializer;
} AVDec_t;

typedef enum StatementTypes
{
        STMT_SCOPE,

        STMT_UNSAFE,

        STMT_RETURN,

        STMT_IF,
        STMT_ELSE,
        STMT_ELIF,

        STMT_FOR,
        STMT_FORAN,
        STMT_WHILE,

        STMT_ASSIGN,

        STMT_SWITCH,
        STMT_CASE,
} STypes_t;

typedef struct AstStatement
{
        STypes_t type;
        void* stmtData;
} AStmt_t;

typedef enum ExpressionTypes
{
        EXPR_LITERAL,   // 5s
        EXPR_ID,        // foo

        EXPR_UNARY,     // x++
        EXPR_BINARY,    // x + y

        EXPR_ARRLITERAL // [1s, 2s, 3s]
        EXPR_ARRACCES,  // foo[0]

        EXPR_MEMBACCES, // foo.a
        EXPR_CALL,      // foo(1s)

        EXPR_HEAPALLOC, // heap 1*[0s]
        EXPR_REF,       // ref value

        EXPR_INTERSTR   // ascii"num = {x}"

        EXPR_TYPECAST,   // (sint8)value

        EXPR_GROUP,       // (a + b)
} ETypes_t;

typedef struct AstExpression
{
        ETypes_t type;
        void* exprData;
} AExpr_t;

#endif // AST_H
