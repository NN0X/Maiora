#ifndef TOKEN_H
#define TOKEN_H

#include <stdint.h>

enum Token
{
        // --- KEYWORDS ---

        TOK_KEYW_RETURN = 0,    // return

        // modifiers
        TOK_KEYW_PUBLIC,        // public
        TOK_KEYW_PRIVATE,       // private
        TOK_KEYW_ENTRY,         // entry

        // functions
        TOK_KEYW_INSTANCE,      // instance
        TOK_KEYW_FUNCTION,      // function

        // memory
        TOK_KEYW_HEAP,          // heap
        TOK_KEYW_ADDRESS,       // address
        TOK_KEYW_REFERENCE,     // reference
        TOK_KEYW_REF,           // ref
        TOK_KEYW_AT,            // at

        // control flow
        TOK_KEYW_WHILE,         // while
        TOK_KEYW_FOR,           // for
        TOK_KEYW_IF,            // if
        TOK_KEYW_ELSE,          // else
        TOK_KEYW_CONTINUE,      // continue
        TOK_KEYW_BREAK,         // break
        TOK_KEYW_SWITCH,        // switch
        TOK_KEYW_CASE,          // case


        // --- OPERATORS ---

        // comparators
        TOK_OP_EQUALS,          // ==
        TOK_OP_LESS,            // <
        TOK_OP_MORE,            // >
        TOK_OP_LESS_EQUALS,     // <=
        TOK_OP_MORE_EQUALS,     // >=
        TOK_OP_NOT_EQUALS,      // !=
        TOK_OP_AND,             // &&
        TOK_OP_OR,              // ||
        TOK_OP_NOT,             // !

        // accessors
        TOK_OP_FROM,            // .
        TOK_OP_IN,              // :
        TOK_OP_FROM_MODULE,     // ::

        // modifiers
        TOK_OP_ASSIGN,          // =
        TOK_OP_ADD,             // +
        TOK_OP_SUBTRACT,        // -
        TOK_OP_DIVIDE,          // /
        TOK_OP_MULTIPLY,        // *
        TOK_OP_MODULO,          // %
        TOK_OP_INCREMENT,       // ++
        TOK_OP_DECREMENT,       // --
        TOK_OP_ADD_ASSIGN,      // +=
        TOK_OP_SUB_ASSIGN,      // -=
        TOK_OP_DIV_ASSIGN,      // /=
        TOK_OP_MUL_ASSIGN,      // *=
        TOK_OP_BITWISE_AND,     // &
        TOK_OP_BITWISE_OR,      // |
        TOK_OP_BITWISE_XOR,     // ^
        TOK_OP_BITWISE_NOT,     // ~
        TOK_OP_SHIFT_LEFT,      // <<
        TOK_OP_SHIFT_RIGHT,     // >>

        // multi-purpose
        TOK_OP_LCURLY,          // {
        TOK_OP_RCURLY,          // }
        TOK_OP_LBRACKET,        // [
        TOK_OP_RBRACKET,        // ]
        TOK_OP_LPAR,            // (
        TOK_OP_RPAR,            // )
        TOK_OP_QUOTE,           // '
        TOK_OP_DQUOTE,          // "

        // comma
        TOK_OP_COMMA,           // ,

        // --- TYPES ---

        TOK_TYPE_NONE,          // none
        TOK_TYPE_BOOL,          // bool

        // integers
        TOK_TYPE_SINT8,         // sint8
        TOK_TYPE_SINT16,        // sint16
        TOK_TYPE_SINT32,        // sint32
        TOK_TYPE_SINT64,        // sint64
        TOK_TYPE_UINT8,         // uint8
        TOK_TYPE_UINT16,        // uint16
        TOK_TYPE_UINT32,        // uint32
        TOK_TYPE_UINT64,        // uint64

        // floats
        TOK_TYPE_FLOAT8,        // float8
        TOK_TYPE_FLOAT16,       // float16
        TOK_TYPE_FLOAT32,       // float32
        TOK_TYPE_FLOAT64,       // float64

        // characters
        TOK_TYPE_ASCII,         // ascii
        TOK_TYPE_UTF8,          // utf8


        // --- STRINGS ---

        TOK_STR_STUB,           // placeholder for tokens that can be considered strings before lexer string filtering phase
        TOK_SPACE_STUB,         // placeholder for spaces

        // --- LITERALS ---

        TOK_LIT_STUB,           // placeholder for literals before lexer literation phase

        TOK_LIT_SINT8,          // -1s
        TOK_LIT_SINT16,         // -1s
        TOK_LIT_SINT32,         // -1s
        TOK_LIT_SINT64,         // 1s
        TOK_LIT_UINT8,          // 1s
        TOK_LIT_UINT16,         // 1s
        TOK_LIT_UINT32,         // 1s
        TOK_LIT_UINT64,         // 1s

        TOK_LIT_FLOAT8,         // 0.1
        TOK_LIT_FLOAT16,        // 0.1
        TOK_LIT_FLOAT32,        // 0.1
        TOK_LIT_FLOAT64,        // 0.1

        TOK_LIT_ASCII,          // 'a'
        TOK_LIT_UTF8,           // 'ą'


        // --- IDENTIFIERS ---

        TOK_ID_STUB,            // placeholder for ids before lexer id phase

        // functions
        TOK_ID_FUNCTION,        // fooFunction

        // instances
        TOK_ID_INSTANCE,        // fooInstance
        TOK_ID_INSTANCE_ALIAS,  // string

        // variables
        TOK_ID_VARIABLE,        // fooVar

        // modules
        TOK_ID_MODULE,          // <tok_id_module>::<tok_id_function> -> IO::print

        TOK_META_NUM,
};

#define TOK_META_STR_BEGIN TOK_STR_STUB

extern const char* TOKENS[TOK_META_NUM];

typedef struct LexerToken
{
        char* data;
        uint32_t token;
        uint64_t line;
        uint64_t pos;
        uint64_t len;
} LTok_t;

#endif // TOKEN_H
