#ifndef LOADER_H
#define LOADER_H

#include "../lexer/lexer.h"

int loadTokensFromFile(LMeta_t* metadata, LData_t* lexerData, const char* filename);

#endif // LOADER_H
