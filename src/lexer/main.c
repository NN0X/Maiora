#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "loader.h"
#include "tokenizer.h"
#include "token.h"
#include "writer.h"

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
        free(src);

        // -- DEBUG INFO ---
        writeTokensToOut(stdout, &metadata, &lexerData);
        // ----------------

        if (writeTokensToFile(&metadata, &lexerData, argv[2]) == 1)
        {
                fprintf(stderr, "Failed to write tokens to output file.\n");
                for (uint64_t i = 0; i < metadata.numTokens; i++)
                {
                        free(lexerData.tokens[i].data);
                }
                free(lexerData.tokens);
                free(metadata.filename);
                return 1;
        }

        return 0;
}
