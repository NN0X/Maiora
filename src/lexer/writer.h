#ifndef WRITER_H
#define WRITER_H

#include <stdio.h>
#include <stdlib.h>

int writeTokensToOut(FILE* out, LMeta_t* metadata, LData_t* lexerData);
int writeTokensToFile(LMeta_t* metadata, LData_t* lexerData, const char* filename);

#endif // WRITER_H
