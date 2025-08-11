#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "token.h"
#include "lexer.h"
#include "loader.h"

int generateTokens(LTok_t* tokens, char* statement, uint64_t len, uint64_t* num)
{
        *num = 0;

        // --- DEBUG INFO ---
        printf("Generating tokens from: ");
        fwrite(statement, 1, len, stdout);
        printf("\n");
        // ------------------

        uint64_t* spacePositions = (uint64_t*)malloc(1024*sizeof(uint64_t));
        if (spacePositions == NULL)
        {
                fprintf(stderr, "Memory allocation failed for spacePositions");
                return 1;
        }

        uint64_t numSpaces = 0;
        for (uint64_t i = 0; i < len; i++)
        {
                if (statement[i] == ' ')
                {
                        spacePositions[numSpaces] = i;
                        numSpaces++;
                }
        }


        // INFO: tokenFit should match tokens to data so the chosen token is the longest possible match
        // INFO: ex. statement: "sint32 var = 5s" -> tokens = [TOK_TYPE_SINT32, TOK_ID_VARIABLE, TOK_OP_ASSIGN, TOK_LIT_SINT32]

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
                //printf("Statement %lu at line %lu: ", statementNum, line);
                //fwrite(statement, 1, pos, stdout);
                //getchar();
                // ------------------

                char* tokenData = (char*)malloc(MAX_STATEMENT_LEN);
                LTok_t* tokens = (LTok_t*)malloc(sizeof(LTok_t) * MAX_STATEMENT_LEN);
                if (tokens == NULL)
                {
                    fprintf(stderr, "Memory allocation failed for tokens");
                    free(statement);
                    return 1;
                }

                uint64_t numGenerated;
                if (generateTokens(tokens, statement, pos, &numGenerated) == 1)
                {
                    fprintf(stderr, "Failed to tokenize statement: ");
                    fwrite(statement, 1, pos, stderr);
                    free(statement);
                    return 1;
                }

                for (uint64_t i = 0; i < numGenerated; i++)
                {
                    lexerData->tokens[metadata->numTokens] = tokens[i];
                    metadata->numTokens++;
                }
                pos = 0;
                free(tokenData);
        }

        free(statement);

        return 0;
}

