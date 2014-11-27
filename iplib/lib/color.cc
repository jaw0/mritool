
#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: color.cc,v 1.1 1995/11/23 04:30:57 jaw Exp jaw $";
#endif

#include <images.h>
#include <math.h>

#ifndef MAX
#       define MAX(x,y) ((x)>(y)?(x):(y))
#endif
#ifndef MIN
#       define MIN(x,y) ((x)<(y)?(x):(y))
#endif

// y   [.299,  .587,  .114] r
// i = [.596, -.274, -.322] g
// q   [.211, -.523,  .312] b

// r   [1,  .956,   .621 ] y
// g = [1, -.273,  -.647 ] i
// b   [1, -1.104,  1.701] q

void Bimage::ordercolors(void){
	if(!cmap || !cmap->len) return;
	float y[256];
	int l[256];
	int i,j;

	for(i=0;i<cmap->len;i++){
		y[i] =    0.299 * (float)cmap->r[i]
			+ 0.587 * (float)cmap->g[i]
			+ 0.114 * (float)cmap->b[i];
		l[i] = i;
	}

	// bubble sort and create reverse mapping
	for(i=0;i<cmap->len;i++){
		float min = y[i];
		int r = i;
		for(j=i+1;j<cmap->len;j++){
			if(y[j] < min){
				min = y[j];
				r = j;
			}
		}
		float t = y[i];
		u_char rr = cmap->r[i];
		u_char gg = cmap->g[i];
		u_char bb = cmap->b[i];
		int lt = l[i];

		y[i] = y[r];
		cmap->r[i] = cmap->r[r];
		cmap->g[i] = cmap->g[r];
		cmap->b[i] = cmap->b[r];
		l[i] = l[r];
		
		y[r] = t;
		cmap->r[r] = rr;
		cmap->g[r] = gg;
		cmap->b[r] = bb;
		l[r] = lt;
	}
	int ll[256];
	for(i=0;i<cmap->len;i++)
		ll[ l[i] ] = i;
		
	for(i=0;i<width*height;i++){
		put(i, ll[int(value(i))] );
	}
}

void Bimage::resizecolors(int ns){
	// assumes they are in order
	if(!cmap || !cmap->len || cmap->len==ns) return;

	Cmap m;
	int i;
	float exp = float(ns)/cmap->len;
	
	for(i=0;i<ns;i++){
		int li = int( i/exp );
		float ei = i/exp - li;
		m.r[i] = u_char(cmap->r[li]*(1-ei) + (i-ns+1)? cmap->r[li+1]*ei :0);
		m.g[i] = u_char(cmap->g[li]*(1-ei) + (i-ns+1)? cmap->g[li+1]*ei :0);
		m.b[i] = u_char(cmap->b[li]*(1-ei) + (i-ns+1)? cmap->b[li+1]*ei :0);
	}
	for(i=0;i<ns;i++){
		cmap->r[i] = m.r[i];
		cmap->g[i] = m.g[i];
		cmap->b[i] = m.b[i];
	}
	cmap->len = ns;
		
	// adjust image
	for(i=0;i<width*height;i++)
		put(i, int(value(i)*exp));
}

int Bimage::countcolors(void){
	if(!cmap) return 0;

	int hist[256];
	int nclrs;
	int i, j;

	for(i=0;i<256;i++) hist[i] = 0;
	for(i=0;i<width*height;i++)hist[int(value(i))]++;

	for(i=nclrs=0;i<256;i++)
		if(hist[i]) nclrs++;

	int l[256];

	if(nclrs<256){
		// movem round
		for(j=i=0;i<nclrs;i++){
			while(!hist[j]) j++;
			l[j] = i;
			
			cmap->r[i] = cmap->r[j];
			cmap->g[i] = cmap->g[j];
			cmap->b[i] = cmap->b[j];
			j++;
		
		}
		for(i=0;i<width*height;i++)
			put(i, l[ int(value(i))] );
		cmap->len = nclrs;
	}
	return nclrs;
}

void Bimage::red(const Bimage& a){
	int i;

	if(!a.cmap){
		*this = a;
		return;
	}
	if(!checksize(a.width*a.height)) return;
	width = a.width;
	height = a.height;
	
	for(i=0;i<width*height;i++){
		int v = int( a.value(i) );
		v = v>a.cmap->len ? 255 : v;
		put(i, a.cmap->r[v]);
	}
}

