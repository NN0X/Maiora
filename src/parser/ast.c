#include <stdio.h>

#include "ast.h"
#include "../lexer/lexer.h"

int getGroup(uint64_t* end, LTok_t* tokens, uint64_t numTokens, uint64_t pos)
{
        // TODO: find first group separated by ; or {
        return 0;
}

ANTypes_t decideNodeType(uint64_t* pos, LTok_t* tokens, uint64_t numTokens)
{
        return AST_INVALID;
}

int generateAST(LData_t lexerData, uint64_t numTokens, ANode_t* root)
{
        uint64_t pos = 0;
        while (pos != numTokens)
        {
                uint64_t end;
                if (getGroup(&end, lexerData.tokens, numTokens, pos) == 1)
                {
                        fprintf(stderr, "There are tokens with no ; or { to specify group end.");
                        return 1;
                }

                ANTypes_t nodeType = decideNodeType(&pos, lexerData.tokens, end);
                if (nodeType == AST_INVALID)
                {
                        fprintf(stderr, "Invalid AST Node");
                        return 1;
                }
        }

        return 0;
}
