#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "token.h"
#include "lexer.h"

const uint64_t MAX_FILE_NAME_LEN = 1024;
const uint64_t MAX_STATEMENT_LEN = 1024;

int openSourceFile(FILE** file, char* filename, LMeta_t* metadata)
{
        uint64_t nameSize = strlen(filename);
        if (nameSize == 0)
        {
                fprintf(stderr, "nameSize cannot be 0\n");
                return 1;
        }

        if (nameSize < 4)
        {
                fprintf(stderr, "nameSize cannot be smaller than 4\n");
                return 1;
        }

        char extension[4] = {filename[nameSize - 4], filename[nameSize - 3],
                             filename[nameSize - 2], filename[nameSize - 1]};

        if (strcmp(extension, ".mai") != 0)
        {
                fprintf(stderr, "Provided file is not a Maiora source file.\n");
                return 1;
        }

        char *srcname = (char*)malloc(MAX_FILE_NAME_LEN + 1);
        if (srcname == NULL)
        {
                fprintf(stderr, "Memory allocation failed for srcname.\n");
                return 1;
        }
        srcname[MAX_FILE_NAME_LEN] = '\0';

        srcname = strncpy(srcname, filename, MAX_FILE_NAME_LEN);
        if (strcmp(srcname, filename) != 0)
        {
                fprintf(stderr, "Filename of: %s is too long or strncpy failed.\n", filename);
                free(srcname);
                return 1;
        }

        *file = fopen(srcname, "r");
        if (*file == NULL)
        {
                fprintf(stderr, "File: %s could not be opened.\n", filename);
                free(srcname);
                return 1;
        }

        metadata->filename = (char*)malloc(nameSize + 1);
        if (metadata->filename == NULL)
        {
                fprintf(stderr, "Memory allocation failed for metadata->filename.\n");
                free(srcname);
                return 1;
        }
        metadata->filename[nameSize] = '\0';

        metadata->filename = strncpy(metadata->filename, srcname, nameSize);
        if (strcmp(metadata->filename, srcname) != 0)
        {
                fprintf(stderr, "strncpy failed on metadata->filename.\n");
                free(srcname);
                return 1;
        }

        free(srcname);
        uint64_t currLine = 1;

        return 0;
}

int loadSourceFile(char** src, FILE* file, LMeta_t* metadata)
{
        fseek(file, 0, SEEK_END);
        uint64_t fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);
        if (fileSize <= 0)
        {
                fprintf(stderr, "File size is %lu", fileSize);
                return 1;
        }

        *src = (char*)malloc(fileSize);
        if (*src == NULL)
        {
                fprintf(stderr, "Memory allocation failed for data.\n");
                return 1;
        }

        uint64_t size = fread(*src, 1, fileSize, file);
        if (size != fileSize)
        {
                fprintf(stderr, "Wrong amount of data read. Got: %lu Expected: %lu\n", size, fileSize);
                return 1;
        }

        metadata->fileSize = fileSize;

        return 0;
}

int generateTokens(LTok_t* token, char* statement, uint64_t begin, uint64_t end, uint64_t* num)
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

        uint64_t tempDataPos = 0;
        for (uint64_t i = begin; i < end; i++)
        {
                tempData[tempDataPos] = statement[i];
                tempDataPos++;

                // TODO: here compare with keywords etc and create tokens if applicable
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


        // INFO: statement is a line of code between semicolons or before and after curlies in case of blocks

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
                if (c != ';' && c != '{' && c != '}')
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
                if (c == '{' || c == '}')
                {
                        // TODO: create token
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
                                LTok_t* tokens = (LTok_t*)malloc(sizeof(LTok_t)*MAX_STATEMENT_LEN);
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

int main(int argc, char* argv[])
{
        // expected call: ./main <input_file>.mai <output_file>.tok

        if (argc != 3) {
                fprintf(stderr, "Usage: %s <input_file>.mai <output_file>.tok\n", argv[0]);
                return 1;
        }

        LMeta_t metadata;

        FILE* file;
        if (openSourceFile(&file, argv[1], &metadata) == 1) {
                fprintf(stderr, "Failed to open source file.\n");
                return 1;
        }

        char* src;
        if (loadSourceFile(&src, file, &metadata) == 1) {
                fprintf(stderr, "Failed to load source file.\n");
                fclose(file);
                free(src);
                return 1;
        }
        fclose(file);

        LData_t lexerData;
        if (tokenizeSource(&lexerData, src, &metadata) == 1)
        {
                fprintf(stderr, "Failed to tokenize source file.\n");
                free(src);
                return 1;
        }

        free(src);

        return 0;
}
