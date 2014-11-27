

// Copyright (c) 1994 Jeff Weisberg
// University of Rochester
//      Dept. of Electrical Engineering
//      Dept. of Neurology, C.E.P.
//
// see the file "License"


#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: cli-misc.cc,v 1.1 1995/11/23 04:30:07 jaw Exp jaw $";
#endif
 
// Optimization: None

#include <jlisp.h>
#include <jl_proto.h>
#include <defproto.h>
 
#include <mritool.h>
#include <solid.h>
#include <mrilisp.h>
#include <X11/Xutil.h>

extern UserI *ui_jl;
extern int current_level, current_window;

extern "C" print_stack();

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
 

DEFVAR("mri:version", Vmri_version, "mri:version  what version of mritool are we using",
       makstr( (char*)version ))

DEFVAR("mri:versn", Vmri_versn, "mri:version  what version of mritool are we using",
       makstr( VERSION_SHRT ))


DEFUN("current-level", Fcurr_level, Scurr_level, 0,0,1,0,
      "(current-level) return the current setting of the level",
      ())
{
	return MAKINT( current_level );
}
DEFUN("current-window", Fcurr_window, Scurr_window, 0,0,1,0,
      "(current-window) return the current setting of the window",
      ())
{
	return MAKINT( current_window );
}



DEFUN("refresh", Frefresh, Srefresh, 0,2,1,0,
      "(refresh [img] [ui]) refresh the image window",
      (Obj img, Obj ui))
{
	UserI *uui;

	if( IMAGEP(img)){
		Uimage foo( * CIMAGE(img));
		if( UI_P(ui)) uui = CUI(ui);
		else uui = ui_jl;
		uui->refresh(foo, 0);
	}else{
		if( UI_P(img))
			uui = CUI(img);
		else
			uui = ui_jl;
		if( uui && uui->c_image)
			uui->c_image->refresh( *uui );
	}
	return IC_UNSPEC;
}

DEFUN("visine", Fvisine, Svisine, 0,2,1,0,
      "(visine [img] [ui]) gets the red out",
      (Obj img, Obj ui))
{
	UserI *uui;
	
	if( IMAGEP(img)){
		if( UI_P(ui)) uui = CUI(ui);
		else uui = ui_jl;
		uui->visine( CIMAGE(img));
	}else{
		if( UI_P(img))
			uui = CUI(img);
		else
			uui = ui_jl;
		if( uui)
			uui->visine();
	}
	return IC_UNSPEC;
}

DEFUN("segment", Fsegment, Ssegment, 4,5,1,0,
      "(segment from to levels alpha [wsize]) segment a series of images",
      (Obj from, Obj to, Obj lev, Obj alf, Obj wsz))
{

	if( NBOUNDP(wsz))
		wsz = MAKINT(1);
	if( INUMP(wsz))
		wsz = makfloat( CINT(wsz));
	if( INUMP(alf))
		alf = makfloat( CINT(alf));
	
	if(! INUMP(to))
		return jlerror(Ssegment.name, to, "WTA: intp");
	if(! INUMP(from))
		return jlerror(Ssegment.name, from, "WTA: intp");
	if(! INUMP(lev))
		return jlerror(Ssegment.name, lev, "WTA: intp");
	if( ! FLOATP(alf))
		return jlerror(Ssegment.name, alf, "WTA: numberp");
	if( ! FLOATP(wsz))
		return jlerror(Ssegment.name, wsz, "WTA: numberp");

	if(! ui_jl)
		return jlerror(Ssegment.name, IC_NIL, "no ui");

	ui_jl->segment( CINT(from), CINT(to), CINT(lev), CFLOAT(alf), CFLOAT(wsz) );

	return IC_UNSPEC;
}

