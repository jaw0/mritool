

// Copyright (c) 1993 Jeff Weisberg

#ifndef _images_h
#define _images_h

#ifdef RCSID
static const char *const rcsid_image_h
= "@(#)$Id: images.h,v 1.1 1995/11/23 04:31:14 jaw Exp jaw $";
#endif


#include <point.h>
/* #include <shmem.h> */
#include <ptable.h>
#undef complex
#undef Complex
#include <complex.h>

#define PSAVE

#ifndef INRANGE
#	define INRANGE(l,x,h)  ( (x)>=(l) ? (x)<(h) ? 1 : 0 : 0 )
#endif

class Fimage;
class Uimage;
class Bimage;
class Cimage;
class ColorImage;
class Contour;
class Vector;
class Matrix;

typedef unsigned char u_char;
extern void bug(const char*), bug(const char*, const char*);

class Cmap {
	
  public:
	int refs;
	u_char r[256], b[256], g[256];
	int len;

	Cmap(){len = 256; refs=1;}
};

class Bimage {
	// Abstract
    friend class Fimage;
    friend class Uimage;
    friend class Cimage;
    friend class Uimage_fixed;
    friend class Fimage_fixed;
  protected:
	int size;
	int flags;
#	define HAS_PRV_CMAP	1	
	static Ptable* static_ptable;
	Cmap* prv_cmap;
	
  public:
	int height, width;
	Point offset;
	Ptable* ptable;
	Cmap* cmap;

  protected:	
	Bimage();
	virtual int checksize(int)  =0;		// returns 0 on error
  public:
	virtual float value(int) const=0;
	virtual void  put(int, float) =0;

	virtual complex cvalue(int) const;
	virtual void    cput(int, complex);
	
  protected:
	virtual void swdat(Bimage*) =0;		// only call with a clone()ed object
	virtual void stdat(void**, int&) =0;	// do not call directly
  public:
	void put(Point p, float f){put(p.x, p.y, f);}
	float value(Point p) const {return value(p.x, p.y);}
	void put(int a, int b, float f){
		put( (b-offset.y)*width+(a-offset.x), f);}
	float value(int a, int b)const{
		return value((b-offset.y)*width+(a-offset.x));}

	void cput(Point p, complex c){cput(p.x, p.y, c);}
	complex cvalue(Point p) const {return cvalue(p.x, p.y);}
	void cput(int a, int b, complex c){
		cput( (b-offset.y)*width+(a-offset.x), c);}
	complex cvalue(int a, int b)const{
		return cvalue((b-offset.y)*width+(a-offset.x));}

	// n* do not add in the offset
	void nput(Point p, float f){nput(p.x, p.y, f);}
	void nput(int a, int b, float f){
		put( (b)*width+(a), f);}

	float nvalue(Point p) const {return nvalue(p.x, p.y);}
	float nvalue(int a, int b)const {
		return value((b)*width+(a));}

	void ncput(Point p, complex c){ncput(p.x, p.y, c);}
	void ncput(int a, int b, complex c){
		cput( (b)*width+(a), c);}

	complex ncvalue(Point p) const {return ncvalue(p.x, p.y);}
	complex ncvalue(int a, int b)const {
		return cvalue((b)*width+(a));}

	
  public:
	enum Support { Square, Round };		// for dilation
	
	virtual ~Bimage();
	int within(Point)const;
	int nwithin(Point)const;
	virtual void  cleanup(void);
	virtual Bimage* clone(void)const =0;	// return same type (and size) of image 

	// members stored in sep. file
#include <image-ops.h>	

	
};

class Fimage : public Bimage {
    friend class Uimage;
	
  protected:
	float *data;

	virtual int checksize(int);
	virtual void swdat(Bimage*);
	virtual void stdat(void**, int&);

  public:

	Fimage(){data=0;}
	Fimage(int, int, int m=1);	// m=1 std mem; m=0 shared mem
	Fimage(const Bimage&);
	virtual ~Fimage();
	virtual Bimage* clone(void)const;
	
	Fimage& operator=(const Bimage&);
	Fimage& operator=(float);
	virtual float value(int) const;
	virtual void  put(int, float);
	float & operator[](int i){
		if(i>=size||i<0){
			bug(__FILE__ ": Fimage::[], out of range");i=0;}
		return data[i];}

        float & operator[](Point p){
		return operator()(p.x, p.y);
	}
	
	float & operator()(int a, int b){
		return operator[]( (b-offset.y)*width
				  + (a-offset.x) );
	}
        float operator[](Point p) const {
		return operator()(p.x, p.y);
	}
	float operator()(int x, int y) const{
		return operator[]((y-offset.y)*width
				  + (x-offset.x));
	}
	float operator[](int i) const {
		if(i>=size||i<0){
			bug(__FILE__ ": Fimage::[]const, out of range");i=0;}
		return data[i];
	}
	
