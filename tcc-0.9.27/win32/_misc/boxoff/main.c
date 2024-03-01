#include <assert.h> // assert
#include <stdbool.h> // bool
#include <stddef.h> // size_t
#include <stdio.h> // puts

#include "boxoff.h" // boBookingSys
#include "common.h" // cmCNT

#define TEST

#ifdef TEST
static void debugprintBookingSys(const boBookingSys *bs, const boTicket *t, const char *ttl)
{
	puts(ttl);
	printf("bn%d\n", t->bookingNum);
	for (size_t fi = 0; fi < cmCNT(bofilmTtls); ++fi) {
		printf("fi%zu\n", fi);
		for (size_t di = 0; di < cmCNT(bodateStrs); ++di) {
			printf("di%zu\n", di);
			for (size_t ti = 0; ti < cmCNT(botimeStrs); ++ti) {
				printf("ti%zu\n", ti);
				for (size_t hi = 0; hi < cmCNT(bohallLayouts); ++hi) {
					printf("hi%zu\n", hi);
					const boHallLayout *const hl = bohallLayouts + hi;
					for (int r = 0; r < hl->nRows; ++r) {
						printf("r%d\t", r);
						for (int c = 0; c < hl->nCols; ++c) {
							printf("c%d:%d ", c, bs->bookingNums[fi][di][ti][hi][r][c]);
						}
						putchar('\n');
					}
					putchar('\n');
				}
				putchar('\n');
			}
			putchar('\n');
		}
		putchar('\n');
	}
	putchar('\n');
}

static void debugprintStructSavings(void)
{
	#define PRINT(Old, New) printf(cmSTRIFY(Old) " %zu %zu %f\n", sizeof(Old), sizeof(New), sizeof(New) / (double)sizeof(Old))

	typedef struct {
		int nRows;
		int nCols;
		int seatCnts[boSeatKind_max];
	} HallLayout;
	PRINT(HallLayout, boHallLayout);

	typedef struct {
		int dateIdx;
		int timeIdx;
	} Showtime;
	PRINT(Showtime, boShowtime);

	typedef struct {
		int row;
		int col;
	} SeatPos;
	PRINT(SeatPos, boSeatPos);

	typedef struct {
		int bookingNum;
		int filmIdx;
		Showtime showtime;
		int hallIdx;
		int nSelected;
		SeatPos seatPoss[bomaxSeatsSel];
	} Ticket;
	PRINT(Ticket, boTicket);

	typedef struct {
		int bookingNums[cmCNT(bofilmTtls)]
			       [cmCNT(bodateStrs)]
			       [cmCNT(botimeStrs)]
			       [cmCNT(bohallLayouts)]
			       [bo_maxRows]
			       [bo_maxCols];
		Ticket curTicket;
	} BookingSys;
	PRINT(BookingSys, boBookingSys);

	typedef struct {
		boSeatKind seatKinds[bo_maxRows][bo_maxCols];
	} HallDisp;
	PRINT(HallDisp, boHallDisp);
}
#endif

// Returns `true` if user chooses to book and `false` if user chooses to exit
static bool dispStart(void)
{
	boprintMenuTop(bocinemaName);
	puts("Welcome! We are open from 10 a.m. to 10 p.m.");
	static const char *const choices[] = {
		"Start booking",
		"Exit",
	};
	boprintChoices(choices, cmCNT(choices));
	// Use `bopromptYesno` instead of `bopromptChoice` because the choices resemble yes/no
	return bopromptYesno("Start?");
}

static size_t dispFilms(void)
{
	boprintMenuTop("Choose Film");
	boprintChoices(bofilmTtls, cmCNT(bofilmTtls));
	const size_t fi = bopromptChoice("Film", cmCNT(bofilmTtls)) - 1;
	boasrtFilmIdx(fi);
	return fi;
}

static void dispShowtime(boShowtime *s)
{
	boprintMenuTop("Choose Showtime");
	puts("Choose date:");
	boprintChoices(bodateStrs, cmCNT(bodateStrs));
	s->dateIdx = bopromptChoice("Date", cmCNT(bodateStrs)) - 1;
	puts("Choose time:");
	boprintChoices(botimeStrs, cmCNT(botimeStrs));
	s->timeIdx = bopromptChoice("Time", cmCNT(botimeStrs)) - 1;
	boasrtShowtime(s);
}

