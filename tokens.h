#ifndef HEADER_FILE
#define HEADER_FILE

enum yytokentype{
        // RESERVED KEYWORDS
        KW_BREAK = 258,
        KW_FUNC = 259,
        KW_ELSE = 260,
        KW_PACKAGE = 261,
        KW_IF = 262,
        KW_CONTINUE = 263,
        KW_FOR = 264,
        KW_IMPORT = 265,
        KW_RETURN = 266,
        KW_VAR = 267,
        KW_TRUE = 268,
        KW_FALSE = 269,
        // OTHERS
        TK_IDENT = 300,
        TK_NUMBER = 301,
        TK_STRING = 302,
        TK_FLOAT32   = 303,
        // OPERATORS & PUNCTUATIONS
        OP_PLUS = 400,
        OP_PLUS_EQ = 401,
        OP_PLUS_PLUS = 403,
        OP_MINUS = 404,
        OP_MINUS_EQ = 405,
        OP_MINUS_MINUS = 406,
        OP_MULT = 407,
        OP_MULT_EQ = 408,
        OP_DIV = 409,
        OP_DIV_EQ = 410,
        OP_EQ = 411,
        OP_EQ_EQ = 412,
        OP_PIPE_PIPE = 413,
        OP_PIPE_EQ = 414,
        OP_GREATER_THAN = 415,
        OP_GREATER_THAN_EQ = 416,
        OP_LESS_THAN = 417,
        OP_LESS_THAN_EQ = 418,
        OP_CARET = 419,
        OP_CARET_EQ = 420,
        OP_AND_AND = 421,
        OP_AND_EQUAL = 422,
        OP_EXCLAMATION = 423,
        OP_EXCLAMATION_EQ = 424,
        OP_OPEN_PAR = 425,
        OP_CLOSE_PAR = 426,
        OP_OPEN_BRACKET = 427,
        OP_CLOSE_BRACKET = 428,
        OP_OPEN_BRACES = 429,
        OP_CLOSE_BRACES = 430,
        OP_COLON = 431,
        OP_COLON_EQ = 432,
        OP_SEMICOLON = 433,
        OP_COMMA = 434,
        OP_PERCENT = 435,
        OP_PERCENT_EQ = 436,
        // DATA TYPES
        DTYPE_INT = 500,
        DTYPE_FLOAT32 = 501,
        DTYPE_BOOL = 502
        // ARRAY ?
    };

#endif