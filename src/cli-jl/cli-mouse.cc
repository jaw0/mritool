

// Copyright (c) 1994 Jeff Weisberg
// University of Rochester
//      Dept. of Electrical Engineering
//      Dept. of Neurology, C.E.P.
//
// see the file "License"

// Optimization: None

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: cli-mouse.cc,v 1.1 1995/11/23 04:30:09 jaw Exp jaw $";
#endif
 
#include <jlisp.h>
#include <jl_proto.h>
#include <defproto.h>
 
#include <mritool.h>
#include <solid.h>
#include <mrilisp.h>
#include <handy.h>
#include <sys/types.h>
#include <sys/time.h>

extern UserI *ui_jl;

extern "C" Obj maktimel(time_t, int);

#define CODE_SECTION SECTION_LISP

DEFUN("adjust-wl", Fadjust_wl, Sadjust_wl, 2,3,1,0,
      "(adjust-wl window level) adjust the window and level",
      (Obj w, Obj l, Obj u))
{
	UserI *ui;
	XView *foo;
	
	if(! INUMP(w))
		return jlerror(Sadjust_wl.name, w, "WTA: intp");
	if(! INUMP(l))
		return jlerror(Sadjust_wl.name, l, "WTA: intp");
	if( UI_P(u)) ui = CUI(u);
	else ui = ui_jl;
	
	if( !ui || !(foo=ui->get_xv()))
		return jlerror(Sadjust_wl.name, IC_NIL, "no ui");

	foo->adjust_wl(CINT(w), CINT(l));

	return IC_UNSPEC;
}

DEFUN("draw-line", Fdraw_line, Sdraw_line, 5,7,1,0,
      "(draw-line xi yi xf yf s [ui] [img]) ...",
      (Obj xi, Obj yi, Obj xf, Obj yf, Obj s, Obj u, Obj uu))
{
	UserI *ui;
	Bimage *img = 0;
	
	if(! INUMP(xi))
		return jlerror(Sdraw_line.name, xi, "WTA: intp");
	if(! INUMP(yi))
		return jlerror(Sdraw_line.name, yi, "WTA: intp");
	if(! INUMP(xf))
		return jlerror(Sdraw_line.name, xf, "WTA: intp");
	if(! INUMP(yf))
		return jlerror(Sdraw_line.name, yf, "WTA: intp");
	if(! INUMP(s))
		return jlerror(Sdraw_line.name, s, "WTA: intp");

	if( UI_P(u)){
		ui = CUI(u);
		if( IMAGEP(uu))
			img = CIMAGE(uu);
	}else{
		ui = ui_jl;
		if( IMAGEP(u))
			img = CIMAGE(u);
	}

	if(! ui )
		return jlerror(Sadjust_wl.name, IC_NIL, "no ui");

	if(! img)
		ui->c_image->S(Image::IFB_DRAWN_ON);	// invalidate any snake laying around
	ui->put_line(Point(CINT(xi), CINT(yi)),
		Point(CINT(xf), CINT(yf)), CINT(s), img);

	return IC_UNSPEC;
}

DEFUN("draw-point", Fdraw_point, Sdraw_point, 3,5,1,0,
      "(draw-point x y s [ui] [img]) ...",
      (Obj x, Obj y, Obj s, Obj u, Obj uu))
{
	UserI *ui;	
	Bimage *img = 0;
	
	if(! INUMP(x))
		return jlerror(Sdraw_point.name, x, "WTA: intp");
	if(! INUMP(y))
		return jlerror(Sdraw_point.name, y, "WTA: intp");
	if(! INUMP(s))
		return jlerror(Sdraw_point.name, s, "WTA: intp");

	if( UI_P(u)){
		ui = CUI(u);
		if( IMAGEP(uu))
			img = CIMAGE(uu);
	}else{
		ui = ui_jl;
		if( IMAGEP(u))
			img = CIMAGE(u);
	}

	if(! img)
		ui->c_image->S(Image::IFB_DRAWN_ON);	// invalidate any snake laying around
	ui->put_dot(Point(CINT(x), CINT(y)), CINT(s), img);

	return IC_UNSPEC;
}

