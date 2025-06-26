#ifndef LEXER_H
#define LEXER_H

#include "token.h"

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

#endif // LEXER_H
