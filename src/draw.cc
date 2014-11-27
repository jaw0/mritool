

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: draw.cc,v 1.1 1995/11/23 04:30:14 jaw Exp jaw $";
#endif

// Optimization: Full

#include <mritool.h>
#include <window.h>
#include <X11/Xutil.h>
#include <jl_proto.h>
#include <defproto.h>

Obj erasor_sym = 0;

// put a dot on contour
void UserI::put_dot(Point p, int s, Bimage* img){

	if( img)
		img->put(p, s+1);
	else{
		c_image->drawing[p]  = 0x80;
		c_image->drawing2[p] = s + 1;
	}
}
void XView::put_dot(Point p, int s, Bimage *img){
	// draw a dot on the contour

	UserI::put_dot(p, s, img);

	XDrawPoint(dpy, pdrawing, gc_img, p.x, p.y);
	
	if( img || c_image && !c_image->T(Image::IFB_HIDE_CONTOUR) )
		XDrawPoint(dpy, canvas_xid, gc_draw,  p.x, p.y);
}

// put a line on the contour
void UserI::put_line(Point l, Point p, int s, Bimage *img){

	if(img)
		img->line(l, p, s+1);
	else{
		c_image->drawing.line( l,p, 0x80);
		c_image->drawing2.line(l,p, s + 1);
	}
}

void XView::put_line(Point l, Point p, int s, Bimage *img){
	// draw a line on the contour

	UserI::put_line(l, p, s, img);
	
	XDrawLine(dpy, pdrawing, gc_img, l.x, l.y, p.x, p.y);
	
	if( img || c_image && !c_image->T(Image::IFB_HIDE_CONTOUR) )
		XDrawLine(dpy, canvas_xid, gc_draw, l.x, l.y, p.x, p.y);
}

// erase a small block from contour
void UserI::clear_spot(Point p, Uimage *img){
	int i;
	if(! erasor_sym){
		erasor_sym = maksym("mri:erasor");
		internal_gc_protect = Fcons( erasor_sym, internal_gc_protect);
	}
	int es = CINT( getvalue( erasor_sym));
	Point a,b, inc(0,1), e1(es/2), e2(es, 0);

	
	for(i=0,a=p-e1;i<es;i++,a+=inc){
		b = a + e2;
		if(img)
			img->line(a, b, 0);
		else{
			c_image->drawing. line(a, b, 0);
			c_image->drawing2.line(a, b, 0);
		}
	}
}
void XView::clear_spot(Point p, Uimage *img){
	int x,y;
	if(! erasor_sym){
		erasor_sym = maksym("mri:erasor");
		internal_gc_protect = Fcons( erasor_sym, internal_gc_protect);
	}
	int es = CINT( getvalue( erasor_sym));
	Point e1(es/2), e2(es/2, 0);
	Point ps = p- e1;

	UserI::clear_spot(p, img);

	if( img){
		ximage->data = (char*)img;
		ximage->width = ximage->bytes_per_line = img->width;
		ximage->height = img->height;
	}else{
		ximage->data = (char*)( c_image->view );
		ximage->width = ximage->bytes_per_line = ximage->height = 512;
	}
	XPutImage(dpy, canvas_xid, gc_img, ximage,
		  ps.x, ps.y, ps.x, ps.y, es, es );
	
	for(y=p.y-es/2; y<p.y+es/2;y++)
		for(x=p.x-es/2; x<p.x+es/2;x++){
			XDrawPoint(dpy, pdrawing,   gc_erase, x, y);
			// XDrawPoint(dpy, canvas_xid, gc_erase, x, y);
		}
}

