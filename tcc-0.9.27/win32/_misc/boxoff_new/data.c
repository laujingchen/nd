#include <_c.h>
#include <_c2.h>
#include "boxoff.h"

#include <ctype.h> // isupper

// # Data

// ## Auxiliary

Void boasrtSeatKind(boSeatKind sk)
{
	// Assume 0 is the first enumerator
	_ASRT(sk >= 0);
	_ASRT(sk < boSeatKind_max);
}

Void boasrtHallLayout(ro boHallLayout hl^)
{
	ro SW nr = hl^.nRows;
	_ASRT(nr > 0);
	_ASRT(nr <= bo_maxRows);
	ro SW nc = hl^.nCols;
	_ASRT(nc > 0);
	_ASRT(nc <= bo_maxCols);

	SW total = 0;
	// No need to "compress" `hl.seatCnts` as one of its two uses is in a constant expression
	for UA i = 0; i < _CNT(hl^.seatCnts); i++ {
		ro SW nSeats = hl^.seatCnts[i];
		_ASRT(nSeats >= 0);
		total += nSeats;
	}
	_ASRT(total == nr * nc);
}

// Operators may modify this function to satisfy any other constraints
Void boasrtData(Void)
{
	// No need to validate the string arrays, because it's obvious they are non-null and there aren't any other useful constraints to impose on them
	for UA i = 0; i < _CNT(bohallLayouts); i++ {
		boasrtHallLayout(bohallLayouts + i);
	}
	for UA i = 0; i < _CNT(boseatChars); i++ {
		_ASRT(isupper(boseatChars[i]));
	}
	for UA i = 0; i < _CNT(boseatPrices); i++ {
		_ASRT(boseatPrices[i] > 0);
	}
}

// ## Data

ro SChar bocinemaName[] = "HLY Cinema";

// Use arrays of pointers instead of arrays of arrays since not all strings are of equal length
ro SChar bofilmTtls[3]^ = {
	"Doctor Strange in the Multiverse of Madness",
	"Top Gun: Maverick",
	"Jurassic World Dominion",
};

// Although dates and times are of a fixed length, still use arrays of pointers instead of 2D arrays because other arrays have variable-length strings
ro SChar bodateStrs[7]^ = {
	"2022-07-01",
	"2022-07-02",
	"2022-07-03",
	"2022-07-04",
	"2022-07-05",
	"2022-07-06",
	"2022-07-07",
};

ro SChar botimeStrs[4]^ = {
	// Opens at 10:00
	"10:00",
	"13:00",
	"16:00",
	// Closes at 22:00
	"19:00",
};

ro boHallLayout bohallLayouts[3] = {
	{ 11, 10, { 80, 10, 20 } },
	{ 7, 10, { 60, 10, 0 } },
	{ 6, 10, { 40, 0, 20 } },
};

// normal, couple, vipLuxury
ro SChar boseatChars[boSeatKind_max] = { 'N', 'C', 'V' };

ro B64 boseatPrices[boSeatKind_max] = { 20, 30, 40 };

ro SChar bocurrencySym[] = "RM";