DEFUN("snake-range", Fsnake_range, Ssnake_range, 7,7,1,0,
      "(snake-range from to elast attr stiff 3d siml) snake a set of images",
      (Obj frm, Obj to, Obj ela, Obj atr, Obj stf, Obj thd, Obj sml))
{
	int start;
	
	if(! ui_jl)
		return jlerror(Ssnake_range.name, IC_NIL, "no ui");
	if(! INUMP(frm))
		return jlerror(Ssnake_range.name, frm, "WTA: intp");
	if(! INUMP(to))
		return jlerror(Ssnake_range.name, to,  "WTA: intp");
	if(! INUMP(ela))
		return jlerror(Ssnake_range.name, ela, "WTA: intp");
	if(! INUMP(atr))
		return jlerror(Ssnake_range.name, atr, "WTA: intp");
	if(! INUMP(stf))
		return jlerror(Ssnake_range.name, stf, "WTA: intp");
	if(! INUMP(thd))
		return jlerror(Ssnake_range.name, thd, "WTA: intp");
	if(! INUMP(sml))
		return jlerror(Ssnake_range.name, sml, "WTA: intp");

	start = ui_jl->c_image->num;
	
	SLICE_ITER( CINT(frm), CINT(to), ui_jl->c_image->num,
		ui_jl->snakit( CINT(ela), CINT(atr), CINT(stf), CINT(thd), CINT(sml),
			((sl_i==start)?0:((sl_i<start)?2:1)))	/* how */
	);
	ui_jl->set_contour(ui_jl->c_image->drawing);

	return IC_UNSPEC;
}

DEFUN("snake-slice", Fsnake_slice, Ssnake_slice, 6,6,1,0,
      "(snake-slice how elast attr stiff 3d siml) snake an image",
      (Obj how, Obj ela, Obj atr, Obj stf, Obj thd, Obj sml))
{
	if(! ui_jl)
		return jlerror(Ssnake_slice.name, IC_NIL, "no ui");
	if(! INUMP(how))
		return jlerror(Ssnake_slice.name, how, "WTA: intp");
	if(! INUMP(ela))
		return jlerror(Ssnake_slice.name, ela, "WTA: intp");
	if(! INUMP(atr))
		return jlerror(Ssnake_slice.name, atr, "WTA: intp");
	if(! INUMP(stf))
		return jlerror(Ssnake_slice.name, stf, "WTA: intp");
	if(! INUMP(thd))
		return jlerror(Ssnake_slice.name, thd, "WTA: intp");
	if(! INUMP(sml))
		return jlerror(Ssnake_slice.name, sml, "WTA: intp");

	ui_jl->snakit( CINT(ela), CINT(atr), CINT(stf), CINT(thd), CINT(sml), CINT(how) );
	ui_jl->set_contour(ui_jl->c_image->drawing);
	
	return IC_UNSPEC;
}

DEFUN("keyhole", Fkeyhole, Skeyhole, 3,3,1,0,
      "(keyhole expansion keywidth keyheight) generate a keyhole head",
      (Obj exp, Obj kw, Obj kh))
{
	float erate = fltof(exp),
		keywid = fltof(kw),
		keyhgt = fltof(kh);
	Point p;

	if(! ui_jl)
		return jlerror(Skeyhole.name, IC_NIL, "no ui");
	
	for(p.z=1; p.z <ui_jl->c_series->num_of_images; p.z++){
		Image *curr = ui_jl->c_image;
		ui_jl->c_image = ui_jl->c_series->find( p.z );
		Image *ci = ui_jl->c_image;
		ci->doit();
		int wd = ci->segm.width, ht = ci->segm.height;
		
		for(p.y=0;p.y<ht; p.y++) for(p.x=0; p.x<wd; p.x++){
			float rr = (p.x-wd/2)*(p.x-wd/2)
				+  (p.y-ht/2)*(p.y-ht/2);
			
			if( p.z*p.z*erate*erate > rr ){
				ci->segm[ p ] = 255;
			} else {
				ci->segm[ p ] = 0;
			}
			
			if( rr < (p.z+keyhgt)*(p.z+keyhgt)*erate*erate
			   && p.y >= ht/2
			   && p.x >= wd/2 - keywid/2
			   && p.x <= wd/2 + keywid/2
		   ){
				ci->segm[ p ] = 255;
			}
		}
		ci->S(Image::IFB_SEGM_PRESENT);
		ui_jl->c_image = curr;
	}
}


DEFUN("current-image", Fcurr_img, Scurr, 0,0,1,0,
      "(current-image) what is the current slice number?",
      ())
{
	if( ui_jl && ui_jl->c_image)
		return MAKINT( ui_jl->c_image->num);
	return IC_FALSE;
}

