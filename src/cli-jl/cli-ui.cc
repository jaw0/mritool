

// Copyright (c) 1994 Jeff Weisberg
// University of Rochester
//      Dept. of Electrical Engineering
//      Dept. of Neurology, C.E.P.
//
// see the file "License"


#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: cli-ui.cc,v 1.1 1995/11/23 04:30:08 jaw Exp jaw $";
#endif
 
// Optimization: -O2

#include <jlisp.h>
#include <jl_proto.h>
#include <defproto.h>
 
#include <mritool.h>
#include <solid.h>
#include <mrilisp.h>
 
extern UserI *ui_jl;

DEFUN("new-xv", Fnew_xv, Snew_xv, 0,0,1,0,
      "(new-xv) create a new window frame",
      ())
{
	return mak_ui( new XView );
}

DEFUN("new-nowin", Fnew_nowin, Snew_nowin, 0,0,1,0,
      "(new-nowin) create a new non-window interactive frame",
      ())
{
	return mak_ui( new NoWin );
}


DEFUN("current-ui", Fcurrent_ui, Scurrent_ui, 0,0,1,0,
      "(current-ui)  return the current ui",
      ())
{

	if( ui_jl)
		return mak_ui( ui_jl );
	else
		return IC_NIL;
}

DEFUN("set-current-ui!", Fset_current_ui, Sset_current_ui, 1,1,1,0,
      "(set-current-ui! ui) set the current ui",
      (Obj ui))
{

	if( UI_P(ui))
		UI = ui_jl = CUI(ui);
	else
		ui_jl = 0;

	return mak_ui( ui_jl );
}


DEFUN("ui-p", Fui_p, Sui_p, 1,1,1,0,
      "(ui-p obj) is this a ui?",
      (Obj ui))
{
	return UI_P(ui) ? IC_TRUE : IC_FALSE;
}

#if 0
DEFUN("base-ui-p", Fbase_ui_p, Sbase_ui_p, 1,1,1,0,
      "(base-ui-p obj) is this the base xv ui?",
      (Obj ui))
{
	if( ui == XView::base_frame)
		return IC_TRUE;

	return IC_FALSE;
}

#endif

DEFUN("set-right-footer-text", Frfooter_text, Srfooter_text, 1,2,1,0,
      "(set-right-footer-text text [ui]) set the right footer text",
      (Obj txt, Obj ui))
{
	UserI *uui;
	XView *foo;
	
	if( UI_P(ui)) uui = CUI(ui);
	else uui = ui_jl;

	if(! (foo=uui->get_xv()))
		return jlerror(Srfooter_text.name, IC_NIL, "WTA: bad ui");

	xv_set(foo->main_frame, FRAME_RIGHT_FOOTER,
	       STRINGP(txt) ? CCHARS(txt) : "", 0);

	return IC_UNSPEC;
}

DEFUN("set-left-footer-text", Flfooter_text, Slfooter_text, 1,2,1,0,
      "(set-left-footer-text text [ui]) set the left footer text",
      (Obj txt, Obj ui))
{
	UserI *uui;
	XView *foo;
	
	if( UI_P(ui)) uui = CUI(ui);
	else uui = ui_jl;

	if(! (foo=uui->get_xv()))
		return jlerror(Slfooter_text.name, IC_NIL, "WTA: bad ui");

	xv_set(foo->main_frame, FRAME_LEFT_FOOTER,
	       STRINGP(txt) ? CCHARS(txt) : "", 0);

	return IC_UNSPEC;
}

DEFUN("set-header-text", Fheader_text, Sheader_text, 1,2,1,0,
      "(set-headerr-text text [ui]) set the header text",
      (Obj txt, Obj ui))
{
	UserI *uui;
	XView *foo;
	
	if( UI_P(ui)) uui = CUI(ui);
	else uui = ui_jl;

	if(! (foo=uui->get_xv()))
		return jlerror(Sheader_text.name, IC_NIL, "WTA: bad ui");

	xv_set(foo->main_frame, XV_LABEL,
	       STRINGP(txt) ? CCHARS(txt) : "", 0);

	return IC_UNSPEC;
}

DEFUN("set-frame-size", Fframe_size, Sframe_size, 2,3,1,0,
      "(set-frame-size width height [ui])",
      (Obj w, Obj h, Obj ui))
{
	UserI *uui;
	XView *foo;
	
	if( UI_P(ui)) uui = CUI(ui);
	else uui = ui_jl;

	if(! (foo=uui->get_xv()))
		return jlerror(Sframe_size.name, IC_NIL, "WTA: bad ui");
	if(! INUMP(w))
		return jlerror(Sframe_size.name, w, "WTA: intp");
	if(! INUMP(h))
		return jlerror(Sframe_size.name, h, "WTA: intp");

	xv_set(foo->main_frame, XV_WIDTH, CINT(w),
	       XV_HEIGHT, CINT(h), 0);

	return IC_UNSPEC;
}

