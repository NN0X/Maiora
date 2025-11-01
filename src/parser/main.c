#include <stdio.h>

#include "ast.h"
#include "../lexer/lexer.h"
#include "loader.h"

int main(int argc, char *argv[])
{
        // example call: ./main <input_file>.tok <output_file>.ll

        LData_t lexerData;
        if (loadTokensFromFile(&lexerData, "example.tok") == 1)
        {
                fprintf(stderr, "loadTokensFromFile failed.\n");
                return 1;
        }
        printf("Number of tokens: %lu\n", lexerData.metadata.numTokens);

        ANode_t root;
        root.type = AST_ROOT;
        //if (generateAST(lexerData, &root) == 1)
        //{
        //        fprintf(stderr, "generateAST failed.\n");
        //        return 1;
        //}

        return 0;
}
