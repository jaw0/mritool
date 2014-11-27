

// $Id: adjust_wl.cc,v 1.1 1995/11/23 04:30:12 jaw Exp jaw $

// Optimization: -O2

#include <mritool.h>
#include <window.h>

extern void footer(char *s);

int current_window;
int current_level;

void XView::adjust_wl(int w, int l){

	int a=l-w/2, b=l+w/2;
	int v, i;
	float r;

	
	a = (a>255)?255:(a<0)?0:a;
	b = (b>255)?255:(b<0)?0:b;

	if(a==b)return;

	current_window = w;
	current_level = l;
	
	r = 255.999/(b-a);


	for(i=64;i<128;i++){
		v = ((i-64)*(255.0/63)-a)*r;
		map[i].blue=map[i].green=map[i].red
			= (u_char) (v<0)?0:(v>255)?255:v;
	}

	// sprintf(foo, "W/L: %d %d", w, l);
	// footer(foo);
	
	xv_set(cms, CMS_COLORS, map, 0);
	xv_set(paint,  WIN_CMS, cms, 0);

}
	
	

	
