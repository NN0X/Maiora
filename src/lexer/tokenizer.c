#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "token.h"
#include "lexer.h"
#include "loader.h"

// TODO: map statement number to line number for error reporting

int sortTokensByPos(LTok_t* tokens, uint64_t low, uint64_t high)
{
        if (low >= high) return 0;
        uint64_t pivot = tokens[high].pos;
        uint64_t i = low;

        for (uint64_t j = low; j < high; j++)
        {
                if (tokens[j].pos <= pivot)
                {
                        LTok_t temp = tokens[i];
                        tokens[i] = tokens[j];
                        tokens[j] = temp;
                        i++;
                }
        }
        LTok_t temp = tokens[i];
        tokens[i] = tokens[high];
        tokens[high] = temp;

        if (i > 0) sortTokensByPos(tokens, low, i - 1);
        sortTokensByPos(tokens, i + 1, high);
        return 0;
}

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

int getLongestTokenFit(char** fit, char** beforefit, char** afterfit, LTok_t* token, char* word, uint64_t offset, uint64_t wordPos)
{
        uint64_t wordLen = strlen(word);
        if (offset >= wordLen)
        {
                return 1;
        }
        uint64_t fitLen = wordLen - offset;
        memcpy(*fit, word + offset, fitLen);
        (*fit)[fitLen] = '\0';
        (*beforefit)[0] = '\0';
        (*afterfit)[0] = '\0';

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
                        token->pos = wordPos + offset;
                        token->len = fitLen;

                        if (offset > 0)
                        {
                                memcpy(*beforefit, word, offset);
                                (*beforefit)[offset] = '\0';
                        }
                        if (offset + fitLen < wordLen)
                        {
                                uint64_t afterfitLen = wordLen - (offset + fitLen);
                                memcpy(*afterfit, word + offset + fitLen, afterfitLen);
                                (*afterfit)[afterfitLen] = '\0';
                        }

                        // --- DEBUG INFO ---
                        //printf("Fit: %s\n", *fit);
                        //printf("Beforefit: %s\n", *beforefit);
                        //printf("Afterfit: %s\n", *afterfit);
                        // ------------------

                        return 0;
                }

                fitLen--;
                memcpy(*fit, word + offset, fitLen);
                (*fit)[fitLen] = '\0';
        }

        return 0;
}

int getFirstLongestTokenFit(char** fit, char** beforefit, char** afterfit, LTok_t* token, char* word, uint64_t wordPos)
{
        uint64_t offset = 0;
        if (getLongestTokenFit(fit, beforefit, afterfit, token, word, offset, wordPos) != 0)
        {
                fprintf(stderr, "getLongestTokenFit failed for word %s\n", word);
                return 1;
        }

        while (strlen(*fit) == 0 && offset < strlen(word) - 1)
        {
                offset++;
                if (getLongestTokenFit(fit, beforefit, afterfit, token, word, offset, wordPos) != 0)
                {
                        fprintf(stderr, "getLongestTokenFit failed for word %s\n", word);
                        return 1;
                }
        }

        if (strlen(*fit) == 0)
        {
                // --- DEBUG INFO ---
                printf("No fit found for word: %s\n", word);
                // ------------------

                token->token = TOK_STR_STUB;
                token->pos = wordPos;
                token->data = (char*)malloc(strlen(word) + 1);
                memcpy(token->data, word, strlen(word) + 1);
                token->len = strlen(word);
                memcpy(*fit, word, strlen(word) + 1);
                (*fit)[strlen(word)] = '\0';
                (*beforefit)[0] = '\0';
                (*afterfit)[0] = '\0';
        }

        return 0;
}

