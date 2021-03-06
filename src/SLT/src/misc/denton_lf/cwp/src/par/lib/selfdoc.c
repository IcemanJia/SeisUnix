/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

/*----------------------------------------------------------------------
 * Copyright (c) Colorado School of Mines, 1989.
 * All rights reserved.
 *
 * This code is part of SU.  SU stands for Seismic Unix, a processing line
 * developed at the Colorado School of Mines, partially based on Stanford
 * Exploration Project (SEP) software.  Inquiries should be addressed to:
 *
 *  Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 *  Golden, CO 80401  (jkc@dix.mines.colorado.edu)
 *----------------------------------------------------------------------
 */

#include "par.h"

/* selfdoc - print self documentation string
 *
 * Returns:
 *	void
 *
 * Synopsis:
 *	void selfdoc()
 *
 * Credits:
 *	SEP: Einar, Stew
 *	CWP: Jack, Shuki
 *
 * Example:
 *	if (xargc != 3) selfdoc();
 *
 *
 */


void selfdoc(void)
{
	extern string sdoc;
	FILE *fp;

	efflush(stdout);
	fp = epopen("more -22 1>&2", "w");
	(void) fprintf(fp, "%s", sdoc);
	epclose(fp);

	exit(EXIT_FAILURE);
}
