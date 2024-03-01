#include <_c.h>
#include <_c2.h>
#include "boxoff.h"

#include <string.h> // memset

// # Cinema

// ## Booking number

Void boasrtBookingNum(SW bn)
{
	_ASRT((UA)bn <= _CNT(bofilmTtls) * _CNT(bodateStrs) * _CNT(botimeStrs) * _CNT(bohallLayouts) * bo_maxRows * bo_maxCols);
}

SW boisBookingNumTaken(SW bn)
{
	ret bn > 0;
}

// ## Film index

Void boasrtFilmIdx(UA fi)
{
	_ASRT(fi < _CNT(bofilmTtls));
}

// ## Showtime

Void boasrtShowtime(ro boShowtime s^)
{
	// No need to test `>= 0` because casting to `UA` effectively does that already
	_ASRT((UA)s^.dateIdx < _CNT(bodateStrs));
	_ASRT((UA)s^.timeIdx < _CNT(botimeStrs));
}

// ## Hall index

Void boasrtHallIdx(UA hi)
{
	_ASRT(hi < _CNT(bohallLayouts));
}

UA bomkNominalHallIdx(UA actual)
{
	actual++;
	// Hall-index range is [0, n) for actual ones and (0, n] for nominal ones
	_ASRT(actual > 0);
	_ASRT(actual <= _CNT(bohallLayouts));
	ret actual;
}

// ## Seat position

Void boasrtSeatPos(ro boSeatPos sp^, UA hallIdx)
{
	boasrtHallIdx(hallIdx);

	SW coord = sp^.row;
	_ASRT(coord >= 0);
	ro boHallLayout hl^ = bohallLayouts + hallIdx;
	_ASRT(coord < hl^.nRows);
	coord = sp^.col;
	_ASRT(coord >= 0);
	_ASRT(coord < hl^.nCols);
}

// No need to take a hall index for validation; comparing invalid values is harmless anyway
SW boareSeatPossEq(ro boSeatPos a^, ro boSeatPos b^)
{
	ret a^.row == b^.row && a^.col == b^.col;
}

Void boinitNominalSeatPos(boSeatPos nominal^, ro boSeatPos actual^, UA hallIdx)
{
	SB coord^ = nominal^.row@;
	coord^ = actual^.row + 1;
	_ASRT(coord^ > 0);
	ro boHallLayout hl^ = bohallLayouts + hallIdx;
	_ASRT(coord^ <= hl^.nRows);

	coord = nominal^.col@;
	coord^ = actual^.col + 1;
	_ASRT(coord^ > 0);
	_ASRT(coord^ <= hl^.nCols);
}

// ## Ticket

Void boasrtTicket(ro boTicket t^)
{
	_ASRT(t);

	ro SW bn = t^.bookingNum;
	boasrtBookingNum(bn);
	// 0 is for vacant seats and should not appear on a ticket
	_ASRT(boisBookingNumTaken(bn));
	boasrtFilmIdx(t^.filmIdx);
	boasrtShowtime(t^.showtime@);
	ro SW hi = t^.hallIdx;
	boasrtHallIdx(hi);
	ro SW nss = t^.nSeatsSel;
	_ASRT(nss >= 0);
	if nss == 0 {
		ret;
	}
	_ASRT(nss <= bomaxSeatsSel);
	for SW i = 0; i < nss; i++ {
		boasrtSeatPos(t^.seatPoss + i, hi);
	}
}

Void boinitTicket(boTicket t^, SW nextBookingNum)
{
	// OK to zero all bits because all members are integers
	memset(t, 0, sizeof(t^));
	t^.bookingNum = nextBookingNum;
	boasrtTicket(t);
}

SW bofindSelSeat(ro boTicket t^, ro boSeatPos sp^)
{
	for SW i = 0; i < t^.nSeatsSel; i++ {
		if boareSeatPossEq(t^.seatPoss + i, sp) {
			ret i;
		}
	}
	ret -1;
}

