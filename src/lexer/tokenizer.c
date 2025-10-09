#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "token.h"
#include "lexer.h"
#include "loader.h"

int posCompare(const void* a, const void* b)
{
        LTok_t* tokenA = (LTok_t*)a;
        LTok_t* tokenB = (LTok_t*)b;
        if (tokenA->pos < tokenB->pos)
        {
                return -1;
        }
        else if (tokenA->pos > tokenB->pos)
        {
                return 1;
        }
        else
        {
                return 0;
        }
}

void sortTokensByPos(LTok_t* tokens, uint64_t num)
{
        qsort(tokens, num, sizeof(LTok_t), posCompare);
}

int posAndLineCompare(const void* a, const void* b)
{
        LTok_t* tokenA = (LTok_t*)a;
        LTok_t* tokenB = (LTok_t*)b;
        if (tokenA->line < tokenB->line)
        {
                return -1;
        }
        else if (tokenA->line > tokenB->line)
        {
                return 1;
        }
        else
        {
                if (tokenA->pos < tokenB->pos)
                {
                        return -1;
                }
                else if (tokenA->pos > tokenB->pos)
                {
                        return 1;
                }
                else
                {
                        return 0;
                }
        }
}

void sortTokensByPosAndLine(LTok_t* tokens, uint64_t num)
{
        qsort(tokens, num, sizeof(LTok_t), posAndLineCompare);
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
                        if (token->data == NULL)
                        {
                                fprintf(stderr, "Memory allocation failed for token data\n");
                                return 1;
                        }
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
                //printf("No fit found for word: %s\n", word);
                // ------------------

                token->token = TOK_STR_STUB;
                token->pos = wordPos;
                token->data = (char*)malloc(strlen(word) + 1);
                if (token->data == NULL)
                {
                        fprintf(stderr, "Memory allocation failed for token data\n");
                        return 1;
                }
                memcpy(token->data, word, strlen(word) + 1);
                token->len = strlen(word);
                token->data[token->len] = '\0';
                memcpy(*fit, word, strlen(word));
                (*fit)[strlen(word)] = '\0';
                (*beforefit)[0] = '\0';
                (*afterfit)[0] = '\0';
        }

        // --- DEBUG INFO ---
        //printf("Fit: %s\n", *fit);
        //printf("Beforefit: %s\n", *beforefit);
        //printf("Afterfit: %s\n", *afterfit);
        // ------------------

        return 0;
}

int filterStrings(LTok_t** tokens, uint64_t *num)
{
        // TODO: string filtering phase (take care of TOK_STR_STUB that are actual strings - so TOK_LIT_CHAR)

        LTok_t* newTokens = (LTok_t*)malloc(sizeof(LTok_t) * MAX_STATEMENT_LEN);
        if (newTokens == NULL)
        {
                fprintf(stderr, "Memory allocation failed for newTokens\n");
                return 1;
        }
        uint64_t numNewTokens = 0;

        int isInDquotes = 0;
        int isInDquotesAndCurlies = 0;

        for (uint64_t i = 0; i < *num; i++)
        {
                LTok_t currentToken = (*tokens)[i];
                if (currentToken.token != TOK_OP_DQUOTE &&
                    currentToken.token != TOK_OP_RCURLY &&
                    currentToken.token != TOK_OP_LCURLY &&
                    currentToken.token != TOK_STR_STUB)
                {
                        continue;
                }

                if (currentToken.token == TOK_OP_DQUOTE)
                {
                        if (isInDquotes == 0)
                        {
                                isInDquotes = 1;
                        }
                        else
                        {
                                isInDquotes = 0;
                                isInDquotesAndCurlies = 0;
                        }
                }
                else if (currentToken.token == TOK_OP_LCURLY && isInDquotes == 1)
                {
                        isInDquotesAndCurlies = 1;
                }
                else if (currentToken.token == TOK_OP_RCURLY && isInDquotesAndCurlies == 1)
                {
                        isInDquotesAndCurlies = 0;
                }
                else if (currentToken.token == TOK_STR_STUB && isInDquotes == 1 && isInDquotesAndCurlies == 0)
                {
                        for (uint64_t j = 0; j < currentToken.len; j++)
                        {
                                LTok_t newToken;
                                newToken.token = TOK_LIT_CHAR;
                                newToken.pos = currentToken.pos + j;
                                newToken.line = currentToken.line;
                                newToken.len = 1;
                                newToken.data = (char*)malloc(2);
                                if (newToken.data == NULL)
                                {
                                        fprintf(stderr, "Memory allocation failed for newToken data\n");
                                        return 1;
                                }
                                newToken.data[0] = currentToken.data[j];
                                newToken.data[1] = '\0';
                                newTokens[numNewTokens] = newToken;
                                numNewTokens++;
                        }
                        (*tokens)[i].line = UINT64_MAX;
                        (*tokens)[i].pos = UINT64_MAX;
                }
        }

        for (uint64_t i = 0; i < numNewTokens; i++)
        {
                (*tokens)[*num + i] = newTokens[i];
        }

        *num += numNewTokens;
        free(newTokens);

        sortTokensByPosAndLine(*tokens, *num);

        for (uint64_t i = *num - 1; i > 0; i--)
        {
                if ((*tokens)[i].line == UINT64_MAX && (*tokens)[i].pos == UINT64_MAX)
                {
                        free((*tokens)[i].data);
                        (*num)--;
                }
                else
                {
                        break;
                }
        }

        return 0;
}

