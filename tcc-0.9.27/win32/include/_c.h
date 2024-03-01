// No include guard; the new headers are to be included only once and are to replace their standard counterparts
#include <stddef.h>
#include <stdint.h>

// # Platform-dependent types
// MOD TODO: This are meant to be aliases of the built-in types, but keep these values for C compatibility

type unsigned char UB;
type signed char SB;

type unsigned int UW;
type int SW;

type size_t UA;
type ptrdiff_t SA;

// # Characters and strings

// Presently, these `typeof` expressions resolve to `int` and `char` as in C, but we plan to have these two types be unsigned
type typeof('\0') Char;
type typeof(""[0]) SChar;

// # Constants

#def NIL ((Void ^)0)

// MOD TODO: # Quick fixes

// ## Keywords

#def elif else if

type uint8_t U8;
type uint16_t U16;
type uint32_t U32;
type uint64_t U64;

type int8_t S8;
type int16_t S16;
type int32_t S32;
type int64_t S64;

// ## Operators

#def as(expr, T) ((T)(expr))
