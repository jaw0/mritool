

// Copyright (c) 1993 Jeff Weisberg

#ifndef _image_ops_h
#define _image_ops_h

// @(#)$Id: image-ops.h,v 1.1 1995/11/23 04:31:14 jaw Exp jaw $

#ifdef _LISP_IP_
Title image-ops.h
#endif

/*
   Bimage member functions

   this file is used directly BOTH for the c++ class 
   and to automagically generate stubs for lisp
*/

#ifdef _LISP_IP_
#	define DOC(x)	x
#else
#	define DOC(x)	/* x */
#endif
#ifndef _LISP_IP_
int  load(const char*);		// return 0 on error
int  save(const char*) const;
int  save(ostream&)    const;
int  load(istream&);
#else
bool load(const char*)	DOC("load image");
bool save(const char*)	DOC("save image");
#endif

#ifdef _LISP_IP_
void put(int, int, float)	DOC("set pixel value");
void nput(int, int, float)	DOC("set pixel value");
void cput(int, int, complex)	DOC("set complex pixel value");
void ncput(int, int, complex)	DOC("set complex pixel value");	
float value(int, int)		DOC("get pixel value");
float nvalue(int, int)		DOC("get pixel value");
complex cvalue(int, int)	DOC("get complex pixel value");
complex ncvalue(int, int)	DOC("get complex pixel value");
Bimage* clone(void)		DOC("return a new image of the same type");
#endif

Bimage& operator == (u_char)	DOC("what pixels are at this value");
#ifndef _LISP_IP_
Bimage& operator = (const Bimage&);
Bimage& operator =  (float);
#endif
Bimage& operator <= (float)		DOC("what pixels are less/equal than this value");	
Bimage& operator >= (float)		DOC("what pixels are more/equal than this value");
Bimage& operator *= (const Bimage&)	DOC("pixel by pixel product");
Bimage& operator /= (const Bimage&)	DOC("pixel by pixel division");
Bimage& operator += (const Bimage&)	DOC("pixel by pixel sum");
Bimage& operator -= (const Bimage&)	DOC("pixel by pixel difference");
Bimage& operator |= (const Bimage&)	DOC("pixel by pixel difference");
Bimage& operator &= (const Bimage&)	DOC("pixel by pixel and");
Bimage& operator ^= (const Bimage&)	DOC("pixel by pixel or");

void lapl(float)		DOC("laplacian filter");
void edge(float)	 	DOC("sobel edge map");
void thresh(float)		DOC("threshhold");
int  resize(void)		DOC("gets memory");
void blur_nl(int, float)	DOC("a non-linear blurlike operation");
void blur(int, float)		DOC("gaussian blur");
void median(int)		DOC("median filter");
#ifndef _LISP_IP_
void dilate(int, Support s = Square)		DOC("binary dilation");
#endif
void quantize(int l = 0)	DOC("quantize image");
void equalize(void)		DOC("histogram equalize");
void zoom(float)		DOC("change size, pixel replcates");
void rotate(float)		DOC("rotate degrees");
void gamma(float)	      	DOC("change gamma");
#ifndef _LISP_IP_
void addnoise(float)		DOC("add noise of variance");
#endif
void punch(float, float, float lo = 0, float hi = 256)	DOC("punch");
void interp(float, float)	DOC("interpolate");
#ifndef _LISP_IP_
void kmeans(Bimage&, int, int)	DOC("k-means segm");
void histogram(Vector&);
#endif

int  area(void) const		DOC("area of binary image");
int  levels(void)		DOC("return # of levels in image");
float mean(void) const		DOC("return mean");
float variance(void) const	DOC("return variance");
float minval(void) const		DOC("the minimum value");
float maxval(void) const		DOC("the maximum value");

void line(Point, Point, float l = 255)		DOC("draw a line");
int  fill(Point, float l = 255)			DOC("fill a region");
void invert(void)				DOC("invert image");
void circle(Point, float, float l = 255)	DOC("draw a circle");
#ifndef _LISP_IP_
void draw(const Contour&, float)	DOC("draw the contour");
#endif
void overlay(Bimage&, Point)	DOC("overlay with another image");
void cut(Point, Point)		DOC("cut out a piece of image");

int  countcolors(void)		DOC("reduce size of cmap");
void ordercolors(void)		DOC("arrange colors in Y order");
void resizecolors(int)		DOC("change cmap size");
void fbordercolor(void)		DOC("put colors in good order for framebuffer");
void grayramp(void)		DOC("set cmap to a gray scale");

void cannongray(void)		DOC("put colors in cannonical order");

#ifndef _LISP_IP_
void red(const Bimage&);	// extract color channels
void blu(const Bimage&);
void grn(const Bimage&);
void yyy(const Bimage&);
void iii(const Bimage&);
void qqq(const Bimage&);
void yyy(const ColorImage&);
void iii(const ColorImage&);
void qqq(const ColorImage&);

void quant(const ColorImage&, int);
void quant(const Bimage&, const Bimage&, const Bimage&, int);
#else
new red(void)	DOC("extract the red channel");
new blu(void)	DOC("extract the blue channel");
new grn(void)	DOC("extract the green channel");
new yyy(void)	DOC("extract the Y channel");
new iii(void)	DOC("extract the I channel");
new qqq(void)	DOC("extract the Q channel");

new quant(Bimage&, Bimage&, int l = 255)	DOC("color quantize");
#endif
       

void xl_displ(void)	DOC("display image with xloadimage");
#ifndef _LISP_IP_
void xl_displ(const char*);	// with a title
#endif

void fft(int dir = -1, int cntr = 0)	DOC("perform FFT");
void ifft(int dir = 1, int cntr = 0)	DOC("perform IFFT");
#ifndef _LISP_IP_
void real(const Bimage&);
void imag(const Bimage&);
void magnitude(const Bimage&);
void phase(const Bimage&);
#else
new real(void)		DOC("return the real portion of a complex image");
new imag(void)		DOC("return the imaginary portion of a complex image");
new magnitude(void)	DOC("return the magnitude of a complex image");
new phase(void)		DOC("return the phase of a complex image");
#endif
	
void edgemap(void)	DOC("edgemap");
void fractal(float)	DOC("create a fractal landscape of specified fractal dimension");
void stereo(void)	DOC("random-dot stereogram-ify");
void wipe(float)	DOC("set all pixels to a value");

void hough(int how = 0)			DOC("do a hough xform");
void pretty(int, int maxpass = 10)	DOC("make line thin and nice");
void dxform(void)			DOC("distance xform");
float cmp_cnts(const Bimage&)		DOC("compare contours");

void convolve(const Bimage&)		DOC("convolve images");
void emboss(void)			DOC("perform embossing effect");


#endif /* ! _image_ops_h */