enum Type
{
        TYPE_UNDEFINED = 0,
        TYPE_INT,
        TYPE_INT_OR_FLOAT,
        TYPE_FLOAT,
        TYPE_SINT,
        TYPE_UINT,
        TYPE_FLOAT32,
        TYPE_FLOAT64,
        TYPE_BOOL
};

uint8_t inferLitType(LTok_t token)
{
        if (strcmp(token.data, "true") == 0 || strcmp(token.data, "false") == 0)
        {
                return TYPE_BOOL;
        }

        for (uint64_t i = 0; i < token.len - 1; i++)
        {
                if (token.data[i] < '0' || token.data[i] > '9')
                {
                        return TYPE_UNDEFINED;
                }
        }

        if (token.len > 1 && token.data[token.len - 1] == 's')
        {
                return TYPE_SINT;
        }
        else if (token.len > 1 && token.data[token.len - 1] == 'u')
        {
                return TYPE_UINT;
        }
        else if (token.len > 1 && token.data[token.len - 1] == 'f')
        {
                return TYPE_FLOAT32;
        }
        else if (token.len > 1 && token.data[token.len - 1] == 'd')
        {
                return TYPE_FLOAT64;
        }

        return TYPE_INT_OR_FLOAT;
}

uint8_t literalLookAhead(LTok_t* tokens, uint64_t num, uint64_t index)
{
        if (index + 2 >= num)
        {
                return TYPE_INT;
        }

        LTok_t nextToken = tokens[index + 1];
        LTok_t nextNextToken = tokens[index + 2];

        if (nextToken.token == TOK_OP_FROM && nextNextToken.token == TOK_STR_STUB)
        {
                return inferLitType(nextNextToken);
        }
        else
        {
                return TYPE_INT;
        }
}

