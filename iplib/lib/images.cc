

// Copyright (c) 1993 Jeff Weisberg
// University of Rochester
//	Dept. of Electrical Engineering
//	Dept. of Neurology, C.E.P.
//
// see the file "License"

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: images.cc,v 1.1 1995/11/23 04:31:04 jaw Exp jaw $";
#endif

#include <images.h>
#include <fstream.h>
#include <rasterfile.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <fullname.h>
#include <vfork.h>

#define loop(i,j)	for(i=0;i<j;i++)
#ifndef MAX
#       define MAX(x,y) ((x)>(y)?(x):(y))
#endif
#ifndef MIN
#       define MIN(x,y) ((x)<(y)?(x):(y))
#endif
 

complex Bimage::cvalue(int i) const{
	return complex( value(i), 0);
}
void Bimage::cput(int i, complex c){
	put(i, ::real(c));
}

void Bimage::cleanup(void){
	size = 0;
	bug(__FILE__ ": Bimage::cleanup");
}

int Bimage::resize(void){

	return checksize(height*width);
}

Bimage::Bimage(){
	size = height = width = flags = 0;
	if(!static_ptable){
		// if no ptable yet, create one
		static_ptable = new Ptable;
		
		// make sure we have have some defaults
		static_ptable->add("gz", "gzip -c",     "gunzip -c"    );
		static_ptable->add("Z",  "compress -c", "uncompress -c");
		
		// load in users ~/.ptable
		static_ptable->addfile("~/.ptable");
	}
	ptable = static_ptable;
	prv_cmap = cmap = 0;
	offset = 0;
}

Bimage::~Bimage(){

	if( flags & HAS_PRV_CMAP){
		flags &= ~HAS_PRV_CMAP;
		prv_cmap->refs--;
		if(! prv_cmap->refs)
			delete prv_cmap;
	}
}
Uimage::~Uimage(){
	cleanup();
}
Fimage::~Fimage(){
	cleanup();
}
Cimage::~Cimage(){
	cleanup();
}

Fimage::Fimage(int x, int y, int m){

	height = y;
	width = x;
	data = 0;
	if(m)
		checksize( x*y );
}

Uimage::Uimage(int x, int y, int m){

	height = y;
	width = x;
	data = 0;
	if(m)
		checksize( x*y );
}
Cimage::Cimage(int x, int y, int m){

	height = y;
	width = x;
	data = 0;
	if(m)
		checksize( x*y );
}

Uimage::Uimage(const Bimage& c){

	data = 0;
	size = height = width = 0;
	offset = 0;

	operator=(c);
}
Fimage::Fimage(const Bimage& c){

	data = 0;
	size = height = width = 0;
	offset = 0;

	operator=(c);
}
Cimage::Cimage(const Bimage& c){

	data = 0;
	size = height = width = 0;
	offset = 0;

	operator=(c);
}
	
int Fimage::checksize(int check){
	int i;
	
	if( check > size){
		// need more memory
		float* nd = new float [ check];
		if(nd){
			loop(i,size) nd[i] = data[i];
			for(;i<check;i++) nd[i] = 0;
			delete [] data;
			data = nd;
			size = check;
		}else{
			bug(__FILE__ ": Fimage::checksize, out of memory");
		}
	}
	return size >= check;
}

Fimage& Fimage::operator=(const Bimage& s){
	int i;

	if(checksize(s.size)){
		height = s.height;
		width =  s.width;
		loop(i,s.size) data[i] = s.value(i);

		if(s.prv_cmap){
			prv_cmap = s.prv_cmap;
			prv_cmap->refs++;
		}
	}
	return *this;
}

void Fimage::cleanup(void){

	if(data) delete [] data;
	size = 0;
	data = 0;
}

int Uimage::checksize(int check){
	int i;
	
	if( check > size){
		// need more memory
		u_char* nd = new u_char [ check];
		if(nd){
			loop(i,size) nd[i] = data[i];
			for(;i<check;i++) nd[i] = 0;
			delete [] data;
			data = nd;
			size = check;
		}else{
			bug(__FILE__ ": Uimage::checksize, out of memory");
		}
	}
	return size >= check;
}

