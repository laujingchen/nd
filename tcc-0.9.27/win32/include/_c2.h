// Also no include guard
#include <assert.h>
#include <float.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>

//#include <_c.h> // UA

// # Essential macros

// MOD TODO BUG: Doesn't work ("macro parameter after '#' expected")
#def _STRIFY_DIR(a) #s
#def _STRIFY(a) _STRIFY_DIR(a)
#def _CAT_DIR(a, b) a##b
#def _CAT(a, b) _CAT_DIR(a, b)

#def _CNT(arr) (sizeof(arr) / sizeof((arr)[0]))

// Must reimplement `offsetof` because operators have changed
//#define offsetof(type, field) ((size_t)&((type *)0)->field)
#def _GETOFF(Struct, member) ((UA)((Struct ^)0)^.member@)

// # Constants

// ## Built-in types

#def _U8MAX UINT8_MAX
#def _U16MAX UINT16_MAX
#def _U32MAX UINT32_MAX
#def _U64MAX UINT64_MAX

#def _S8MIN INT8_MIN
#def _S8MAX INT8_MAX
#def _S16MIN INT16_MIN
#def _S16MAX INT16_MAX
#def _S32MIN INT32_MIN
#def _S32MAX INT32_MAX
#def _S64MIN INT64_MIN
#def _S64MAX INT64_MAX

// MOD TODO: There are more floating-point limits beyond min and max
#def _B32MIN FLT_MIN
#def _B32MAX FLT_MAX
#def _B64MIN DBL_MIN
#def _B64MAX DBL_MAX

// ## Platform-dependent types

#def _BYTEWIDTH CHAR_BIT
#def _UBMAX UCHAR_MAX
#def _SBMIN SCHAR_MIN
#def _SBMAX SCHAR_MAX

#def _UWMAX UINT_MAX
#def _SWMIN INT_MIN
#def _SWMAX INT_MAX

#def _UAMAX SIZE_MAX
#def _SAMIN PTRDIFF_MIN
#def _SAMAX PTRDIFF_MAX

// ## Characters and strings

// Although `Char` and `SChar` are currently aliased to signed types, there is no defined minimum as they are supposed to be unsigned
#def _CHARMAX INT_MAX
#def _SCHARMAX CHAR_MAX

// # Basic functions

#def _ASRT(expr) assert(expr)
