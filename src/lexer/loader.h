#ifndef LOADER_H
#define LOADER_H

#include "lexer.h"

int openSourceFile(FILE** file, char* filename, LMeta_t* metadata);
int loadSourceFile(char** src, FILE* file, LMeta_t* metadata);

#endif // LOADER_H