Uimage& Uimage::operator=(const Bimage& s){
	int i;

	if(checksize(s.size)){
		height = s.height;
		width =  s.width;
		loop(i,s.size) data[i] = u_char(s.value(i));

		if(s.prv_cmap){
			prv_cmap = s.prv_cmap;
			prv_cmap->refs++;
		}
	}
	return *this;
}

void Uimage::cleanup(void){

	if(data) delete [] data;
	size = 0;
	data = 0;
}
int Cimage::checksize(int check){
	int i;
	
	if( check > size){
		// need more memory
		complex* nd = new complex [ check ];
		if(nd){
			loop(i,size) nd[i] = data[i];
			for(;i<check;i++) nd[i] = 0;
			delete [] data;
			data = nd;
			size = check;
		}else{
			bug(__FILE__ ": Cimage::checksize, out of memory");
		}
	}
	return size >= check;
}

Cimage& Cimage::operator=(const Bimage& s){
	int i;

	if(checksize(s.size)){
		height = s.height;
		width =  s.width;
		loop(i,s.size) data[i] = s.cvalue(i);

		if(s.prv_cmap){
			prv_cmap = s.prv_cmap;
			prv_cmap->refs++;
		}
	}
	return *this;
}

void Cimage::cleanup(void){

	if(data) delete [] data;
	size = 0;
	data = 0;
}


float Fimage::value(int i) const {

#ifdef DEBUG	
	if(i>= size || i<0){
		bug( __FILE__ ": Fimage::value, out of range");
		return 0;
	}
#endif
	return data[i];
}
float Uimage::value(int i) const {
#ifdef DEBUG
	if(i>= size || i<0){
		bug( __FILE__ ": Uimage::value, out of range");
		return 0;
	}
#endif
	return data[i];
}

void Fimage::put(int i, float f){
#ifdef DEBUG
	if(i>=size || i<0){
		bug( __FILE__ ": Fimage::put, out of range");
		return;
	}
#endif
	data[i] = f;
}
void Uimage::put(int i, float f){
#ifdef DEBUG
	if(i>=size || i<0){
		bug( __FILE__ ": Uimage::put, out of range");		
		return;
	}
#endif
	data[i] = u_char(f);
}
complex Cimage::cvalue(int i) const {
#ifdef DEBUG
	if(i>= size || i<0){
		bug( __FILE__ ": Cimage::cvalue, out of range");
		return 0;
	}
#endif
	return data[i];
}
void Cimage::cput(int i, complex c){
#ifdef DEBUG
	if(i>=size || i<0){
		bug( __FILE__ ": Cimage::cput, out of range");
		return;
	}
#endif
	data[i] = c;
}
float Cimage::value(int i) const {
#ifdef DEBUG
	if(i>= size || i<0){
		bug( __FILE__ ": Cimage::value, out of range");
		return 0;
	}
#endif
	return ::real( data[i] );
}

void Cimage::put(int i, float f){
#ifdef DEBUG
	if(i>=size || i<0){
		bug( __FILE__ ": Cimage::put, out of range");
		return;
	}
#endif
	data[i] = complex( f, 0 );
}

Fimage& Fimage::operator=(float f){
	int i;

	for(i=0;i<size;i++) data[i] = f;
	return *this;
}

Uimage& Uimage::operator=(u_char c){
	int i;

	for(i=0;i<size;i++) data[i] = c;
	return *this;
}
Cimage& Cimage::operator=(complex c){
	int i;

	for(i=0;i<size;i++) data[i] = c;
	return *this;
}

Bimage& Bimage::operator=(const Bimage& s){
	int i;

	if(!checksize(s.size)) return *this;
	height = s.height;
	width =  s.width;
	loop(i,s.size)
		put(i, s.value(i));
	return *this;
}
Bimage& Bimage::operator=(float f){
	int i;
	for(i=0;i<size;i++) put(i, f);
	return *this;
}
void Bimage::line(Point a, Point b, float c){
	Point ap, dp, sp;
	int d;

	if( !within(a) || !within(b) ) return;

	// Bresenham

	dp = b - a;
	ap = abs( dp )*2;
	sp = sign( dp );

	if( ap.x > ap.y ){
		d = ap.y - (ap.x>>1);
		while(1){
			put(a, c);
			if(a.x == b.x) break; // final point?
			if(d >= 0){
				a.y += sp.y;
				d -= ap.x;
			}
			a.x += sp.x;
			d += ap.y;
		}
	}else{
		d = ap.x - (ap.y>>1);
		while(1){
			put(a, c);
			if(a.y == b.y) break; // final point?
			if(d >= 0){
				a.x += sp.x;
				d -= ap.y;
			}
			a.y += sp.y;
			d += ap.x;
		}
	}
}

