#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "token.h"
#include "lexer.h"
#include "loader.h"

int tokenFit(LTok_t* token, char* data, int dataLen)
{
        return 1;
}

int generateTokens(LTok_t* tokens, char* statement, uint64_t begin, uint64_t end, uint64_t* num)
{
        *num = 0;

        char* temp = statement + begin;
        printf("Generating token from: ");
        fwrite(temp, 1, end - begin, stdout);
        printf("\n");

        char* tempData = (char*)malloc(MAX_STATEMENT_LEN);
        if (tempData == NULL)
        {
                fprintf(stderr, "Memory allocation failed for tokens");
                return 1;
        }

        // FIX: rewrite below loop to pass whole statement to tokenFit
        // INFO: tokenFit should match tokens to data so the chosen token is the longest possible match
        uint64_t tempDataPos = 0;
        for (uint64_t i = begin; i < end; i++)
        {
                tempData[tempDataPos] = statement[i];
                tempDataPos++;

                // ex. statement: "sint32 var = 5s" -> tokens = [TOK_TYPE_SINT32, TOK_ID_VARIABLE, TOK_OP_ASSIGN, TOK_LIT_SINT32]

                // TODO: here compare with keywords etc and create tokens if applicable

                LTok_t token;
                if (tokenFit(&token, tempData, tempDataPos) == 0)
                {
                        tokens[*num] = token;
                        (*num)++;
                        free(tempData);
                        tempDataPos = 0;
                        tempData = (char*)malloc(MAX_STATEMENT_LEN);
                }
        }

        free(tempData);

        return 0;
}

int tokenizeSource(LData_t* lexerData, char* src, LMeta_t* metadata)
{
        lexerData->metadata = *metadata;
        lexerData->tokens = (LTok_t*)malloc(sizeof(LTok_t) * metadata->fileSize);
        metadata->numTokens = 0;
        if (lexerData->tokens == NULL)
        {
                fprintf(stderr, "Memory allocation failed for tokens");
                return 1;
        }


        // INFO: statement is a line of code between semicolons or before curlies

        uint64_t statementNum = 0;
        uint64_t line = 1;
        char* statement = (char*)malloc(MAX_STATEMENT_LEN + 1);

        uint64_t pos = 0;
        for (uint64_t i = 0; i < metadata->fileSize; i++)
        {
                char c = src[i];
                if (c == '\n')
                {
                        line++;
                        continue;
                }
                else if (c != ';' && c != '{' && c != '}')
                {
                        statement[pos] = c;
                        pos++;
                        if (pos > MAX_STATEMENT_LEN)
                        {
                                fprintf(stderr, "Maximum statement length reached.");
                                free(statement);
                                return 1;
                        }
                        continue;
                }
                else if (c == '{' || c == '}')
                {
                        LTok_t token;
                        if (c == '{')
                        {
                                token.token = TOK_OP_LCURLY;
                        }
                        else
                        {
                                token.token = TOK_OP_RCURLY;
                        }
                        token.line = statementNum;
                        token.pos = 0;

                        lexerData->tokens[metadata->numTokens] = token;
                        metadata->numTokens++;
                }

                statementNum++;

                // --- DEBUG INFO ---
                printf("Statement %lu at line %lu: ", statementNum, line);
                fwrite(statement, 1, pos, stdout);
                getchar();
                // ------------------

                char* tokenData = (char*)malloc(MAX_STATEMENT_LEN);
                uint64_t tokenPos = 0;
                for (uint64_t currPos = 0; currPos < pos; currPos++)
                {
                        if (statement[currPos] == ' ' && tokenPos != 0)
                        {
                                // TODO: create token
                                LTok_t* tokens = (LTok_t*)malloc(sizeof(LTok_t) * MAX_STATEMENT_LEN);
                                if (tokens == NULL)
                                {
                                        fprintf(stderr, "Memory allocation failed for tokens");
                                        free(statement);
                                        return 1;
                                }

                                uint64_t numGenerated;
                                if (generateTokens(tokens, statement, currPos - tokenPos, currPos, &numGenerated) == 1)
                                {
                                        fprintf(stderr, "Failed to tokenize statement: ");
                                        fwrite(statement, 1, pos, stderr);
                                        free(statement);
                                        return 1;
                                }

                                tokenPos = 0;
                                for (uint64_t i = 0; i < numGenerated; i++)
                                {
                                        lexerData->tokens[metadata->numTokens] = tokens[i];
                                        metadata->numTokens++;
                                }
                                continue;
                        }
                        tokenData[tokenPos] = statement[currPos];
                        tokenPos++;
                }
                pos = 0;
                free(tokenData);
        }

        free(statement);

        return 0;
}

