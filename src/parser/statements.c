#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "statements.h"
#include "../defines.h"
#include "../lexer/token.h"

int generateScopeNode(ANode_t* node)
{
        if (node == NULL)
        {
                fprintf(stderr, "generateScopeNode: node is NULL.\n");
                return 1;
        }

        node->type = AST_STATEMENT;
        AStmt_t* stmt = (AStmt_t*)malloc(sizeof(AStmt_t));
        if (stmt == NULL)
        {
                fprintf(stderr, "malloc failed for stmt.\n");
                return 1;
        }

        AScpStmt_t* scopeData = (AScpStmt_t*)malloc(sizeof(AScpStmt_t));
        if (scopeData == NULL)
        {
                fprintf(stderr, "malloc failed for scopeData.\n");
                return 1;
        }
        scopeData->body = (ANode_t**)malloc(MAX_STATEMENT_LEN * sizeof(ANode_t*));
        if (scopeData->body == NULL)
        {
                fprintf(stderr, "malloc failed for scopeData->body.\n");
                return 1;
        }
        scopeData->bodyNum = 0;

        stmt->type = STMT_SCOPE;
        stmt->stmtData = (void*)scopeData;

        node->astData = (void*)stmt;

        return 0;
}

int isStatementScoped(ANode_t* node, bool* result)
{
        if (node == NULL)
        {
                fprintf(stderr, "isStatementScoped: node is NULL.\n");
                return 1;
        }
        if (result == NULL)
        {
                fprintf(stderr, "isStatementScoped: result is NULL.\n");
                return 1;
        }

        AStmt_t* stmt = (AStmt_t*)(node->astData);
        if (stmt == NULL)
        {
                fprintf(stderr, "isStatementScoped: statement is NULL.\n");
                return 1;
        }

        if (stmt->type < STMT_META_SCOPED)
        {
                *result = false;
        }
        else if (stmt->type > STMT_META_SCOPED)
        {
                *result = true;
        }
        else
        {
                fprintf(stderr, "isStatementScoped: statement type is META_SCOPED.\n");
                return 1;
        }

        return 0;
}

int linkNodeToStatement(ANode_t* node, ANode_t* parent)
{
        if (node == NULL)
        {
                fprintf(stderr, "linkNodeToStatement: node is NULL.\n");
                return 1;
        }
        if (parent == NULL)
        {
                fprintf(stderr, "linkNodeToStatement: parent is NULL.\n");
                return 1;
        }

        return 0;
}

int generateStatementNode(LTok_t* tokens, uint64_t begin, uint64_t end, ANode_t* node)
{
        if (tokens == NULL)
        {
                return 1;
        }
        if (node == NULL)
        {
                return 1;
        }

        // TODO: decide statement type and enter appropriate generation function

        // --- DEBUG ---
        for (uint64_t i = begin + 1; i < end; i++)
        {
                TTypes_t token = tokens[i].token;
                printf("<%s> ", TOKENS[token]);
        }
        printf("\n");

        return 0;
}
