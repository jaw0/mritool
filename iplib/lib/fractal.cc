
// Copyright (c) 1993 Jeff Weisberg
 
#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: fractal.cc,v 1.1 1995/11/23 04:31:03 jaw Exp jaw $";
#endif
 
#include <images.h>
#include <math.h>
#include <stdlib.h>


static float nrand(){
        return (random() & 0xFFFFFFF) / 268435455.0;
}

void Bimage::fractal(float h){
	int x, y, xx, yy, i;
	float a, p, t;

	for(y=0; y<height; y++) for(x=0; x<width; x++){

		xx = x> width/2  ? width  - x : x;
		yy = y> height/2 ? height - y : y;
		
		t = 0;
		for(i=0; i<12; i++) t += random() - .5;
		
		a = t * exp( -(h+.5) * log( 2.0 + 2.0 * ( xx + yy )));
		p = nrand() * 2 * M_PI;

		cput(x, y, complex( a*cos( p ), a*sin( p )));

	}
	ifft();
}





