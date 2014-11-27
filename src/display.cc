

// Copyright (c) 1993 Jeff Weisberg
// University of Rochester
//	Dept. of Electrical Engineering
//	Dept. of Neurology, C.E.P.
//
// see the file "License"

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: display.cc,v 1.1 1995/11/23 04:30:14 jaw Exp jaw $";
#endif

// Optimization: Full

#include <mritool.h>
#include <window.h>


void Image::update_view(){
	// load view image from processed float data
	int x,y;

	if( T(IFB_SEGM_PRESENT) && T(IFB_VIEW_SEGM) ){

		view = *(Bimage*)&segm;
	}else{
		view = fimg;
	}

	if( view.height != 512 )
		view.zoom( 512.0 / view.height);
	for(y=0; y<view.height; y++) for(x=0; x<view.width; x++)
		view(x,y) = view(x,y) / 4 + 64;
}

extern "C" void update_view(Xv_opaque it){
	// from button press,... (not usefull, will not visine)
	if(me(it).c_image) me(it).c_image->update_view();
}

void Image::refresh(UserI& ui){
	// blast view image and drawings to canvas
	// so as we can take a looksee ...

	ui.refresh(view, T(IFB_HIDE_CONTOUR));
}

extern "C" void refresh_view(Xv_opaque it){
	// called by button, and repaint events

	if(me(it).c_image)
		// me(it).interpret("refresh");
		me(it).interpret("(refresh)");
}

void XView::refresh(Uimage& view, int f){

	// ready phasors
	ximage->data = (char*)view;
	ximage->width = ximage->bytes_per_line = view.width;
	ximage->height = view.height;
	XPutImage(dpy, canvas_xid, gc_img, ximage,
		0,0,0,0, view.width, view.height);

	if (!f)
		XCopyArea(dpy, pdrawing, canvas_xid, gc_draw,
			0,0, view.width, view.height, 0,0 );
	XFlush( dpy );
}

extern "C" void view_pal(Xv_opaque it){
	// put the cmap (LUT) in the top of the canvas
	XView& m = me(it);
	
	int i,j;
	u_char *s, img[512*512];

	s = img;
	for(i=0; i<512; i++) {
            for(j=0; j<512; j++) {
		*s= j>>1;
		s++;
            }
        }
	m.ximage->data = (char*)img;
	m.ximage->width = m.ximage->bytes_per_line = 512;
	XPutImage(m.dpy, m.canvas_xid, m.gc_img, m.ximage,
		0,0,0,0,512,10);
	// silly rabbit, trix are for kids
}


void XView::set_contour(Bimage& d){

	Uimage foo(512, 512);
	foo = d;
	foo.save("=d/bar.ras");
	xdrawing->width = xdrawing->bytes_per_line = 512;
	xdrawing->height = 512;
	xdrawing->data = (char*)foo;
	XPutImage(dpy, pdrawing, gc_img, xdrawing,
		0,0,0,0,512,512);
}
	
extern "C" void visine(Xv_opaque it){
	// clear out contour drawing, called from button

	me(it).interpret("(visine)");
	refresh_view(it);		// redraw screen
}

void Image::visine(void){
	int i;

	if(! this) return;
	drawing = 0;
	drawing2 = 0;
}


void XView::visine(Bimage *img){
	GC gc;
	XGCValues values;
	
	values.foreground = 0;
	values.background = 0;
	gc = XCreateGC(dpy, pdrawing, GCForeground|GCBackground, &values);

	// clear out contours
	XFillRectangle(dpy, pdrawing, gc, 0, 0, 1024, 1024);
	
	XFreeGC(dpy, gc);

	if( img)
		img->operator=( 0.0 );
	else
		c_image->visine();
}

void NoWin::visine(Bimage* img){
	int i;

	drawing = 0;
	
	if( img)
		img->operator=( 0.0 );
	else
		c_image->visine();
}

