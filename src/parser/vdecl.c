#include <stdio.h>
#include <stdlib.h>

#include "ast.h"

int generateVarDeclNode(LTok_t* tokens, uint64_t begin, uint64_t end, ANode_t* node)
{
        if (tokens == NULL)
        {
                return 1;
        }
        if (node == NULL)
        {
                return 1;
        }

        VTypes_t visibility = VIS_PRIVATE;
        VMods_t modifier = VAR_NONE;
        LTypes_t varType;

        AVDec_t* varDeclData = (AVDec_t*)malloc(sizeof(AVDec_t));
        if (varDeclData == NULL)
        {
                fprintf(stderr, "malloc failed for varDeclData.\n");
                return 1;
        }

        // INFO:
        // possible layouts (random order):
        // 1. <visibility> <type> <id> [initializer]
        // 2. <visibility> <modifier> <type> <id> [initializer]
        // 3. <visibility> <type> <array modifier> [initializer]
        // 4. <type> <id> [initializer]
        // 5. <modifier> <id> [initializer]
        // 6. <type> <array modifier> [initializer]
        // special cases for instance creation with ID as type:
        // 7. <visibility> <id> <id> [initializer]
        // 8. <id> <id> [initializer]
        // doesn't fit layout -> error

        for (uint64_t i = begin + 1; i < end; i++)
        {
                TTypes_t token = tokens[i].token;
                printf("<%s> ", TOKENS[token]);
        }
        printf("\n");

        return 0;
}
