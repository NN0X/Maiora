#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

const uint64_t MAX_FILE_NAME_LEN = 1024;

const char* KEYW_RETURN = "return";

const char* KEYW_PUBLIC = "public";
const char* KEYW_PRIVATE = "private";
const char* KEYW_ENTRY = "entry";

const char* KEYW_INSTANCE = "instance";
const char* KEYW_FUNCTION = "function";

const char* KEYW_HEAP = "heap";
const char* KEYW_ADDRESS = "address";
const char* KEYW_REFERENCE = "reference";
const char* KEYW_REF = "ref";
const char* KEYW_AT = "at";

const char* KEYW_WHILE = "while";
const char* KEYW_FOR = "for";
const char* KEYW_IF = "if";
const char* KEYW_ELSE = "else";
const char* KEYW_CONTINUE = "continue";
const char* KEYW_BREAK = "break";
const char* KEYW_SWITCH = "switch";
const char* KEYW_CASE = "case";

const char* OP_EQUALS = "==";
const char* OP_LESS = "<";
const char* OP_MORE = ">";

const char* OP_FROM = ".";
const char* OP_IN = ":";

const char* OP_ASSIGN = "=";
const char* OP_ADD = "+";
const char* OP_SUBTRACT = "-";
const char* OP_DIVIDE = "/";
const char* OP_MULTIPLY = "*";

const char* OP_LCURLY = "{";
const char* OP_RCURLY = "}";
const char* OP_LBRACKET = "[";
const char* OP_RBRACKET = "]";
const char* OP_LPAR = "(";
const char* OP_RPAR = ")";

const char* TYPE_NONE = "none";
const char* TYPE_BOOL = "bool";

const char* TYPE_SINT8 = "sint8";
const char* TYPE_SINT16 = "sint16";
const char* TYPE_SINT32 = "sint32";
const char* TYPE_SINT64 = "sint64";
const char* TYPE_UINT8 = "uint8";
const char* TYPE_UINT16 = "uint16";
const char* TYPE_UINT32 = "uint32";
const char* TYPE_UINT64 = "uint64";

const char* TYPE_FLOAT8 = "float8";
const char* TYPE_FLOAT16 = "float16";
const char* TYPE_FLOAT32 = "float32";
const char* TYPE_FLOAT64 = "float64";

const char* TYPE_ASCII = "ascii";
const char* TYPE_UTF8 = "utf8";

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

LData_t tokenizeSource(char* src, LMeta_t* metadata)
{
        LData_t lexerData;
        lexerData.metadata = *metadata;
        lexerData.tokens = (LTok_t*)malloc(sizeof(LTok_t) * metadata->fileSize);
        if (lexerData.tokens == NULL)
        {
                fprintf(stderr, "Memory allocation failed for tokens");
                return NULL;
        }

        return lexerData;
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

        LData_t lexerData = tokenizeSource(src, &metadata);
        if (lexerData = NULL)
        {
                fprintf(stderr, "Failed to tokenize source file.\n");
                return 1;
        }

        return 0;
}
