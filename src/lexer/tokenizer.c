#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "token.h"
#include "lexer.h"
#include "loader.h"

// TODO: fix so tokens are in correct order

int getTokenMatch(char *str)
{
        for (uint64_t i = 0; i < TOK_META_STR_BEGIN; i++)
        {
                if (strcmp(str, TOKENS[i]) == 0)
                {
                        return i;
                }
        }
        return -1;
}

int getLongestTokenFit(char** fit, char** antifit, LTok_t* token, char* word, uint64_t offset)
{
        uint64_t wordLen = strlen(word);
        if (offset >= wordLen)
        {
                return 1;
        }
        uint64_t fitLen = wordLen - offset;
        memcpy(*fit, word + offset, fitLen);
        (*fit)[fitLen] = '\0';
        (*antifit)[0] = '\0';

        // --- DEBUG INFO ---
        //printf("Word: %s\n", word);
        // ------------------

        while (fitLen > 0)
        {
                int tokenMatch = getTokenMatch(*fit);
                if (tokenMatch >= 0)
                {
                        token->data = (char*)malloc(fitLen + 1);
                        memcpy(token->data, *fit, fitLen);
                        token->data[fitLen] = '\0';
                        token->token = tokenMatch;
                        token->len = fitLen;
                        break;
                }
                memcpy(*fit, word + offset, fitLen);
                fitLen--;
                (*fit)[fitLen] = '\0';
                memcpy(*antifit, word + offset + fitLen, wordLen - offset - fitLen + 1);
                (*antifit)[wordLen - offset - fitLen] = '\0';

                // --- DEBUG INFO ---
                //printf("Fit: %s\n", *fit);
                //printf("Antifit: %s\n", *antifit);
                // ------------------
        }

        return 0;
}

int getFirstLongestTokenFit(char** fit, char** antifit, LTok_t* token, char* word)
{
        // INFO: example 1
        // example start word: "main(none)"
        // input: word = "main(none)"
        // output: fit = "main", antifit = "(none)", token = "TOK_STR_STUB"
        // input: word = "(none)"
        // output: fit = "(", antifit = "none)", token = "TOK_OP_LPAR"
        // input: word = "none)"
        // output: fit = "none", antifit = ")", token = "TOK_TYPE_NONE"
        // input: word = ")"
        // output: fit = ")", antifit = "", token = "TOK_OP_RPAR"
        // result: [TOK_STR_STUB, TOK_OP_LPAR, TOK_TYPE_NONE, TOK_OP_RPAR]

        // INFO: example 2
        // example start word: "\"foo\\"\"" -> string foo" within double quotes so in code "foo\"
        // input: word = "\"foo\\"\""
        // output: fit = "\"", antifit = "foo\\"\"", token = "TOK_OP_DQUOTE"
        // input: word = "foo\\"\""
        // output: fit = "foo\\"", antifit = "\"", token = "TOK_STR_STUB"
        // input: word = "\""
        // output: fit = "\"", antifit = "", token = "TOK_OP_DQUOTE"
        // result: [TOK_OP_DQUOTE, TOK_STR_STUB, TOK_OP_DQUOTE]

        uint64_t offset = 0;
        if (getLongestTokenFit(fit, antifit, token, word, offset) != 0)
        {
                fprintf(stderr, "getLongestTokenFit failed for word %s\n", word);
                return 1;
        }

        while (strlen(*fit) == 0 && offset < strlen(word) - 1)
        {
                offset++;
                if (getLongestTokenFit(fit, antifit, token, word, offset) != 0)
                {
                        fprintf(stderr, "getLongestTokenFit failed for word %s\n", word);
                        return 1;
                }
        }

        if (strlen(*fit) == 0)
        {
                // --- DEBUG INFO ---
                //printf("No fit found for word: %s\n", word);
                // ------------------

                token->token = TOK_STR_STUB;
                token->data = (char*)malloc(strlen(word) + 1);
                memcpy(token->data, word, strlen(word) + 1);
                token->len = strlen(word);
                memcpy(*fit, word, strlen(word) + 1);
                (*fit)[strlen(word)] = '\0';
                (*antifit)[0] = '\0';
        }

        return 0;
}

