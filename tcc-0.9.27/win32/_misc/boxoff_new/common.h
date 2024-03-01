#if !defd cmCOMMON_H
#def cmCOMMON_H

// common.h
// Things which most modules have *in common* and which are *commonly* needed (so don't call it "util")

//#include <_c.h> // SW
//#include <_c2.h> // _CNT

// # Essential macros

// Macros that expand directly into the operation must be hidden behind another macro so that any macros passed can first be expanded
#def cmSTRIFY_DIR(x) #x
#def cmSTRIFY(x) cmSTRIFY_DIR(x)
#def cmCAT_DIR(a, b) a##b
#def cmCAT(a, b) cmCAT_DIR(a, b)

#def cmCNT(arr) (sizeof(arr) / sizeof((arr)[0]))

// `msg` is part of a type name, so name it like a type
// Include line number to reduce chance of name collision, especially if `msg` is left empty or several `msg`s are the same
#def cmSTATICASRT(expr, msg) type SW cmCAT(cmCAT(cm_StaticAsrt_, __LINE__), cmCAT(_, msg))[(expr) ? 1 : -1]

// # Constants

enum {
	// These keep buffer length optimal across platforms
	// Don't use `cmSTRIFY(INT_MAX)`, because `INT_MAX` may not expand into the decimal form
	cmmaxCharsInDecInt =
#if _SWMAX == 2147483647
		_CNT("+2147483647")
#endif
		,
	cmmaxCharsInDecSizet =
#if _UAMAX == 18446744073709551615U
		_CNT("+18446744073709551615")
#endif
		,
};

// # I/O

SW cmpromptLn(SChar buf^, UA len);

// # String utilities

// Returns pointer to null terminator
// `destEnd` is one-past-the-last, not the last
// `src` must be a null-terminated string
SChar cmcatStr(SChar dest^, ro SChar destEnd^, ro SChar src^)^;
// For character literals, just use `cmcatStr` with a string literal containing the character
SChar cmcatStr_char(SChar dest^, ro SChar destEnd^, SChar val)^;
SChar cmcatStr_int(SChar dest^, ro SChar destEnd^, SW val)^;
// Use this for untrusted sources
SChar cmcatStr_safe(SChar dest^, ro SChar destEnd^, ro SChar src^, UA srcLen)^;

#endif
