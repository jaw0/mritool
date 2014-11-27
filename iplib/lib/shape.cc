
#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: shape.cc,v 1.1 1995/11/23 04:31:06 jaw Exp jaw $";
#endif

#include <images.h>
#include <math.h>

void Bimage::circle(Point p, float r, float v){
	float r2 = r*r;
	int x,y;

	for(y=0;y<height;y++)for(x=0;x<width;x++){
		float rx = (x - p.x)*(x - p.x) + (y - p.y)*(y - p.y);
		if(rx < r2) put(x, y, v);
	}
}


	
