#ifndef AST_H
#define AST_H

#include <stdint.h>
#include <stdbool.h>

enum AstNodeType
{
        AST_ROOT,           // the root of the AST
        AST_FUNC_DECLARE,   // function declaration
        AST_VAR_DECLARE,    // int8 x
        AST_EXPRESSION,     // binary ops, literals
        AST_STATEMENT,      // if, while, return
};

typedef struct AstNode ANode_t;

typedef struct AstNode
{
        uint8_t type;       // AstNodeType

        uint64_t line;
        uint64_t pos;

        ANode_t* parent;

        void* astData;
} ANode_t;

enum Visibilities
{
        VIS_PRIVATE,
        VIS_PUBLIC,
};

enum LiteralTypes
{

};

typedef struct AstFunctionDeclare
{
        char* name;
        uint64_t nameLen;

        uint8_t visibility;     // Visibilities
        bool isEntry;
        uint8_t returnType;     // LiteralTypes

        ANode_t** params;
        uint64_t numParams;

        ANode_t** body;
        uint64_t numBody;
} AFDec_t;

enum VariableModifiers
{
        VAR_ADDRESS,
        VAR_REFERENCE,
};

// TODO: finish below
typedef struct AstVariableDeclare
{
        char* name;
        uint64_t nameLen;

        uint8_t visibility;     // Visibilities
        uint8_t modifier;       // VariableModifiers
        uint8_t type;           // LiteralTypes

        ANode_t* initializer;
} AVDec_t;

#endif // AST_H
