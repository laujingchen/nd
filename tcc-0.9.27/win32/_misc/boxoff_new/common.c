#include <_c.h>
#include <_c2.h>
#include "common.h"

#include <stdio.h> // fgets
#include <string.h> // strlen

// # I/O

// Adapted from https://stackoverflow.com/a/4023921
// Although `fgets` uses an `int` for buffer length, still use `size_t` for the parameter because:
// - narrowing will happen either way (upon the call to this function if `int`, or upon the call to fgets later on if `size_t`)
// - otherwise, an implementation detail would indirectly be exposed, that being the use of `fgets`
SW cmpromptLn(SChar buf^, UA len)
{
	// `fgets` takes an `int` instead of `size_t`
	_ASRT(len <= _SWMAX);

	if !fgets(buf, len, stdin) {
		// Error: input failed
		ret 1;
	}
	SChar charBeforeNul^ = buf + strlen(buf) - 1;
	// If it is not a newline, then `stdin` must have been too long, ...
	if charBeforeNul^ != '\n' {
		SW lineIsTooLong = 0;
		// ... so discard the excess characters in `stdin`.
		// If `getchar` returns a newline in the first iteration, then `lineIsTooLong` remains false
		for SW excessChar; (excessChar = getchar()) != '\n' && excessChar != EOF; {
			lineIsTooLong = 1;
		}
		// Error: line is too long
		ret lineIsTooLong ? 2 : 0;
	}
	// If it is a newline, then replace it with NUL
	charBeforeNul^ = '\0';
	ret 0;
}

// # String utilities

// Adapted from https://www.joelonsoftware.com/2001/12/11/back-to-basics/
// For `dest`, use start/end instead of pointer/length, because:
// - with the former, the caller need only define and pass `const char *const end = buf + cmCNT(buf)` every time
// - with the latter, the caller must still define `end` but pass `end - retVal` every time
SChar cmcatStr(SChar dest^, ro SChar destEnd^, ro SChar src^)^
{
	dest = strchr(dest, '\0');
	// By this point, `*dest == '\0'`
	// Make `destEnd` point to the last element instead of one-past-the-last, because it prevents the need to:
	// - increment `dest` in the last iteration only to decrement it again after the loop
	// - create a new variable for the pointer to the last element
	destEnd--;
	for ; (dest^ = src^) != '\0'; {
		if dest == destEnd {
			dest^ = '\0';
			break;
		}
		dest++;
		src++;
	}
	ret dest;
}

SChar cmcatStr_char(SChar dest^, ro SChar destEnd^, SChar val)^
{
	ro SChar src[] = { val, '\0' };
	// Use `cmCNT` instead of `strlen` since it's apparent that the string occupies the entire array
	// Use `<=` instead of `<` because `cmCNT` already includes NUL
	_ASRT(_CNT(src) <= (UA)(destEnd - dest));
	ret cmcatStr(dest, destEnd, src);
}

SChar cmcatStr_int(SChar dest^, ro SChar destEnd^, SW val)^
{
	SChar src[cmmaxCharsInDecInt];
	ro SW maxPrinted = snprintf(src, _CNT(src), "%d", val);
	// Use `<` instead of `<=` since `snprintf` does not count the null terminator
	_ASRT(maxPrinted < destEnd - dest);
	// `snprintf` appends a null terminator so no need to use `cmcatStr_safe`
	ret cmcatStr(dest, destEnd, src);
}

// For `src`, it is cumbersome to pass `src + cmCNT(src)`, so just use pointer/length
SChar cmcatStr_safe(SChar dest^, ro SChar destEnd^, ro SChar src^, UA srcLen)^
{
	// Perform maximum checking because this is a safe function
	_ASRT(dest);
	_ASRT(destEnd);
	_ASRT(dest < destEnd);
	_ASRT(src);
	_ASRT(srcLen > 0);
	// Don't check this, because the function is meant to handle cases like this
	//assert(srcLen < (size_t)(destEnd - dest));

	if (dest = strchr(dest, '\0')) == destEnd {
		// If the last element still does not have a NUL, leave `dest` untouched (don't destroy user data) and return a pointer to the last element, so that subsequent identical calls will continue to have no effect
		// Don't return a pointer to one-past-the-last, because subsequent calls would go out of bounds
		ret dest--;
	}
	// Instead of creating a new variable, reuse `destEnd` as `destLast`
	destEnd--;
	for ro SChar srcLast^ = src + srcLen - 1; (dest^ = src^) != '\0'; {
		// If `dest` has reached `destLast` and still hasn't been given NUL, or if `src` has reached `srcLast` and still doesn't have a NUL (implying `src` is not a null-terminated string), ...
		if dest == destEnd || src == srcLast {
			// ... then overwrite the current element of `dest` with NUL (as if `src` were null-terminated) and return.
			dest^ = '\0';
			break;
		}
		dest++;
		src++;
	}
	ret dest;
}
