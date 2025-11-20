#include <stdio.h>

#include "ast.h"
#include "expressions.h"
#include "../lexer/token.h"

int generateExpressionNode(LTok_t* tokens, uint64_t begin, uint64_t end, ANode_t* node)
{
        if (tokens == NULL)
        {
                return 1;
        }
        if (node == NULL)
        {
                return 1;
        }

        // --- DEBUG ---
        for (uint64_t i = begin + 1; i < end; i++)
        {
                TTypes_t token = tokens[i].token;
                printf("<%s> ", TOKENS[token]);
        }
        printf("\n");

        return 0;
}
