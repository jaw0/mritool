

// Copyright (c) 1993 Jeff Weisberg
// University of Rochester
//	Dept. of Electrical Engineering
//	Dept. of Neurology, C.E.P.
//
// see the file "License"

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: ip.cc,v 1.1 1995/11/23 04:31:04 jaw Exp jaw $";
#endif

#include <images.h>
#include <math.h>
#include <handy.h>

void Bimage::edge(float val){
	/* Sobel */
	
	int x,y;
	float h1,h2;
	float S;
	
	Bimage *foo = clone();
	
	for(y=1;y<height-1;y++) for(x=1;x<width-1;x++){
		h1 =      2* nvalue( x+1, y)
			- 2* nvalue( x-1, y)
			+    nvalue( x+1, y+1)
			+    nvalue( x+1, y-1)
			-    nvalue( x-1, y-1)
			-    nvalue( x-1, y+1);
		
		h2 =      2* nvalue( x,   y-1)
			- 2* nvalue( x,   y+1)
			+    nvalue( x+1, y-1)
			+    nvalue( x-1, y-1)
			-    nvalue( x+1, y+1)
			-    nvalue( x-1, y+1);
		
		S = nvalue( x, y ) * sqrt( (double)(h1*h1 + h2*h2) ) / 100.0;

		foo->nput(x, y, ((S >= val)?0xFF:0x00) );
		
	}
	swdat(foo);
	delete foo;
}

void Bimage::thresh(float t){
	int x, y;
	
	for(y=0;y<height;y++) for(x=0;x<width;x++){

		nput(x, y, (nvalue(x, y) >= t ? 0xFF : 0));
	}
}

void Bimage::dxform(void){
	// image ought be binary
	
	Bimage *nimg = clone();
	int x,y, c;
	int movedp;

	*nimg = *this;
	
	for(y=0;y<height;y++)for(x=0;x<width;x++)
		put(x,y, int(value(x,y)) ? 1 : 0);
	
	do{
		movedp = 0;
		for(y=1;y<height-1;y++)for(x=1;x<width-1;x++){
			c = int(value(x, y));
			if( c )
				    nimg->put(Point(x,y), c+1);
			else if(   (int)value(Point(x-1, y))
				|| (int)value(Point(x-1, y+1))
				|| (int)value(Point(x-1, y-1))
				|| (int)value(Point(x+1, y))
				|| (int)value(Point(x+1, y+1))
				|| (int)value(Point(x+1, y-1))
				|| (int)value(Point(x,   y+1))
				|| (int)value(Point(x,   y-1))){

				nimg->put(Point(x,y), 1);
				movedp = 1;
			}else
				nimg->put(Point(x,y), 0);
		}
		/* unroll */
		for(y=1;y<height-1;y++)for(x=1;x<width-1;x++){
			c = int( nimg->value(x, y));
			if( c )
				put(Point(x,y), c+1);
			else if(   (int)nimg->value(Point(x-1, y))
				|| (int)nimg->value(Point(x-1, y+1))
				|| (int)nimg->value(Point(x-1, y-1))
				|| (int)nimg->value(Point(x+1, y))
				|| (int)nimg->value(Point(x+1, y+1))
				|| (int)nimg->value(Point(x+1, y-1))
				|| (int)nimg->value(Point(x,   y+1))
				|| (int)nimg->value(Point(x,   y-1))){

				put(Point(x,y), 1);
				movedp = 1;
			}else
				put(Point(x,y), 0);
		}
	}while( movedp );

	delete nimg;
}
		
void Bimage::blur_nl(int ws, float konst){
	// image ought be binary
	
	Bimage *nimg = clone();
	int x,y, d, c;
	float dist, closest, v, hdist;
	
	for(y=0;y<height;y++)for(x=0;x<width;x++){
		closest = dist = 1e20;

		for(d=0;d<MAX(width,height);d++){
			for(c=-d;c<=d;c++){

				if(INRANGE(0,c+x,width)
				   && INRANGE(0, y-d, height)
				   && (nvalue(x+c, y-d)>=1)){
					dist = c*c + d*d;
					if(dist<closest) closest = dist;
				}
				if(INRANGE(0,c+x,width)
				   && INRANGE(0, y+d,height)
				   && (nvalue(x+c, y+d)>=1)){
					dist = c*c + d*d;
					if(dist<closest) closest = dist;
				}
				if(INRANGE(0,x-d,width)
				   && INRANGE(0, y+c,height)
				   && (nvalue(x-d, y+c)>=1)){
					dist = c*c + d*d;
					if(dist<closest) closest = dist;
				}
				if(INRANGE(0,x+d,width)
				   && INRANGE(0, y+c,height)
				   && (nvalue(x+d, y+c)>=1)){
					dist = c*c + d*d;
					if(dist<closest) closest = dist;
				}
			}
			hdist = c*c;
			if(closest < hdist)
				// there can be nothing closer
				break;
		}
		
		v = exp(-closest*konst/(ws*ws));	// gaussify it
		nimg->nput(x, y, 255.9 * v);
	}

	swdat(nimg);
	delete nimg;
}

