#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../inc/complex.h"
#include "../../inc/proto.h"
#include <string.h>
#include "../../inc/mach.h"
#include "../../inc/dfm.h"
#include "../../inc/hdr.h"
#include "../../inc/mem.h"
#include "../../inc/scm.h"
#include "../../inc/sam.h"
#include "coda.h"
int get_input();

void /*FUNCTION*/ xcoda( index, nerr)

long int index, *nerr;
{
	long lhorz, lnpin, lnpout;
	long int ic1a, ic1b, ic2a, ic2b, jdfl, jdfl_, ndx1, ndx2, ndxx, 
	 ndxy, nlen, nlen1, nlen2, notused, ncpfn;
	void *_p0;
	int horizontals=0;
	int vertical=1;
	float C_begin, C_delta, C_dist;
        int npts, nbands;
	struct envelope envelopes[MAXBANDS];
        struct global_params global_params;
        int linteractive=0,namelength,lcalibrate=0;
        char evid[80] = "coda";

	/*=====================================================================
	 * PURPOSE:  To execute the action command CODA.
	 *           This command applies the Mayeda coda algorithm to data files in memory
	 *=====================================================================
	 * OUTPUT ARGUMENTS:
	 *    NERR:    Error flag. Set to 0 if no error occurred.
	 *             Potential error numbers: 2001, 2002, 2003, 2004, 2010.
	 *=====================================================================
	 * MODULE/LEVEL:  coda
	 *=====================================================================
	 * GLOBAL INPUT:
	 *    mach:
	 *    hdr:
	 *=====================================================================
	 * GLOBAL OUTPUT:
	 *    
	 *=====================================================================
	 * MODIFICATION HISTORY:
	 *    9908??:  Original version.
	 *=====================================================================
	 * DOCUMENTED/REVIEWED:
	 *===================================================================== */
	/* PROCEDURE: */
	*nerr = 0;

	/* PARSING PHASE: */

	/* - Loop on each token in command: */
        fprintf(stderr, "Executing CODA command \n");
        sprintf(cmsam.knmfir, "test.input");

L_1000:
	if( lcmore( nerr ) ){

	        if( lkchar( "F#ILE$",7, 80 , cmsam.knmfir ,81,  &ncpfn ) )
		  { /* do nothing */ }

	        else if( lkchar( "ID$",4, 80 , evid ,81,  &ncpfn ) )
		  { /* do nothing */ }

		else if( lklog( "INT#ERACTIVE$",14, &linteractive ) )
		  { /* do nothing */ }

		else if( lklog( "CAL#IBRATE$",12, &lcalibrate ) )
		  { /* do nothing */ }

		else{
			cfmt( "ILLEGAL OPTION:$",17 );
			cresp();

			}
		goto L_1000;

		}

	/* - The above loop is over when one of two conditions has been met:
	 *   (1) An error in parsing has occurred.  In this case NERR is > 0 .
	 *   (2) All the tokens in the command have been successfully parsed. */

	if( *nerr != 0 )
		goto L_8888;


	/* CHECKING PHASE: */

	/* - Check for null data file list. */

	vflist( nerr );
	if( *nerr != 0 )
		goto L_8888;

	/* - Make sure there is at least 1 data file. */

	if( cmdfm.ndfl < 1 ){
		*nerr = 1301;
		setmsg( "ERROR", *nerr );
		goto L_8888;
		}

	/* EXECUTION PHASE: */

	/* - Perform the requested function on each pair of files in DFL. */

	jdfl = 1;
	jdfl_ = jdfl - 1;

	/* -- Get the first file  */


	getfil( 1, TRUE, &nlen1, &ndx1, &notused, nerr );
	if( *nerr != 0 )
	   goto L_8888;
       
	/* -- Retrieve the file name indices. */
	lnumcl( kmdfm.kdfl,MAXCHARS, jdfl, &ic1a, &ic2a );

	/* -- Get the second file if there is one */
        if(cmdfm.ndfl > 1) {
	  getfil( 2, TRUE, &nlen2, &ndx2, &notused, nerr );
	  if( *nerr != 0 )
	    goto L_8888;
	  lnumcl( kmdfm.kdfl,MAXCHARS, jdfl + 1, &ic1b, &ic2b );
	  horizontals=1;
	  vertical=0;
	  /* -- Check to make sure both files have the same number of points. */          
	  if( nlen1 == nlen2 ){
	    nlen = nlen1;
	  }
	  else{
	    *nerr = 2010;
	    setmsg( "ERROR", *nerr );
            fprintf(stderr, "ERROR file lengths not equal \n");
	    goto L_8888;
	  }
	}

        namelength = strcspn(evid," ");
        evid[namelength]= '\0';

	/* read input file */

	*nerr = get_input(cmsam.knmfir, &envelopes,&nbands,&global_params);
        if(*nerr != 0) {
	  fprintf(stderr, "Error reading input file! %s\n", cmsam.knmfir);
	  goto L_8888;
	}

        /* need to get begin and delta from somewhere!
	 *b = begin, *delta, *dist are defined in hdr.h
	*/
 
        C_begin = *b;
        C_delta = *delta;
        C_dist = *dist;

        if(C_dist > 0.0) {
	  fprintf(stderr, "begin=%f delta=%f dist=%f ID=%s\n",C_begin,C_delta,C_dist,&evid);
	} else {
	  fprintf(stderr, "dist not defined in header! f\n");
	  goto L_8888;          
	}
	calc_envelopes(cmmem.sacmem[ndx1], cmmem.sacmem[ndx2],nlen1,horizontals,&envelopes,&nbands,C_begin,C_delta,&npts,C_dist,&evid);

	calc_coda_amplitudes(&envelopes,nbands,C_dist,C_begin,&evid,lcalibrate);

        if(linteractive) {
	  pickwindows(&envelopes,nbands,C_begin,nerr);
	  calc_coda_amplitudes(&envelopes,nbands,C_dist,C_begin,&evid,lcalibrate);
	  pickwindows(&envelopes,nbands,C_begin,nerr);
	}

	calc_moment_magnitude(&envelopes,nbands,&global_params);
	calc_energy(envelopes,nbands,&evid,lcalibrate,&global_params);
	send_output(&envelopes,nbands,&evid,lcalibrate,&global_params);
	plotspec(&envelopes,nbands,C_begin,nerr);
	/* - Calculate and set new range of dependent variable. */

	setrng();

L_8888:
	return;

} /* end of function */
