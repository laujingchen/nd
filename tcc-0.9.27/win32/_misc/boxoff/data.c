#include "boxoff.h"

#include <assert.h> // assert
#include <ctype.h> // isupper

// # Data

// ## Auxiliary

void boasrtSeatKind(boSeatKind sk)
{
	// Assume 0 is the first enumerator
	assert(sk >= 0);
	assert(sk < boSeatKind_max);
}

void boasrtHallLayout(const boHallLayout *hl)
{
	const int nr = hl->nRows;
	assert(nr > 0);
	assert(nr <= bo_maxRows);
	const int nc = hl->nCols;
	assert(nc > 0);
	assert(nc <= bo_maxCols);

	int total = 0;
	// No need to "compress" `hl.seatCnts` as one of its two uses is in a constant expression
	for (size_t i = 0; i < cmCNT(hl->seatCnts); ++i) {
		const int nSeats = hl->seatCnts[i];
		assert(nSeats >= 0);
		total += nSeats;
	}
	assert(total == nr * nc);
}

// Operators may modify this function to satisfy any other constraints
void boasrtData(void)
{
	// No need to validate the string arrays, because it's obvious they are non-null and there aren't any other useful constraints to impose on them
	for (size_t i = 0; i < cmCNT(bohallLayouts); ++i) {
		boasrtHallLayout(bohallLayouts + i);
	}
	for (size_t i = 0; i < cmCNT(boseatChars); ++i) {
		assert(isupper(boseatChars[i]));
	}
	for (size_t i = 0; i < cmCNT(boseatPrices); ++i) {
		assert(boseatPrices[i] > 0);
	}
}

// ## Data

const char bocinemaName[] = "HLY Cinema";

// Use arrays of pointers instead of arrays of arrays since not all strings are of equal length
const char *const bofilmTtls[3] = {
	"Doctor Strange in the Multiverse of Madness",
	"Top Gun: Maverick",
	"Jurassic World Dominion",
};

// Although dates and times are of a fixed length, still use arrays of pointers instead of 2D arrays because other arrays have variable-length strings
const char *const bodateStrs[7] = {
	"2022-07-01",
	"2022-07-02",
	"2022-07-03",
	"2022-07-04",
	"2022-07-05",
	"2022-07-06",
	"2022-07-07",
};

const char *const botimeStrs[4] = {
	// Opens at 10:00
	"10:00",
	"13:00",
	"16:00",
	// Closes at 22:00
	"19:00",
};

const boHallLayout bohallLayouts[3] = {
	{ 11, 10, { 80, 10, 20 } },
	{ 7, 10, { 60, 10, 0 } },
	{ 6, 10, { 40, 0, 20 } },
};

// normal, couple, vipLuxury
const char boseatChars[boSeatKind_max] = { 'N', 'C', 'V' };

const double boseatPrices[boSeatKind_max] = { 20, 30, 40 };

const char bocurrencySym[] = "RM";
