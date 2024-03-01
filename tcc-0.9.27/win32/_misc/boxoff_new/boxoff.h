#if !defd boBOXOFF_H
#def boBOXOFF_H

// boxoff.h
// Cinema ticket-booking system

//#include <_c.h> // SB
//#include <_c2.h> // _CNT

// # Data

// ## Auxiliary

// Not auxiliary, but `enum`s cannot be forward-declared so it must be here
type enum {
	boSeatKind_normal,
	boSeatKind_couple,
	boSeatKind_vipLuxury,
	boSeatKind_max,
} boSeatKind;

enum {
	bo_maxRows = 16,
	bo_maxCols = 16,
};

type struct {
	SB nRows;
	SB nCols;
	SB seatCnts[boSeatKind_max];
} boHallLayout;

Void boasrtSeatKind(boSeatKind sk);
Void boasrtHallLayout(ro boHallLayout hl^);
// To be called at the start of the program
// This lets other functions assume that the data is valid
Void boasrtData(Void);

// ## Data

extern ro SChar bocinemaName[];
// Specify array length directly so `cmCNT` can be used; `enum`s just add one more way of getting a constant
extern ro SChar bofilmTtls[3]^;
extern ro SChar bodateStrs[7]^;
extern ro SChar botimeStrs[4]^;
extern ro boHallLayout bohallLayouts[3];
extern ro SChar boseatChars[boSeatKind_max];
extern ro B64 boseatPrices[boSeatKind_max];
extern ro SChar bocurrencySym[];

// # Cinema

// ## Booking number

// Negative means invalid, 0 means the seat is vacant, positive means the seat is booked
Void boasrtBookingNum(SW bn);
SW boisBookingNumTaken(SW bn);

// ## Film index

Void boasrtFilmIdx(UA fi);

// ## Showtime

type struct {
	SB dateIdx;
	SB timeIdx;
} boShowtime;

Void boasrtShowtime(ro boShowtime s^);

// ## Hall index

Void boasrtHallIdx(UA hi);
// Nominal hall index is what the user sees
UA bomkNominalHallIdx(UA actual);

// ## Seat position

// Starts at 0
type struct {
	SB row;
	SB col;
} boSeatPos;

Void boasrtSeatPos(ro boSeatPos sp^, UA hallIdx);
SW boareSeatPossEq(ro boSeatPos a^, ro boSeatPos b^);
// Nominal seat position is what the user sees
// Hall index is for error checking
Void boinitNominalSeatPos(boSeatPos nominal^, ro boSeatPos actual^, UA hallIdx);

// ## Ticket

enum {
	bomaxSeatsSel = 8,
};

type struct {
	// Use `int` instead of `short` because 3 * 7 * 4 * 3 * 16 * 16 = 64512 > 32767
	SW bookingNum;
	SB filmIdx;
	boShowtime showtime;
	SB hallIdx;
	// Must always match number of populated `seatPoss` elements
	// For example, if the user must enter the number of seats to be selected before selecting them, store it externally and only increment this member upon the selection of each seat
	// This allows for behavior consistent with other approaches such as letting the user select seats until he decides to stop (or hits `bomaxSeatsSel`)
	// `bobufHallDisp` also expects `nSeatsSel` to be 0 so as not to mistakenly mark a seat as taken
	SB nSeatsSel;
	boSeatPos seatPoss[bomaxSeatsSel];
} boTicket;

Void boasrtTicket(ro boTicket t^);
Void boinitTicket(boTicket t^, SW nextBookingNum);
SW bofindSelSeat(ro boTicket t^, ro boSeatPos sp^);

// ## Booking system

type struct {
	// "This film is screening on this date, on this time, in this hall, of which you'll choose the seats"
	SW bookingNums[_CNT(bofilmTtls)]
		      [_CNT(bodateStrs)]
		      [_CNT(botimeStrs)]
		      [_CNT(bohallLayouts)]
		      [bo_maxRows]
		      [bo_maxCols];
} boBookingSys;

// This function is heavyweight, use sparingly
Void boasrtBookingSys(ro boBookingSys bs^);
Void boinitBookingSys(boBookingSys bs^);
// A convenient alternative to indexing `boBookingSys::bookingNums` yourself
// Usually just for checking whether a seat is booked (recall that 0 means vacant)
SW bogetBookingNum(ro boBookingSys bs^, ro boTicket t^, ro boSeatPos sp^);
SW bocntVacant(ro boBookingSys bs^, ro boTicket t^);
// Call this only after the current ticket has been displayed to the user, or else:
// - the next booking number will be displayed instead
// - the selected seat positions will not be displayed
Void boaddBooking(boBookingSys bs^, ro boTicket t^);

// ## Hall display

// Hall as it is displayed
type struct {
	SB seatKinds[bo_maxRows][bo_maxCols];
} boHallDisp;

Void boasrtHallDisp(ro boHallDisp hd^, UA hallIdx);
Void boinitHallDisp(boHallDisp hd^, UA hallIdx);

// # UI

// ## Menu components

Void boprintMenuTop(ro SChar ttl^);

Void boprintChoices(ro SChar choices^^, UA nChoices);
Void boprintYesno(Void);

// Remember to subtract 1 for indices
UA bopromptChoice(ro SChar prompt^, UA nChoices);
// Returns true if "yes" and false if "no"
SW bopromptYesno(ro SChar prompt^);

Void boawaitNewline(ro SChar msg^);

// ## Miscellaneous

Void boawaitRetToMainMenu(Void);

Void boprintTicket(ro boTicket t^);
Void boprintHallDisp(ro boHallDisp hd^, ro boBookingSys bs^, ro boTicket t^);

#endif