int filterLiterals(LTok_t** tokens, uint64_t *num)
{
        // TODO: literation phase (take care of literals that are not chars)

        for (uint64_t i = 0; i < *num; i++)
        {
                LTok_t currentToken = (*tokens)[i];
                if (currentToken.token != TOK_STR_STUB)
                {
                        continue;
                }

                uint8_t type = inferLitType(currentToken);
                switch (type)
                {
                        case TYPE_UNDEFINED:
                        {
                                continue;
                        }
                        case TYPE_SINT:
                        {
                                (*tokens)[i].token = TOK_LIT_SINT;
                                break;
                        }
                        case TYPE_UINT:
                        {
                                (*tokens)[i].token = TOK_LIT_UINT;
                                break;
                        }
                        case TYPE_FLOAT32:
                        {
                                (*tokens)[i].token = TOK_LIT_FLOAT32;
                                break;
                        }
                        case TYPE_FLOAT64:
                        {
                                (*tokens)[i].token = TOK_LIT_FLOAT64;
                                break;
                        }
                        case TYPE_BOOL:
                        {
                                (*tokens)[i].token = TOK_LIT_BOOL;
                                break;
                        }
                        case TYPE_INT_OR_FLOAT:
                        {
                                uint8_t lookAheadType = literalLookAhead(*tokens, *num, i);
                                if (lookAheadType == TYPE_INT)
                                {
                                        (*tokens)[i].token = TOK_LIT_INT;
                                }
                                else if (lookAheadType == TYPE_FLOAT)
                                {
                                        (*tokens)[i].token = TOK_LIT_FLOAT;
                                        (*tokens)[i].len += 1 + (*tokens)[i + 2].len;
                                        (*tokens)[i].data = (char*)realloc((*tokens)[i].data, (*tokens)[i].len + 1);
                                        if ((*tokens)[i].data == NULL)
                                        {
                                                fprintf(stderr, "realloc failed for token data\n");
                                                return 1;
                                        }
                                        memcpy((*tokens)[i].data + currentToken.len, ".", 1);
                                        memcpy((*tokens)[i].data + currentToken.len + 1, (*tokens)[i + 2].data, (*tokens)[i + 2].len);
                                        (*tokens)[i].data[(*tokens)[i].len] = '\0';
                                        (*tokens)[i + 1].line = UINT64_MAX;
                                        (*tokens)[i + 1].pos = UINT64_MAX;
                                        (*tokens)[i + 2].line = UINT64_MAX;
                                        (*tokens)[i + 2].pos = UINT64_MAX;
                                        i++;
                                }
                                else if (lookAheadType == TYPE_FLOAT32)
                                {
                                        (*tokens)[i].token = TOK_LIT_FLOAT32;
                                        (*tokens)[i].len += 1 + (*tokens)[i + 2].len;
                                        (*tokens)[i].data = (char*)realloc((*tokens)[i].data, (*tokens)[i].len + 1);
                                        if ((*tokens)[i].data == NULL)
                                        {
                                                fprintf(stderr, "realloc failed for token data\n");
                                                return 1;
                                        }
                                        memcpy((*tokens)[i].data + currentToken.len, ".", 1);
                                        memcpy((*tokens)[i].data + currentToken.len + 1, (*tokens)[i + 2].data, (*tokens)[i + 2].len);
                                        (*tokens)[i].data[(*tokens)[i].len] = '\0';
                                        (*tokens)[i + 1].line = UINT64_MAX;
                                        (*tokens)[i + 1].pos = UINT64_MAX;
                                        (*tokens)[i + 2].line = UINT64_MAX;
                                        (*tokens)[i + 2].pos = UINT64_MAX;
                                        i++;
                                }
                                else if (lookAheadType == TYPE_FLOAT64)
                                {
                                        (*tokens)[i].token = TOK_LIT_FLOAT64;
                                        (*tokens)[i].len += 1 + (*tokens)[i + 2].len;
                                        (*tokens)[i].data = (char*)realloc((*tokens)[i].data, (*tokens)[i].len + 1);
                                        if ((*tokens)[i].data == NULL)
                                        {
                                                fprintf(stderr, "realloc failed for token data\n");
                                                return 1;
                                        }
                                        memcpy((*tokens)[i].data + currentToken.len, ".", 1);
                                        memcpy((*tokens)[i].data + currentToken.len + 1, (*tokens)[i + 2].data, (*tokens)[i + 2].len);
                                        (*tokens)[i].data[(*tokens)[i].len] = '\0';
                                        (*tokens)[i + 1].line = UINT64_MAX;
                                        (*tokens)[i + 1].pos = UINT64_MAX;
                                        (*tokens)[i + 2].line = UINT64_MAX;
                                        (*tokens)[i + 2].pos = UINT64_MAX;
                                        i++;
                                }
                                break;
                        }
                        default:
                        {
                                fprintf(stderr, "Unknown type inferred for token: %s\n", currentToken.data);
                                break;
                        }
                }
        }

        sortTokensByPosAndLine(*tokens, *num);

        for (uint64_t i = *num - 1; i > 0; i--)
        {
                if ((*tokens)[i].line == UINT64_MAX && (*tokens)[i].pos == UINT64_MAX)
                {
                        free((*tokens)[i].data);
                        (*num)--;
                }
                else
                {
                        break;
                }
        }

        return 0;
}

