#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../defines.h"
#include "../lexer/lexer.h"

#include "../lexer/token.h"

int loadTokensFromFile(LData_t* lexerData, const char* filename)
{
        uint64_t nameSize = strlen(filename);
        if (nameSize == 0)
        {
                fprintf(stderr, "nameSize cannot be 0\n");
                return 1;
        }

        if (nameSize > MAX_FILE_NAME_LEN)
        {
                fprintf(stderr, "Filename too long: %s\n", filename);
                return 1;
        }

        FILE* inFile = fopen(filename, "rb");
        if (inFile == NULL)
        {
                fprintf(stderr, "Failed to open input file: %s\n", filename);
                return 1;
        }

        int64_t expectedFileSize = -1;
        fseek(inFile, -sizeof(int64_t), SEEK_END);
        fread(&expectedFileSize, sizeof(int64_t), 1, inFile);
        if (expectedFileSize < 0)
        {
                fprintf(stderr, "Negative expectedFileSize.\n");
                fclose(inFile);
                return 1;
        }
        fseek(inFile, 0, SEEK_END);
        int64_t actualFileSize = ftell(inFile);;
        if (actualFileSize < 0)
        {
                fprintf(stderr, "ftell failed during verification.\n");
                fclose(inFile);
                return 1;
        }
        if (actualFileSize != expectedFileSize)
        {
                fprintf(stderr, "Output size mismatch: expected %ld, got %ld\n", expectedFileSize, actualFileSize);
                return 1;
        }

        fseek(inFile, 0, SEEK_SET);
        uint64_t lexerVersion;
        fread(&lexerVersion, sizeof(uint64_t), 1, inFile);
        if (lexerVersion != (uint64_t)LEXER_VERSION)
        {
                fprintf(stderr, "Lexer version mismatch: expected %lu, got %lu\n", (uint64_t)LEXER_VERSION, lexerVersion);
                fclose(inFile);
                return 1;
        }

        LMeta_t metadata;

        uint64_t srcFilenameLen;
        fread(&srcFilenameLen, sizeof(uint64_t), 1, inFile);

        metadata.filename = (char*)malloc(srcFilenameLen);
        if (metadata.filename == NULL)
        {
                fprintf(stderr, "malloc failed for metadata.filename.\n");
                return 1;
        }
        fread(metadata.filename, sizeof(char), srcFilenameLen, inFile);
        fread(&metadata.fileSize, sizeof(uint64_t), 1, inFile);
        fread(&metadata.numTokens, sizeof(uint64_t), 1, inFile);
        metadata.fileSize = srcFilenameLen;

        lexerData->tokens = (LTok_t*)malloc(sizeof(LTok_t) * MAX_STATEMENT_LEN);
        if (lexerData->tokens == NULL)
        {
                fprintf(stderr, "malloc for lexerData->tokens failed.\n");
                return 1;
        }

        for (uint64_t i = 0; i < metadata.numTokens; i++)
        {
                LTok_t token;
                fread(&token.token, sizeof(uint32_t), 1, inFile);
                fread(&token.line, sizeof(uint64_t), 1, inFile);
                fread(&token.pos, sizeof(uint64_t), 1, inFile);
                fread(&token.len, sizeof(uint64_t), 1, inFile);

                token.data = (char*)malloc(token.len);
                if (token.data == NULL)
                {
                        fprintf(stderr, "malloc for token.data failed.\n");
                        return 1;
                }
                fread(token.data, sizeof(char), token.len, inFile);

                lexerData->tokens[i] = token;
        }
        fclose(inFile);

        lexerData->metadata = metadata;

        // print all tokens for debugging
        /*for (uint64_t i = 0; i < metadata.numTokens; i++)
        {
                printf("Token %lu %s\n", i, TOKENS[lexerData->tokens[i].token]);
        }*/

        return 0;
}
