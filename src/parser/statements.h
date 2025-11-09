#ifndef STATEMENTS_H
#define STATEMENTS_H

#include "ast.h"

typedef struct AstScopeStatement
{
        ANode_t** body;
        uint64_t bodyNum;
} AScpStmt_t;

typedef struct AstUnsafeStatement
{
        ANode_t* scope;
} AUsfStmt_t;

typedef struct AstReturnStatement
{
        ANode_t* value;
} ARetStmt_t;

typedef struct AstIfStatement
{
        ANode_t* condition;
        ANode_t* scope;

        ANode_t** elses;
        uint64_t numElses;
} AIfStmt_t;

typedef struct AstElseStatement
{
        ANode_t* scope;
} AElStmt_t;

typedef struct AstElifStatement
{
        ANode_t* condition;
        ANode_t* scope;
} AEifStmt_t;

typedef struct AstForStatement
{
        ANode_t* initializer;
        ANode_t* condition;
        ANode_t* update;
        ANode_t* scope;
} AForStmt_t;

typedef struct AstForRangeStatement
{
        ANode_t* iterator;
        ANode_t* target;
        ANode_t* scope;
} AFRanStmt_t;

typedef struct AstWhileStatement
{
        ANode_t* condition;
        ANode_t* scope;
} AWhStmt_t;

typedef struct AstAssignStatement
{
        ANode_t* target;
        ANode_t* initializer;
} AAsgnStmt_t;

typedef struct AstSwitchStatement
{
        ANode_t* switcher;

        ANode_t** cases;
        uint64_t casesNum;
} ASwiStmt_t;

typedef struct AstCaseStatement
{
        ANode_t** body;
        uint64_t bodyNum;
} ACasStmt_t;

typedef struct AstSwitchElseStatement
{
        ANode_t** body;
        uint64_t bodyNum;
} ASwElStmt_t;

int generateScopeNode(ANode_t* node);
int isStatementScoped(ANode_t* node, bool* result);
int linkNodeToStatement(ANode_t* node, ANode_t* parent);

#endif // STATEMENTS_H
