#ifndef ERRORS_H
#define ERRORS_H

#include <stdio.h>
#include <stdarg.h>

#include "defines.h"

typedef enum PreprocessorErrors
{
        PRE_ERR_UNKNOWN = -1,
} PrErr_t;

typedef enum LexerErrors
{
        LEX_SUCCESS = 0,

        LEX_MALLOC_FAIL,

        LEX_LOADER_FILENAME_ZERO,
        LEX_LOADER_FILENAME_TOO_SMALL,
        LEX_LOADER_FILE_EXT_INCORRECT,
        LEX_LOADER_FILENAME_TOO_BIG,
        LEX_LOADER_FAILED_OPEN,
        LEX_LOADER_FILESIZE_ZERO,
        LEX_LOADER_FTELL_FAIL,
        LEX_LOADER_DATAREAD_MISMATCH,

        LEX_ARGS_INCORRECT,

        LEX_WRITER_FILENAME_ZERO,
        LEX_WRITER_FILENAME_TOO_BIG,
        LEX_WRITER_FAILED_OPEN,
        LEX_WRITER_FAILED_OPEN_VERIFY,
        LEX_WRITER_FTELL_FAIL_VERIFY,
        LEX_WRITER_OUTPUT_SIZE_MISMATCH,

        LEX_ERR_UNKNOWN,

        LEX_META_ERROR_NUM
} LErr_t;

static const char* LEXER_ERROR_MSGS[LEX_META_ERROR_NUM] = {
        [LEX_ERR_UNKNOWN] = "Unknown error occured.\n",

        [LEX_MALLOC_FAIL] = "malloc failed for %s.\n",

        [LEX_LOADER_FILENAME_ZERO] = "Loader: Filename cannot have length 0.\n",
        [LEX_LOADER_FILENAME_TOO_SMALL] = "Loader: Filename cannot have length below 4.\n",
        [LEX_LOADER_FILE_EXT_INCORRECT] = "Loader: Provided file is not a Maiora source file (wrong extension)\n",
        [LEX_LOADER_FILENAME_TOO_BIG] = "Loader: Filename too long.\n",
        [LEX_LOADER_FAILED_OPEN] = "Loader: File could not be opened.\n",
        [LEX_LOADER_FILESIZE_ZERO] = "Loader: File size cannot be 0.\n",
        [LEX_LOADER_FTELL_FAIL] = "Loader: ftell failed.\n",
        [LEX_LOADER_DATAREAD_MISMATCH] = "Loader: Data mismatch.\n",

        [LEX_ARGS_INCORRECT] = "Usage: %s <input_file>.mai <output_file>.tok\n",

        [LEX_WRITER_FILENAME_ZERO] = "Writer: Filename cannot have length 0.\n",
        [LEX_WRITER_FILENAME_TOO_BIG] = "Writer: Filename too long.\n",
        [LEX_WRITER_FAILED_OPEN] = "Writer: File could not be opened.\n",
        [LEX_WRITER_FAILED_OPEN_VERIFY] = "Writer: File could not be opened for verification.\n",
        [LEX_WRITER_FTELL_FAIL_VERIFY] = "Writer: ftell failed for verification.\n",
        [LEX_WRITER_OUTPUT_SIZE_MISMATCH] = "Writer: Output size mismatch.\n",
};

static inline void printErrorLex(LErr_t error, ...)
{
        va_list args;
        va_start(args, error);
        vfprintf(stderr, LEXER_ERROR_MSGS[error], args);
        va_end(args);
}

#define ERROR_LEX(error, ...) printErrorLex(error, ##__VA_ARGS__)
#ifdef ERROR_VERBOSE
#define ERROR_LEX_VERBOSE(msg, ...) fprintf(stderr, msg, ##__VA_ARGS__)
#else
#define ERROR_LEX_VERBOSE(msg, ...)
#endif // ERROR_VERBOSE

typedef enum ParserErrors
{
        PAR_ERR_UNKNOWN = -1,
} PaErr_t;

#endif
