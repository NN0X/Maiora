#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "loader.h"
#include "tokenizer.h"

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

        // -- DEBUG INFO ---
        printf("Generated %lu tokens:\n", metadata.numTokens);
        for (uint64_t i = 0; i < metadata.numTokens; i++)
        {
                LTok_t token = lexerData.tokens[i];
                if (token.token < TOK_STR_STUB)
                    printf("Token %lu: | %s | line: %lu | pos: %lu\n", i, TOKENS[token.token], token.line, token.pos);
                else if (token.token != TOK_SPACE_STUB)
                    printf("STR_STUB %lu: | %s | line: %lu | pos: %lu\n", i, token.data, token.line, token.pos);
        }
        // ----------------

        free(src);

        return 0;
}