void Bimage::xl_displ(const char *filename){

	save(filename);

	if( !vfork() ){
		if(!vfork()){
			// grandkid
			execlp("xloadimage", "xl_displ", filename, 0);
		}else{
			// child
			_exit(0);
		}
	}
	sleep(15);
	unlink(filename);
	wait(0);
}

void Bimage::xl_displ(void){
	char filename[256];

	tmpnam(filename);
	xl_displ(filename);
}

int Bimage::levels(void){

	int hist[256];
	int nclrs;
	int i;

	for(i=0;i<256;i++) hist[i] = 0;
	for(i=0;i<width*height;i++)hist[int(value(i))]++;

	for(i=nclrs=0;i<256;i++)
		if(hist[i]) nclrs++;

	return nclrs;
}

float Bimage::maxval(void) const {
	int i;
	float m = value(0);

	for(i=0;i<width*height;i++)
		m = MAX(m, value(i));
	return m;
}
float Bimage::minval(void) const {
	int i;
	float m = value(0);

	for(i=0;i<width*height;i++)
		m = MIN(m, value(i));
	return m;
}

int Uimage_fixed::checksize(int check){

	if(size < check){
		bug(__FILE__": Uimage_fixed::checksize, out of range");
		return 0;
	}
	return 1;
}
int Fimage_fixed::checksize(int check){

	if(size < check){
		bug(__FILE__": Fimage_fixed::checksize, out of range");
		return 0;
	}
	return 1;
}

void Uimage_fixed::init(u_char* d, int s){
	data = d;
	size = s;
}
void Fimage_fixed::init(float* d, int s){
	data = d;
	size = s;
}
Uimage_fixed::~Uimage_fixed(){
	cleanup();
}
Fimage_fixed::~Fimage_fixed(){
	cleanup();
}

void Fimage::stdat(void**d, int& sz){
	int s = size;
	size = sz;
	sz = s;

	void *x = data;
	data = (float*)*d;
	*d = x;
}

void Uimage::stdat(void**d, int& sz){
	int s = size;
	size = sz;
	sz = s;

	void *x = data;
	data = (u_char*)*d;
	*d = x;
}
void Cimage::stdat(void**d, int& sz){
	int s = size;
	size = sz;
	sz = s;

	void *x = data;
	data = (complex*)*d;
	*d = x;
}

void Fimage::swdat(Bimage* a){
	int i;
	
	// a->stdat((void**)&data, size);
	
	for(i=0;i<MIN(size, a->size); i++)
		put(i, a->value(i));
	width  = a->width;
	height = a->height;
}
void Uimage::swdat(Bimage* a){
	int i;
	
	// a->stdat((void**)&data, size);
	
	for(i=0;i<MIN(size, a->size); i++)
		put(i, a->value(i));
	width  = a->width;
	height = a->height;
}
void Cimage::swdat(Bimage* a){
	int i;
	
	// a->stdat((void**)&data, size);
	
	for(i=0;i<MIN(size, a->size); i++)
		cput(i, a->cvalue(i));
	width  = a->width;
	height = a->height;
}


void Uimage_fixed::swdat(Bimage* a){
	int i;
	
	for(i=0;i<MIN(size, a->size); i++)
		put(i, a->value(i));
	width  = a->width;
	height = a->height;
}
void Fimage_fixed::swdat(Bimage* a){
	int i;
	
	for(i=0;i<MIN(size, a->size); i++)
		put(i, a->value(i));
	width  = a->width;
	height = a->height;
}
void Uimage_fixed::stdat(void**, int&){
}
void Fimage_fixed::cleanup(void){
}
void Uimage_fixed::cleanup(void){
}

void Fimage_fixed::stdat(void**, int&){
}

Bimage* Fimage::clone(void) const{
	return (Bimage*)new Fimage( width, height );
}
Bimage* Uimage::clone(void) const{
	return (Bimage*)new Fimage( width, height );
}
Bimage* Cimage::clone(void) const{
	return (Bimage*)new Cimage( width, height );
}


	

	