DEFUN("install-cmap", Finstall_cmap, Sinstall_cmap, 1,2,1,0,
      "(install-cmap img [ui])",
      (Obj im, Obj ui))
{
	UserI *uui;
	XView *foo;
	int i;
	
	if( UI_P(ui)) uui = CUI(ui);
	else uui = ui_jl;

	if(! (foo=uui->get_xv()))
		return jlerror(Sinstall_cmap.name, IC_NIL, "WTA: bad ui");

	if( IMAGEP(im)){
		if( CIMAGE(im)->cmap){
			for(i=0; i<CIMAGE(im)->cmap->len; i++){
				foo->map[i].red   = CIMAGE(im)->cmap->r[i];
				foo->map[i].green = CIMAGE(im)->cmap->g[i];
				foo->map[i].blue  = CIMAGE(im)->cmap->b[i];
			}
		}else{
			for(i=0; i<256; i++){
				foo->map[i].red = foo->map[i].green = foo->map[i].blue = i;
			}
		}
	}else
		return jlerror(Sinstall_cmap.name, im, "WTA: cmap p");

	xv_set(foo->cms, CMS_COLORS, foo->map, 0);
	xv_set(foo->paint,  WIN_CMS, foo->cms, 0);

	return IC_UNSPEC;
}

DEFUN("modify-cmap", Fmod_cmap, Smod_cmap, 1,3,1,0,
      "(modify-cmap n #(r g b) [ui]) install color map entry\n"
      "(modify-cmap #t [ignored [ui]]) install the colormap",
      (Obj n, Obj v, Obj ui))
{
	UserI *uui;
	XView *foo;
	int i;
	
	if( UI_P(ui)) uui = CUI(ui);
	else uui = ui_jl;

	if(! (foo=uui->get_xv()))
		return jlerror(Smod_cmap.name, IC_NIL, "WTA: bad ui");

	if( n == IC_TRUE){
		xv_set(foo->cms, CMS_COLORS, foo->map, 0);
		xv_set(foo->paint,  WIN_CMS, foo->cms, 0);
		return IC_UNSPEC;
	}

	if(! INUMP(n))
		return jlerror(Smod_cmap.name, n, "WTA: intp");
	if(! VECTORP(v))
		return jlerror(Smod_cmap.name, v, "WTA: vectorp");

	if( CLENGTH(v)!=3
	   || !INUMP(CVECTOR(v)[0])
	   || !INUMP(CVECTOR(v)[1])
	   || !INUMP(CVECTOR(v)[2]))
		return jlerror(Smod_cmap.name, v, "WTA: vector of 3 ints p");
	
	foo->map[ CINT(n) ].red   = CINT( CVECTOR(v)[0]);
	foo->map[ CINT(n) ].green = CINT( CVECTOR(v)[1]);
	foo->map[ CINT(n) ].blue  = CINT( CVECTOR(v)[2]);

	return IC_UNSPEC;
}

DEFUN("thinking", Fthinking, Sthinking, 0,1,1,0,
      "(thinking [ui]) begin pondering",
      (Obj ui))
{
	UserI *uui;
	if( UI_P(ui)) uui = CUI(ui);
	else uui = ui_jl;

	uui->thinking();

	return IC_UNSPEC;
}

DEFUN("done-thinking", Fdone_thinking, Sdone_thinking, 0,1,1,0,
      "(done-thinking [ui]) ponder no more",
      (Obj ui))
{
	UserI *uui;
	if( UI_P(ui)) uui = CUI(ui);
	else uui = ui_jl;
	
	uui->done_thinking();

	return IC_UNSPEC;
}

DEFUN("show-box", Fshow_box, Sshow_box, 4,5,1,0,
      "(show-box xo yo xi yi [ui]) show a box",
      (Obj xo, Obj yo, Obj xi, Obj yi, Obj ui))
{
	UserI *uui;
	XView *foo;
	
	if( UI_P(ui)) uui = CUI(ui);
	else uui = ui_jl;

	if(! (foo=uui->get_xv()))
		return jlerror(Sshow_box.name, IC_NIL, "WTA: bad ui");

	if(! INUMP(xo))
		return jlerror(Sshow_box.name, xo, "WTA: intp");
	if(! INUMP(yo))
		return jlerror(Sshow_box.name, yo, "WTA: intp");
	if(! INUMP(xi))
		return jlerror(Sshow_box.name, xi, "WTA: intp");
	if(! INUMP(yi))
		return jlerror(Sshow_box.name, yi, "WTA: intp");

	XDrawRectangle(foo->dpy, foo->canvas_xid, foo->gc_draw,
		       CINT(xo), CINT(yo),
		       CINT(xi)-CINT(xo), CINT(yi)-CINT(yo));

	return IC_UNSPEC;
}

