#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "loader.h"
#include "tokenizer.h"
#include "token.h"

int main(int argc, char* argv[])
{
        // expected call: ./main <input_file>.mai <output_file>.tok

        if (argc != 3)
        {
                fprintf(stderr, "Usage: %s <input_file>.mai <output_file>.tok\n", argv[0]);
                return 1;
        }

        LMeta_t metadata;

        FILE* file;
        if (openSourceFile(&file, argv[1], &metadata) == 1)
        {
                fprintf(stderr, "Failed to open source file.\n");
                return 1;
        }

        char* src;
        if (loadSourceFile(&src, file, &metadata) == 1)
        {
                fprintf(stderr, "Failed to load source file.\n");
                fclose(file);
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

        // -- DEBUG INFO ---
        printf("Generated %lu tokens:\n", metadata.numTokens);
        for (uint64_t i = 0; i < metadata.numTokens; i++)
        {
                LTok_t token = lexerData.tokens[i];
                if (TOKENS[token.token] != NULL)
                        printf("Token %-6lu: %-20s | Line: %-4lu | Pos: %-4lu | Data: %s\n",
                                i, TOKENS[token.token], token.line, token.pos, token.data);
                else
                        printf("Token %-6lu: %-20d | Line: %-4lu | Pos: %-4lu | Data: %s\n",
                                i, token.token, token.line, token.pos, token.data);
        }
        // ----------------

        free(src);

        return 0;
}
