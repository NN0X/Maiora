#ifndef AST_H
#define AST_H

#include <stdint.h>
#include <stdbool.h>

#include "../lexer/lexer.h"

typedef enum AstNodeTypes
{
        AST_INVALID = -1,
        AST_EMPTY = 0,      // child node for scopes with no content
        AST_ROOT,           // the root of the AST
        AST_FUNC_DECLARE,   // function declaration
        AST_VAR_DECLARE,    // int8 x
        AST_STATEMENT,      // if, while, return
        AST_EXPRESSION,     // binary ops, literals
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
        LIT_SINT8,         // sint8
        LIT_SINT16,        // sint16
        LIT_SINT32,        // sint32
        LIT_SINT64,        // sint64
        LIT_UINT8,         // uint8
        LIT_UINT16,        // uint16
        LIT_UINT32,        // uint32
        LIT_UINT64,        // uint64

        // floats
        LIT_FLOAT8,        // float8
        LIT_FLOAT16,       // float16
        LIT_FLOAT32,       // float32
        LIT_FLOAT64,       // float64

        // characters
        LIT_ASCII,         // ascii
        LIT_UTF8,          // utf8
} LTypes_t;

#define LARGEST_LIT_INT_TYPE LIT_SINT64
#define LARGEST_LIT_FLOAT_TYPE LIT_FLOAT64
#define LARGEST_LIT_CHAR_TYPE LIT_UTF8

typedef struct AstFunctionDeclare
{
        char* name;
        uint64_t nameLen;

        VTypes_t visibility;
        bool isEntry;
        ANode_t* scope;
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
        // unscoped
        STMT_SCOPE,
        STMT_RETURN,
        STMT_ASSIGN,

        STMT_META_SCOPED,

        // scoped
        STMT_UNSAFE,
        STMT_ASM,

        STMT_IF,
        STMT_ELSE,
        STMT_ELIF,

        STMT_FOR,
        STMT_FORAN,
        STMT_WHILE,

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

        EXPR_ARRLITERAL,// [1s, 2s, 3s]
        EXPR_ARRACCES,  // foo[0]

        EXPR_MEMBACCES, // foo.a
        EXPR_CALL,      // foo(1s)

        EXPR_HEAPALLOC, // heap 1*[0s]
        EXPR_REF,       // ref value

        EXPR_INTERSTR,  // ascii"num = {x}"

        EXPR_TYPECAST,  // (sint8)value

        EXPR_GROUP,     // (a + b)
} ETypes_t;

typedef struct AstExpression
{
        ETypes_t type;
        void* exprData;
} AExpr_t;

typedef struct PassesBoundaries
{
        uint64_t size;
        uint64_t offset;

        uint64_t* begins;
        uint64_t* ends;
        ANode_t** parentNodes;
} PBound_t;

int generateAST(LData_t lexerData, ANode_t* root);

#endif // AST_H
