#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

const uint64_t MAX_FILE_NAME_LEN = 1024;

const char* KEYW_RETURN = "return";
const char* KEYW_PUBLIC = "public";

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

        // accessors
        TOK_OP_FROM,            // .
        TOK_OP_IN,              // :

        // modifiers
        TOK_OP_ASSIGN,
        TOK_OP_ADD,
        TOK_OP_SUBTRACT,
        TOK_OP_DIVIDE,
        TOK_OP_MULTIPLY,

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

        TOK_ID_FUNCTION,
        TOK_ID_VARIABLE,
        TOK_ID_INSTANCE,
        TOK_ID_INSTANCE_ALIAS,

};

typedef struct Tok
{
        char* data;
        uint32_t token;
        uint64_t line;
        uint64_t pos;
        uint64_t len;
} Tok_t;

typedef struct LexerMetadata
{
        char* filename;
        uint64_t fileSize;
        uint64_t numTokens;
} LMeta_t;

FILE* openSourceFile(char* filename, LMeta_t* metadata)
{
        uint64_t nameSize = strlen(filename);
        if (nameSize == 0)
        {
                fprintf(stderr, "nameSize cannot be 0\n");
                return NULL;
        }

        char extension[4] = {filename[nameSize - 4], filename[nameSize - 3],
                             filename[nameSize - 2], filename[nameSize - 1]};

        if (strcmp(extension, ".mai") != 0)
        {
                fprintf(stderr, "Provided file is not a Maiora source file.\n");
                return NULL;
        }

        char *srcname = (char*)malloc(MAX_FILE_NAME_LEN);
        if (srcname == NULL)
        {
                fprintf(stderr, "Memory allocation failed for srcname.\n");
                return NULL;
        }

        srcname = strncpy(srcname, filename, MAX_FILE_NAME_LEN);
        if (strcmp(srcname, filename) != 0)
        {
                fprintf(stderr, "Filename of: %s is too long or strncpy failed.\n", filename);
                return NULL;
        }

        FILE* file = fopen(srcname, "r");
        if (file == NULL)
        {
                fprintf(stderr, "File: %s could not be opened.\n", filename);
                return NULL;
        }

        metadata->filename = (char*)malloc(nameSize);
        if (metadata->filename == NULL)
        {
                fprintf(stderr, "Memory allocation failed for metadata->filename.\n");
                return NULL;
        }

        metadata->filename = strncpy(metadata->filename, srcname, nameSize);
        if (strcmp(metadata->filename, srcname) != 0)
        {
                fprintf(stderr, "strncpy failed on metadata->filename.\n");
                return NULL;
        }

        return file;
}

char* loadSourceFile(FILE* file, LMeta_t* metadata)
{
        fseek(file, 0, SEEK_END);
        uint64_t fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);
        if (fileSize <= 0)
        {
                fprintf(stderr, "File size is %lu", fileSize);
                return NULL;
        }

        char* data = (char*)malloc(fileSize);
        if (data == NULL)
        {
                fprintf(stderr, "Memory allocation failed for data.\n");
                return NULL;
        }

        uint64_t size = fread(data, 1, fileSize, file);
        if (size != fileSize)
        {
                fprintf(stderr, "Wrong amount of data read. Got: %lu Expected: %lu\n", size, fileSize);
                return NULL;
        }

        metadata->fileSize = fileSize;

        return data;
}

int main(int argc, char *argv[])
{
        // expected call: ./main <input_file>.mai

        if (argc != 2) {
                fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
                return 1;
        }

        LMeta_t metadata;

        FILE* file = openSourceFile(argv[1], &metadata);
        if (file == NULL) {
                fprintf(stderr, "Failed to open source file.\n");
                return 1;
        }

        char* src = loadSourceFile(file, &metadata);
        fclose(file);
        if (src == NULL) {
                fprintf(stderr, "Failed to load source file.\n");
                return 1;
        }

        return 0;
}
