#ifndef LOADER_H
#define LOADER_H

#include "lexer.h"

#include "../errors.h"

LErr_t openSourceFile(FILE** file, char* filename, LMeta_t* metadata);
LErr_t loadSourceFile(char** src, FILE* file, LMeta_t* metadata);

#endif // LOADER_H
