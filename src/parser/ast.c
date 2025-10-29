#include <stdio.h>

#include "ast.h"
#include "../lexer/lexer.h"
#include "../lexer/token.h"

int getGroup(uint64_t* end, LTok_t* tokens, uint64_t numTokens, uint64_t pos)
{
        uint64_t currPos = pos;
        while (currPos != numTokens)
        {
                if (tokens[currPos].token == TOK_OP_SEMICOLON || tokens[currPos].token == TOK_OP_LCURLY)
                {
                        *end = currPos;
                        return 0;
                }
                currPos++;
        }

        if (*end + 1 == numTokens && tokens[currPos].token != TOK_OP_RCURLY)
        {
                fprintf(stderr, "Missing closing bracket.\n");
                return 1;
        }
        *end = currPos;

        return 0;
}

ANTypes_t decideNodeType(uint64_t pos, LTok_t* tokens, uint64_t numTokens)
{
        // INFO: starts with public/private/entry and contains ( -> function declaration
        // INFO: starts with public/private or type and contains = -> variable declaration
        // INFO: check if it's a statement
        // INFO: if none of the above, it's an expression
        return AST_INVALID;
}

int generateASTPass(LData_t lexerData, ANode_t* root)
{
        uint64_t pos = 0;
        uint64_t numTokens = lexerData.metadata.numTokens;
        while (pos != numTokens)
        {
                uint64_t end;
                if (getGroup(&end, lexerData.tokens, numTokens, pos) == 1)
                {
                        fprintf(stderr, "Missing statement closing symbol: ; {  }.\n");
                        return 1;
                }

                if (end == pos)
                {
                        break;
                }

                ANTypes_t nodeType = decideNodeType(pos, lexerData.tokens, end);
                if (nodeType == AST_INVALID)
                {
                        fprintf(stderr, "Invalid AST Node.\n");
                        return 1;
                }

                switch (nodeType)
                {
                        case AST_FUNC_DECLARE:
                                // INFO: create function declaration node and attach to root
                                break;
                        case AST_VAR_DECLARE:
                                // INFO: create variable declaration node and attach to root
                                break;
                        case AST_STATEMENT:
                                // INFO: create statement node and attach to root
                                break;
                        case AST_EXPRESSION:
                                // INFO: create expression node and attach to root
                                break;
                        default:
                                fprintf(stderr, "Unhandled AST Node Type.\n");
                                return 1;
                }


                pos = end;
        }

        return 0;
}

int getASTSize(uint64_t* size, ANode_t* root)
{
        // INFO: traverse the AST and count nodes
        return 0;
}

int generateAST(LData_t lexerData, ANode_t* root)
{
        // INFO: call generateASTPass until no changes are made

        uint64_t size = 1;
        uint64_t prevSize = 0;
        while (size != prevSize)
        {
                prevSize = size;
                if (generateASTPass(lexerData, root) == 1)
                {
                        fprintf(stderr, "Failed to generate AST.\n");
                        return 1;
                }
                if (getASTSize(&size, root) == 1)
                {
                        fprintf(stderr, "Failed to get AST size.\n");
                        return 1;
                }
        }

        return 0;
}
