

// Copyright (c) 1994 Jeff Weisberg

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: edgemap.cc,v 1.1 1995/11/23 04:30:59 jaw Exp jaw $";
#endif
 
#include <images.h>
#include <math.h>
#include <handy.h>


static float sobel_x(const Bimage &img, int x, int y){
	
	return    2* img.nvalue( x+1, y)
		- 2* img.nvalue( x-1, y)
		+    img.nvalue( x+1, y+1)
		+    img.nvalue( x+1, y-1)
		-    img.nvalue( x-1, y-1)
		-    img.nvalue( x-1, y+1);
}

static float sobel_y(const Bimage &img, int x, int y){

	return    2* img.nvalue( x,   y-1)
		- 2* img.nvalue( x,   y+1)
		+    img.nvalue( x+1, y-1)
		+    img.nvalue( x-1, y-1)
		-    img.nvalue( x+1, y+1)
		-    img.nvalue( x-1, y+1);
}

	
void Bimage::edgemap(void){
	int x, y;
	float gx, gy;
        Bimage *foo = clone();

	for(y=2;y<height-2;y++) for(x=2;x<width-2;x++){
		// run over with pair of sobel operators

		gx = sobel_x(*this, x, y);
		gy = sobel_y(*this, x, y);

		if(   gx >  sobel_x(*this, x-1, y)
		   && gx >= sobel_x(*this, x+1, y)
		   || gy >  sobel_y(*this, x, y-1)
		   && gy >= sobel_y(*this, x, y+1)){

			foo->nput(x,y, sqrt( gx*gx + gy*gy ));
		}else{
			foo->nput(x,y, 0);
		}
	}
	swdat(foo);
	delete foo;
}

 

	
