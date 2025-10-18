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
} ANodeTypes_t;

typedef struct AstNode ANode_t;

typedef struct AstNode
{
        uint8_t type;       // AstNodeType

        uint64_t line;
        uint64_t pos;

        ANode_t* parent;

        void* astData;
} ANode_t;

typedef enum VisibilityTypes
{
        VIS_PRIVATE,
        VIS_PUBLIC,
} VTypes_t;

typedef enum LiteralTypes
{

} LTypes_t;

typedef struct AstFunctionDeclare
{
        char* name;
        uint64_t nameLen;

        VTypes_t visibility;
        bool isEntry;
        LTypes_t returnType;

        ANode_t** params;
        uint64_t numParams;

        ANode_t** body;
        uint64_t numBody;
} AFDec_t;

typedef enum VariableModifiers
{
        VAR_ADDRESS,
        VAR_REFERENCE,
} VMods_t;

// TODO: finish below
typedef struct AstVariableDeclare
{
        char* name;
        uint64_t nameLen;

        VTypes_t visibility;
        VMods_t modifier;
        LTypes_t type;

        ANode_t* initializer;
} AVDec_t;

#endif // AST_H
