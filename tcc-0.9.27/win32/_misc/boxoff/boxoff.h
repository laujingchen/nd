#ifndef boBOXOFF_H
#define boBOXOFF_H

// boxoff.h
// Cinema ticket-booking system

#include <stdbool.h> // bool
#include <stddef.h> // size_t

#include "common.h" // cmCNT

// # Data

// ## Auxiliary

// Not auxiliary, but `enum`s cannot be forward-declared so it must be here
typedef enum {
	boSeatKind_normal,
	boSeatKind_couple,
	boSeatKind_vipLuxury,
	boSeatKind_max,
} boSeatKind;

enum {
	bo_maxRows = 16,
	bo_maxCols = 16,
};

typedef struct {
	signed char nRows;
	signed char nCols;
	signed char seatCnts[boSeatKind_max];
} boHallLayout;

void boasrtSeatKind(boSeatKind sk);
void boasrtHallLayout(const boHallLayout *hl);
// To be called at the start of the program
// This lets other functions assume that the data is valid
void boasrtData(void);

// ## Data

extern const char bocinemaName[];
// Specify array length directly so `cmCNT` can be used; `enum`s just add one more way of getting a constant
extern const char *const bofilmTtls[3];
extern const char *const bodateStrs[7];
extern const char *const botimeStrs[4];
extern const boHallLayout bohallLayouts[3];
extern const char boseatChars[boSeatKind_max];
extern const double boseatPrices[boSeatKind_max];
extern const char bocurrencySym[];

// # Cinema

// ## Booking number

// Negative means invalid, 0 means the seat is vacant, positive means the seat is booked
void boasrtBookingNum(int bn);
bool boisBookingNumTaken(int bn);

// ## Film index

void boasrtFilmIdx(size_t fi);

// ## Showtime

typedef struct {
	signed char dateIdx;
	signed char timeIdx;
} boShowtime;

void boasrtShowtime(const boShowtime *s);

// ## Hall index

void boasrtHallIdx(size_t hi);
// Nominal hall index is what the user sees
size_t bomkNominalHallIdx(size_t actual);

// ## Seat position

// Starts at 0
typedef struct {
	signed char row;
	signed char col;
} boSeatPos;

void boasrtSeatPos(const boSeatPos *sp, size_t hallIdx);
bool boareSeatPossEq(const boSeatPos *a, const boSeatPos *b);
// Nominal seat position is what the user sees
// Hall index is for error checking
void boinitNominalSeatPos(boSeatPos *nominal, const boSeatPos *actual, size_t hallIdx);

// ## Ticket

enum {
	bomaxSeatsSel = 8,
};

typedef struct {
	// Use `int` instead of `short` because 3 * 7 * 4 * 3 * 16 * 16 = 64512 > 32767
	int bookingNum;
	signed char filmIdx;
	boShowtime showtime;
	signed char hallIdx;
	// Must always match number of populated `seatPoss` elements
	// For example, if the user must enter the number of seats to be selected before selecting them, store it externally and only increment this member upon the selection of each seat
	// This allows for behavior consistent with other approaches such as letting the user select seats until he decides to stop (or hits `bomaxSeatsSel`)
	// `bobufHallDisp` also expects `nSeatsSel` to be 0 so as not to mistakenly mark a seat as taken
	signed char nSeatsSel;
	boSeatPos seatPoss[bomaxSeatsSel];
} boTicket;

void boasrtTicket(const boTicket *t);
void boinitTicket(boTicket *t, int nextBookingNum);
int bofindSelSeat(const boTicket *t, const boSeatPos *sp);

// ## Booking system

typedef struct {
	// "This film is screening on this date, on this time, in this hall, of which you'll choose the seats"
	int bookingNums[cmCNT(bofilmTtls)]
		       [cmCNT(bodateStrs)]
		       [cmCNT(botimeStrs)]
		       [cmCNT(bohallLayouts)]
		       [bo_maxRows]
		       [bo_maxCols];
} boBookingSys;

// This function is heavyweight, use sparingly
void boasrtBookingSys(const boBookingSys *bs);
void boinitBookingSys(boBookingSys *bs);
// A convenient alternative to indexing `boBookingSys::bookingNums` yourself
// Usually just for checking whether a seat is booked (recall that 0 means vacant)
int bogetBookingNum(const boBookingSys *bs, const boTicket *t, const boSeatPos *sp);
int bocntVacant(const boBookingSys *bs, const boTicket *t);
// Call this only after the current ticket has been displayed to the user, or else:
// - the next booking number will be displayed instead
// - the selected seat positions will not be displayed
void boaddBooking(boBookingSys *bs, const boTicket *t);

// ## Hall display

// Hall as it is displayed
typedef struct {
	signed char seatKinds[bo_maxRows][bo_maxCols];
} boHallDisp;

void boasrtHallDisp(const boHallDisp *hd, size_t hallIdx);
void boinitHallDisp(boHallDisp *hd, size_t hallIdx);

// # UI

// ## Menu components

void boprintMenuTop(const char *ttl);

void boprintChoices(const char *const *choices, size_t nChoices);
void boprintYesno(void);

// Remember to subtract 1 for indices
size_t bopromptChoice(const char *prompt, size_t nChoices);
// Returns true if "yes" and false if "no"
bool bopromptYesno(const char *prompt);

void boawaitNewline(const char *msg);

// ## Miscellaneous

void boawaitRetToMainMenu(void);

void boprintTicket(const boTicket *t);
void boprintHallDisp(const boHallDisp *hd, const boBookingSys *bs, const boTicket *t);

#endif
