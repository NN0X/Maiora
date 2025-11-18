#ifndef ERRORS_H
#define ERRORS_H

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

        [LEX_LOADER_FILENAME_ZERO] = "",
};


typedef enum ParserErrors
{
        PAR_ERR_UNKNOWN = -1,
} PaErr_t;

#define ERROR_LEX(error, args...) fprintf(stderr, LEXER_ERROR_MSGS[error], args)

#endif
