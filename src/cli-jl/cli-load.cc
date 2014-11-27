
// Copyright (c) 1994 Jeff Weisberg
// University of Rochester
//      Dept. of Electrical Engineering
//      Dept. of Neurology, C.E.P.
//
// see the file "License"


#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: cli-load.cc,v 1.1 1995/11/23 04:30:07 jaw Exp jaw $";
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


DEFUN("load-cont", Fload_cont, Sload_cont, 1,1,1,0,
      "(load-cont filename) load in a contour",
      (Obj a))
{

	if(! STRINGP(a))
		return jlerror(Sload_cont.name, a, "WTA: stringp");

	if( !ui_jl || !ui_jl->c_image )
		return jlerror(Sload_cont.name, IC_NIL, "no ui");

	if(! ui_jl->c_image->load_cont( CCHARS(a)))
		return jlerror(Sload_cont.name, a, "Could not load");

	return IC_UNSPEC;
}

DEFUN("load-segm", Fload_segm, Sload_segm, 1,1,1,0,
      "(load-segm filename) load in a segmented image",
      (Obj a))
{

	if(! STRINGP(a))
		return jlerror(Sload_segm.name, a, "WTA: stringp");

	if( !ui_jl || !ui_jl->c_image )
		return jlerror(Sload_segm.name, IC_NIL, "no ui");

	if(! ui_jl->c_image->segm.load( mriname(CCHARS(a))))
		return jlerror(Sload_segm.name, a, "Could not load");

	return IC_UNSPEC;
}

DEFUN("load-view", Fload_view, Sload_view, 1,1,1,0,
      "(load-view filename) load in a view image",
      (Obj a))
{

	if(! STRINGP(a))
		return jlerror(Sload_view.name, a, "WTA: stringp");

	if( !ui_jl || !ui_jl->c_image )
		return jlerror(Sload_view.name, IC_NIL, "no ui");

	if(! ui_jl->c_image->view.load( mriname(CCHARS(a))))
		return jlerror(Sload_view.name, a, "Could not load");

	return IC_UNSPEC;
}
