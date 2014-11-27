

// Copyright (c) 1994 Jeff Weisberg
 
#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: hough.cc,v 1.1 1995/11/23 04:31:03 jaw Exp jaw $";
#endif
 
#include <images.h>
#include <math.h>
#include <handy.h>
 
 
void Bimage::hough(int how){
        int x, y;
	int r, th;
	float val, theta, rad, ka, km;
        Bimage *foo = clone();

	*foo = 0;
	ka = height / 2.0;
	km = float(height) / (width + height) / 2.0 / M_SQRT2;
	
        for(y=0;y<height;y++) for(x=0;x<width;x++){
		val = value(x, y);

		if( int(val) || how )

			for( th=0; th<width; th++){
				theta = th * M_PI / width - M_PI/2.0;
				rad = x * cos( theta ) + y * sin( theta );
				r = int( rad * km + ka );

				foo->put(th, r, foo->value(th,r) + 
					(how ? val : 1));
			}
	}
	swdat( foo );
	delete foo;
}