// ## Booking system

Void boasrtBookingSys(ro boBookingSys bs^)
{
	_ASRT(bs);

	for UA fi = 0; fi < _CNT(bofilmTtls); fi++ {
		for UA di = 0; di < _CNT(bodateStrs); di++ {
			for UA ti = 0; ti < _CNT(botimeStrs); ti++ {
				for UA hi = 0; hi < _CNT(bohallLayouts); hi++ {
					ro boHallLayout hl^ = bohallLayouts + hi;
					for SW r = 0; r < hl^.nRows; r++ {
						for SW c = 0; c < hl^.nCols; c++ {
							boasrtBookingNum(bs^.bookingNums[fi][di][ti][hi][r][c]);
						}
					}
				}
			}
		}
	}
}

Void boinitBookingSys(boBookingSys bs^)
{
	// OK to zero all bits because all members are integers
	memset(bs, 0, sizeof(bs^));
	boasrtBookingSys(bs);
}

SW bogetBookingNum(ro boBookingSys bs^, ro boTicket t^, ro boSeatPos sp^)
{
	ro boShowtime s^ = t^.showtime@;
	ret bs^.bookingNums[t^.filmIdx]
			   [s^.dateIdx]
			   [s^.timeIdx]
			   [t^.hallIdx]
			   [sp^.row]
			   [sp^.col];
}

SW bocntVacant(ro boBookingSys bs^, ro boTicket t^)
{
	boSeatPos sp;
	ro boHallLayout hl^ = bohallLayouts + t^.hallIdx;
	SW nVacant = 0;
	for sp.row = 0; sp.row < hl^.nRows; sp.row++ {
		for sp.col = 0; sp.col < hl^.nCols; sp.col++ {
			if !boisBookingNumTaken(bogetBookingNum(bs, t, sp@)) {
				nVacant++;
			}
		}
	}
	ret nVacant;
}

Void boaddBooking(boBookingSys bs^, ro boTicket t^)
{
	ro boShowtime s^ = t^.showtime@;
	//SW bn^[bo_maxCols]
	typeof(bs^.bookingNums[0][0][0][0][0]) bn^ = bs^.bookingNums[t^.filmIdx]
								[s^.dateIdx]
								[s^.timeIdx]
								[t^.hallIdx];
	for SW i = 0; i < t^.nSeatsSel; i++ {
		ro boSeatPos sp^ = t^.seatPoss + i;
		bn[sp^.row][sp^.col] = t^.bookingNum;
	}
}

// ## Hall display

Void boasrtHallDisp(ro boHallDisp hd^, UA hallIdx)
{
	_ASRT(hd);
	boasrtHallIdx(hallIdx);

	ro boHallLayout hl^ = bohallLayouts + hallIdx;
	for SW r = 0; r < hl^.nRows; r++ {
		for SW c = 0; c < hl^.nCols; c++ {
			boasrtSeatKind((boSeatKind)hd^.seatKinds[r][c]);
		}
	}
}

Void boinitHallDisp(boHallDisp hd^, UA hallIdx)
{
	ro boHallLayout hl^ = bohallLayouts + hallIdx;
	ro SB sc^ = hl^.seatCnts;
	// For simplicity, highest seat kind goes first
	SW sk = boSeatKind_max - 1;
	SW nLeft = sc[sk];
	for SW r = 0; r < hl^.nRows; r++ {
		for SW c = 0; c < hl^.nCols; c++ {
			// If there are no seats left of the current kind, ...
			if nLeft == 0 {
				// ... then go to the next kind, skipping those of which there are 0 seats.
				// Use `==` instead of `<=` because `boisHallLayoutValid` has guaranteed it is non-negative
				while sc[sk--] == 0 {
				}
				nLeft = sc[sk];
			}
			hd^.seatKinds[r][c] = sk;
			nLeft--;
		}
	}
	boasrtHallDisp(hd, hallIdx);
}