DEFUN("mriname", Fmriname, Smriname, 1,1,1,0,
      "(mriname name) expand name",
      (Obj nm))
{
	if(! STRINGP(nm))
		return jlerror(Smriname.name, nm, "WTA: stringp");

	return makstr( (char*)mriname( CCHARS(nm)));
}

DEFUN("analyze", Fanalyze, Sanalyze, 3,3,1,0,
      "(analyze from to item) analyze structures",
      (Obj fr, Obj to, Obj it))
{

	if(! INUMP(fr))
		return jlerror(Sanalyze.name, fr, "WTA: intp");
	if(! INUMP(to))
		return jlerror(Sanalyze.name, to, "WTA: intp");
	if(! INUMP(it))
		return jlerror(Sanalyze.name, it, "WTA: intp");
	if(! ui_jl)
		return jlerror(Sanalyze.name, IC_NIL, "no ui");

	ui_jl->c_series->analyze(*ui_jl, CINT(it), CINT(fr), CINT(to) );

	return IC_UNSPEC;
}

DEFUN("set-debug-flag!", Fset_debug, Sset_debug, 2,3,1,0,
      "(set-debug-flag! section field [flag]) set debugging flags",
      (Obj s, Obj f, Obj v))
{
	int o;
	
	if(! INUMP(s))
		return jlerror(Sset_debug.name, s, "WTA: intp");
	if(! INUMP(f))
		return jlerror(Sset_debug.name, f, "WTA: intp");

	o = debug_flags[ CINT(s) ][ CINT(f) ];
	
	debug_flags[ CINT(s) ][ CINT(f) ] =
		FALSEP(v) ? 0 : 1;

	return o ? IC_TRUE : IC_FALSE;
}

DEFUN("debug-flag", Fdebug_flag, Sdebug_flag, 2,2,1,0,
      "(debug-flag section field) return the current flag",
      (Obj s, Obj f))
{
	if(! INUMP(s))
		return jlerror(Sdebug_flag.name, s, "WTA: intp");
	if(! INUMP(f))
		return jlerror(Sdebug_flag.name, f, "WTA: intp");

	return debug_flags[ CINT(s) ][ CINT(f) ] ? IC_TRUE : IC_FALSE;
}


DEFUN("graph", Fgraph, Sgraph, 1,1,1,0,
      "(graph vector) graph the vector",
      (Obj v))
{
	int i=0;
	XView *xv;
	int ox = 0;
	int oy = 128;
	XGCValues values;
	GC      gc;

	if(! ui_jl || ! (xv=ui_jl->get_xv()))
		return jlerror(Sgraph.name, IC_NIL, "no ui");

		
	values.foreground = 0x00;
	values.line_width = 1;

	gc = XCreateGC(xv->dpy, xv->histo_xid,
		GCForeground|GCLineWidth,
		&values);

	// clear out graph
	XFillRectangle(xv->dpy, xv->histo_xid, gc, 0,0, 256, 128);

	XFreeGC(xv->dpy, gc);
	
	values.foreground = 0x80;       // Red
	gc = XCreateGC(xv->dpy, xv->histo_xid,
		GCForeground|GCLineWidth,
		&values);

	if( VECTORP(v))
		for(i=0;i<CLENGTH(v); i++){
			Obj o = CVECTOR(v)[i];
			int y;
			if( INUMP(o))
				y = 128 - CINT(o);
			else if( FLOATP(o))
				y = 128 - CFLOAT(o);
			else if ( DOUBLEP(o))
				y = 128 - CDOUBLE(o);
			else
				continue;
			XDrawLine(xv->dpy, xv->histo_xid, gc, ox, oy, i, y);
			ox = i;
			oy = y;
		}
	else if( CONSP(v))
		while( CONSP(v)){
			Obj o = CAR(v);
			v = CDR(v);
			int y;
			if( INUMP(o))
				y = 128 - CINT(o);
			else if( FLOATP(o))
				y = 128 - CFLOAT(o);
			else if ( DOUBLEP(o))
				y = 128 - CDOUBLE(o);
			else
				continue;
			XDrawLine(xv->dpy, xv->histo_xid, gc, ox, oy, i, y);
			ox = i++;
			oy = y;
		}

	XFreeGC(xv->dpy, gc);
	
	return IC_UNSPEC;
}

#if 0
DEFUN("dump-stack", Fdump_stack, Sdump_stack, 0,0,1,0,
      "(dump-stack) print a dump of the stack in adb format",
      ())
{

	print_stack();
	return IC_UNSPEC;
}
#endif

