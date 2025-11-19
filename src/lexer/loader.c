#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../defines.h"
#include "loader.h"
#include "lexer.h"

#include "../errors.h"

LErr_t openSourceFile(FILE** file, char* filename, LMeta_t* metadata)
{
        uint64_t nameSize = strlen(filename);
        if (nameSize == 0)
        {
                ERROR_LEX_VERBOSE("openSourceFile: nameSize cannot be 0\n", NULL);
                return LEX_LOADER_FILENAME_ZERO;
        }

        if (nameSize < 4)
        {
                ERROR_LEX_VERBOSE("openSourceFile: nameSize cannot be smaller than 4\n", NULL);
                return LEX_LOADER_FILENAME_TOO_SMALL;
        }

        char extension[5] = {filename[nameSize - 4], filename[nameSize - 3],
                             filename[nameSize - 2], filename[nameSize - 1], '\0'};

        if (strcmp(extension, ".mai") != 0)
        {
                ERROR_LEX_VERBOSE("openSourceFile: Provided file is not a Maiora source file.\n", NULL);
                return LEX_LOADER_FILE_EXT_INCORRECT;
        }

        char *srcname = (char*)malloc(MAX_FILE_NAME_LEN + 1);
        if (srcname == NULL)
        {
                ERROR_LEX_VERBOSE("openSourceFile: Memory allocation failed for srcname.\n", NULL);
                return LEX_MALLOC_FAIL;
        }


        if (nameSize >= MAX_FILE_NAME_LEN)
        {
                ERROR_LEX_VERBOSE("openSourceFile: Filename too long: %s\n", filename);
                return LEX_LOADER_FILENAME_TOO_BIG;
        }
        strcpy(srcname, filename);

        *file = fopen(srcname, "r");
        if (*file == NULL)
        {
                ERROR_LEX_VERBOSE("openSourceFile: File: %s could not be opened.\n", filename);
                free(srcname);
                return LEX_LOADER_FAILED_OPEN;
        }

        metadata->filename = (char*)malloc(nameSize + 1);
        if (metadata->filename == NULL)
        {
                ERROR_LEX_VERBOSE("openSourceFile: Memory allocation failed for metadata->filename.\n", NULL);
                free(srcname);
                return LEX_MALLOC_FAIL;
        }

        strcpy(metadata->filename, srcname);
        metadata->filename[nameSize] = '\0';

        free(srcname);

        return LEX_SUCCESS;
}

LErr_t loadSourceFile(char** src, FILE* file, LMeta_t* metadata)
{
        fseek(file, 0, SEEK_END);
        int64_t fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);
        if (fileSize == 0)
        {
                ERROR_LEX_VERBOSE("loadSourceFile: File size is 0.\n", NULL);
                return LEX_LOADER_FILENAME_ZERO;
        }
        else if (fileSize < 0)
        {
                ERROR_LEX_VERBOSE("loadSourceFile: ftell failed.\n", NULL);
                return LEX_LOADER_FTELL_FAIL;
        }

        *src = (char*)malloc((uint64_t)fileSize + 1);
        if (*src == NULL)
        {
                ERROR_LEX_VERBOSE("loadSourceFile: Memory allocation failed for data.\n", NULL);
                return LEX_MALLOC_FAIL;
        }

        uint64_t size = fread(*src, 1, (uint64_t)fileSize, file);
        (*src)[fileSize] = '\0';
        if (size != (uint64_t)fileSize)
        {
                ERROR_LEX_VERBOSE("loadSourceFile: Wrong amount of data read. Got: %lu Expected: %lu\n", size, fileSize);
                return LEX_LOADER_DATAREAD_MISMATCH;
        }

        metadata->fileSize = (uint64_t)fileSize;

        return LEX_SUCCESS;
}
