#ifndef cmCOMMON_H
#define cmCOMMON_H

// common.h
// Things which most modules have *in common* and which are *commonly* needed (so don't call it "util")

#include <limits.h> // INT_MAX
#include <stddef.h> // size_t
#include <stdint.h> // SIZE_MAX

// # Essential macros

// Macros that expand directly into the operation must be hidden behind another macro so that any macros passed can first be expanded
#define cmSTRIFY_DIR(x) #x
#define cmSTRIFY(x) cmSTRIFY_DIR(x)
#define cmCAT_DIR(a, b) a##b
#define cmCAT(a, b) cmCAT_DIR(a, b)

#define cmCNT(arr) (sizeof(arr) / sizeof((arr)[0]))

// `msg` is part of a type name, so name it like a type
// Include line number to reduce chance of name collision, especially if `msg` is left empty or several `msg`s are the same
#define cmSTATICASRT(expr, msg) typedef int cmCAT(cmCAT(cm_StaticAsrt_, __LINE__), cmCAT(_, msg))[(expr) ? 1 : -1]

// # Constants

enum {
	// These keep buffer length optimal across platforms
	// Don't use `cmSTRIFY(INT_MAX)`, because `INT_MAX` may not expand into the decimal form
	cmmaxCharsInDecInt =
#if INT_MAX == 2147483647
		cmCNT("+2147483647")
#endif
		,
	cmmaxCharsInDecSizet =
#if SIZE_MAX == 18446744073709551615U
		cmCNT("+18446744073709551615")
#endif
		,
};

// # I/O

int cmpromptLn(char *buf, size_t len);

// # String utilities

// Returns pointer to null terminator
// `destEnd` is one-past-the-last, not the last
// `src` must be a null-terminated string
char *cmcatStr(char *dest, const char *destEnd, const char *src);
// For character literals, just use `cmcatStr` with a string literal containing the character
char *cmcatStr_char(char *dest, const char *destEnd, char val);
char *cmcatStr_int(char *dest, const char *destEnd, int val);
// Use this for untrusted sources
char *cmcatStr_safe(char *dest, const char *destEnd, const char *src, size_t srcLen);

#endif
