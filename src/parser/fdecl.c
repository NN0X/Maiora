#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "ast.h"

// TODO: move requires to a struct

typedef enum FunctionDeclarationGenerationSteps
{
        FUNC_VIS_OR_TYPE_STEP = 0,
        FUNC_TYPE_STEP,
        FUNC_MODULE_OR_ID_STEP,
        FUNC_ID_STEP,
        FUNC_PARAMS_STEP,
        FUNC_END_STEP,
} FDGSteps_t;

int funcDeclTypeStep(TTypes_t token, LTypes_t* type, FDGSteps_t* step, bool* requiresSpace)
{
        if (type == NULL)
        {
                return 1;
        }
        if (step == NULL)
        {
                return 1;
        }
        if (requiresSpace == NULL)
        {
                return 1;
        }

        if (token < TOK_TYPE_NONE || token > TOK_TYPE_UTF8)
        {
                fprintf(stderr, "expected type.\n");
                return 1;
        }

        switch (token)
        {
                case TOK_TYPE_NONE:
                        *type = LIT_NONE;
                        break;
                case TOK_TYPE_BOOL:
                        *type = LIT_BOOL;
                        break;
                case TOK_TYPE_SINT8:
                        *type = LIT_SINT8;
                        break;
                case TOK_TYPE_SINT16:
                        *type = LIT_SINT16;
                        break;
                case TOK_TYPE_SINT32:
                        *type = LIT_SINT32;
                        break;
                case TOK_TYPE_SINT64:
                        *type = LIT_SINT64;
                        break;
                case TOK_TYPE_UINT8:
                        *type = LIT_UINT8;
                        break;
                case TOK_TYPE_UINT16:
                        *type = LIT_UINT16;
                        break;
                case TOK_TYPE_UINT32:
                        *type = LIT_UINT32;
                        break;
                case TOK_TYPE_UINT64:
                        *type = LIT_UINT64;
                        break;
                case TOK_TYPE_FLOAT8:
                        *type = LIT_FLOAT8;
                        break;
                case TOK_TYPE_FLOAT16:
                        *type = LIT_FLOAT16;
                        break;
                case TOK_TYPE_FLOAT32:
                        *type = LIT_FLOAT32;
                        break;
                case TOK_TYPE_FLOAT64:
                        *type = LIT_FLOAT64;
                        break;
                case TOK_TYPE_ASCII:
                        *type = LIT_ASCII;
                        break;
                case TOK_TYPE_UTF8:
                        *type = LIT_UTF8;
                        break;
                default:
                        return 1;
        }

        *step = FUNC_MODULE_OR_ID_STEP;
        *requiresSpace = true;

        return 0;
}

int funcDeclVisibilityOrTypeStep(TTypes_t token, VTypes_t* visibility, LTypes_t* type, FDGSteps_t* step, bool* requiresSpace)
{
        if (visibility == NULL)
        {
                return 1;
        }
        if (type == NULL)
        {
                return 1;
        }
        if (step == NULL)
        {
                return 1;
        }
        if (requiresSpace == NULL)
        {
                return 1;
        }

        if (token >= TOK_KEYW_PUBLIC && token <= TOK_KEYW_ENTRY)
        {
                switch (token)
                {
                        case TOK_KEYW_PUBLIC:
                                *visibility = VIS_PUBLIC;
                                break;
                        case TOK_KEYW_PRIVATE:
                                *visibility = VIS_PRIVATE;
                                break;
                        case TOK_KEYW_ENTRY:
                                *visibility = VIS_ENTRY;
                                break;
                        default:
                                return 1;
                }
                *step = FUNC_TYPE_STEP;
                *requiresSpace = true;
        }
        else if (token >= TOK_TYPE_NONE && token <= TOK_TYPE_UTF8)
        {
                if (funcDeclTypeStep(token, type, step, requiresSpace) != 0)
                {
                        return 1;
                }
        }
        else
        {
                fprintf(stderr, "expected vis or type.\n");
                return 1;
        }

        return 0;
}

int funcDeclModuleStep(LTok_t token, AFDec_t* data, FDGSteps_t* step, bool* requiresFromModule)
{
        if (data == NULL)
        {
                return 1;
        }
        if (step == NULL)
        {
                return 1;
        }
        if (requiresFromModule == NULL)
        {
                return 1;
        }

        if(token.len <= 0)
        {
                return 1;
        }

        data->module = (char*)malloc(sizeof(char) * token.len);
        if (data->module == NULL)
        {
                return 1;
        }
        data->moduleLen = token.len;

        *step = FUNC_ID_STEP;
        *requiresFromModule = true;

        return 0;
}

int funcDeclIDStep(LTok_t token, AFDec_t* data, FDGSteps_t* step, bool* requiresLPar)
{
        if (data == NULL)
        {
                return 1;
        }
        if (step == NULL)
        {
                return 1;
        }
        if (requiresLPar == NULL)
        {
                return 1;
        }

        if(token.len <= 0)
        {
                return 1;
        }

        data->name = (char*)malloc(sizeof(char) * token.len);
        if (data->name == NULL)
        {
                return 1;
        }
        data->nameLen = token.len;

        *step = FUNC_PARAMS_STEP;
        *requiresLPar = true;

        return 0;
}