int filterIds(LTok_t* tokens, uint64_t num)
{
        // TODO: id phase (take care of ids)

        for (uint64_t i = 0; i < num; i++)
        {
                if (tokens[i].token == TOK_STR_STUB)
                {
                        tokens[i].token = TOK_ID;
                }
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

        uint64_t* spacePositions = (uint64_t*)malloc(sizeof(uint64_t) * MAX_STATEMENT_LEN);
        if (spacePositions == NULL)
        {
                fprintf(stderr, "Memory allocation failed for spacePositions\n");
                return 1;
        }

        uint64_t numSpaces = 0;
        uint64_t firstNonSpace = len;
        for (uint64_t i = 0; i < len; i++)
        {
                if (statement[i] == ' ')
                {
                        LTok_t spaceToken;
                        spaceToken.token = TOK_SPACE;
                        spaceToken.pos = i;
                        spaceToken.len = 1;
                        spaceToken.data = (char*)malloc(2);
                        if (spaceToken.data == NULL)
                        {
                                fprintf(stderr, "Memory allocation failed for spaceToken data\n");
                                return 1;
                        }
                        spaceToken.data[0] = ' ';
                        spaceToken.data[1] = '\0';
                        tokens[*num] = spaceToken;
                        (*num)++;

                        spacePositions[numSpaces] = i;
                        numSpaces++;
                }
                else if (i < firstNonSpace)
                {
                        firstNonSpace = i;
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

        uint64_t *wordPositions = (uint64_t*)malloc(sizeof(uint64_t) * MAX_STATEMENT_LEN);
        if (wordPositions == NULL)
        {
                fprintf(stderr, "Memory allocation failed for wordPositions\n");
                return 1;
        }

        for (uint64_t i = 0; i < numWords; i++)
        {
                if (i == 0)
                {
                        wordPositions[i] = firstNonSpace;
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
                if (words[i] == NULL)
                {
                        fprintf(stderr, "Memory allocation failed for words[i]\n");
                        return 1;
                }

                uint64_t beforeLen = strlen(beforefit);
                if (strlen(beforefit) > 0 && strlen(afterfit) > 0)
                {
                        numWords++;
                        words[numWords - 1] = (char*)malloc(MAX_STATEMENT_LEN);
                        if (words[numWords - 1] == NULL)
                        {
                                fprintf(stderr, "Memory allocation failed for words[numWords - 1]\n");
                                return 1;
                        }
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

                tokens[*num] = token;
                (*num)++;

                free(fit);
                free(beforefit);
                free(afterfit);

                i++;
                if (i == numWords) i = 0;
        }

        sortTokensByPos(tokens, *num);

        if (filterStrings(&tokens, num) != 0)
        {
                fprintf(stderr, "filterStrings failed\n");
                return 1;
        }
        if (filterLiterals(&tokens, num) != 0)
        {
                fprintf(stderr, "filterLiterals failed\n");
                return 1;
        }
        if (filterIds(tokens, *num) != 0)
        {
                fprintf(stderr, "filterIds failed\n");
                return 1;
        }

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
        lexerData->tokens = (LTok_t*)malloc(sizeof(LTok_t) * metadata->fileSize * 2);
        if (lexerData->tokens == NULL)
        {
                fprintf(stderr, "Memory allocation failed for tokens\n");
                return 1;
        }
        metadata->numTokens = 0;

        // INFO: statement is a line of code between semicolons or before curlies

        uint64_t statementNum = 0;
        uint64_t line = 1;
        char* statement = (char*)calloc(MAX_STATEMENT_LEN + 1, 1);
        if (statement == NULL)
        {
                fprintf(stderr, "Memory allocation failed for statement\n");
                return 1;
        }

        uint64_t *statementPosToColumn = (uint64_t*)malloc(sizeof(uint64_t) * MAX_STATEMENT_LEN);
        if (statementPosToColumn == NULL)
        {
                fprintf(stderr, "Memory allocation failed for statementPosToColumn\n");
                return 1;
        }

        uint64_t *statementPosToLine = (uint64_t*)malloc(sizeof(uint64_t) * MAX_STATEMENT_LEN);
        if (statementPosToLine == NULL)
        {
                fprintf(stderr, "Memory allocation failed for statementPosToLine\n");
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
                        statementPosToColumn[pos] = column;
                        statementPosToLine[pos] = line;
                        pos++;
                        column++;
                        if (pos > MAX_STATEMENT_LEN)
                        {
                                fprintf(stderr, "Maximum statement length reached.\n");
                                return 1;
                        }
                        continue;
                }
                else if (c == '{' || c == '}' || c == ';')
                {
                        column++;
                        LTok_t token;
                        if (c == '{')
                        {
                                token.token = TOK_OP_LCURLY;
                                token.data = (char*)malloc(2);
                                if (token.data == NULL)
                                {
                                        fprintf(stderr, "Memory allocation failed for token data\n");
                                        return 1;
                                }
                                token.data[0] = '{';
                                token.data[1] = '\0';
                                token.len = 1;
                        }
                        else if (c == '}')
                        {
                                token.token = TOK_OP_RCURLY;
                                token.data = (char*)malloc(2);
                                if (token.data == NULL)
                                {
                                        fprintf(stderr, "Memory allocation failed for token data\n");
                                        return 1;
                                }
                                token.data[0] = '}';
                                token.data[1] = '\0';
                        }
                        else
                        {
                                token.token = TOK_OP_SEMICOLON;
                                token.data = (char*)malloc(2);
                                if (token.data == NULL)
                                {
                                        fprintf(stderr, "Memory allocation failed for token data\n");
                                        return 1;
                                }
                                token.data[0] = ';';
                                token.data[1] = '\0';
                        }
                        token.len = 1;
                        token.line = line;
                        token.pos = column;

                        lexerData->tokens[metadata->numTokens] = token;
                        metadata->numTokens++;
                }

                if (pos == 0)
                {
                        continue;
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

                for (uint64_t j = 0; j < numGenerated; j++)
                {
                    tokens[j].line = statementPosToLine[tokens[j].pos];
                    tokens[j].pos = statementPosToColumn[tokens[j].pos] + 1;

                    lexerData->tokens[metadata->numTokens] = tokens[j];
                    metadata->numTokens++;
                }

                memset(statement, 0, MAX_STATEMENT_LEN);
                pos = 0;
        }

        sortTokensByPosAndLine(lexerData->tokens, metadata->numTokens);

        free(statement);

        return 0;
}
