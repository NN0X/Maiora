#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "loader.h"
#include "tokenizer.h"
#include "token.h"
#include "writer.h"

#include "../errors.h"

int main(int argc, char* argv[])
{
        // expected call: ./main <input_file>.mai <output_file>.tok

        if (argc != 3)
        {
                ERROR_LEX(LEX_ARGS_INCORRECT);
                return 1;
        }

        LMeta_t metadata;

        FILE* file;
        LErr_t code = openSourceFile(&file, argv[1], &metadata);
        if (code != LEX_SUCCESS)
        {
                ERROR_LEX(code);
                return 1;
        }

        char* src;
        code = loadSourceFile(&src, file, &metadata);
        if (code != LEX_SUCCESS)
        {
                ERROR_LEX(code);
                fclose(file);
                return 1;
        }
        fclose(file);

        LData_t lexerData;
        code = tokenizeSource(&lexerData, src, &metadata);
        if (code != LEX_SUCCESS)
        {
                ERROR_LEX(code);
                free(src);
                return 1;
        }
        free(src);

        // -- DEBUG INFO ---
        writeTokensToOut(stdout, &metadata, &lexerData);
        // ----------------

        code = writeTokensToFile(&metadata, &lexerData, argv[2]);
        if (code != LEX_SUCCESS)
        {
                ERROR_LEX(code);
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
