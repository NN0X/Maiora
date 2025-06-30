#ifndef LOADER_H
#define LOADER_H

#include "lexer.h"

#define MAX_FILE_NAME_LEN 1024
#define MAX_STATEMENT_LEN 1024

int openSourceFile(FILE** file, char* filename, LMeta_t* metadata);
int loadSourceFile(char** src, FILE* file, LMeta_t* metadata);

#endif // LOADER_H