DEFUN("set-contour", Fset_contour, Sset_contour, 1,2,1,0,
      "(set-contour img [ui]) ",
      (Obj img, Obj u))
{
	UserI *ui;
	XView *foo;
	
	if(! IMAGEP(img))
		return jlerror(Sset_contour.name, img, "WTA: imagep");
	
	if( UI_P(u)) ui = CUI(u);
	else ui = ui_jl;
	
	ui->c_image->drawing2 = * CIMAGE(img);
	return IC_UNSPEC;
}

DEFUN("get-contour", Fget_contour, Sget_contour, 0,1,1,0,
      "(get-contour [ui]) return the contour",
      (Obj u))
{
	UserI *ui;
	XView *foo;
	Bimage *bar;
	
	if( UI_P(u)) ui = CUI(u);
	else ui = ui_jl;

	bar = ui->c_image->drawing2.clone();
	*bar = ui->c_image->drawing2;
	
	return makimage( *bar );
}

DEFUN("get-contour-v", Fget_contourv, Sget_contourv, 0,1,1,0,
      "(get-contour-v [ui]) return the contour",
      (Obj u))
{
	UserI *ui;
	XView *foo;
	Bimage *bar;
	
	if( UI_P(u)) ui = CUI(u);
	else ui = ui_jl;

	bar = ui->c_image->drawing.clone();
	*bar = ui->c_image->drawing;
	
	return makimage( *bar );
}

DEFUN("set-view", Fset_view, Sset_view, 1,2,1,0,
      "(set-view img [ui]) set the view image",
      (Obj img, Obj u))
{
	UserI *ui;
	XView *foo;
	int x, y, w,h;

	if(! IMAGEP(img))
		return jlerror(Sset_view.name, img, "WTA: imagep");
	
	if( UI_P(u)) ui = CUI(u);
	else ui = ui_jl;
	
	w = CIMAGE( img )->width;
	h = CIMAGE( img )->height;
	for(y=0; y<h; y++) for(x=0; x<w; x++){
		CIMAGE(img)->put(x, y,
				 CIMAGE(img)->value(x,y) / 4 + 64);
	}
	
	ui->c_image->view = * CIMAGE(img);
	return IC_UNSPEC;
}

DEFUN("get-view", Fget_view, Sget_view, 0,1,1,0,
      "(get-view [ui]) return the view",
      (Obj u))
{
	UserI *ui;
	XView *foo;
	Bimage *bar;
	Obj img;
	int x, y,w,h;
	float v;
	
	if( UI_P(u)) ui = CUI(u);
	else ui = ui_jl;

	bar = ui->c_image->view.clone();
	*bar = ui->c_image->view;

	img = makimage( *bar );

	w = CIMAGE( img )->width;
	h = CIMAGE( img )->height;
	for(y=0; y<h; y++) for(x=0; x<w; x++){
		v = (CIMAGE(img)->value(x,y) - 64.0) * 4.0;
		v = MIN(255, MAX(0, v));
		CIMAGE(img)->put(x, y, v);
	}

	return img;
}	
DEFUN("get-view-raw", Fget_view_raw, Sget_view_raw, 0,1,1,0,
      "(get-view-raw [ui]) return the view",
      (Obj u))
{
	UserI *ui;
	XView *foo;
	Bimage *bar;
	Obj img;
	int x, y,w,h;
	float v;
	
	if( UI_P(u)) ui = CUI(u);
	else ui = ui_jl;

	bar = ui->c_image->view.clone();
	*bar = ui->c_image->view;

	img = makimage( *bar );

	return img;
}	

DEFUN("set-fimg", Fset_fimg, Sset_fimg, 1,2,1,0,
      "(set-fimg img [ui]) set the fimg",
      (Obj img, Obj u))
{
	UserI *ui;
	XView *foo;
	
	if(! IMAGEP(img))
		return jlerror(Sset_view.name, img, "WTA: imagep");
	
	if( UI_P(u)) ui = CUI(u);
	else ui = ui_jl;
	
	ui->c_image->fimg = * CIMAGE(img);
	return IC_UNSPEC;
}

