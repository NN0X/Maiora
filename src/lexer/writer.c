#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../defines.h"
#include "lexer.h"
#include "loader.h"
#include "writer.h"

int writeTokensToOut(FILE* out, LMeta_t* metadata, LData_t* lexerData)
{
        printf("Generated %lu tokens:\n", metadata->numTokens);
        for (uint64_t i = 0; i < metadata->numTokens; i++)
        {
                LTok_t token = lexerData->tokens[i];
                if (TOKENS[token.token] != NULL)
                {
                        printf("Token %-6lu: %-20s | Line: %-4lu | Pos: %-4lu | Data: %s\n",
                                i, TOKENS[token.token], token.line, token.pos, token.data);
                }
                else
                {
                        printf("Token %-6lu: %-20d | Line: %-4lu | Pos: %-4lu | Data: %s\n",
                                i, token.token, token.line, token.pos, token.data);
                }
        }

        return 0;
}

int writeTokensToFile(LMeta_t* metadata, LData_t* lexerData, const char* filename)
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

        FILE* outFile = fopen(filename, "wb");
        if (outFile == NULL)
        {
                fprintf(stderr, "Failed to open output file: %s\n", filename);
                return 1;
        }

        uint64_t srcFilenameLen = strlen(metadata->filename);
        fwrite(&srcFilenameLen, sizeof(uint64_t), 1, outFile);
        fwrite(metadata->filename, sizeof(char), srcFilenameLen, outFile);
        fwrite(&metadata->fileSize, sizeof(uint64_t), 1, outFile);
        fwrite(&metadata->numTokens, sizeof(uint64_t), 1, outFile);

        int64_t expectedOutputSize = sizeof(int64_t) + sizeof(uint64_t) + srcFilenameLen * sizeof(char) + sizeof(uint64_t) + sizeof(uint64_t);
        for (uint64_t i = 0; i < metadata->numTokens; i++)
        {
                LTok_t token = lexerData->tokens[i];
                fwrite(&token.token, sizeof(uint32_t), 1, outFile);
                fwrite(&token.line, sizeof(uint64_t), 1, outFile);
                fwrite(&token.pos, sizeof(uint64_t), 1, outFile);
                fwrite(&token.len, sizeof(uint64_t), 1, outFile);
                fwrite(token.data, sizeof(char), token.len, outFile);
                expectedOutputSize += sizeof(uint32_t) + sizeof(uint64_t) + sizeof(uint64_t) + sizeof(uint64_t) + token.len * sizeof(char);
        }
        fwrite(&expectedOutputSize, sizeof(int64_t), 1, outFile);
        fclose(outFile);

        int64_t actualOutputSize = 0;
        FILE* verifyFile = fopen(filename, "rb");
        if (verifyFile == NULL)
        {
                fprintf(stderr, "Failed to open output file for verification: %s\n", filename);
                return 1;
        }

        fseek(verifyFile, 0, SEEK_END);
        actualOutputSize = ftell(verifyFile);
        if (actualOutputSize < 0)
        {
                fprintf(stderr, "ftell failed during verification.\n");
                fclose(verifyFile);
                return 1;
        }
        fclose(verifyFile);

        if (actualOutputSize != expectedOutputSize)
        {
                fprintf(stderr, "Output size mismatch: expected %ld, got %ld\n", expectedOutputSize, actualOutputSize);
                return 1;
        }

        return 0;
}