int funcDeclParamsStep(LTok_t* tokens, uint64_t* begin, uint64_t end, AFDec_t* data,
                       FDGSteps_t* step, bool* requiresRPar)
{
        if (tokens == NULL)
        {
                return 1;
        }
        if (begin == NULL)
        {
                return 1;
        }
        if (data == NULL)
        {
                return 1;
        }
        if (*begin >= end)
        {
                return 1;
        }

        data->params = (ANode_t**)malloc(sizeof(ANode_t*) * (end - *begin));
        if (data->params == NULL)
        {
                return 1;
        }
        data->numParams = 0;

        uint64_t currentBegin = *begin;
        for (uint64_t i = *begin; i < end - 1; i++)
        {
                TTypes_t nextToken = tokens[i + 1].token;
                if (nextToken == TOK_OP_RPAR)
                {
                        if (tokens[i].token == TOK_TYPE_NONE)
                        {
                                break;
                        }
                        ANode_t* paramNode = (ANode_t*)malloc(sizeof(ANode_t));
                        if (paramNode == NULL)
                        {
                                return 1;
                        }
                        if (generateVarDeclNode(tokens, currentBegin, i, paramNode) != 0)
                        {
                                return 1;
                        }
                        data->params[data->numParams] = paramNode;
                        data->numParams++;

                        *begin = i;
                        break;
                }

                if (nextToken == TOK_OP_COMMA)
                {
                        ANode_t* paramNode = (ANode_t*)malloc(sizeof(ANode_t));
                        if (paramNode == NULL)
                        {
                                return 1;
                        }
                        if (generateVarDeclNode(tokens, currentBegin, i, paramNode) != 0)
                        {
                                return 1;
                        }
                        data->params[data->numParams] = paramNode;
                        data->numParams++;

                        currentBegin = i + 1;
                }
        }

        *step = FUNC_END_STEP;
        *requiresRPar = true;

        return 0;
}

int generateFuncDeclNode(LTok_t* tokens, uint64_t begin, uint64_t end, ANode_t* node)
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
        LTypes_t returnType;

        AFDec_t* funcDeclData = (AFDec_t*)malloc(sizeof(AFDec_t));
        if (funcDeclData == NULL)
        {
                fprintf(stderr, "malloc failed for funcDeclData.\n");
                return 1;
        }

        // INFO:
        // possible layouts (in order of canonicity):
        // 1. <visibility> <type> <id>::<id>([params])
        // 2. <visibility> <type> <id>([params])
        // 3. <type> <id>::<id>([params])
        // 4. <type> <id>([params])
        // doesn't fit layout -> error

        FDGSteps_t step = 0;
        bool requiresSpace = false;
        bool requiresFromModule = false;
        bool requiresLPar = false;
        bool requiresRPar = false;
        for (uint64_t i = begin + 1; i < end; i++)
        {
                TTypes_t token = tokens[i].token;
                printf("<%s> ", TOKENS[token]);

                if (requiresSpace && token != TOK_SPACE)
                {
                        fprintf(stderr, "expected <tok_space>, got <%s>.\n", TOKENS[token]);
                        return 1;
                }
                if (token == TOK_SPACE && requiresSpace)
                {
                        requiresSpace = false;
                        continue;
                }
                else if (token == TOK_SPACE)
                {
                        continue;
                }

                if (requiresFromModule && token != TOK_OP_FROM_MODULE)
                {
                        fprintf(stderr, "expected <::>.\n");
                        return 1;
                }
                if (token == TOK_OP_FROM_MODULE && requiresFromModule)
                {
                        requiresFromModule = false;
                        continue;
                }

                if (requiresLPar && token != TOK_OP_LPAR)
                {
                        fprintf(stderr, "expected <(>.\n");
                        return 1;
                }
                if (token == TOK_OP_LPAR && requiresLPar)
                {
                        requiresLPar = false;
                        continue;
                }

                if (requiresRPar && token != TOK_OP_RPAR)
                {
                        fprintf(stderr, "expected <)>.\n");
                        return 1;
                }
                if (token == TOK_OP_RPAR && requiresRPar)
                {
                        requiresRPar = false;
                        continue;
                }

                switch (step)
                {
                        case FUNC_VIS_OR_TYPE_STEP:
                                if (funcDeclVisibilityOrTypeStep(token, &visibility, &returnType, &step, &requiresSpace) != 0)
                                {
                                        return 1;
                                }
                                break;
                        case FUNC_TYPE_STEP:
                                if (funcDeclTypeStep(token, &returnType, &step, &requiresSpace) != 0)
                                {
                                        return 1;
                                }
                                break;
                        case FUNC_MODULE_OR_ID_STEP:
                                if (token != TOK_ID)
                                {
                                        fprintf(stderr, "expected id.\n");
                                        return 1;
                                }
                                if (i + 1 < end && tokens[i + 1].token == TOK_OP_FROM_MODULE)
                                {
                                        if (funcDeclModuleStep(tokens[i], funcDeclData, &step, &requiresFromModule) != 0)
                                        {
                                                return 1;
                                        }
                                }
                                else
                                {
                                        if (funcDeclIDStep(tokens[i], funcDeclData, &step, &requiresLPar) != 0)
                                        {
                                                return 1;
                                        }
                                }
                                break;
                        case FUNC_ID_STEP:
                                if (token != TOK_ID)
                                {
                                        fprintf(stderr, "expected id.\n");
                                        return 1;
                                }
                                if (funcDeclIDStep(tokens[i], funcDeclData, &step, &requiresLPar) != 0)
                                {
                                        return 1;
                                }
                                break;
                        case FUNC_PARAMS_STEP:
                                if (funcDeclParamsStep(tokens, &i, end, funcDeclData, &step, &requiresRPar) != 0)
                                {
                                        return 1;
                                }
                                break;
                        default:
                                fprintf(stderr, "unexpected step.\n");
                                return 1;
                }
        }
        printf("\n");

        return 0;
}

