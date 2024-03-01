#include <_c.h>
#include <_c2.h>
#include <stdio.h> // puts

#include "boxoff.h" // boBookingSys
#include "common.h" // cmmaxCharsInDecInt

#def TEST

#if defd TEST
priv Void debugprintBookingSys(ro boBookingSys bs^, ro boTicket t^, ro SChar ttl^)
{
	puts(ttl);
	printf("bn%d\n", t^.bookingNum);
	for UA fi = 0; fi < _CNT(bofilmTtls); fi++ {
		printf("fi%zu\n", fi);
		for UA di = 0; di < _CNT(bodateStrs); di++ {
			printf("di%zu\n", di);
			for UA ti = 0; ti < _CNT(botimeStrs); ti++ {
				printf("ti%zu\n", ti);
				for UA hi = 0; hi < _CNT(bohallLayouts); hi++ {
					printf("hi%zu\n", hi);
					ro boHallLayout hl^ = bohallLayouts + hi;
					for SW r = 0; r < hl^.nRows; r++ {
						printf("r%d\t", r);
						for SW c = 0; c < hl^.nCols; c++ {
							printf("c%d:%d ", c, bs^.bookingNums[fi][di][ti][hi][r][c]);
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

priv Void debugprintStructSavings(Void)
{
	#def PRINT(Old, New) printf(cmSTRIFY(Old) " %zu %zu %f\n", sizeof(Old), sizeof(New), sizeof(New) / (B64)sizeof(Old))

	type struct {
		SW nRows;
		SW nCols;
		SW seatCnts[boSeatKind_max];
	} HallLayout;
	PRINT(HallLayout, boHallLayout);

	type struct {
		SW dateIdx;
		SW timeIdx;
	} Showtime;
	PRINT(Showtime, boShowtime);

	type struct {
		SW row;
		SW col;
	} SeatPos;
	PRINT(SeatPos, boSeatPos);

	type struct {
		SW bookingNum;
		SW filmIdx;
		Showtime showtime;
		SW hallIdx;
		SW nSelected;
		SeatPos seatPoss[bomaxSeatsSel];
	} Ticket;
	PRINT(Ticket, boTicket);

	type struct {
		SW bookingNums[_CNT(bofilmTtls)]
			       [_CNT(bodateStrs)]
			       [_CNT(botimeStrs)]
			       [_CNT(bohallLayouts)]
			       [bo_maxRows]
			       [bo_maxCols];
		Ticket curTicket;
	} BookingSys;
	PRINT(BookingSys, boBookingSys);

	type struct {
		boSeatKind seatKinds[bo_maxRows][bo_maxCols];
	} HallDisp;
	PRINT(HallDisp, boHallDisp);
}
#endif

// Returns `true` if user chooses to book and `false` if user chooses to exit
priv SW dispStart(Void)
{
	boprintMenuTop(bocinemaName);
	puts("Welcome! We are open from 10 a.m. to 10 p.m.");
	priv ro SChar choices[]^ = {
		"Start booking",
		"Exit",
	};
	boprintChoices(choices, _CNT(choices));
	// Use `bopromptYesno` instead of `bopromptChoice` because the choices resemble yes/no
	ret bopromptYesno("Start?");
}

priv UA dispFilms(Void)
{
	boprintMenuTop("Choose Film");
	boprintChoices(bofilmTtls, _CNT(bofilmTtls));
	ro UA fi = bopromptChoice("Film", _CNT(bofilmTtls)) - 1;
	boasrtFilmIdx(fi);
	ret fi;
}

priv Void dispShowtime(boShowtime s^)
{
	boprintMenuTop("Choose Showtime");
	puts("Choose date:");
	boprintChoices(bodateStrs, _CNT(bodateStrs));
	s^.dateIdx = bopromptChoice("Date", _CNT(bodateStrs)) - 1;
	puts("Choose time:");
	boprintChoices(botimeStrs, _CNT(botimeStrs));
	s^.timeIdx = bopromptChoice("Time", _CNT(botimeStrs)) - 1;
	boasrtShowtime(s);
}

// Returns the number of seats selected
priv UA dispSeats(boTicket t^, ro boHallDisp hd^, ro boBookingSys bs^)
{
	// Display hall
	boprintMenuTop("Choose Seats");
	boprintHallDisp(hd, bs, t);

	// Get number of seats to select
	SW nToSel = bocntVacant(bs, t);
	ro SW hi = t^.hallIdx;
	if nToSel == 0 {
		ro boShowtime s^ = t^.showtime@;
		printf("Hall %zu is fully booked at %s on %s; please select another film or showtime\n",
			bomkNominalHallIdx(hi),
			botimeStrs[s^.timeIdx],
			bodateStrs[s^.dateIdx]);
		ret 0;
	}
	if nToSel > bomaxSeatsSel {
		nToSel = bomaxSeatsSel;
	}
	printf("How many seats would you like to book? (1 to %d)\n", nToSel);
	nToSel = bopromptChoice("Seats", nToSel);

	// Get seats
	SB nss^ = t^.nSeatsSel@;
	for nss^ = 0; nss^ < nToSel; nss^++ {
		ro SW ordinal = nss^ + 1;
		printf("Select seat %d:\n", ordinal);
		for boSeatPos sp^ = t^.seatPoss + nss^;; {
			SChar prompt[3 + cmmaxCharsInDecInt];
			snprintf(prompt, _CNT(prompt), "Row%d", ordinal);
			ro boHallLayout hl^ = bohallLayouts + hi;
			sp^.row = bopromptChoice(prompt, hl^.nRows) - 1;
			snprintf(prompt, _CNT(prompt), "Col%d", ordinal);
			sp^.col = bopromptChoice(prompt, hl^.nCols) - 1;
			boasrtSeatPos(sp, hi);
			// Did user select an already booked seat?
			if boisBookingNumTaken(bogetBookingNum(bs, t, sp)) {
				puts("Seat is booked; please choose another.");
				cont;
			}
			// By this point, the user has chosen a vacant seat
			// Did the user reselect a seat he has already selected?
			ro SW selseatIdx = bofindSelSeat(t, sp);
			if selseatIdx >= 0 {
				printf("You have already chosen this seat as seat %d.\n", selseatIdx + 1);
				cont;
			}
			// By this point, the seats selected are all vacant and unique
			break;
		}
	}
	_ASRT(nss^ == nToSel);
	ret nss^;
}

priv Void restartBooking(boTicket t^, ro SChar msg^)
{
	puts(msg);
	boinitTicket(t, t^.bookingNum);
	boawaitRetToMainMenu();
}

priv SW dispConfirm(ro boHallDisp hd^, ro boBookingSys bs^, ro boTicket t^)
{
	boprintMenuTop("Confirm Booking");
	boprintTicket(t);
	boprintHallDisp(hd, bs, t);
	puts("Proceed with this booking?");
	boprintYesno();
	ret bopromptYesno("Book?");
}

priv SW dispPay(ro boHallDisp hd^, ro boTicket t^)
{
	boprintMenuTop("Payment");
	ro SW nss = t^.nSeatsSel;
	B64 total = 0;
	for SW i = 0; i < nss; i++ {
		ro boSeatPos sp^ = t^.seatPoss + i;
		ro B64 p = boseatPrices[hd^.seatKinds[sp^.row][sp^.col]];
		// Append " + " up to and excluding the last iteration
		printf("%s%.2f%s", bocurrencySym, p, "\0 + " + (i < nss - 1));
		total += p;
	}
	printf("\nTotal: %s%.2f\n", bocurrencySym, total);
	puts("Proceed with payment?");
	boprintYesno();
	ret bopromptYesno("Pay?");
}

priv Void dispDone(boBookingSys bs^, boTicket t^)
{
	boprintMenuTop("Your Ticket");
	boprintTicket(t);
	boaddBooking(bs, t);
	boinitTicket(t, t^.bookingNum + 1);
	puts("Booking complete. Enjoy your movie!");
}

SW main(Void)
{
	boasrtData();
	boHallDisp hallDisps[cmCNT(bohallLayouts)];
	for UA i = 0; i < _CNT(hallDisps); i++ {
		boinitHallDisp(hallDisps + i, i);
	}
	boBookingSys bs;
	boinitBookingSys(bs@);
	boTicket t;
	boinitTicket(t@, 1);
#if defd TEST
	debugprintBookingSys(bs@, t@, "Debug");
	debugprintStructSavings();
	ret 0;
#endif
	for ;; {
		// 0. Main menu
		if !dispStart() {
			boawaitNewline("Press Enter to exit.");
			break;
		}

		// 1. Film index
		SB fi^ = t.filmIdx@;
		fi^ = dispFilms();

		// Hall index = film index
		SB hi^ = t.hallIdx@;
		hi^ = fi^;
		boasrtHallIdx(hi^);

		// 2. Showtime
		dispShowtime(t.showtime@);

		// 3. Choose seats
		ro boHallDisp hd^ = hallDisps + hi^;
		if dispSeats(t@, hd, bs@) == 0 {
			restartBooking(t@, "");
			cont;
		}

		// 4. Confirm booking
		if !dispConfirm(hd, bs@, t@) {
			restartBooking(t@, "Booking canceled.");
			cont;
		}

		// 5. Payment
		if !dispPay(hd, t@) {
			restartBooking(t@, "Payment canceled; ticket not booked.");
			cont;
		}

		// 6. Print ticket
		dispDone(bs@, t@);
		boawaitRetToMainMenu();
	}
	ret 0;
}