int generateTokens(LTok_t* tokens, char* statement, uint64_t len, uint64_t* num)
{
        *num = 0;

        if (len == 0)
        {
                return 0;
        }

        // --- DEBUG INFO ---
        //printf("Generating tokens from: ");
        //fwrite(statement, 1, len, stdout);
        //printf("\n");
        // ------------------

        uint64_t* spacePositions = (uint64_t*)malloc(1024*sizeof(uint64_t));
        if (spacePositions == NULL)
        {
                fprintf(stderr, "Memory allocation failed for spacePositions\n");
                return 1;
        }

        uint64_t numSpaces = 0;
        for (uint64_t i = 0; i < len; i++)
        {
                if (statement[i] == ' ')
                {
                        LTok_t spaceToken;
                        spaceToken.token = TOK_SPACE_STUB;
                        tokens[*num] = spaceToken;
                        (*num)++;

                        spacePositions[numSpaces] = i;
                        numSpaces++;
                }
        }

        char** words = (char**)malloc(MAX_STATEMENT_LEN * sizeof(char*));
        if (words == NULL)
        {
                fprintf(stderr, "Memory allocation failed for words\n");
                return 1;
        }

        uint64_t numWords = 0;
        if (statement[0] != ' ')
        {
                uint64_t wordLen = 0;
                if (numSpaces == 0)
                {
                        wordLen = len;
                }
                else
                {
                        wordLen = spacePositions[0];
                }

                char* word = (char*)malloc(wordLen + 1);
                if (word == NULL)
                {
                        fprintf(stderr, "Memory allocation failed for word\n");
                        return 1;
                }

                memcpy(word, statement, wordLen);
                word[wordLen] = '\0';

                words[numWords] = word;
                numWords++;
        }

        for(uint64_t i = 0; i < numSpaces; i++)
        {
                uint64_t wordLen = 0;
                if (i + 1 == numSpaces)
                {
                        wordLen = len - spacePositions[i] - 1;
                }
                else
                {
                        wordLen = spacePositions[i + 1] - spacePositions[i] - 1;
                }

                if (wordLen > MAX_STATEMENT_LEN)
                {
                        fprintf(stderr, "Word length larger than MAX_STATEMENT_LEN\n");
                        return 1;
                }
                else if (wordLen == 0)
                {
                        continue;
                }

                char* word = (char*)malloc(wordLen + 1);
                if (word == NULL)
                {
                        fprintf(stderr, "Memory allocation failed for word\n");
                        return 1;
                }

                memcpy(word, statement + spacePositions[i] + 1, wordLen);
                word[wordLen] = '\0';

                words[numWords] = word;
                numWords++;
        }

        // INFO: ex. statement: "sint32 var = 5s" -> tokens = [TOK_TYPE_SINT32, TOK_ID_VARIABLE, TOK_OP_ASSIGN, TOK_LIT_SINT32]

        uint64_t numWordsProcessed = 0;
        uint64_t i = 0;
        while (numWordsProcessed < numWords)
        {
                if (strlen(words[i]) == 0)
                {
                        i++;
                        if (i == numWords) i = 0;
                        continue;
                }

                char* fit = (char*)malloc(MAX_STATEMENT_LEN);
                if (fit == NULL)
                {
                        fprintf(stderr, "Memory allocation failed for fit\n");
                        return 1;
                }

                char* antifit = (char*)malloc(MAX_STATEMENT_LEN);
                if (antifit == NULL)
                {
                        fprintf(stderr, "Memory allocation failed for antifit\n");
                        return 1;
                }

                LTok_t token;
                if (getFirstLongestTokenFit(&fit, &antifit, &token, words[i]) == 1)
                {
                        fprintf(stderr, "getFirstLongestTokenFit failed for word %s\n", words[i]);
                        return 1;
                }

                free(words[i]);
                words[i] = antifit;

                if (strlen(words[i]) == 0)
                {
                        numWordsProcessed++;
                }

                // TODO: take care of token

                tokens[*num] = token;
                (*num)++;

                i++;
                if (i == numWords) i = 0;
        }

        // TODO: string filtering phase (take care of TOK_STR_STUB that are actual strings)

        // TODO: literation phase (take care of literals)

        // TODO: id phase (take care of ids)

        for (uint64_t i = 0; i < numWords; i++)
        {
                free(words[i]);
        }
        free(words);
        free(spacePositions);

        return 0;
}

int tokenizeSource(LData_t* lexerData, char* src, LMeta_t* metadata)
{
        lexerData->metadata = *metadata;
        lexerData->tokens = (LTok_t*)malloc(sizeof(LTok_t) * metadata->fileSize);
        metadata->numTokens = 0;
        if (lexerData->tokens == NULL)
        {
                fprintf(stderr, "Memory allocation failed for tokens\n");
                return 1;
        }


        // INFO: statement is a line of code between semicolons or before curlies

        uint64_t statementNum = 0;
        uint64_t line = 1;
        char* statement = (char*)calloc(MAX_STATEMENT_LEN + 1, 1);

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
                                fprintf(stderr, "Maximum statement length reached.\n");
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

                LTok_t* tokens = (LTok_t*)malloc(sizeof(LTok_t) * MAX_STATEMENT_LEN);
                if (tokens == NULL)
                {
                    fprintf(stderr, "Memory allocation failed for tokens\n");
                    return 1;
                }

                uint64_t numGenerated;
                if (generateTokens(tokens, statement, pos, &numGenerated) != 0)
                {
                    fprintf(stderr, "Failed to tokenize statement: ");
                    fwrite(statement, 1, pos, stderr);
                    fprintf(stderr, "\n");
                    return 1;
                }

                for (uint64_t i = 0; i < numGenerated; i++)
                {
                    lexerData->tokens[metadata->numTokens] = tokens[i];
                    metadata->numTokens++;
                }

                memset(statement, 0, MAX_STATEMENT_LEN);
                pos = 0;
        }

        free(statement);

        return 0;
}
