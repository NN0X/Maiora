#include <stdio.h>

#include "ast.h"
#include "../lexer/lexer.h"
#include "loader.h"

int main(int argc, char *argv[])
{
        // example call: ./main <input_file>.tok <output_file>.ll

        LData_t lexerData;
        loadTokensFromFile(&lexerData, "example.tok");
        printf("Number of tokens: %lu", lexerData.metadata.numTokens);

        return 0;
}