void Bimage::blur(int ws, float konst){
	int i,j, x,y;
	float tot, r2;
	float k, vol=0;

	Bimage *nimg = clone();
	Fimage weight(ws+2, ws+2);
	weight.offset = -(ws/2+1);

	for(j=-ws/2;j<ws/2;j++) for(i=-ws/2;i<ws/2;i++){
		Point p(i,j);
		r2 = (i*i+j*j);
		k = exp( - konst*r2/(ws*ws) );	// try a gaussian blur
		if(!j)vol += k;	
		weight[ p ] = k;
	}
	
	for(j=0;j<ws*ws;j++) weight[j] /= vol;
	
	for(y=0;y<height;y++) for(x=0;x<width;x++){
		tot=0;
		for(j=-ws/2;j<ws/2;j++) for(i=-ws/2;i<ws/2;i++){
			Point q(i,j);
			if( INRANGE(0,x+i,width) && INRANGE(0,y+j,height))
				tot += weight[q]
					* nvalue( x+i, y+j );
					
		}
		nimg->nput(x,y, tot);
	}

	swdat(nimg);
	delete nimg;
}


int Bimage::within(Point p) const {

	return (p.x>=offset.x)
		&& (p.y>=offset.y)
		&& (p.x<offset.x+width)
		&& (p.y<offset.y+height);
}
	
int Bimage::nwithin(Point p) const {

	return (p.x>=0)
		&& (p.y>=0)
		&& (p.x<width)
		&& (p.y<height);
}
		
void Bimage::invert(void){
	int i;
	float v = cmap?cmap->len-1:255;

	for(i=0;i<width*height;i++)
		put(i, v-value(i));
}

int Bimage::fill(Point p, float v){
	int i, ar=0;

	if(!within(p))
		return 0;

	i = int(value(p));
	if(i)
		return 1;
	put(p, v);

	ar =      fill(p+Point(0,-1), v)
		+ fill(p+Point(0, 1), v)
		+ fill(p+Point(-1,0), v)
		+ fill(p+Point(1, 0), v)
		;
	
	return ar;
}

int Bimage::area(void) const {
	int i, a=0;

	for(i=0;i<width*height;i++){
		if( int(value(i))) a++;
	}
	return a;
}

void Bimage::median(int sz){
	int x,y, i,j;
	int num[256];
	Bimage *img = clone();

	*img = 0.0;
	for(y=sz/2;y<height-sz/2;y++)for(x=sz/2;x<width-sz/2;x++){
		for(i=0;i<256;i++) num[i] = 0;
		for(j=-sz/2;j<=sz/2;j++) for(i=-sz/2;i<=sz/2;i++){
			num[ (int)nvalue( x+i , y+j ) ]++;
		}
		i=j=0;
		while(i<sz*sz/2) i+= num[j++];
		img->nput(x,y, --j);
	}
	swdat(img);
	delete img;
}


void Bimage::dilate(int sz, Support supp){
	int x,y,i,j;
	Bimage *img = clone();
	*img = 0.0;
		
	for(y=sz/2;y<height-sz/2;y++)for(x=sz/2;x<width-sz/2;x++){
		if( int(nvalue( x, y ))){
			switch( supp ){
			  case Square:
				for(j=-sz/2;j<=sz/2;j++)for(i=-sz/2;i<=sz/2;i++)
					img->nput(x+i, y+j, u_char( nvalue(x, y) ) );
				break;
			  case Round:
				for(j=-sz/2;j<=sz/2;j++)for(i=-sz/2;i<=sz/2;i++)
					if( ABS(i*j) <= sz*sz )
						img->nput(x+i, y+j, u_char( nvalue(x, y) ) );
				break;
			}
		}
	}
	swdat(img);
	delete img;
}