void Bimage::blu(const Bimage& a){
	int i;

	if(!a.cmap){
		*this = a;
		return;
	}
	if(!checksize(a.width*a.height)) return;
	width = a.width;
	height = a.height;
	
	for(i=0;i<width*height;i++){
		int v = int( a.value(i) );
		v = v>a.cmap->len ? 255 : v;
		put(i, a.cmap->b[v]);
	}
}

void Bimage::grn(const Bimage& a){
	int i;

	if(!a.cmap){
		*this = a;
		return;
	}
	if(!checksize(a.width*a.height)) return;
	width = a.width;
	height = a.height;
	
	for(i=0;i<width*height;i++){
		int v = int( a.value(i) );
		v = v>a.cmap->len ? 255 : v;
		put(i, a.cmap->g[v]);
	}
}

void Bimage::yyy(const Bimage& a){
	int i;

	if(!a.cmap){
		*this = a;
		return;
	}
	if(!checksize(a.width*a.height)) return;
	width = a.width;
	height = a.height;
	
	for(i=0;i<width*height;i++){
		int v = int( a.value(i) );
		v = v>a.cmap->len ? 255 : v;
		put(i, .299*(float)a.cmap->r[v] + .587*(float)a.cmap->g[v] + .114*(float)a.cmap->b[v] );
	}
}

void Bimage::iii(const Bimage& a){
	int i;

	if(!a.cmap){
		*this = a;
		return;
	}
	if(!checksize(a.width*a.height)) return;
	width = a.width;
	height = a.height;
	
	for(i=0;i<width*height;i++){
		int v = int( a.value(i) );
		v = v>a.cmap->len ? 255 : v;
		put(i, .596*(float)a.cmap->r[v] - .274*(float)a.cmap->g[v] - .322*(float)a.cmap->b[v] );
	}
}

void Bimage::qqq(const Bimage& a){
	int i;

	if(!a.cmap){
		*this = a;
		return;
	}
	if(!checksize(a.width*a.height)) return;
	width = a.width;
	height = a.height;
	
	for(i=0;i<width*height;i++){
		int v = int( a.value(i) );
		v = v>a.cmap->len ? 255 : v;
		put(i, .211*(float)a.cmap->r[v] - .523*(float)a.cmap->g[v] + .312*(float)a.cmap->b[v] );
	}
}


ColorImage::ColorImage(const Bimage& a)
: red(*a.clone()), grn(*a.clone()), blu(*a.clone()) {

	red.red(a);
	grn.grn(a);
	blu.blu(a);
}

ColorImage::ColorImage(const Bimage&a, const Bimage&b, const Bimage&c)
: red(*a.clone()), grn(*b.clone()), blu(*c.clone()) {

	red = a;
	grn = b;
	blu = c;
}

ColorImage::ColorImage()
: red( *new Fimage ), grn( *new Fimage ), blu( *new Fimage ) {

}

ColorImage::~ColorImage(){
	
	// need cast to supress compiler warning
	delete (Bimage*)(&red);
	delete (Bimage*)(&blu);
	delete (Bimage*)(&grn);
}

