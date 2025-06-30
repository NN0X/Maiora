#ifndef TOKEN_H
#define TOKEN_H

#include <stdint.h>

enum Token
{
        // --- KEYWORDS ---

        TOK_KEYW_RETURN = 0,

        // modifiers
        TOK_KEYW_PUBLIC,
        TOK_KEYW_PRIVATE,
        TOK_KEYW_ENTRY,

        // functions
        TOK_KEYW_INSTANCE,
        TOK_KEYW_FUNCTION,

        // memory
        TOK_KEYW_HEAP,
        TOK_KEYW_ADDRESS,
        TOK_KEYW_REFERENCE,
        TOK_KEYW_REF,
        TOK_KEYW_AT,

        // control flow
        TOK_KEYW_WHILE,
        TOK_KEYW_FOR,
        TOK_KEYW_IF,
        TOK_KEYW_ELSE,
        TOK_KEYW_CONTINUE,
        TOK_KEYW_BREAK,
        TOK_KEYW_SWITCH,
        TOK_KEYW_CASE,


        // --- OPERATORS ---

        // comparators
        TOK_OP_EQUALS,
        TOK_OP_LESS,
        TOK_OP_MORE,
        TOK_OP_LESS_EQUALS,
        TOK_OP_MORE_EQUALS,
        TOK_OP_NOT_EQUALS,
        TOK_OP_AND,             // &&
        TOK_OP_OR,              // ||
        TOK_OP_NOT,             // !

        // accessors
        TOK_OP_FROM,            // .
        TOK_OP_IN,              // :
        TOK_OP_FROM_MODULE,     // ::

        // modifiers
        TOK_OP_ASSIGN,
        TOK_OP_ADD,
        TOK_OP_SUBTRACT,
        TOK_OP_DIVIDE,
        TOK_OP_MULTIPLY,
        TOK_OP_MODULO,          // %
        TOK_OP_INCREMENT,       // ++
        TOK_OP_DECREMENT,       // --
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


        // --- TYPES ---

        TOK_TYPE_NONE,
        TOK_TYPE_BOOL,

        // integers
        TOK_TYPE_SINT8,
        TOK_TYPE_SINT16,
        TOK_TYPE_SINT32,
        TOK_TYPE_SINT64,
        TOK_TYPE_UINT8,
        TOK_TYPE_UINT16,
        TOK_TYPE_UINT32,
        TOK_TYPE_UINT64,

        // floats
        TOK_TYPE_FLOAT8,
        TOK_TYPE_FLOAT16,
        TOK_TYPE_FLOAT32,
        TOK_TYPE_FLOAT64,

        // characters
        TOK_TYPE_ASCII,
        TOK_TYPE_UTF8,


        // --- LITERALS ---

        TOK_LIT_SINT8,
        TOK_LIT_SINT16,
        TOK_LIT_SINT32,
        TOK_LIT_SINT64,
        TOK_LIT_UINT8,
        TOK_LIT_UINT16,
        TOK_LIT_UINT32,
        TOK_LIT_UINT64,

        TOK_LIT_FLOAT8,
        TOK_LIT_FLOAT16,
        TOK_LIT_FLOAT32,
        TOK_LIT_FLOAT64,

        TOK_LIT_ASCII,
        TOK_LIT_UTF8,


        // --- IDENTIFIERS ---

        // functions
        TOK_ID_FUNCTION,

        // instances
        TOK_ID_INSTANCE,
        TOK_ID_INSTANCE_ALIAS,

        // variables
        TOK_ID_VARIABLE,

        // modules
        TOK_ID_MODULE,          // <tok_id_module>::<tok_id_function>

};

typedef struct LexerToken
{
        char* data;
        uint32_t token;
        uint64_t line;
        uint64_t pos;
        uint64_t len;
} LTok_t;

#endif // TOKEN_H