void Bimage::quantize(int levels){
	// quantize into levels levels
	int bar, i;
	float mn, mx, k;

	if(!levels)levels = 256;
	mn = minval();
	mx = maxval();
	
	k = levels/(mx-mn);
	for(i=0;i<width*height;i++){
		bar = int((value(i)-mn)*k);
		put(i, bar);
	}
}

void Bimage::equalize(void){
	// histgram equalize
	int hist[256], cum[256];
	int i;
	float v = 255.0/(width * height);	// assume range 0..255
	
	for(i=0;i<256;i++) hist[i] = 0;
	for(i=0;i<width*height;i++)
		hist[ int(value(i)) ]++;
	cum[0] = hist[0];
	for(i=1;i<256;i++) cum[i]= cum[i-1]+hist[i];
	for(i=0;i<width*height;i++)
		put(i, float(v * cum[int(value(i))]));
}

void Bimage::zoom(float f){
	int x,y;
	int nw = int( f * width ), nh = int( f * height );

	if(!width || !height) return;
	if (!checksize(nw * nh)) return;
	
	if(f>1.0){
		for(y=nh-1;y>=0;y--) for(x=nw-1;x>=0;x--){
			put( y*nw+x, nvalue(int(x/f), int(y/f)) );
		}
	}else{
		for(y=0;y<height;y++) for(x=0;x<width;x++){
			put( int(x*f)+int(y*f)*nw, value(x,y) );
		}
	}
	height = nh;
	width  = nw;
}

void Bimage::interp(float xz, float yz){
	int x, y;
	int nw = int( xz * width ), nh = int( yz * height );
	float v;

	if(! nw) nw ++;
	if(! nh) nh ++;
	Bimage *foo = clone();
	
	int ow = width, oh = height;
	foo->height = nh;
	foo->width = nw;
	if(!foo->resize()) return;

	for(y=0;y<nh;y++) for(x=0;x<nw;x++){
		int jl = int(float(x)/xz);
		int il = int(float(y)/yz);
		float tx = x/xz - jl;
		float ty = y/yz - il;
		int   ti = jl + il * ow;
		if(jl<0 || il<0 || jl>=ow-1 || il>=oh-1)
			v = 0;
		else
			v =   value(ti)      * (1-tx) * (1-ty)
			    + value(ti+1)    * tx     * (1-ty)
			    + value(ti+ow)   * (1-tx) * ty
			    + value(ti+ow+1) * tx     * ty
			    ;
		foo->nput(x,y, v);
	}
	height = nh;
	width  = nw;
	resize();
	swdat(foo);
	delete foo;
}
		
	
void Bimage::rotate(float ang){

	if(!width || !height) return;
	ang -= floor(ang/360) * 360;
	if(ang==0.0) return;
	ang = 360 - ang;

	float sa=sin(ang*M_PI/180), ca=cos(ang*M_PI/180);

	Bimage *foo = clone();
	foo->width = (int)floor(MAX(fabs(width*ca-height*sa),fabs(width*ca+height*sa)));
	foo->height = (int)floor(MAX(fabs(width*sa+height*ca),fabs(width*sa-height*ca)));
	foo->resize();

	int ti2 = 0;
	for(int i=0;i<foo->height; i++){
		float ry = i - foo->height/2;
		for(int j=0;j<foo->width;j++,ti2++){

			float rx = j - foo->width/2;
			float x = rx * ca - ry * sa + width/2;
			float y = rx * sa + ry * ca + height/2;
			if(x<0 || x>=width-1 || y<0 || y>=height-1)
				foo->put(ti2, 0);
			else{
				int jl = (int)floor(x);
				int il = (int)floor(y);
				float tx = x - jl;
				float ty = y - il;
				int ti = jl + il * width;
				foo->put(ti2, 
					value(ti)*(1-tx)*(1-ty)
					+ value(ti+1)*tx*(1-ty)
					+ value(ti+width)*(1-tx)*ty
					+ value(ti+width+1)*tx*ty
				        );
			}
		}
	}
	if(!checksize(foo->width*foo->height)) return;
	width = foo->width;
	height = foo->height;

	swdat(foo);
	delete foo;
}


void Bimage::overlay(Bimage&a, Point p){
	if(!a.width || !a.height) return;

	for(int y=0;y<a.height;y++) for(int x=0;x<a.width;x++){
		if(within(Point(p.x+x, p.y+y)))
			put(p.x+x, p.y+y,  a.nvalue(x,y));
	}
}

