#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "loader.h"
#include "lexer.h"

int openSourceFile(FILE** file, char* filename, LMeta_t* metadata)
{
        uint64_t nameSize = strlen(filename);
        if (nameSize == 0)
        {
                fprintf(stderr, "nameSize cannot be 0\n");
                return 1;
        }

        if (nameSize < 4)
        {
                fprintf(stderr, "nameSize cannot be smaller than 4\n");
                return 1;
        }

        char extension[5] = {filename[nameSize - 4], filename[nameSize - 3],
                             filename[nameSize - 2], filename[nameSize - 1], '\0'};

        if (strcmp(extension, ".mai") != 0)
        {
                fprintf(stderr, "Provided file is not a Maiora source file.\n");
                return 1;
        }

        char *srcname = (char*)malloc(MAX_FILE_NAME_LEN + 1);
        if (srcname == NULL)
        {
                fprintf(stderr, "Memory allocation failed for srcname.\n");
                return 1;
        }


        if (nameSize >= MAX_FILE_NAME_LEN)
        {
                fprintf(stderr, "Filename too long: %s\n", filename);
                return 1;
        }
        strcpy(srcname, filename);

        *file = fopen(srcname, "r");
        if (*file == NULL)
        {
                fprintf(stderr, "File: %s could not be opened.\n", filename);
                free(srcname);
                return 1;
        }

        metadata->filename = (char*)malloc(nameSize + 1);
        if (metadata->filename == NULL)
        {
                fprintf(stderr, "Memory allocation failed for metadata->filename.\n");
                free(srcname);
                return 1;
        }

        strcpy(metadata->filename, srcname);
        metadata->filename[nameSize] = '\0';

        free(srcname);
        uint64_t currLine = 1;

        return 0;
}

int loadSourceFile(char** src, FILE* file, LMeta_t* metadata)
{
        fseek(file, 0, SEEK_END);
        uint64_t fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);
        if (fileSize <= 0)
        {
                fprintf(stderr, "File size is %lu", fileSize);
                return 1;
        }

        *src = (char*)malloc(fileSize + 1);
        if (*src == NULL)
        {
                fprintf(stderr, "Memory allocation failed for data.\n");
                return 1;
        }

        uint64_t size = fread(*src, 1, fileSize, file);
        (*src)[fileSize] = '\0';
        if (size != fileSize)
        {
                fprintf(stderr, "Wrong amount of data read. Got: %lu Expected: %lu\n", size, fileSize);
                return 1;
        }

        metadata->fileSize = fileSize;

        return 0;
}