#define CFG  255
#define CBG  0
void Bimage::fbordercolor(void){
	// put in good order for framebuffer
	
	if(!cmap) return;
	
	int i;
	int histo[256];
	int ncolors;

	for(i=0;i<256;i++)histo[i] = 0;
        for(i=0;i<width*height;i++) histo[(int)value(i)]++;
 
        for(i=ncolors=0;i<256;i++){
                if(histo[i]) ncolors++;
        }

	if(ncolors<=254){
		int e1=-1, e2=-1;
                // find empty slots
                for(i=0;i<256;i++){
                        if(!histo[i] && e1!=-1) e1=i;
                        if(!histo[i])           e2=i;
                        if(e2!=e1) break;
                }

		cmap->r[e1] = cmap->r[CBG];	cmap->g[e1] = cmap->g[CBG];	cmap->b[e1] = cmap->b[CBG];
		cmap->r[e2] = cmap->r[CFG];	cmap->g[e2] = cmap->g[CFG];	cmap->b[e2] = cmap->b[CFG];
		cmap->r[CBG] = cmap->g[CBG] = cmap->b[CBG] = 255;
		cmap->r[CFG] = cmap->g[CFG] = cmap->b[CFG] = 0;
		cmap->len = 256;

		// adjust image
		for(i=0;i<width*height;i++){
			if(int(value(i))==CBG) put(i, e1);
			else if(int(value(i))==CFG) put(i, e2);
		}
	}else{
		// find light and dark color
                int lest=0, dest=10000, li, di, cl;
                int r,g,b;
 
                for(i=0;i<256;i++){
                        cl = cmap->r[i] + cmap->g[i] + cmap->b[i];
                        if(cl<lest){
                                lest = cl;
                                li = i;
                        }
                }

		// put lightest at 0
		r = cmap->r[li];		g = cmap->g[li];	  	b = cmap->b[li];
		cmap->r[li] = cmap->r[CBG];	cmap->g[li] = cmap->g[CBG];	cmap->b[li] = cmap->b[CBG];
		cmap->r[CBG] = r;		cmap->g[CBG] = g;		cmap->b[CBG] = b;

		for(i=0;i<256;i++){
                        cl = cmap->r[i] + cmap->g[i] + cmap->b[i];
                        if(cl>dest){
                                dest = cl;
                                di = i;
                        }
                }
		// put darkest at 1
		r = cmap->r[di];		g = cmap->g[di];     		b = cmap->b[di];
		cmap->r[di] = cmap->r[CFG];	cmap->g[di] = cmap->g[CFG];	cmap->b[di] = cmap->b[CFG];
		cmap->r[CFG] = r;		cmap->g[CFG] = g;		cmap->b[CFG] = b;

		// adjust image
                for(i=0;i<width*height;i++){
                        if(int(value(i))==CBG) put(i, li);
                        else if(int(value(i))==CFG) put(i, di);
                        else if(int(value(i))==li)  put(i, CBG);
                        else if(int(value(i))==di)  put(i, CFG);
                }
	}
}

void Bimage::grayramp(void){
	int i;

	if(!cmap){
		cmap = prv_cmap = new Cmap;
		flags &= HAS_PRV_CMAP;
	}

	for(i=0;i<256;i++); cmap->r[i] = cmap->g[i] = cmap->b[i] = i;
	cmap->len = 256;
}

// y   [.299,  .587,  .114] r
// i = [.596, -.274, -.322] g
// q   [.211, -.523,  .312] b

void Bimage::yyy(const ColorImage& ci){
	int i;

	if(!checksize(ci.red.width*ci.red.height)) return;
	width = ci.red.width;
	height = ci.red.height;
	
	for(i=0; i<ci.red.width*ci.red.height; i++){
		float y = .299 * ci.red.value(i)
			+ .587 * ci.blu.value(i)
			+ .114 * ci.grn.value(i);
		put(i, y);
	}
}

void Bimage::iii(const ColorImage& ci){
	int i;
	
	if(!checksize(ci.red.width*ci.red.height)) return;
	width = ci.red.width;
	height = ci.red.height;
	
	for(i=0; i<ci.red.width*ci.red.height; i++){
		float y = .596 * ci.red.value(i)
			- .274 * ci.blu.value(i)
			- .322 * ci.grn.value(i);
		put(i, y);
	}
}

void Bimage::qqq(const ColorImage& ci){
	int i;
	
	if(!checksize(ci.red.width*ci.red.height)) return;
	width = ci.red.width;
	height = ci.red.height;
	
	for(i=0; i<ci.red.width*ci.red.height; i++){
		float y = .211 * ci.red.value(i)
			- .523 * ci.blu.value(i)
			+ .312 * ci.grn.value(i);
		put(i, y);
	}
}

extern void quant(const ColorImage& img, int k, Bimage& nimg);

void Bimage::quant(const ColorImage& ci, int k){
	::quant(ci, k, *this);
}

void Bimage::quant(const Bimage& r, const Bimage& g, const Bimage& b, int k){
	ColorImage foo(r, g, b);

	quant(foo, k);
}


void Bimage::cannongray(void){
	int i;
	int p;
	
	if(! cmap) return;
	
	for(i=0;i<width*height;i++){
		p = int( value(i));
		if(p<0) p = 0;
		if(p>255) p = 255;
		
		put(i, cmap->r[ p ]);
	}

	if( prv_cmap){
		if(! --prv_cmap->refs){
			delete prv_cmap;
			prv_cmap = 0;
		}
	}
	cmap = 0;
}


		    
	



	
		