int generateTokens(LTok_t* tokens, char* statement, uint64_t len, uint64_t* num, uint64_t line)
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
                        spaceToken.pos = i;
                        spaceToken.line = line;
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

        uint64_t *wordPositions = (uint64_t*)malloc(numWords * sizeof(uint64_t));
        if (wordPositions == NULL)
        {
                fprintf(stderr, "Memory allocation failed for wordPositions\n");
                return 1;
        }

        for (uint64_t i = 0; i < numWords; i++)
        {
                if (i == 0)
                {
                        wordPositions[i] = 0;
                }
                else
                {
                        wordPositions[i] = wordPositions[i - 1] + strlen(words[i - 1]) + 1;
                }
        }

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

                char* beforefit = (char*)malloc(MAX_STATEMENT_LEN);
                if (beforefit == NULL)
                {
                        fprintf(stderr, "Memory allocation failed for beforefit\n");
                        return 1;
                }
                beforefit[0] = '\0';
                char* afterfit = (char*)malloc(MAX_STATEMENT_LEN);
                if (afterfit == NULL)
                {
                        fprintf(stderr, "Memory allocation failed for afterfit\n");
                        return 1;
                }
                afterfit[0] = '\0';

                LTok_t token;
                if (getFirstLongestTokenFit(&fit, &beforefit, &afterfit, &token, words[i], wordPositions[i]) != 0)
                {
                        fprintf(stderr, "getFirstLongestTokenFit failed for word %s\n", words[i]);
                        return 1;
                }

                free(words[i]);
                words[i] = (char*)malloc(MAX_STATEMENT_LEN);

                uint64_t beforeLen = strlen(beforefit);
                if (strlen(beforefit) > 0 && strlen(afterfit) > 0)
                {
                        numWords++;
                        words[numWords - 1] = (char*)malloc(MAX_STATEMENT_LEN);
                        memcpy(words[numWords - 1], beforefit, strlen(beforefit) + 1);
                        wordPositions[numWords - 1] = wordPositions[i];

                        memcpy(words[i], afterfit, strlen(afterfit) + 1);
                        wordPositions[i] += strlen(fit) + beforeLen;
                }
                else if (strlen(beforefit) == 0 && strlen(afterfit) > 0)
                {
                        memcpy(words[i], afterfit, strlen(afterfit) + 1);
                        wordPositions[i] += strlen(fit) + beforeLen;
                }
                else if (strlen(beforefit) > 0 && strlen(afterfit) == 0)
                {
                        memcpy(words[i], beforefit, strlen(beforefit) + 1);
                }
                else
                {
                        words[i][0] = '\0';
                }

                if (strlen(words[i]) == 0)
                {
                        numWordsProcessed++;
                }

                token.line = line;
                tokens[*num] = token;
                (*num)++;

                i++;
                if (i == numWords) i = 0;
        }

        if (sortTokensByPos(tokens, 0, *num - 1) != 0)
        {
                fprintf(stderr, "sortTokensByPos failed\n");
                return 1;
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
        free(wordPositions);

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
        // TODO: add mapping from statement number to line number for error reporting

        uint64_t statementNum = 0;
        uint64_t line = 1;
        char* statement = (char*)calloc(MAX_STATEMENT_LEN + 1, 1);
        if (statement == NULL)
        {
                fprintf(stderr, "Memory allocation failed for statement\n");
                return 1;
        }

        uint64_t pos = 0;
        uint64_t column = 0;
        for (uint64_t i = 0; i < metadata->fileSize; i++)
        {
                char c = src[i];
                if (c == '\n')
                {
                        line++;
                        column = 0;
                        continue;
                }
                else if (c != ';' && c != '{' && c != '}')
                {
                        statement[pos] = c;
                        pos++;
                        column++;
                        if (pos > MAX_STATEMENT_LEN)
                        {
                                fprintf(stderr, "Maximum statement length reached.\n");
                                return 1;
                        }
                        continue;
                }
                else if (c == '{' || c == '}')
                {
                        column++;
                        LTok_t token;
                        if (c == '{')
                        {
                                token.token = TOK_OP_LCURLY;
                        }
                        else
                        {
                                token.token = TOK_OP_RCURLY;
                        }
                        token.line = line;
                        token.pos = column;

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
                if (generateTokens(tokens, statement, pos, &numGenerated, line) != 0)
                {
                    fprintf(stderr, "Failed to tokenize statement: ");
                    fwrite(statement, 1, pos, stderr);
                    fprintf(stderr, "\n");
                    return 1;
                }

                for (uint64_t j = 0; j < numGenerated; j++)
                {
                    // TODO: convert token->pos from position in statement to position in line

                    lexerData->tokens[metadata->numTokens] = tokens[j];
                    metadata->numTokens++;
                }

                memset(statement, 0, MAX_STATEMENT_LEN);
                pos = 0;
        }

        free(statement);

        return 0;
}
