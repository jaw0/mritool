 
// Copyright (c) 1994 Jeff Weisberg
// University of Rochester
//      Dept. of Electrical Engineering
//      Dept. of Neurology, C.E.P.
//
// see the file "License"
 
#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: cli-ps.cc,v 1.1 1995/11/23 04:30:10 jaw Exp jaw $";
#endif
 
#include <jlisp.h>
#include <jl_proto.h>
#include <defproto.h>
 
#include <mritool.h>
#include <solid.h>
#include <handy.h>
#include <mrilisp.h>

#include <fstream.h>
#include <iomanip.h>

#define CODE_SECTION SECTION_LISP
#define NUMBERP(x)      (INUMP(x) || FLOATP(x) || DOUBLEP(x) || BIGNUMP(x))



DEFUN("img:ps", Fimg_ps, Simg_ps, 5,6,1,0,
      "(img:ps img file width height border extras) convert img to postscript",
      (Obj img, Obj file, Obj wi, Obj hi, Obj border, Obj extra))
{
	
	if(! IMAGEP(img))
		return jlerror(Simg_ps.name, img, "WTA: imagep");
	if(! STRINGP(file))
		return jlerror(Simg_ps.name, file, "WTA:stringp");
	if(! NUMBERP(wi))
		return jlerror(Simg_ps.name, wi, "WTA: numberp");
	if(! NUMBERP(hi))
		return jlerror(Simg_ps.name, hi, "WTA: numberp");
	if(! NUMBERP(border))
		return jlerror(Simg_ps.name, border, "WTA: numberp");

	ofstream of( CCHARS(file));
	if(! of)
		return IC_FALSE;

	int xd=CIMAGE(img)->width;
	int yd=CIMAGE(img)->height;
	float w = fltof(wi);
	float h = fltof(hi);
	float b = fltof(border);
	float aspecti = float(yd) / xd;
	float aspectp = float(h) / w;
	float bx, by, scx, scy;

	if( aspecti > aspectp){
		bx  = (w - (h-2*b)/aspecti)/2;
		by  = b;
		scx = (h-2*b)/aspecti;
		scy = h-2*b;
	}else{
		bx  = b;
		by  = (h - (w-2*b)*aspecti)/2;
		scx = w-2*b;
		scy = (w-2*b)*aspecti;
	}
	
	// spew forth postscript (pun intended)
	
	of << "%!PS-Adobe-3.0 EPSF-3.0" << endl;
	of << "%%Creator: mritool img:ps" << endl;
	of << "%%Title: Pur-tee Picture" << endl;
	of << "%%Author: Jeff Weisberg" << endl;
	of << "%%Notice: " << version << endl;
	of << "%%BoundingBox: 0 0 " << w * 72 << " " << h * 72 << endl;
	of << "%%DocumentNeededResources: font TimesRoman" << endl;
	of << "%%EndComments" << endl;
	of << "" << endl;
	of << "%%BeginProlog" << endl;
	of << "64 dict begin" << endl;
	of << "/gamma 1.5 def % adjust for best results (will depend on printer)" << endl;
	of << "/scp true def" << endl;
	of << "/picstr  1 string def" << endl;
	of << "/cpicstr 3 string def" << endl;
	of << "/v 0 def" << endl;
	of << "/cr 256 string def" << endl;
	of << "/cg 256 string def" << endl;
	of << "/cb 256 string def" << endl;
	of << "/in { 72 mul } def" << endl;
	of << "/xoff " << bx << " in def" << endl;
	of << "/yoff " << by << " in def" << endl;
	of << "/xdim " << xd << " def" << endl;
	of << "/ydim " << yd << " def" << endl;
	of << "/scx " << scx << " in def" << endl;
	of << "/scy " << scy << " in def" << endl;
	of << "/height " << h << " in def" << endl;
	of << "/width "  << w << " in def" << endl;
	of << "/xpix { xdim div scx mul xoff add } bind def" << endl;
	of << "/ypix { ydim div scy mul yoff add } bind def" << endl;
	// sets font to times roman in specified size
	// ex: 12 tr (Foobar) show
	of << "/tr { /Times-Roman findfont exch scalefont setfont } bind def" << endl;
	// make a pointer -- line with a circle on end first point has the circle
	// ex: 100 100 260 150 ptr
	of << "/ptr {\n\tgsave 3 setlinewidth moveto 2 copy lineto gsave 1 setgray stroke grestore" << endl;
	of << "\t1 setlinewidth stroke 2 0 360 arc fill grestore\n} bind def" << endl;

	of << "scp {" << endl;
	of << "\t0 1 255 { dup 127 gt {\n\t\tcr exch dup 383 exch sub put" << endl;
	of << "\t}{\tcr exch dup 2 mul 255 div 1.0 gamma div exp 255 mul floor cvi put" << endl;
	of << "\t} ifelse } for" << endl;
	of << "\t0 1 255 { dup 127 gt {\n\t\tcg exch 0 put" << endl;
	of << "\t}{\tcg exch dup 2 mul 255 div 1.0 gamma div exp 255 mul floor cvi put" << endl;
	of << "\t} ifelse } for" << endl;
	of << "\t0 1 255 { dup 127 gt {\n\t\tcb exch 0 put" << endl;
	of << "\t}{\tcb exch dup 2 mul 255 div 1.0 gamma div exp 255 mul floor cvi put" << endl;
	of << "\t} ifelse } for" << endl;
	of << "} {" << endl;
	of << "\t0 1 255 { cr exch dup 255 div 1.0 gamma div exp 255 mul floor cvi put } for" << endl;
	of << "\t0 1 255 { cb exch dup 255 div 1.0 gamma div exp 255 mul floor cvi put } for" << endl;
	of << "\t0 1 255 { cg exch dup 255 div 1.0 gamma div exp 255 mul floor cvi put } for" << endl;
	of << "} ifelse" << endl;
	
	// xform pixel values (gamma correction), color lookup, ...
	of << "/xform {" << endl;
	of << "\t% gamma correct" << endl;
	of << "\tpop" << endl;
	of << "\t0 1 picstr length 1 sub {" << endl;
	of << "\t\t/v exch picstr exch get" << endl;
	of << "\t\tstore" << endl;
	of << "\t} for" << endl;
	of << "\tcpicstr 0 cr v get put" << endl;
	of << "\tcpicstr 1 cg v get put" << endl;
	of << "\tcpicstr 2 cb v get put" << endl;
	of << "\tcpicstr" << endl;
	of << "} bind def" << endl;
	of << "%%EndProlog" << endl;
	of << "" << endl;
	of << "gsave" << endl;

	of << "xoff yoff translate" << endl;
	of << "scx scy scale" << endl;
	
	of << "xdim ydim 8" << endl;
	of << "[ xdim 0 0 ydim neg 0 ydim ]" << endl;
	of << "{ currentfile picstr readhexstring pop xform }" << endl;
	of << "false 3 colorimage" << endl;

	int x, y, c;

	for(y=0; y<yd; y++){
		for(x=c=0; x<xd; x++){
			if(c >= 40){
				of << "" << endl;
				c = 0;
			}
			c ++;
			of << setw(2) << setfill('0') << hex << (unsigned int) CIMAGE(img)->value(x,y);
		}
		of << endl;
	}

	of << "" << endl;
	of << "grestore" << endl;
	of << "" << endl;
	of << "%%Trailers" << endl;
	of << "gsave" << endl;

	// do extras ...
	of << "% decorations" << endl;
	while( CONSP( extra )){
		if(! STRINGP( CAR(extra)))
			return jlerror(Simg_ps.name, CAR(extra), "WTA: stringp");
		of << CCHARS( CAR(extra)) << endl;
		extra = CDR(extra);
	}

	// and trailers...
	of << "grestore" << endl;
	of << "showpage" << endl;
	of << "end" << endl;
	of << "" << endl;
	of << "%%EOF" << endl;
	
	return IC_TRUE;
}


		

		
	
	



