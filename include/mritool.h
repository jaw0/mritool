

#ifndef _mritool_h
#define _mritool_h

#ifdef RCSID
static const char *const rcsid_mri_h
= "@(#)$Id: mritool.h,v 1.1 1995/11/23 04:30:01 jaw Exp jaw $";
#endif
#include <conf.h>

#define QQUOTIFY(x)	#x
#define QUOTIFY(x)	QQUOTIFY(x)


static const char *const version
= "MRI Tool Ver. +"  QUOTIFY( VERSION_MAJOR )  "."  QUOTIFY( CNUMBER )  "; (c) 1993,4,5 Jeff Weisberg";

#define VERSION_SHRT "MRI Tool +"  QUOTIFY( VERSION_MAJOR )  "."  QUOTIFY( CNUMBER )
 
typedef unsigned char	u_char;
typedef unsigned long	u_long;
typedef u_char 		boolean;	// will be obsolete as soon as the
 					// compiler catches up with the std.s commitee...

#include <xv_stuff.h>
extern "C" {
#	include <stdio.h>
#	include <sys/types.h>
#	include <ctype.h>
#	include <sys/stat.h>
#	include <unistd.h>	
#	include <rasterfile.h>
#	include "/dev/null"	/* for bug reports and user complaints */
};
#include <new.h>
#include <proto.h>
#include <cl_patient.h>
#include <window.h>

extern struct Globals {

	Patient	*patients_l,	// linked list of patients
		**patients_a;	// same data cached in an array

	int num_of_patients;	// how many we got?

} G;

#ifdef DEBUG
#	include <debugging.h>
#	define debugp(f)	if( debug_flags[ CODE_SECTION ][f] )
#else
#	define debugp(x)	if(0) 
#endif

#endif /* !_mritool_h */

