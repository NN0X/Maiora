#include <stdio.h>

#include "ast.h"
#include "../lexer/lexer.h"
#include "../lexer/token.h"

int getGroup(uint64_t* end, LTok_t* tokens, uint64_t numTokens, uint64_t pos)
{
        uint64_t currPos = pos;
        while (currPos != numTokens)
        {
                if (tokens[currPos] == TOK_OP_SEMICOLON || tokens[currPos] == TOK_OP_LCURLY)
                {
                        *end = currPos;
                        return 0;
                }
                currPos++;
        }

        if (*end + 1 == numTokens && tokens[currPos] != TOK_OP_RCURLY)
        {
                fprintf(stderr, "Missing closing bracket.\n");
                return 1;
        }
        *end = currPos;

        return 0;
}

ANTypes_t decideNodeType(uint64_t pos, LTok_t* tokens, uint64_t numTokens)
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
                        fprintf(stderr, "Missing statement closing symbol: ; {  }.\n");
                        return 1;
                }

                ANTypes_t nodeType = decideNodeType(pos, lexerData.tokens, end);
                if (nodeType == AST_INVALID)
                {
                        fprintf(stderr, "Invalid AST Node");
                        return 1;
                }
                pos = end;
        }

        return 0;
}
