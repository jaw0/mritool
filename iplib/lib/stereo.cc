
// Copyright (c) 1993 Jeff Weisberg
 
#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: stereo.cc,v 1.1 1995/11/23 04:31:07 jaw Exp jaw $";
#endif
 
#include <images.h>
#include <math.h>
 
 
// from IEEE Computer Oct. 1994
// "Displaying 3D Images: Algorithms for Single Image Random Dot Stereograms"
// Thimbley, Inglis, Witten


#define round(x)	int( (x) + 0.5)
#define DPI		72
#define E		round( 2.5 * DPI )
#define mu		(1/3.0)
#define separation(z)	round( (1-mu*z)*E / (2-mu*z))
#define far		separation(0)


void Bimage::stereo(void){
	// image ought be 0..255
	
	Bimage *foo = clone();
	int x, y;
	int *pix = new int[ width ];
	int *same= new int[ width ];
	float v;
	
	for(y=0; y<height; y++){
		int s, left, right;

		for(x=0; x<width; x++)
			same[x] = x;

		for(x=0; x<width; x++){
			v = value(x, y) / 255.0;
			s = separation( v );
			left = x - (s + (s & y & 1)) / 2;
			right = left + s;

			if( 0 <= left && right < width ){
				int visible;
				int t = 1;
				float zt;

				do{
					zt = v + 2*(2 - mu*v)*t/(mu*E);
					visible = value(x-t, y)/255.0 < zt && value(x+t, y)/255.0 < zt;
					t++;
				}while( visible && zt < 1);

				if( visible ){
					int k;
					for(k=same[left]; k!= left && k!= right; k= same[left])
						if( k < right ){
							left = k;
						}else{
							left = right;
							right = k;
						}
					same[left] = right;
				}
			}
		}
		for(x=width-1; x>=0; x--){
			if( same[x] == x)
				pix[x] = random() & 1;
			else
				pix[x] = pix[same[x]];
			foo->put(x, y, pix[x]? 255 : 0);
		}
	}

	circle(Point(width/2 - far/2, height*19/20), 2, 255);
	circle(Point(width/2 + far/2, height*19/20), 2, 255);

	swdat(foo);
	delete foo;
	delete [] pix;
	delete [] same;
}


	
	       

		


