#include <stdio.h>
#include <stdbool.h>

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

        if (*end + 1 == numTokens && tokens[currPos - 1].token != TOK_OP_RCURLY)
        {
                fprintf(stderr, "Missing closing bracket.\n");
                return 1;
        }
        *end = currPos;

        return 0;
}

ANTypes_t decideNodeType(bool* isNextChild, uint64_t pos, LTok_t* tokens, uint64_t end)
{
        // INFO: function declaration rules:
        // 1. starts with public/private/entry and contains ( -> entry none main(none) {}
        // INFO: variable declaration rules:
        // 1. starts with public/private and contains = -> private sint32 a = 1s;
        // 2. starts with type and contains = -> sint32 a = 1s;
        // 3. contains keyword instance and contains = -> private instance foo1 = foo(none);
        // 4. contains keyword function and contains = -> private function foo1 = foo;
        // INFO: statement rules:
        // 1. 
        // INFO: expression rules:
        //
        *isNextChild = false;

        bool startsAccModifier = false;
        bool startsEntry = false;
        bool startsVarType = false;
        bool containsLeftPar = false;
        bool containsAssign = false;
        bool containsInstance = false;
        bool containsFunction = false;

        TTypes_t firstTok = tokens[pos].token;
        while(firstTok == TOK_SPACE)
        {
                firstTok = tokens[++pos].token;
        }
        switch (firstTok)
        {
                case TOK_KEYW_PUBLIC:
                case TOK_KEYW_PRIVATE:
                        startsAccModifier = true;
                        break;
                case TOK_KEYW_ENTRY:
                        startsEntry = true;
                        break;
                default:
                        if (firstTok >= TOK_TYPE_BOOL && firstTok <= TOK_TYPE_UTF8)
                        {
                                startsVarType = true;
                        }
                        break;
        }

        for (uint64_t i = pos; i < end; i++)
        {
                TTypes_t token = tokens[i].token;
                switch (token)
                {
                        case TOK_OP_LPAR:
                                containsLeftPar = true;
                                break;
                        case TOK_OP_ASSIGN:
                                containsAssign = true;
                                break;
                        case TOK_KEYW_INSTANCE:
                                containsInstance = true;
                                break;
                        case TOK_KEYW_FUNCTION:
                                containsFunction = true;
                                break;
                        default:
                                break;
                }
        }

        if (startsEntry)
        {
                return AST_FUNC_DECLARE;
        }
        if (startsAccModifier && containsLeftPar)
        {
                return AST_FUNC_DECLARE;
        }

        if (startsAccModifier && containsAssign)
        {
                return AST_VAR_DECLARE;
        }
        if (startsVarType && containsAssign)
        {
                return AST_VAR_DECLARE;
        }
        if (containsInstance && containsAssign)
        {
                return AST_VAR_DECLARE;
        }
        if (containsFunction && containsAssign)
        {
                return AST_VAR_DECLARE;
        }

        return AST_INVALID;
}

int generateASTPass(LData_t lexerData, ANode_t* root)
{
        uint64_t pos = 0;
        uint64_t numTokens = lexerData.metadata.numTokens;

        bool isNextChild = false;
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
                        printf("end == pos\n");
                        break;
                }

                ANTypes_t nodeType = decideNodeType(&isNextChild, pos, lexerData.tokens, end);
                if (nodeType == AST_INVALID)
                {
                        fprintf(stderr, "Invalid AST Node.\n");
                        return 1;
                }
                else
                {
                        printf("Detected AST Node: %d.\n", nodeType);
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
