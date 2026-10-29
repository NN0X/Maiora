#ifndef STRING_H
#define STRING_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct String {
        char* data;
        uint64_t len;
} Str_t;

static inline int strMalloc(Str_t* str, uint64_t len)
{
        str->data = (char*)malloc(sizeof(char) * (len + 1));
        if (!str->data)
        {
                return 1;
        }
        str->len = len;
        str->data[len] = '\0';

        return 0;
}

static inline int strSet(Str_t* str, const char* src)
{
        uint64_t i = 0;
        while (src[i] != '\0')
        {
                i++;
        }

        if (strMalloc(str, i) != 0)
        {
                return 1;
        }

        for (uint64_t j = 0; j < i; j++)
        {
                str->data[j] = src[j];
        }

        return 0;
}

static inline int strCopy(Str_t* dest, const Str_t* src)
{
        if (strMalloc(dest, src->len) != 0)
        {
                return 1;
        }

        for (uint64_t i = 0; i < src->len; i++)
        {
                dest->data[i] = src->data[i];
        }

        return 0;
}

static inline void strFree(Str_t* str)
{
        if (str->data)
        {
                free(str->data);
                str->data = NULL;
                str->len = 0;
        }
}

static inline bool strEqual(const Str_t* str1, const Str_t* str2)
{
        if (str1->len != str2->len)
        {
                return false;
        }

        for (uint64_t i = 0; i < str1->len; i++)
        {
                if (str1->data[i] != str2->data[i])
                {
                        return false;
                }
        }

        return true;
}

#endif // STRING_H
