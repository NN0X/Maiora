#ifndef TOKEN_H
#define TOKEN_H

#include <stdint.h>

typedef enum TokenTypes
{
        TOK_INVALID = -1,

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

        // block management
        TOK_KEYW_UNSAFE,        // unsafe
        TOK_KEYW_ASM,           // asm

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

        // comma and semicolon
        TOK_OP_COMMA,           // ,
        TOK_OP_SEMICOLON,       // ;

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
        TOK_SPACE,              // space

        // --- LITERALS ---

        TOK_LIT_INT,            // 123
        TOK_LIT_SINT,           // 123s
        TOK_LIT_UINT,           // 123u
        TOK_LIT_BOOL,           // true
        TOK_LIT_FLOAT,          // 0.1
        TOK_LIT_FLOAT32,        // 1f
        TOK_LIT_FLOAT64,        // 1d
        TOK_LIT_CHAR,           // 'a'


        // --- IDENTIFIERS ---

        TOK_ID,                 // variable_name

        TOK_META_NUM,
} TTypes_t;

#define TOK_META_STR_BEGIN TOK_STR_STUB

typedef struct LexerToken
{
        char* data;
        TTypes_t token;
        uint64_t line;
        uint64_t pos;
        uint64_t len;
} LTok_t;

static const char* TOKENS[TOK_META_NUM] = {
        [TOK_KEYW_RETURN] = "return",
        [TOK_KEYW_PUBLIC] = "public",
        [TOK_KEYW_PRIVATE] = "private",
        [TOK_KEYW_ENTRY] = "entry",

        [TOK_KEYW_INSTANCE] = "instance",
        [TOK_KEYW_FUNCTION] = "function",

        [TOK_KEYW_HEAP] = "heap",
        [TOK_KEYW_ADDRESS] = "address",
        [TOK_KEYW_REFERENCE] = "reference",
        [TOK_KEYW_REF] = "ref",
        [TOK_KEYW_AT] = "at",

        [TOK_KEYW_WHILE] = "while",
        [TOK_KEYW_FOR] = "for",
        [TOK_KEYW_IF] = "if",
        [TOK_KEYW_ELSE] = "else",
        [TOK_KEYW_CONTINUE] = "continue",
        [TOK_KEYW_BREAK] = "break",
        [TOK_KEYW_SWITCH] = "switch",
        [TOK_KEYW_CASE] = "case",

        [TOK_KEYW_UNSAFE] = "unsafe",
        [TOK_KEYW_ASM] = "asm",

        [TOK_OP_EQUALS] = "==",
        [TOK_OP_LESS] = "<",
        [TOK_OP_MORE] = ">",
        [TOK_OP_LESS_EQUALS] = "<=",
        [TOK_OP_MORE_EQUALS] = ">=",
        [TOK_OP_NOT_EQUALS] = "!=",
        [TOK_OP_AND] = "&&",
        [TOK_OP_OR] = "||",
        [TOK_OP_NOT] = "!",

        [TOK_OP_FROM] = ".",
        [TOK_OP_IN] = ":",
        [TOK_OP_FROM_MODULE] = "::",

        [TOK_OP_ASSIGN] = "=",
        [TOK_OP_ADD] = "+",
        [TOK_OP_SUBTRACT] = "-",
        [TOK_OP_DIVIDE] = "/",
        [TOK_OP_MULTIPLY] = "*",
        [TOK_OP_MODULO] = "%",
        [TOK_OP_INCREMENT] = "++",
        [TOK_OP_DECREMENT] = "--",
        [TOK_OP_ADD_ASSIGN] = "+=",
        [TOK_OP_SUB_ASSIGN] = "-=",
        [TOK_OP_DIV_ASSIGN] = "/=",
        [TOK_OP_MUL_ASSIGN] = "*=",
        [TOK_OP_BITWISE_AND] = "&",
        [TOK_OP_BITWISE_OR] = "|",
        [TOK_OP_BITWISE_XOR] = "^",
        [TOK_OP_BITWISE_NOT] = "~",
        [TOK_OP_SHIFT_LEFT] = "<<",
        [TOK_OP_SHIFT_RIGHT] = ">>",

        [TOK_OP_LCURLY] = "{",
        [TOK_OP_RCURLY] = "}",
        [TOK_OP_LBRACKET] = "[",
        [TOK_OP_RBRACKET] = "]",
        [TOK_OP_LPAR] = "(",
        [TOK_OP_RPAR] = ")",
        [TOK_OP_QUOTE] = "'",
        [TOK_OP_DQUOTE] = "\"",

        [TOK_OP_COMMA] = ",",
        [TOK_OP_SEMICOLON] = ";",

        [TOK_TYPE_NONE] = "none",
        [TOK_TYPE_BOOL] = "bool",

        [TOK_TYPE_SINT8] = "sint8",
        [TOK_TYPE_SINT16] = "sint16",
        [TOK_TYPE_SINT32] = "sint32",
        [TOK_TYPE_SINT64] = "sint64",
        [TOK_TYPE_UINT8] = "uint8",
        [TOK_TYPE_UINT16] = "uint16",
        [TOK_TYPE_UINT32] = "uint32",
        [TOK_TYPE_UINT64] = "uint64",

        [TOK_TYPE_FLOAT8] = "float8",
        [TOK_TYPE_FLOAT16] = "float16",
        [TOK_TYPE_FLOAT32] = "float32",
        [TOK_TYPE_FLOAT64] = "float64",

        [TOK_TYPE_ASCII] = "ascii",
        [TOK_TYPE_UTF8] = "utf8"
};

#endif // TOKEN_H