DEFUN("get-fimg", Fget_fimg, Sget_fimg, 0,1,1,0,
      "(get-fimg [ui]) return the fimg",
      (Obj u))
{
	UserI *ui;
	XView *foo;
	Bimage *bar;
	
	if( UI_P(u)) ui = CUI(u);
	else ui = ui_jl;

	bar = ui->c_image->fimg.clone();
	*bar = ui->c_image->fimg;
	return makimage( *bar );
}

DEFUN("set-segm", Fset_segm, Sset_segm, 1,2,1,0,
      "(set-segm img [ui]) ",
      (Obj img, Obj u))
{
	UserI *ui;
	XView *foo;
	
	if(! IMAGEP(img))
		return jlerror(Sset_segm.name, img, "WTA: imagep");
	
	if( UI_P(u)) ui = CUI(u);
	else ui = ui_jl;
	
	ui->c_image->segm = * CIMAGE(img);
	return IC_UNSPEC;
}

DEFUN("get-segm", Fget_segm, Sget_segm, 0,1,1,0,
      "(get-segm [ui]) return the segm",
      (Obj u))
{
	UserI *ui;
	XView *foo;
	Bimage *bar;
	
	if( UI_P(u)) ui = CUI(u);
	else ui = ui_jl;

	bar =  ui->c_image->segm.clone();
	*bar =  ui->c_image->segm;
	return makimage( *bar );
}

DEFUN("rdb:mark", Frdb_mark, Srdb_mark, 1,4,1,0,
      "(rdb:mark ...",
      (Obj pn, Obj en, Obj sn, Obj in))
{
	if(! INUMP(pn))
		return jlerror(Srdb_mark.name, pn, "WTA: int p");
	if(! INUMP(en))
		find( CINT(pn))->rdb_mark();
	else if(! INUMP(sn))
		find( CINT(pn))->find( CINT(en))->rdb_mark();
	else if(! INUMP(in))
		find( CINT(pn))->find( CINT(en))->find( CINT(sn))->rdb_mark();
	else
		find( CINT(pn))->find( CINT(en))->find( CINT(sn))->find( CINT(in))->rdb_mark();
	return IC_UNSPEC;
}

DEFUN("rdb:mark-patient", Frdb_mark_pat, Srdb_mark_pat, 0,1,1,0,
      "(rdb:mark-patient [ui]) mark for archiving",
      (Obj u))
{
	UserI *ui;
	
	if( UI_P(u)) ui = CUI(u);
	else ui = ui_jl;

	if(ui->c_patient)
		ui->c_patient->rdb_mark();
	return IC_UNSPEC;
}

DEFUN("rdb:mark-study", Frdb_mark_std, Srdb_mark_std, 0,1,1,0,
      "(rdb:mark-study [ui]) mark for archiving",
      (Obj u))
{
	UserI *ui;
	
	if( UI_P(u)) ui = CUI(u);
	else ui = ui_jl;

	if(ui->c_study)
		ui->c_study->rdb_mark();
	return IC_UNSPEC;
}

DEFUN("rdb:mark-series", Frdb_mark_ser, Srdb_mark_ser, 0,1,1,0,
      "(rdb:mark-series [ui]) mark for archiving",
      (Obj u))
{
	UserI *ui;
	
	if( UI_P(u)) ui = CUI(u);
	else ui = ui_jl;

	if(ui->c_series)
		ui->c_series->rdb_mark();
	return IC_UNSPEC;
}

DEFUN("rdb:mark-image", Frdb_mark_img, Srdb_mark_img, 0,1,1,0,
      "(rdb:mark-image [ui]) mark for archiving",
      (Obj u))
{
	UserI *ui;
	
	if( UI_P(u)) ui = CUI(u);
	else ui = ui_jl;

	if(ui->c_image)
		ui->c_image->rdb_mark();
	return IC_UNSPEC;
}

extern void rdb_arcv(void), rdb_dele(void);

DEFUN("rdb:arcv", Frdb_arcv, Srdb_arcv, 0,0,1,0,
      "(rdb:arcv) archive marked items",
      ())
{
	rdb_arcv();
	return IC_UNSPEC;
}

DEFUN("rdb:dele", Frdb_dele, Srdb_dele, 0,0,1,0,
      "(rdb:arcv) delete marked items",
      ())
{
	rdb_dele();
	return IC_UNSPEC;
}





