#include <_c.h>
#include <_c2.h>
#include "boxoff.h"

#include <limits.h> // ULONG_MAX
#include <stdio.h> // printf
#include <stdlib.h> // strtoul

#include "common.h" // cmmaxCharsInDecSizet

// # UI

// ## Menu components

Void boprintMenuTop(ro SChar ttl^)
{
	printf("\n\n# %s\n", ttl);
}

// Use `size_t` instead of `int` for `nChoices` because it's usually an array length
Void boprintChoices(ro SChar choices^^, UA nChoices)
{
	for UA i = 0; i < nChoices; i++ {
		printf("%zu. %s\n", i + 1, choices[i]);
	}
}

priv ro SChar yesno[]^ = { "Yes", "No" };

Void boprintYesno(Void)
{
	boprintChoices(yesno, _CNT(yesno));
}

// Return a `size_t` instead of an `int` because it's related to `nChoices`
UA bopromptChoice(ro SChar prompt^, UA nChoices)
{
	// Ultimately limited to this because we have to use `strtoul`; see below
	_ASRT(nChoices <= ULONG_MAX);

	for SChar buf[cmmaxCharsInDecSizet];; {
		printf("%s> ", prompt);
		enum {
			// 1 is memorable, but name it anyway since it is used several times with this particular meaning
			first = 1,
		};
		if cmpromptLn(buf, _CNT(buf)) == 0 {
			// Could be `const char *`, but `strtoull` expects `char **`
			SChar charPastNum^;
			// Use `strtoul` instead of:
			// - `sscanf` because we want the input to be strictly numerical; `sscanf(, "%d", )` and `strtoul(, NULL, )` will allow non-numbers as long as the input begins with a number
			// - `strtoull` because:
			//  - TCC does not support it
			//  - the number of choices will never approach anywhere close `ULONG_MAX` anyway (even if it's 32-bit like on Windows)
			ro UA choice = strtoul(buf, charPastNum@, 10);
			// Use <= because choice is in (0, `nChoices`], unlike index which is in [0, `nChoices`)
			if charPastNum^ == '\0' && choice >= first && choice <= nChoices {
				ret choice;
			}
		}
		printf("Choice must be within %d and %zu.\n", first, nChoices);
	}
}

SW bopromptYesno(ro SChar prompt^)
{
	ret bopromptChoice(prompt, _CNT(yesno)) == 1;
}

Void boawaitNewline(ro SChar msg^)
{
	puts(msg);//fputs(msg, stdout);
	// Don't use `getchar`, because any input would still "leak" over to the next prompt
	SChar buf[1];
	cmpromptLn(buf, _CNT(buf));
}

// ## Miscellaneous

Void boawaitRetToMainMenu(Void)
{
	// Put the string in a function instead of an array or macro because:
	// - we want to avoid potential duplication of identical string literals; it is unspecified whether two identical string literals overlap
	// - this string need not be exposed as it is not used for anything else
	boawaitNewline("Press Enter to return to the main menu.");
}

priv Void printHorRule(Void)
{
	puts("***");
}

Void boprintTicket(ro boTicket t^)
{
	ro boShowtime s^ = t^.showtime@;
	ro SW hi = t^.hallIdx;
	printf("Booking number: %d\n"
	       "Film: %s\n"
	       "Showtime: %s, %s\n"
	       "Hall: %zu\n"
	       "Seats: ",
		t^.bookingNum,
		bofilmTtls[t^.filmIdx],
		bodateStrs[s^.dateIdx],
		botimeStrs[s^.timeIdx],
		bomkNominalHallIdx(hi));
	// We could sort `t->seatPoss` first because the order does not matter to the user, but just leave it unsorted because the order does matter when checking whether the user has selected a previously selected seat
	ro SW nss = t^.nSeatsSel;
	for SW i = 0; i < nss; i++ {
		boSeatPos nsp;
		boinitNominalSeatPos(nsp@, t^.seatPoss + i, hi);
		// Print ", " while the final index has not been reached
		printf("(%d, %d)%s", nsp.row, nsp.col, "\0, " + (i < nss - 1));
	}
	putchar('\n');
	printHorRule();
}

Void boprintHallDisp(ro boHallDisp hd^, ro boBookingSys bs^, ro boTicket t^)
{
	SChar buf[512];
	buf[0] = '\0';
	ro SChar bufEnd^ = buf + _CNT(buf);

	// Top left
	SChar bufIter^ = cmcatStr(buf, bufEnd, "Row\\Col\t");

	// Column numbers
	ro SW hi = t^.hallIdx;
	ro boHallLayout hl^ = bohallLayouts + hi;
	ro SW nc = hl^.nCols;
	for SW c = 1; c <= nc; c++ {
		bufIter = cmcatStr_int(bufIter, bufEnd, c);
#def SEPELEMS() (bufIter = cmcatStr(bufIter, bufEnd, "\t"))
		SEPELEMS();
	}
#def ENDROW() (bufIter = cmcatStr(bufIter, bufEnd, "\n"))
	ENDROW();

	enum {
		takenChar_others = 'X',
		takenChar_self = 'Y',
	};
	boSeatPos sp;
	SB r^ = sp.row@;
	// First row internally is 0
	for r^ = 0; r^ < hl^.nRows; r^++ {
		// Row number
		// First row in the menu is 1
		bufIter = cmcatStr_int(bufIter, bufEnd, r^ + 1);
		SEPELEMS();
		SB c^ = sp.col@;
		for c^ = 0; c^ < nc; c^++ {
			// Seat character
			bufIter = cmcatStr_char(bufIter, bufEnd, boseatChars[hd^.seatKinds[r^][c^]]);
			// Character indicating if it's taken
			// `'\0'` means not taken
			SChar tc = '\0';
			if boisBookingNumTaken(bogetBookingNum(bs, t, sp@)) {
				tc = takenChar_others;
			} else {
				for SW i = 0; i < t^.nSeatsSel; i++ {
					if boareSeatPossEq(t^.seatPoss + i, sp@) {
						tc = takenChar_self;
						break;
					}
				}
			}
			bufIter = cmcatStr_char(bufIter, bufEnd, tc);
			SEPELEMS();
		}
		ENDROW();
	}

	// Although the header is not part of a `boHallDisp` per se, keep it in this function until there is a need to print it separately
	printf("Hall %zu\n"
	       "%c: VIP Luxury seat (RM%.2f)\n"
	       "%c: Couple seat (RM%.2f)\n"
	       "%c: Normal seat (RM%.2f)\n"
	       "%c: Seat is booked\n"
	       "%c: Seat you have selected\n"
	       "%s",
		bomkNominalHallIdx(hi),
		boseatChars[boSeatKind_vipLuxury],
		boseatPrices[boSeatKind_vipLuxury],
		boseatChars[boSeatKind_couple],
		boseatPrices[boSeatKind_couple],
		boseatChars[boSeatKind_normal],
		boseatPrices[boSeatKind_normal],
		takenChar_others,
		takenChar_self,
		buf);
	printHorRule();
}
