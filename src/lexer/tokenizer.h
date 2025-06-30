#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "token.h"
#include "lexer.h"

int tokenizeSource(LData_t* lexerData, char* src, LMeta_t* metadata);

#endif // TOKENIZER_H
