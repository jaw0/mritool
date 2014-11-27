
// Copyright (c) 1994 Jeff Weisberg
// University of Rochester
//      Dept. of Electrical Engineering
//      Dept. of Neurology, C.E.P.
//
// see the file "License"


#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: cli-show.cc,v 1.1 1995/11/23 04:30:08 jaw Exp jaw $";
#endif
 
// Optimization: -O2

#include <jlisp.h>
#include <jl_proto.h>
#include <defproto.h>
 
#include <mritool.h>
#include <solid.h>
#include <mrilisp.h>
 
extern UserI *ui_jl;
extern int verbosity_level;



DEFUN("show-stats", Fshow_stats, Sshow_stats, 0,1,1,0,
      "(show-stats [n]) show stats",
      (Obj n))
{
	int i;
	
	if( ui_jl){
		if( INUMP(n))
			cout << ui_jl->c_series->stats[ CINT(n) ] << endl;
		else
			for(i=0; i<ui_jl->c_image->num_of_snakes; i++)
				cout << ui_jl->c_series->stats[ i ] << endl;
	}
	return IC_UNSPEC;
}