DEFUN("erase-area", Ferase_area, Serase_area, 2,4,1,0,
      "(erase-area x y [ui] [img]) ...",
      (Obj x, Obj y, Obj u, Obj uu))
{
	UserI *ui;
	Bimage *img = 0;
	
	if(! INUMP(x))
		return jlerror(Serase_area.name, x, "WTA: intp");
	if(! INUMP(y))
		return jlerror(Serase_area.name, y, "WTA: intp");

	if( UI_P(u)){
		ui = CUI(u);
		if( IMAGEP(uu))
			img = CIMAGE(uu);
	}else{
		ui = ui_jl;
		if( IMAGEP(u))
			img = CIMAGE(u);
	}

	if(! img)
		ui->c_image->S(Image::IFB_DRAWN_ON);	// invalidate any snake laying around

	if( img){
		Uimage foo(* img);
		ui->clear_spot(Point(CINT(x), CINT(y)), &foo);
	}else{
		ui->clear_spot(Point(CINT(x), CINT(y)), 0);
	}

	return IC_UNSPEC;
}

DEFUN("mouse-set-loi", Fmouse_loi, Smouse_loi, 3,4,1,0,
      "(mouse-set-loi x y [ui]) ...",
      (Obj x, Obj y, Obj s, Obj u))
{
	UserI *ui;
	int loi;
	int kx, ky;

	if(! INUMP(x))
		return jlerror(Smouse_loi.name, x, "WTA: intp");
	if(! INUMP(y))
		return jlerror(Smouse_loi.name, y, "WTA: intp");
	if(! INUMP(s))
		return jlerror(Smouse_loi.name, s, "WTA: intp");

	if( UI_P(u)) ui = CUI(u);
	else ui = ui_jl;

	kx = ui->c_image->segm.width * CINT(x) / 512;  	// coord is in view space
	ky = ui->c_image->segm.height* CINT(y) / 512;	// view space is 512 * 512
	
	// I think we need one more level of indirection here...
	loi = ui->c_image->snake[CINT(s)].loi = ui->c_image->segm(kx, ky);
	ui->c_image->snake[CINT(s)].ploi = Point(CINT(x),CINT(y));

	return MAKINT(loi);
}


#if 1
extern "C" void  mouse_controller(Canvas can, 
	Event *event){
	// mouse was pressed in canvas

	int eid = event_id(event);
	int sm = event->ie_shiftmask;
	int x  = event_x(event);
	int y  = event_y(event);
	XView &m = me(can);
	UserI *oui = ui_jl;
	Obj v;
	
	// if ((x>512)||(y>512)||(x<0)||(y<0)) return; // out of bounds
	if( (x<0)||(y<0)) return;
	// if(! m.c_image) return;

	if (eid == MS_RIGHT){
		// in any mode right button will popup menu
		menu_show(m.mouse_menu, m.canvas, event,0);
		return;
	}

	v = makvect(9);
	CVECTOR(v)[0] = MAKINT( event->ie_code );
	CVECTOR(v)[1] = MAKINT( event->ie_flags );
	CVECTOR(v)[2] = MAKINT( event->ie_shiftmask );
	CVECTOR(v)[3] = MAKINT( event->ie_locx );
	CVECTOR(v)[4] = MAKINT( event->ie_locy );
	CVECTOR(v)[5] = maktimel(event->ie_time.tv_sec, event->ie_time.tv_usec);
	CVECTOR(v)[6] = MAKINT( event->action );
	CVECTOR(v)[7] = makstr( event->ie_string );
	CVECTOR(v)[8] = mak_ui( &m );

	ui_jl = &m;
	// how can we avoid the overhead on each event???
	Ffuncall( Fcons(maksym("mouse:handler"),
		Fcons(v, IC_NIL)));

	ui_jl = oui;
}


#endif

	
