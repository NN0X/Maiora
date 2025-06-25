#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

const uint64_t MAX_FILE_NAME_LEN = 1024;
const uint64_t MAX_STATEMENT_LEN = 1024;

enum Token
{
        // --- KEYWORDS ---

        TOK_KEYW_RETURN = 0,

        // modifiers
        TOK_KEYW_PUBLIC,
        TOK_KEYW_PRIVATE,
        TOK_KEYW_ENTRY,

        // functions
        TOK_KEYW_INSTANCE,
        TOK_KEYW_FUNCTION,

        // memory
        TOK_KEYW_HEAP,
        TOK_KEYW_ADDRESS,
        TOK_KEYW_REFERENCE,
        TOK_KEYW_REF,
        TOK_KEYW_AT,

        // control flow
        TOK_KEYW_WHILE,
        TOK_KEYW_FOR,
        TOK_KEYW_IF,
        TOK_KEYW_ELSE,
        TOK_KEYW_CONTINUE,
        TOK_KEYW_BREAK,
        TOK_KEYW_SWITCH,
        TOK_KEYW_CASE,


        // --- OPERATORS ---

        // comparators
        TOK_OP_EQUALS,
        TOK_OP_LESS,
        TOK_OP_MORE,
        TOK_OP_LESS_EQUALS,
        TOK_OP_MORE_EQUALS,
        TOK_OP_NOT_EQUALS,
        TOK_OP_AND,             // &&
        TOK_OP_OR,              // ||
        TOK_OP_NOT,             // !

        // accessors
        TOK_OP_FROM,            // .
        TOK_OP_IN,              // :
        TOK_OP_FROM_MODULE,     // ::

        // modifiers
        TOK_OP_ASSIGN,
        TOK_OP_ADD,
        TOK_OP_SUBTRACT,
        TOK_OP_DIVIDE,
        TOK_OP_MULTIPLY,
        TOK_OP_MODULO,          // %
        TOK_OP_INCREMENT,       // ++
        TOK_OP_DECREMENT,       // --
        TOK_OP_BITWISE_AND,     // &
        TOK_OP_BITWISE_OR,      // |
        TOK_OP_BITWISE_XOR,     // ^
        TOK_OP_BITWISE_NOT,     // ~
        TOK_OP_SHIFT_LEFT,      // <<
        TOK_OP_SHIFT_RIGHT,     // >>

        // multi-purpose
        TOK_OP_LCURLY,          // {
        TOK_OP_RCURLY,          // }
        TOK_OP_LBRACKET,        // [
        TOK_OP_RBRACKET,        // ]
        TOK_OP_LPAR,            // (
        TOK_OP_RPAR,            // )


        // --- TYPES ---

        TOK_TYPE_NONE,
        TOK_TYPE_BOOL,

        // integers
        TOK_TYPE_SINT8,
        TOK_TYPE_SINT16,
        TOK_TYPE_SINT32,
        TOK_TYPE_SINT64,
        TOK_TYPE_UINT8,
        TOK_TYPE_UINT16,
        TOK_TYPE_UINT32,
        TOK_TYPE_UINT64,

        // floats
        TOK_TYPE_FLOAT8,
        TOK_TYPE_FLOAT16,
        TOK_TYPE_FLOAT32,
        TOK_TYPE_FLOAT64,

        // characters
        TOK_TYPE_ASCII,
        TOK_TYPE_UTF8,


        // --- LITERALS ---

        TOK_LIT_SINT8,
        TOK_LIT_SINT16,
        TOK_LIT_SINT32,
        TOK_LIT_SINT64,
        TOK_LIT_UINT8,
        TOK_LIT_UINT16,
        TOK_LIT_UINT32,
        TOK_LIT_UINT64,

        TOK_LIT_FLOAT8,
        TOK_LIT_FLOAT16,
        TOK_LIT_FLOAT32,
        TOK_LIT_FLOAT64,

        TOK_LIT_ASCII,
        TOK_LIT_UTF8,


        // --- IDENTIFIERS ---

        // functions
        TOK_ID_FUNCTION,

        // instances
        TOK_ID_INSTANCE,
        TOK_ID_INSTANCE_ALIAS,

        // variables
        TOK_ID_VARIABLE,

        // modules
        TOK_ID_MODULE,          // <tok_id_module>::<tok_id_function>

};

typedef struct LexerToken
{
        char* data;
        uint32_t token;
        uint64_t line;
        uint64_t pos;
        uint64_t len;
} LTok_t;

typedef struct LexerMetadata
{
        char* filename;
        uint64_t fileSize;
        uint64_t numTokens;
} LMeta_t;

typedef struct LexerData
{
        LTok_t* tokens;
        LMeta_t metadata;
} LData_t;

int openSourceFile(FILE** file, char* filename, LMeta_t* metadata)
{
        uint64_t nameSize = strlen(filename);
        if (nameSize == 0)
        {
                fprintf(stderr, "nameSize cannot be 0\n");
                return 1;
        }

        char extension[4] = {filename[nameSize - 4], filename[nameSize - 3],
                             filename[nameSize - 2], filename[nameSize - 1]};

        if (strcmp(extension, ".mai") != 0)
        {
                fprintf(stderr, "Provided file is not a Maiora source file.\n");
                return 1;
        }

        char *srcname = (char*)malloc(MAX_FILE_NAME_LEN);
        if (srcname == NULL)
        {
                fprintf(stderr, "Memory allocation failed for srcname.\n");
                return 1;
        }

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
                return 1;
        }

        metadata->filename = (char*)malloc(nameSize);
        if (metadata->filename == NULL)
        {
                fprintf(stderr, "Memory allocation failed for metadata->filename.\n");
                return 1;
        }

        metadata->filename = strncpy(metadata->filename, srcname, nameSize);
        if (strcmp(metadata->filename, srcname) != 0)
        {
                fprintf(stderr, "strncpy failed on metadata->filename.\n");
                return 1;
        }

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

int tokenizeSource(LData_t* lexerData, char* src, LMeta_t* metadata)
{
        lexerData->metadata = *metadata;
        lexerData->tokens = (LTok_t*)malloc(sizeof(LTok_t) * metadata->fileSize);
        if (lexerData->tokens == NULL)
        {
                fprintf(stderr, "Memory allocation failed for tokens");
                return 1;
        }


        // INFO: statement is a line of code between semicolons or before curlies in case of blocks

        uint64_t line = 0;
        char* statement = (char*)malloc(MAX_STATEMENT_LEN);

        uint64_t pos = 0;
        for (uint64_t i = 0; i < metadata->fileSize; i++)
        {
                char c = src[i];
                if (c != '\n')
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
                line++;

                char* tokenData = (char*)malloc(MAX_STATEMENT_LEN);
                uint64_t tokenPos = 0;
                for (uint64_t currPos = 0; currPos < pos + 1; currPos++)
                {
                        if (statement[currPos] == ' ' && tokenPos != 0)
                        {
                                // TODO: create token

                                tokenPos = 0;
                                continue;
                        }
                        tokenData[tokenPos] = statement[currPos];
                        tokenPos++;
                }
                free(tokenData);
        }

        free(statement);

        return 0;
}

int main(int argc, char *argv[])
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
