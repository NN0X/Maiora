#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "token.h"
#include "lexer.h"
#include "loader.h"

// INFO: has to begin at word[0]
int getFirstTokenFit(char* fit, char* antifit, LTok_t* token, char* word)
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

        char** words = (char**)malloc(MAX_STATEMENT_LEN * sizeof(char*));
        if (words == NULL)
        {
                fprintf(stderr, "Memory allocation failed for words");
                return 1;
        }

        uint64_t numWords = 0;
        if (statement[0] != ' ')
        {
                uint64_t wordLen = 0;
                if (numSpaces == 0)
                {
                        wordLen = len + 1;
                }
                else
                {
                        wordLen = spacePositions[0] + 1;
                }

                char* word = (char*)malloc(wordLen);
                if (word == NULL)
                {
                        fprintf(stderr, "Memory allocation failed for word");
                        return 1;
                }

                memcpy(word, statement, wordLen - 1);
                if (word == NULL)
                {
                        fprintf(stderr, "memcpy failed for statement into word");
                        return 1;
                }
                word[wordLen - 1] = '\0';

                words[numWords] = word;
                numWords++;
        }

        for(uint64_t i = 0; i < numSpaces; i++)
        {
                uint64_t wordLen = 0;
                if (i + 1 == numSpaces)
                {
                        wordLen = len - spacePositions[i];
                }
                else
                {
                        wordLen = spacePositions[i + 1] - spacePositions[i];
                }

                if (wordLen > MAX_STATEMENT_LEN)
                {
                        fprintf(stderr, "Word length larger than MAX_STATEMENT_LEN");
                        return 1;
                }
                else if (wordLen < 2)
                {
                        continue;
                }

                char* word = (char*)malloc(wordLen);
                if (word == NULL)
                {
                        fprintf(stderr, "Memory allocation failed for word");
                        return 1;
                }

                memcpy(word, statement + spacePositions[i] + 1, wordLen - 1);
                if (word == NULL)
                {
                        fprintf(stderr, "memcpy failed for statement into word");
                        return 1;
                }
                word[wordLen - 1] = '\0';

                words[numWords] = word;
                numWords++;
        }

        // INFO: ex. statement: "sint32 var = 5s" -> tokens = [TOK_TYPE_SINT32, TOK_ID_VARIABLE, TOK_OP_ASSIGN, TOK_LIT_SINT32]

        uint64_t numWordsProcessed = 0;
        for (uint64_t i = 0; i < numWords; i++)
        {
                if (strlen(words[i]) == 0)
                {
                        continue;
                }

                char* fit = (char*)malloc(MAX_STATEMENT_LEN);
                if (fit == NULL)
                {
                        fprintf(stderr, "Memory allocation failed for fit");
                        return 1;
                }

                char* antifit = (char*)malloc(MAX_STATEMENT_LEN);
                if (antifit == NULL)
                {
                        fprintf(stderr, "Memory allocation failed for antifit");
                        return 1;
                }

                LTok_t token;
                if (getFirstTokenFit(fit, antifit, &token, words[i]) == 1)
                {
                        fprintf(stderr, "getFirstTokenFit failed for word %s", words[i]);
                        return 1;
                }

                uint64_t fitLen = strlen(fit);
                uint64_t wordLen = strlen(words[i]);

                free(words[i]);
                words[i] = antifit;

                if (fitLen == wordLen)
                {
                        numWordsProcessed++;
                }
                else
                {
                        words[i] = antifit;
                }

                // TODO: take care of token
        }

        // TODO: string filtering phase

        // TODO: literation phase

        // TODO: id phase

        for (uint64_t i = 0; i < numWords; i++)
        {
                free(words[i]);
        }
        free(words);

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
                    return 1;
                }

                uint64_t numGenerated;
                if (generateTokens(tokens, statement, pos, &numGenerated) == 1)
                {
                    fprintf(stderr, "Failed to tokenize statement: ");
                    fwrite(statement, 1, pos, stderr);
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