// Returns the number of seats selected
static size_t dispSeats(boTicket *t, const boHallDisp *hd, const boBookingSys *bs)
{
	// Display hall
	boprintMenuTop("Choose Seats");
	boprintHallDisp(hd, bs, t);

	// Get number of seats to select
	int nToSel = bocntVacant(bs, t);
	const int hi = t->hallIdx;
	if (nToSel == 0) {
		const boShowtime *const s = &t->showtime;
		printf("Hall %zu is fully booked at %s on %s; please select another film or showtime\n",
			bomkNominalHallIdx(hi),
			botimeStrs[s->timeIdx],
			bodateStrs[s->dateIdx]);
		return 0;
	}
	if (nToSel > bomaxSeatsSel) {
		nToSel = bomaxSeatsSel;
	}
	printf("How many seats would you like to book? (1 to %d)\n", nToSel);
	nToSel = bopromptChoice("Seats", nToSel);

	// Get seats
	signed char *const nss = &t->nSeatsSel;
	for (*nss = 0; *nss < nToSel; ++*nss) {
		const int ordinal = *nss + 1;
		printf("Select seat %d:\n", ordinal);
		for (boSeatPos *const sp = t->seatPoss + *nss;;) {
			char prompt[3 + cmmaxCharsInDecInt];
			snprintf(prompt, cmCNT(prompt), "Row%d", ordinal);
			const boHallLayout *const hl = bohallLayouts + hi;
			sp->row = bopromptChoice(prompt, hl->nRows) - 1;
			snprintf(prompt, cmCNT(prompt), "Col%d", ordinal);
			sp->col = bopromptChoice(prompt, hl->nCols) - 1;
			boasrtSeatPos(sp, hi);
			// Did user select an already booked seat?
			if (boisBookingNumTaken(bogetBookingNum(bs, t, sp))) {
				puts("Seat is booked; please choose another.");
				continue;
			}
			// By this point, the user has chosen a vacant seat
			// Did the user reselect a seat he has already selected?
			const int selseatIdx = bofindSelSeat(t, sp);
			if (selseatIdx >= 0) {
				printf("You have already chosen this seat as seat %d.\n", selseatIdx + 1);
				continue;
			}
			// By this point, the seats selected are all vacant and unique
			break;
		}
	}
	assert(*nss == nToSel);
	return *nss;
}

static void restartBooking(boTicket *t, const char *msg)
{
	puts(msg);
	boinitTicket(t, t->bookingNum);
	boawaitRetToMainMenu();
}

static bool dispConfirm(const boHallDisp *hd, const boBookingSys *bs, const boTicket *t)
{
	boprintMenuTop("Confirm Booking");
	boprintTicket(t);
	boprintHallDisp(hd, bs, t);
	puts("Proceed with this booking?");
	boprintYesno();
	return bopromptYesno("Book?");
}

static bool dispPay(const boHallDisp *hd, const boTicket *t)
{
	boprintMenuTop("Payment");
	const int nss = t->nSeatsSel;
	double total = 0;
	for (int i = 0; i < nss; ++i) {
		const boSeatPos *const sp = t->seatPoss + i;
		const double p = boseatPrices[hd->seatKinds[sp->row][sp->col]];
		// Append " + " up to and excluding the last iteration
		printf("%s%.2f%s", bocurrencySym, p, "\0 + " + (i < nss - 1));
		total += p;
	}
	printf("\nTotal: %s%.2f\n", bocurrencySym, total);
	puts("Proceed with payment?");
	boprintYesno();
	return bopromptYesno("Pay?");
}

static void dispDone(boBookingSys *bs, boTicket *t)
{
	boprintMenuTop("Your Ticket");
	boprintTicket(t);
	boaddBooking(bs, t);
	boinitTicket(t, t->bookingNum + 1);
	puts("Booking complete. Enjoy your movie!");
}

int main(void)
{
	boasrtData();
	boHallDisp hallDisps[cmCNT(bohallLayouts)];
	for (size_t i = 0; i < cmCNT(hallDisps); ++i) {
		boinitHallDisp(hallDisps + i, i);
	}
	boBookingSys bs;
	boinitBookingSys(&bs);
	boTicket t;
	boinitTicket(&t, 1);
#ifdef TEST
	debugprintBookingSys(&bs, &t, "Debug");
	debugprintStructSavings();
	return 0;
#endif
	for (;;) {
		// 0. Main menu
		if (!dispStart()) {
			boawaitNewline("Press Enter to exit.");
			break;
		}

		// 1. Film index
		signed char *const fi = &t.filmIdx;
		*fi = dispFilms();

		// Hall index = film index
		signed char *const hi = &t.hallIdx;
		*hi = *fi;
		boasrtHallIdx(*hi);

		// 2. Showtime
		dispShowtime(&t.showtime);

		// 3. Choose seats
		const boHallDisp *const hd = hallDisps + *hi;
		if (dispSeats(&t, hd, &bs) == 0) {
			restartBooking(&t, "");
			continue;
		}

		// 4. Confirm booking
		if (!dispConfirm(hd, &bs, &t)) {
			restartBooking(&t, "Booking canceled.");
			continue;
		}

		// 5. Payment
		if (!dispPay(hd, &t)) {
			restartBooking(&t, "Payment canceled; ticket not booked.");
			continue;
		}

		// 6. Print ticket
		dispDone(&bs, &t);
		boawaitRetToMainMenu();
	}
	return 0;
}