	operator float*() const { return data; }
	operator int() const { return data?1:0; }
	virtual void  cleanup(void);

};

class Uimage : public Bimage {
    friend class Fimage;
	
  protected:
	u_char *data;

	virtual int checksize(int);
	virtual void swdat(Bimage*);
	virtual void stdat(void**, int&);

  public:

	Uimage(){data=0;}
	Uimage(int, int, int m=1);
	Uimage(const Bimage&);
	virtual ~Uimage();
	virtual Bimage* clone(void)const;

	virtual float value(int) const;
	virtual void  put(int, float);
	Uimage& operator=(const Bimage&);
	Uimage& operator=(u_char);
	u_char& operator[](int i){
		if(i>=size||i<0){
			bug(__FILE__ ": Uimage::[], out of range");i=0;}
		return data[i];
	}
	u_char& operator[](Point p){
		return operator()(p.x, p.y);
	}
	u_char& operator()(int x, int y){
		return operator[]((y-offset.y)*width
				  + (x-offset.x));
	}
	
	u_char operator[](Point p) const {
		return operator()(p.x, p.y);
	}
	u_char operator()(int x, int y) const{
		return operator[]((y-offset.y)*width
				  + (x-offset.x));
	}
	u_char operator[](int i) const {
		if(i>=size||i<0){
			bug(__FILE__ ": Uimage::[]const, out of range");i=0;}
		return data[i];
	}

	operator u_char*() const { return data; }
	operator char*()   const { return (char*)data; }
	operator int() const { return data?1:0; }
	virtual void  cleanup(void);

};

class Uimage_fixed : public Uimage {
  private:
	virtual int checksize(int);
  protected:
	void init(u_char*,int);
	virtual void swdat(Bimage*);
	virtual void stdat(void**, int&);
	
  public:
	Uimage_fixed(u_char*d,int s){init(d,s);}
	Uimage_fixed(){}
	~Uimage_fixed();
	virtual void cleanup(void);
};

class Fimage_fixed : public Fimage {
  private:
	virtual int checksize(int);
  protected:
        void init(float*,int);
	virtual void swdat(Bimage*);
	virtual void stdat(void**, int&);
	
  public:
	Fimage_fixed(float*d,int s){init(d,s);}
	Fimage_fixed(){}
	~Fimage_fixed();
	virtual void cleanup(void);
};
	
#ifdef 0
class ShFimage : public Fimage_fixed, private ShMem {

  public:
	ShFimage();
	ShFimage(int x, int y);
	virtual ~ShFimage();

	virtual void cleanup(void);
};

class ShUimage : public Uimage_fixed, private ShMem {

  public:
	ShUimage();
	ShUimage(int x, int y);
	virtual ~ShUimage();

	virtual void cleanup(void);
};
#endif

class ColorImage {

  public:
	Bimage& red;
	Bimage& grn;
	Bimage& blu;

	ColorImage();
	ColorImage(const Bimage&);
	ColorImage(const Bimage&, const Bimage&, const Bimage&);

	virtual ~ColorImage();

	void zoom(float f){red.zoom(f);grn.zoom(f);blu.zoom(f);}
	void rotate(float f){red.rotate(f);grn.rotate(f);blu.rotate(f);}
	
};

class Cimage : public Bimage {
	
  protected:
        complex *data;

	virtual int checksize(int);
	virtual void swdat(Bimage*);
	virtual void stdat(void**, int&);

  public:
	Cimage(){data=0;}
	Cimage(int, int, int m=1);	// m=1 std mem; m=0 shared mem
	Cimage(const Bimage&);
	virtual ~Cimage();
	virtual Bimage* clone(void)const;
	
	Cimage& operator=(const Bimage&);
	Cimage& operator=(complex);
	virtual float value(int) const;
	virtual void  put(int, float);
	
	virtual complex cvalue(int) const;
	virtual void    cput(int, complex);
	
	complex & operator[](int i){
		if(i>=size||i<0){
			bug(__FILE__ ": Cimage::[], out of range");i=0;}
		return data[i];}

        complex & operator[](Point p){
		return operator()(p.x, p.y);
	}
	
	complex & operator()(int a, int b){
		return operator[]( (b-offset.y)*width
				  + (a-offset.x) );
	}
        complex operator[](Point p) const {
		return operator()(p.x, p.y);
	}
        complex operator()(int x, int y) const{
		return operator[]((y-offset.y)*width
				  + (x-offset.x));
	}
	complex operator[](int i) const {
		if(i>=size||i<0){
			bug(__FILE__ ": Cimage::[]const, out of range");i=0;}
		return data[i];
	}
	
	operator complex*() const { return data; }
	operator int() const { return data?1:0; }
	virtual void  cleanup(void);

};

#endif /* !_images_h */
