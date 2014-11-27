

// Copyright (c) 1994 Jeff Weisberg
// University of Rochester
//      Dept. of Electrical Engineering
//      Dept. of Neurology, C.E.P.
//
// see the file "License"


#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: cli-save.cc,v 1.1 1995/11/23 04:30:06 jaw Exp jaw $";
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

#define SLICE_ITER(lo, hi, start, action)                               \
        if(ui_jl && ui_jl->c_series){                                   \
                int sl_l = ((lo) < 1) ? 1 : (lo);                       \
                int sl_h = ((hi)>ui_jl->c_series->num_of_images)        \
                        ? (ui_jl->c_series->num_of_images) : (hi);      \
                int sl_st= ((start)>(hi)) ? (hi) :                      \
                        (((start)<(lo)) ? (lo) : (start));              \
                int sl_i;                                               \
                Image* sl_curr = ui_jl->c_image;                        \
                for(sl_i=sl_st; sl_i<=sl_h; sl_i++){                    \
                        ui_jl->c_image = ui_jl->c_series->find(sl_i);   \
                        action;                                         \
                }                                                       \
                for(sl_i=sl_st-1; sl_i>=sl_l; sl_i--){                  \
                        ui_jl->c_image = ui_jl->c_series->find(sl_i);   \
                        action;                                         \
                }                                                       \
                ui_jl->c_image = sl_curr;                               \
        }
 

DEFUN("save-contour", Fsave_cont, Ssave_cont, 3,3,1,0,
      "(save-contour from to filename) save contours",
      (Obj from, Obj to, Obj fn))
{
	if(! INUMP(from))
		return jlerror(Ssave_cont.name, from, "WTA: intp");
	if(! INUMP(to))
		return jlerror(Ssave_cont.name, to, "WTA: intp");
	if(! STRINGP(fn))
		return jlerror(Ssave_cont.name, fn, "WTA: stringp");
	if( !ui_jl)
		return jlerror(Ssave_cont.name, IC_NIL, "no ui");

	chdir(mriname("=S"));
	SLICE_ITER( CINT(from), CINT(to), CINT(from),
		ui_jl->c_image->drawing2.save(mriname( CCHARS(fn)))
	);
	return IC_UNSPEC;
}

DEFUN("save-segment", Fsave_segm, Ssave_segm, 3,3,1,0,
      "(save-segment from to filename) save segmented images",
      (Obj from, Obj to, Obj fn))
{
	if(! INUMP(from))
		return jlerror(Ssave_segm.name, from, "WTA: intp");
	if(! INUMP(to))
		return jlerror(Ssave_segm.name, to, "WTA: intp");
	if(! STRINGP(fn))
		return jlerror(Ssave_segm.name, fn, "WTA: stringp");
	if( !ui_jl)
		return jlerror(Ssave_segm.name, IC_NIL, "no ui");

	chdir(mriname("=S"));
	SLICE_ITER( CINT(from), CINT(to), CINT(from),
		ui_jl->c_image->segm.save(mriname( CCHARS(fn)))
	);
	return IC_UNSPEC;
}

DEFUN("save-view", Fsave_view, Ssave_view, 3,3,1,0,
      "(save-view from to filename) save viewable images",
      (Obj from, Obj to, Obj fn))
{
	if(! INUMP(from))
		return jlerror(Ssave_view.name, from, "WTA: intp");
	if(! INUMP(to))
		return jlerror(Ssave_view.name, to, "WTA: intp");
	if(! STRINGP(fn))
		return jlerror(Ssave_view.name, fn, "WTA: stringp");
	if( !ui_jl)
		return jlerror(Ssave_view.name, IC_NIL, "no ui");

	chdir(mriname("=S"));
	SLICE_ITER( CINT(from), CINT(to), CINT(from),
		ui_jl->c_image->view.save(mriname( CCHARS(fn)))
	);
	return IC_UNSPEC;
}