void Bimage::cut(Point a, Point b){

	Point aa( MIN(a.x, b.x), MIN(a.y, b.y) );
	Point bb( MAX(a.x, b.x), MAX(a.y, b.y) );

	if(aa.x==bb.x || aa.y==bb.y) return;
	Bimage *f = clone();
	f->width  = bb.x-aa.x;
	f->height = bb.y-aa.y;
	f->resize();
	
	for(int y=0;y<f->height;y++) for(int x=0;x<f->width;x++)
		f->nput(x,y, value(x+aa.x, y+aa.y));
	swdat(f);
	// offset += a;
	delete f;
}

void Bimage::gamma(float g){
	int i;
	float min=minval(), max=maxval();

	for(i=0;i<width*height;i++){
		put(i, min+max*pow( (value(i)-min)/(max-min), 1.0/g) );
	}
}

float Bimage::mean(void) const {
	int i;
	float x=0;
	
	for(i=0;i<height*width;i++)
		x += value(i);

	return x/(width*height);
}

float Bimage::variance(void) const {
	float a=mean();
	float x=0, y;
	int i;

	for(i=0;i<width*height;i++){
		y = value(i)-a;
		x += y*y;
	}
	return x/(width*height-1);
}

void Bimage::punch(float a, float b, float lo, float high){
	int i;
	float min, max, r, y;

	min = a; // MIN(a, b);
	max = b; // MAX(a, b);
	high -= .00001;
	r = (high-lo)/(max-min);

	for(i=0;i<width*height;i++){
		y = (value(i) - min) * r + lo;
		y = (y>high)?high:y;
		y = (y<lo)  ?lo:y;
		put(i, y);
	}
}

void Bimage::lapl(float alpha){
	int x,y;
        float a=alpha,b=1.0,L, v;
	Bimage *foo = clone();
	
	for (y=0;y<height;y++){
                for(x=0;x<width;x++){
                        /*for @ pt in image */
			
                        if ( x &&(x !=width-1)&&y&&(y !=height-1) ){

				L =     4 * nvalue(x, y)
					-   nvalue(x+1, y)
					-   nvalue(x-1, y)
					-   nvalue(x, y+1)
					-   nvalue(x, y-1);

				v = a * L + b * nvalue(x, y);
				if( v<0 ) v = 0;
				if( v>255) v = 255;
				foo->nput(x, y, v);
                        }else{
				foo->nput(x, y, 0);
			}
                }
        }
	swdat(foo);
	delete foo;
}

void Bimage::wipe(float val){

	*this = val;
}

	
float Bimage::cmp_cnts(const Bimage& test){
	Fimage ddd, iii;

	ddd = *this;
	iii = test;
	
	Bimage &dist = ddd;
	Bimage &img  = iii;
	float total = 0;
	float maxm;
	int cpts = 0, tpts = 0;
	int x,y;

	dist.pretty(0);
	img.pretty(0);

	dist.dxform();
	maxm = dist.maxval();

#define CUTOFF	2	/* only do top half */	
	for(y=0; y<height/CUTOFF; y++) for(x=0; x<width; x++){
		if( int( value(x,y))){
			cpts ++;
		}
		if( int( img.value(x, y))){
			total += maxm - dist.value(x,y);
			tpts ++;
		}
	}
#if 0
	/* adjust for different length contours */
	adjst = (tpts + fabs( tpts - cpts )) / tpts;
	return total / tpts * adjst;
#else
	return (total + fabs( tpts - cpts )) / tpts;
#endif	

}

void Bimage::convolve(const Bimage& b){
	int x,y, i,j;
	int bxl, bxr, byl, byr;
	complex v;
	Bimage *nimg = clone();
	
	bxl = (b.width  -1) / 2;
	byl = (b.height -1) / 2;
	bxr = b.width  - bxl - 1;
	byr = b.height - byl - 1;

	for(y=0;y<height;y++) for(x=0;x<width;x++){
		v = 0;
		for(j=MAX(-y,-byl);j<=MIN(byr,height-y-1);j++)
			for(i=MAX(-x,-bxl);i<=MIN(bxr,width-x);i++){
				v += cvalue(x,y) * b.cvalue(i+bxl, j+byl);
			}
		nimg->cput(x,y, v);
	}
	swdat(nimg);
	delete(nimg);
}


void Bimage::emboss(void){
	Fimage b(3,3);

	b(0,0) = -2; b(1,0) = -1; b(2,0) = 0;
	b(0,1) = -1; b(1,1) =  0; b(2,1) = 1;
	b(0,2) =  0; b(1,2) =  1; b(2,2) = 2;
	convolve(b);
}






