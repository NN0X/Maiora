#include <stdio.h>
#include <cstdint.h>

const uint64_t MAX_FILE_NAME_LEN = 1024;

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
        uint64_t nameSize;
        uint64_t numTokens;
} LMeta_t;

int main(int argc, char *argv[])
{
        // expected call: ./main <input_file>.mai

        if (argc != 2) {
                fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
                return 1;
        }

        char *filename[MAX_FILE_NAME_LEN];
        filename = strncpy(filename, argv[1], MAX_FILE_NAME_LEN);
        if (strcmp(filename, argv[1]) != 0)
        {
                fprintf(stderr, "Filename of: %s is too long.", argv[1]);
                return 1;
        }

        FILE* file = fopen(filename, "r");
        if (file == NULL)
        {
                fprintf(stderr, "File: %s could not be opened.", argv[1]);
                return 1;
        }

        return 0;
}
