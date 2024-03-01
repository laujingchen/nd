#include "boxoff.h"

#include <assert.h> // assert
#include <string.h> // memset

// # Cinema

// ## Booking number

void boasrtBookingNum(int bn)
{
	assert((size_t)bn <= cmCNT(bofilmTtls) * cmCNT(bodateStrs) * cmCNT(botimeStrs) * cmCNT(bohallLayouts) * bo_maxRows * bo_maxCols);
}

bool boisBookingNumTaken(int bn)
{
	return bn > 0;
}

// ## Film index

void boasrtFilmIdx(size_t fi)
{
	assert(fi < cmCNT(bofilmTtls));
}

// ## Showtime

void boasrtShowtime(const boShowtime *s)
{
	// No need to test `>= 0` because casting to `size_t` effectively does that already
	assert((size_t)s->dateIdx < cmCNT(bodateStrs));
	assert((size_t)s->timeIdx < cmCNT(botimeStrs));
}

// ## Hall index

void boasrtHallIdx(size_t hi)
{
	assert(hi < cmCNT(bohallLayouts));
}

size_t bomkNominalHallIdx(size_t actual)
{
	++actual;
	// Hall-index range is [0, n) for actual ones and (0, n] for nominal ones
	assert(actual > 0);
	assert(actual <= cmCNT(bohallLayouts));
	return actual;
}

// ## Seat position

void boasrtSeatPos(const boSeatPos *sp, size_t hallIdx)
{
	boasrtHallIdx(hallIdx);

	int coord = sp->row;
	assert(coord >= 0);
	const boHallLayout *const hl = bohallLayouts + hallIdx;
	assert(coord < hl->nRows);
	coord = sp->col;
	assert(coord >= 0);
	assert(coord < hl->nCols);
}

// No need to take a hall index for validation; comparing invalid values is harmless anyway
bool boareSeatPossEq(const boSeatPos *a, const boSeatPos *b)
{
	return a->row == b->row && a->col == b->col;
}

void boinitNominalSeatPos(boSeatPos *nominal, const boSeatPos *actual, size_t hallIdx)
{
	signed char *coord = &nominal->row;
	*coord = actual->row + 1;
	assert(*coord > 0);
	const boHallLayout *const hl = bohallLayouts + hallIdx;
	assert(*coord <= hl->nRows);

	coord = &nominal->col;
	*coord = actual->col + 1;
	assert(*coord > 0);
	assert(*coord <= hl->nCols);
}

// ## Ticket

void boasrtTicket(const boTicket *t)
{
	assert(t);

	const int bn = t->bookingNum;
	boasrtBookingNum(bn);
	// 0 is for vacant seats and should not appear on a ticket
	assert(boisBookingNumTaken(bn));
	boasrtFilmIdx(t->filmIdx);
	boasrtShowtime(&t->showtime);
	const int hi = t->hallIdx;
	boasrtHallIdx(hi);
	const int nss = t->nSeatsSel;
	assert(nss >= 0);
	if (nss == 0) {
		return;
	}
	assert(nss <= bomaxSeatsSel);
	for (int i = 0; i < nss; ++i) {
		boasrtSeatPos(t->seatPoss + i, hi);
	}
}

void boinitTicket(boTicket *t, int nextBookingNum)
{
	// OK to zero all bits because all members are integers
	memset(t, 0, sizeof(*t));
	t->bookingNum = nextBookingNum;
	boasrtTicket(t);
}

int bofindSelSeat(const boTicket *t, const boSeatPos *sp)
{
	for (int i = 0; i < t->nSeatsSel; ++i) {
		if (boareSeatPossEq(t->seatPoss + i, sp)) {
			return i;
		}
	}
	return -1;
}

// ## Booking system

void boasrtBookingSys(const boBookingSys *bs)
{
	assert(bs);

	for (size_t fi = 0; fi < cmCNT(bofilmTtls); ++fi) {
		for (size_t di = 0; di < cmCNT(bodateStrs); ++di) {
			for (size_t ti = 0; ti < cmCNT(botimeStrs); ++ti) {
				for (size_t hi = 0; hi < cmCNT(bohallLayouts); ++hi) {
					const boHallLayout *const hl = bohallLayouts + hi;
					for (int r = 0; r < hl->nRows; ++r) {
						for (int c = 0; c < hl->nCols; ++c) {
							boasrtBookingNum(bs->bookingNums[fi][di][ti][hi][r][c]);
						}
					}
				}
			}
		}
	}
}

void boinitBookingSys(boBookingSys *bs)
{
	// OK to zero all bits because all members are integers
	memset(bs, 0, sizeof(*bs));
	boasrtBookingSys(bs);
}

int bogetBookingNum(const boBookingSys *bs, const boTicket *t, const boSeatPos *sp)
{
	const boShowtime *const s = &t->showtime;
	return bs->bookingNums[t->filmIdx]
			      [s->dateIdx]
			      [s->timeIdx]
			      [t->hallIdx]
			      [sp->row]
			      [sp->col];
}

int bocntVacant(const boBookingSys *bs, const boTicket *t)
{
	boSeatPos sp;
	const boHallLayout *const hl = bohallLayouts + t->hallIdx;
	int nVacant = 0;
	for (sp.row = 0; sp.row < hl->nRows; ++sp.row) {
		for (sp.col = 0; sp.col < hl->nCols; ++sp.col) {
			if (!boisBookingNumTaken(bogetBookingNum(bs, t, &sp))) {
				++nVacant;
			}
		}
	}
	return nVacant;
}

void boaddBooking(boBookingSys *bs, const boTicket *t)
{
	const boShowtime *const s = &t->showtime;
	int(*const bn)[bo_maxCols] = bs->bookingNums[t->filmIdx]
						    [s->dateIdx]
						    [s->timeIdx]
						    [t->hallIdx];
	for (int i = 0; i < t->nSeatsSel; ++i) {
		const boSeatPos *const sp = t->seatPoss + i;
		bn[sp->row][sp->col] = t->bookingNum;
	}
}

// ## Hall display

void boasrtHallDisp(const boHallDisp *hd, size_t hallIdx)
{
	assert(hd);
	boasrtHallIdx(hallIdx);

	const boHallLayout *const hl = bohallLayouts + hallIdx;
	for (int r = 0; r < hl->nRows; ++r) {
		for (int c = 0; c < hl->nCols; ++c) {
			boasrtSeatKind((boSeatKind)hd->seatKinds[r][c]);
		}
	}
}

void boinitHallDisp(boHallDisp *hd, size_t hallIdx)
{
	const boHallLayout *const hl = bohallLayouts + hallIdx;
	const signed char *const sc = hl->seatCnts;
	// For simplicity, highest seat kind goes first
	int sk = boSeatKind_max - 1;
	int nLeft = sc[sk];
	for (int r = 0; r < hl->nRows; ++r) {
		for (int c = 0; c < hl->nCols; ++c) {
			// If there are no seats left of the current kind, ...
			if (nLeft == 0) {
				// ... then go to the next kind, skipping those of which there are 0 seats.
				// Use `==` instead of `<=` because `boisHallLayoutValid` has guaranteed it is non-negative
				while (sc[--sk] == 0)
					;
				nLeft = sc[sk];
			}
			hd->seatKinds[r][c] = sk;
			--nLeft;
		}
	}
	boasrtHallDisp(hd, hallIdx);
}
